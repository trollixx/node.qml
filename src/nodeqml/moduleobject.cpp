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

Heap::ModuleObject::ModuleObject(QV4::ExecutionEngine *v4, const QString &moduleId, Heap::ModuleObject *moduleParent) :
    QV4::Heap::Object(v4),
    id(moduleId),
    parent(moduleParent)
{
    setVTable(NodeQml::ModuleObject::staticVTable());

    QV4::Scope scope(v4);
    QV4::ScopedObject self(scope, this);

    QV4::ScopedString s(scope);

    self->defineDefaultProperty(QStringLiteral("id"), (s = v4->newString(id)));
    self->defineAccessorProperty(QStringLiteral("exports"), NodeQml::ModuleObject::property_exports_getter, NodeQml::ModuleObject::property_exports_setter);
    self->defineAccessorProperty(QStringLiteral("filename"), NodeQml::ModuleObject::property_filename_getter, nullptr);
    self->defineAccessorProperty(QStringLiteral("loaded"), NodeQml::ModuleObject::property_loaded_getter, nullptr);
    self->defineDefaultProperty(QStringLiteral("require"), NodeQml::ModuleObject::method_require, 1);

    QV4::Scoped<NodeQml::ModuleObject> scopedParent(scope, moduleParent);
    self->defineDefaultProperty(QStringLiteral("parent"), scopedParent);

    if (scopedParent) {
        QV4::ScopedArrayObject parentChildren(scope, scopedParent->d()->childrenArray);
        parentChildren->push_back(self);
    }

    QV4::ScopedArrayObject children(scope, v4->newArrayObject());
    childrenArray = children->d();
    self->defineDefaultProperty(QStringLiteral("children"), children);

    QV4::ScopedObject o(scope, v4->newObject());
    exports = o->d();
}

void ModuleObject::markObjects(QV4::Heap::Base *that, QV4::ExecutionEngine *e)
{
    Heap::ModuleObject *self = static_cast<Heap::ModuleObject *>(that);
    if (self->childrenArray)
        self->childrenArray->mark(e);
    if (self->exports)
        self->exports->mark(e);

    QV4::Object::markObjects(that, e);
}

void ModuleObject::load(QV4::ExecutionEngine *v4, Heap::ModuleObject *moduleObject, const QString &path)
{
    QV4::Scope scope(v4);
    QV4::Scoped<ModuleObject> self(scope, moduleObject);

    const QFileInfo fi(path);
    self->d()->filename = path;
    self->d()->dirname = fi.absolutePath();

    const QString suffix = fi.suffix();
    if (suffix == QStringLiteral("js")) {
        compile(v4, self->d());
    } else if (suffix == QStringLiteral("json")) {
        QScopedPointer<QFile> file(new QFile(self->d()->filename));
        if (!file->open(QIODevice::ReadOnly)) {
            v4->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
            return;
        }
        QJsonDocument json = QJsonDocument::fromJson(file->readAll());
        if (json.isNull()) {
            v4->throwSyntaxError(QStringLiteral("Unexpected end of input"));
            return;
        }

        QV4::ScopedObject o(scope);
        if (json.isObject())
            o = QV4::JsonObject::fromJsonObject(v4, json.object());
        else if (json.isArray())
            o = QV4::JsonObject::fromJsonArray(v4, json.array());
        self->d()->exports = o->d();

    } else {
        qFatal("Wrong file type"); /// TODO: Remove
        return;
    }

    self->d()->loaded = true;
}

void ModuleObject::compile(QV4::ExecutionEngine *v4, Heap::ModuleObject *moduleObject)
{
    QV4::Scope scope(v4);
    QV4::Scoped<ModuleObject> self(scope, moduleObject);

    QV4::ScopedObject global(scope, v4->newObject());
    global->defineReadonlyProperty(QStringLiteral("module"), self);

    QV4::ScopedString s(scope);
    QV4::ScopedObject exports(scope, self->d()->exports);
    global->defineDefaultProperty(QStringLiteral("exports"), exports);

    const QFileInfo fi(self->d()->filename);
    global->defineReadonlyProperty(QStringLiteral("__dirname"),
                                   (s = v4->newString(fi.absoluteFilePath())));
    global->defineReadonlyProperty(QStringLiteral("__filename"),
                                   (s = v4->newString(fi.fileName())));
    // Require
    QV4::ScopedContext rootContext(scope, v4->rootContext());
    QV4::Scoped<RequireFunction> requireFunc(scope, v4->memoryManager->alloc<RequireFunction>(rootContext, self->d()));
    global->defineReadonlyProperty(QStringLiteral("require"), requireFunc);

    QScopedPointer<QFile> file(new QFile(self->d()->filename));
    if (!file->open(QIODevice::ReadOnly)) {
        v4->throwError(QString("require: Cannot open file '%1'").arg(file->fileName()));
        return;
    }

//  QV4::ContextStateSaver ctxSaver(scope, v4);
    QV4::Script script(v4, global, file->readAll(), self->d()->filename);
    script.strictMode = v4->currentContext()->strictMode;
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

        return;
    }
}

QV4::ReturnedValue ModuleObject::require(QV4::ExecutionEngine *v4, const QString &path, Heap::ModuleObject *parent, bool isMain)
{
    Q_UNUSED(isMain)
    qDebug("Require path: %s", qPrintable(path));

    QV4::Scope scope(v4);
    QV4::ScopedObject exports(scope);

    EnginePrivate *node = EnginePrivate::get(v4);
    if (node->hasNativeModule(path)) {
        qDebug("Native module found: %s", qPrintable(path));
        exports = node->nativeModule(path);
    } else {
        const QString parentPath = parent ? parent->dirname : QString();
        qDebug("Parent path: %s", qPrintable(parentPath));
        QString filename = resolveModule(path, parentPath);
        qDebug("Resolved module path: %s", qPrintable(filename));

        if (filename.isEmpty())
            return v4->throwError(QString("Cannot find module '%1'").arg(path));

        if (node->hasCachedModule(filename)) {
            qDebug("Module '%s' is already in cache (%s).", qPrintable(path), qPrintable(filename));
            QV4::ScopedString s(scope);
            exports = node->cachedModule(filename)->get(s = v4->newString("exports"));
        } else {
            QV4::Scoped<NodeQml::ModuleObject> module(scope, v4->memoryManager->alloc<NodeQml::ModuleObject>(v4, filename, parent));
            load(v4, module->d(), filename);

            if (v4->hasException)
                return v4->throwError(QString("Cannot load module '%1'").arg(path));

            node->cacheModule(filename, module);
            exports = module->d()->exports;
        }
    }

    return exports.asReturnedValue();
}

QString ModuleObject::resolveModule(const QString &request, const QString &parentPath)
{
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

QV4::ReturnedValue ModuleObject::property_exports_getter(QV4::CallContext *ctx)
{
    NODE_CTX_SELF(ModuleObject, ctx);
    return self->d()->exports->asReturnedValue();
}

QV4::ReturnedValue ModuleObject::property_exports_setter(QV4::CallContext *ctx)
{
    NODE_CTX_CALLDATA(ctx);
    NODE_CTX_SELF(ModuleObject, ctx);
    QV4::ScopedObject o(scope, callData->args[0]);
    self->d()->exports = o->d();
    return QV4::Encode::undefined();
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
    NODE_CTX_V4(ctx);

    if (!callData->argc || !callData->args[0].isString())
        return v4->throwError(QStringLiteral("require: path must be a string"));

    QV4::ScopedObject exports(scope, require(v4, callData->args[0].toQStringNoThrow(), self->d()));
    return exports.asReturnedValue();
}

Heap::RequireFunction::RequireFunction(QV4::ExecutionContext *ctx, NodeQml::Heap::ModuleObject *moduleObject) :
    QV4::Heap::FunctionObject(ctx, QStringLiteral("require")),
    module(moduleObject)
{
    setVTable(NodeQml::RequireFunction::staticVTable());
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

    return QV4::BuiltinFunction::call(require.getPointer(), cd);
}
