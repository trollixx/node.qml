#include "moduleobject.h"

#include "engine_p.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>

#include <private/qv4script_p.h>
#include <private/qv4jsonobject_p.h>

using namespace NodeQml;

DEFINE_OBJECT_VTABLE(ModuleObject);

ModuleObject::Data::Data(QV4::ExecutionEngine *v4, const QString &moduleId, ModuleObject *moduleParent) :
    QV4::Object::Data(v4),
    id(moduleId),
    loaded(false),
    parent(moduleParent),
    childrenArray(v4->newArrayObject()->getPointer())
{
    setVTable(staticVTable());

    QV4::Scope scope(v4);
    QV4::Scoped<ModuleObject> self(scope, this);
    QV4::ScopedString s(scope);
    QV4::ScopedValue v(scope);

    self->defineDefaultProperty(QStringLiteral("id"), (s = v4->newString(id)));
    self->defineAccessorProperty(QStringLiteral("filename"), property_filename_getter, nullptr);
    self->defineAccessorProperty(QStringLiteral("loaded"), property_loaded_getter, nullptr);

    self->defineDefaultProperty(QStringLiteral("parent"), parent
                                 ? (v = parent->asReturnedValue())
                                 : (v = QV4::Primitive::undefinedValue()));
    if (parent)
        parent->d()->childrenArray->push_back(self);

    QV4::ScopedObject exports(scope, v4->newObject());
    exportsObject = exports->asObject();
    self->defineDefaultProperty(QStringLiteral("exports"), exports);

    QV4::ScopedArrayObject children(scope, v4->newArrayObject());
    childrenArray = children->asArrayObject();
    self->defineDefaultProperty(QStringLiteral("children"), children);

    self->defineDefaultProperty(QStringLiteral("require"), method_require);
}


void ModuleObject::load(QV4::ExecutionContext *ctx, const QString &path)
{
    QV4::ExecutionEngine *v4 = ctx->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<ModuleObject> self(scope, this);
    QV4::ScopedObject exports(scope);

    d()->filename = path;
    d()->dirname = QFileInfo(path).absolutePath();

    QFileInfo fi(path);
    QString suffix = fi.suffix();
    if (suffix == QStringLiteral("js")) {
        exports = self->compile(ctx);
    } else if (suffix == QStringLiteral("json")) {
        QScopedPointer<QFile> file(new QFile(d()->filename));
        if (!file->open(QIODevice::ReadOnly)) {
            v4->currentContext()->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
            return;
        }
        QJsonDocument json = QJsonDocument::fromJson(file->readAll());

        if (json.isNull()) {
            ctx->throwSyntaxError(QStringLiteral("Unexpected end of input"));
            return;
        }

        if (json.isObject())
            exports = QV4::JsonObject::fromJsonObject(v4, json.object());
        else if (json.isArray())
            exports = QV4::JsonObject::fromJsonArray(v4, json.array());

    } else {
        qFatal("Wrong file type"); /// TODO: Remove
        return;
    }

    self->defineDefaultProperty(QStringLiteral("exports"), exports);

    d()->loaded = true;
}

QV4::Object *ModuleObject::compile(QV4::ExecutionContext *ctx)
{
    QV4::ExecutionEngine *v4 = ctx->engine();
    QV4::Scope scope(ctx);
    QV4::Scoped<ModuleObject> self(scope, this);

    QV4::ScopedObject global(scope, v4->newObject());
    global->defineReadonlyProperty(QStringLiteral("module"), self);

    QV4::ScopedString s(scope);
    QV4::ScopedObject exports(scope, self->get(s = v4->newString(QStringLiteral("exports"))));
    global->defineDefaultProperty(QStringLiteral("exports"), exports);

    QFileInfo fi(d()->filename);
    global->defineReadonlyProperty(QStringLiteral("__dirname"),
                                   (s = v4->newString(fi.absoluteFilePath())));
    global->defineReadonlyProperty(QStringLiteral("__filename"),
                                   (s = v4->newString(fi.fileName())));

    // Require
    QV4::Scoped<RequireFunction> requireFunc(scope, v4->memoryManager->alloc<RequireFunction>(ctx, this));
    global->defineReadonlyProperty(QStringLiteral("require"), requireFunc);


    QScopedPointer<QFile> file(new QFile(d()->filename));
    if (!file->open(QIODevice::ReadOnly)) {
        v4->currentContext()->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
        return nullptr;
    }

    QV4::ContextStateSaver ctxSaver(ctx);
    QV4::Script script(v4, global, file->readAll(), d()->filename);
    script.strictMode = v4->currentContext()->d()->strictMode;
    script.inheritContext = true; /// NOTE: Is it needed?
    script.parse();

    QV4::ScopedValue result(scope);
    if (!v4->hasException)
        result = script.run();

    if (v4->hasException) {
        QV4::StackTrace stackTrace;
        QV4::ScopedObject ex(scope, v4->catchException(ctx, &stackTrace));
        QV4::ScopedValue message(scope, ex->get(s = v4->newString(QStringLiteral("message"))));
        qDebug("Exception: %s", qPrintable(message->toQStringNoThrow()));
        foreach (const QV4::StackFrame &frame, stackTrace) {
            qDebug("    at %s (%s:%d:%d)",
                   qPrintable(frame.function), qPrintable(frame.source), frame.line, frame.column);
        }

        return nullptr;
    }

    exports = self->get(s = v4->newString(QStringLiteral("exports")));
    return exports->asObject();
}

QV4::Object *ModuleObject::require(QV4::ExecutionContext *ctx, const QString &path, ModuleObject *parent, bool isMain)
{
    Q_UNUSED(isMain)

    QV4::ExecutionEngine *v4 = ctx->engine();
    QV4::Scope scope(v4);
    QV4::ScopedObject exports(scope);
    QV4::ScopedString s(scope);
    QV4::ScopedValue v(scope);

    QString filename;

    EnginePrivate *node = EnginePrivate::get(v4);
    if (node->hasNativeModule(path)) {
        filename = path;
        exports = node->nativeModule(path);
    } else {
        const QString parentPath = parent ? parent->d()->dirname : QString();
        filename = resolveModule(ctx, path, parentPath);
        qDebug("Resolved module path: %s", qPrintable(filename));

        if (filename.isEmpty()) {
            qWarning() << QString("Cannot find module '%1'").arg(path);
            v4->currentContext()->throwError(QString("Cannot find module '%1'").arg(path));
            return nullptr;
        }

        if (node->hasCachedModule(filename)) {
            qDebug("Module '%s' is already in cache (%s).", qPrintable(path), qPrintable(filename));
            exports = node->cachedModule(filename)->get(s = v4->newString("exports"));
        } else {
            QV4::Scoped<ModuleObject> module(scope, v4->memoryManager->alloc<ModuleObject>(v4, filename, parent));
            module->load(ctx, filename);

            if (v4->hasException) {
                v4->currentContext()->throwError(QString("Cannot load module '%1'").arg(path));
                return nullptr;
            }

            node->cacheModule(filename, module);
            exports = module->get(s = v4->newString("exports"));
        }
    }
    return exports;
}

QString ModuleObject::resolveModule(QV4::ExecutionContext *ctx, const QString &request, const QString &parentPath)
{
    Q_UNUSED(parentPath)

    QV4::ExecutionEngine *v4 = ctx->engine();
    QV4::Scope scope(v4);

    EnginePrivate *node = EnginePrivate::get(v4);
    if (node->hasNativeModule(request))
        return request;

    // Bundled JS module
    QFileInfo fi(QStringLiteral(":/js/") + request + QStringLiteral(".js"));
    if (fi.exists())
        return fi.absoluteFilePath();

    /// TODO: .npm_modules

    fi.setFile(request);

    if (fi.isRelative())
        fi.setFile(QDir(parentPath).filePath(request));

    if (!fi.exists()) {
        fi.setFile(fi.absoluteFilePath() + QStringLiteral(".js"));
        if (!fi.exists() || fi.isDir())
            return QString();
    }

    if (fi.isFile())
        return fi.absoluteFilePath();

    // Checking for index file
    /// TODO: index.[ext]
    QFileInfo indexInfo(fi.absoluteFilePath() + QStringLiteral("/index.js"));
    if (indexInfo.exists() && indexInfo.isFile())
        return indexInfo.absoluteFilePath();

    return QString();
}

ModuleObject *ModuleObject::getThis(QV4::ExecutionContext *ctx)
{
    QV4::Scope scope(ctx);
    QV4::Scoped<ModuleObject> self(scope, ctx->d()->callData->thisObject);
    return self.getPointer();
}

QV4::ReturnedValue ModuleObject::property_filename_getter(QV4::CallContext *ctx)
{
    ModuleObject *self = getThis(ctx);
    return ctx->engine()->newString(self->d()->filename)->asReturnedValue();
}

QV4::ReturnedValue ModuleObject::property_loaded_getter(QV4::CallContext *ctx)
{
    ModuleObject *self = getThis(ctx);
    return QV4::Encode(self->d()->loaded);
}

QV4::ReturnedValue ModuleObject::method_require(QV4::CallContext *ctx)
{
    ModuleObject *self = getThis(ctx);
    const QV4::CallData * const callData = ctx->d()->callData;

    if (!callData->argc || !callData->args[0].isString())
        return ctx->throwError(QStringLiteral("require: path must be a string"));

    QV4::Scope scope(ctx->engine());
    QV4::ScopedObject exports(scope, require(ctx, callData->args[0].toQStringNoThrow(), self));
    return exports.asReturnedValue();
}

DEFINE_OBJECT_VTABLE(RequireFunction);

RequireFunction::Data::Data(QV4::ExecutionContext *scope, ModuleObject *moduleObject) :
    QV4::FunctionObject::Data(scope, QStringLiteral("require")),
    module(moduleObject)
{
    setVTable(staticVTable());
}

QV4::ReturnedValue RequireFunction::call(QV4::Managed *that, QV4::CallData *callData)
{
    QV4::ExecutionEngine *v4 = that->engine();
    QV4::Scope scope(v4);
    QV4::Scoped<RequireFunction> self(scope, that->as<RequireFunction>());
    QV4::Scoped<ModuleObject> module(scope, self->d()->module);
    QV4::ScopedString s(scope);
    QV4::Scoped<QV4::BuiltinFunction> require(
                scope, module->get(s = v4->newString(QStringLiteral("require"))));
    QV4::ScopedCallData cd(scope, 1);
    cd->thisObject = module;
    cd->args[0] = callData->args[0];
    return require->call(require.getPointer(), cd);
}
