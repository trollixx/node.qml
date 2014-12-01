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
DEFINE_OBJECT_VTABLE(RequireFunction);

Heap::ModuleObject::ModuleObject(QV4::ExecutionEngine *v4, const QString &moduleId, NodeQml::ModuleObject *moduleParent) :
    QV4::Heap::Object(v4),
    id(moduleId),
    loaded(false),
    parent(moduleParent)
{
    QV4::Scope scope(v4);
    QV4::ScopedValue protectThis(scope, this);
    Q_UNUSED(protectThis)

    setVTable(NodeQml::ModuleObject::staticVTable());

    QV4::Scoped<NodeQml::ModuleObject> self(scope, this);
    QV4::ScopedString s(scope);
    QV4::ScopedValue v(scope);

    self->defineDefaultProperty(QStringLiteral("id"), (s = v4->newString(id)));
    self->defineAccessorProperty(QStringLiteral("filename"), NodeQml::ModuleObject::property_filename_getter, nullptr);
    self->defineAccessorProperty(QStringLiteral("loaded"), NodeQml::ModuleObject::property_loaded_getter, nullptr);

    self->defineDefaultProperty(QStringLiteral("parent"), parent
                                 ? (v = parent->asReturnedValue())
                                 : (v = QV4::Primitive::undefinedValue()));
    if (parent)
        parent->d()->childrenArray->push_back(self);

    QV4::ScopedObject exports(scope, v4->newObject());
    exportsObject = exports;
    self->defineDefaultProperty(QStringLiteral("exports"), exports);

    QV4::ScopedArrayObject children(scope, v4->newArrayObject());
    childrenArray = children->asArrayObject();
    self->defineDefaultProperty(QStringLiteral("children"), children);

    self->defineDefaultProperty(QStringLiteral("require"), NodeQml::ModuleObject::method_require);
}

void ModuleObject::markObjects(QV4::Heap::Base *that, QV4::ExecutionEngine *e)
{
    Heap::ModuleObject *o = static_cast<Heap::ModuleObject *>(that);
    if (o->childrenArray)
        o->childrenArray->mark(e);

    Object::markObjects(that, e);
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
            v4->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
            return;
        }
        QJsonDocument json = QJsonDocument::fromJson(file->readAll());

        if (json.isNull()) {
            v4->throwSyntaxError(QStringLiteral("Unexpected end of input"));
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
    QV4::Scoped<RequireFunction> requireFunc(scope, v4->memoryManager->alloc<RequireFunction>(v4->rootContext, this));
    global->defineReadonlyProperty(QStringLiteral("require"), requireFunc);

    QScopedPointer<QFile> file(new QFile(d()->filename));
    if (!file->open(QIODevice::ReadOnly)) {
        v4->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
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
        QV4::ScopedObject ex(scope, v4->catchException(&stackTrace));
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
        qDebug("Native module: %s", qPrintable(path));
        filename = path;
        exports = node->nativeModule(path);
    } else {
        const QString parentPath = parent ? parent->d()->dirname : QString();
        qDebug("Parent path: %s", qPrintable(parentPath));
        filename = resolveModule(ctx, path, parentPath);
        qDebug("Resolved module path: %s", qPrintable(filename));

        if (filename.isEmpty()) {
            qWarning() << QString("Cannot find module '%1'").arg(path);
            v4->throwError(QString("Cannot find module '%1'").arg(path));
            return nullptr;
        }

        if (node->hasCachedModule(filename)) {
            qDebug("Module '%s' is already in cache (%s).", qPrintable(path), qPrintable(filename));
            exports = node->cachedModule(filename)->get(s = v4->newString("exports"));
        } else {
            QV4::Scoped<ModuleObject> module(scope, v4->memoryManager->alloc<ModuleObject>(v4, filename, parent));
            module->load(ctx, filename);

            if (v4->hasException) {
                v4->throwError(QString("Cannot load module '%1'").arg(path));
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
    EnginePrivate *node = EnginePrivate::get(ctx->engine());

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

QV4::ReturnedValue ModuleObject::property_filename_getter(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(ModuleObject, ctx);
    return ctx->engine()->newString(self->d()->filename)->asReturnedValue();
}

QV4::ReturnedValue ModuleObject::property_loaded_getter(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(ModuleObject, ctx);
    return QV4::Encode(self->d()->loaded);
}

QV4::ReturnedValue ModuleObject::method_require(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(ModuleObject, ctx);

    if (!callData->argc || !callData->args[0].isString())
        return ctx->engine()->throwError(QStringLiteral("require: path must be a string"));

    QV4::ScopedObject exports(scope, require(ctx, callData->args[0].toQStringNoThrow(), self));
    return exports.asReturnedValue();
}

Heap::RequireFunction::RequireFunction(QV4::ExecutionContext *ctx, NodeQml::ModuleObject *moduleObject) :
    QV4::Heap::FunctionObject(ctx, QStringLiteral("require"))
{
    setVTable(NodeQml::RequireFunction::staticVTable());

    QV4::Scope scope(ctx->engine());
    QV4::ScopedValue protectThis(scope, this);
    Q_UNUSED(protectThis)

    module = moduleObject;
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
