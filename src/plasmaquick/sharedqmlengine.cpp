/*
    SPDX-FileCopyrightText: 2013 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2023 Alexander Lohnau <alexander.lohnau@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "sharedqmlengine.h"
#include "appletcontext_p.h"

#include <KLocalizedContext>
#include <QDebug>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlNetworkAccessManagerFactory>
#include <QQuickItem>
#include <QTimer>

#include <Plasma/Applet>

#include "debug_p.h"

namespace PlasmaQuick
{

class SharedQmlEnginePrivate
{
public:
    SharedQmlEnginePrivate(SharedQmlEngine *parent)
        : q(parent)
        , component(nullptr)
        , delay(false)
        , m_engine(engine())
    {
        executionEndTimer = new QTimer(q);
        executionEndTimer->setInterval(0);
        executionEndTimer->setSingleShot(true);
        QObject::connect(executionEndTimer, &QTimer::timeout, q, [this]() {
            scheduleExecutionEnd();
        });
    }

    ~SharedQmlEnginePrivate()
    {
        // Reset the static engine when we and the static ptr are the last objects holding references to it
        if (s_engine.use_count() == 2) {
            s_engine.reset();

            // QQmlEngine does not take ownership of the QNAM factory so we need to
            // make sure to clean it, but only if we are the last user of the engine
            // otherwise we risk resetting the factory on an engine that is still in
            // use.
            auto factory = engine()->networkAccessManagerFactory();
            engine()->setNetworkAccessManagerFactory(nullptr);
            delete factory;
        }
    }

    void errorPrint(QQmlComponent *component);
    void execute(const QUrl &source);
    void scheduleExecutionEnd();
    void minimumWidthChanged();
    void minimumHeightChanged();
    void maximumWidthChanged();
    void maximumHeightChanged();
    void preferredWidthChanged();
    void preferredHeightChanged();

    SharedQmlEngine *q;

    QUrl source;

    QObject *rootObject = nullptr;
    QQmlComponent *component;
    QTimer *executionEndTimer;
    KLocalizedContext *context{nullptr};
    QQmlContext *rootContext;
    bool delay;
    std::shared_ptr<QQmlEngine> m_engine;

    static std::shared_ptr<QQmlEngine> engine()
    {
        if (!s_engine) {
            s_engine = std::make_shared<QQmlEngine>();
        }
        return s_engine;
    }

private:
    static std::shared_ptr<QQmlEngine> s_engine;
};

std::shared_ptr<QQmlEngine> SharedQmlEnginePrivate::s_engine = std::shared_ptr<QQmlEngine>{};

void SharedQmlEnginePrivate::errorPrint(QQmlComponent *component)
{
    QString errorStr = QStringLiteral("Error loading QML file.\n");
    if (component->isError()) {
        const QList<QQmlError> errors = component->errors();
        for (const QQmlError &error : errors) {
            errorStr +=
                (error.line() > 0 ? QString(QString::number(error.line()) + QLatin1String(": ")) : QLatin1String("")) + error.description() + QLatin1Char('\n');
        }
    }
    qWarning(LOG_PLASMAQUICK) << component->url().toString() << '\n' << errorStr;
}

void SharedQmlEnginePrivate::execute(const QUrl &source)
{
    if (source.isEmpty()) {
        qWarning(LOG_PLASMAQUICK) << "File name empty!";
        return;
    }

    delete component;
    component = new QQmlComponent(engine().get(), q);
    QObject::connect(component, &QQmlComponent::statusChanged, q, &SharedQmlEngine::statusChanged, Qt::QueuedConnection);

    component->loadUrl(source);
    rootObject = component->beginCreate(rootContext);

    if (delay) {
        executionEndTimer->start(0);
    } else {
        scheduleExecutionEnd();
    }
}

void SharedQmlEnginePrivate::scheduleExecutionEnd()
{
    if (component->isReady() || component->isError()) {
        q->completeInitialization();
    } else {
        QObject::connect(component, &QQmlComponent::statusChanged, q, [this]() {
            q->completeInitialization();
        });
    }
}

SharedQmlEngine::SharedQmlEngine(QObject *parent)
    : QObject(parent)
    , d(new SharedQmlEnginePrivate(this))
{
    d->rootContext = new QQmlContext(engine().get());
    d->rootContext->setParent(this); // Delete the context when deleting the shared engine

    d->context = new KLocalizedContext(d->rootContext);
    d->rootContext->setContextObject(d->context);
}

SharedQmlEngine::SharedQmlEngine(Plasma::Applet *applet, QObject *parent)
    : QObject(parent)
    , d(new SharedQmlEnginePrivate(this))
{
    d->rootContext = new AppletContext(engine().get(), applet, this);

    d->context = new KLocalizedContext(d->rootContext);
    d->rootContext->setContextObject(d->context);
}

SharedQmlEngine::~SharedQmlEngine() = default;

void SharedQmlEngine::setTranslationDomain(const QString &translationDomain)
{
    d->context->setTranslationDomain(translationDomain);
}

QString SharedQmlEngine::translationDomain() const
{
    return d->context->translationDomain();
}

void SharedQmlEngine::setSource(const QUrl &source)
{
    d->source = source;
    d->execute(source);
}

QUrl SharedQmlEngine::source() const
{
    return d->source;
}

void SharedQmlEngine::setInitializationDelayed(const bool delay)
{
    d->delay = delay;
}

bool SharedQmlEngine::isInitializationDelayed() const
{
    return d->delay;
}

std::shared_ptr<QQmlEngine> SharedQmlEngine::engine()
{
    return d->engine();
}

QObject *SharedQmlEngine::rootObject() const
{
    return d->rootObject;
}

QQmlComponent *SharedQmlEngine::mainComponent() const
{
    return d->component;
}

QQmlContext *SharedQmlEngine::rootContext() const
{
    return d->rootContext;
}

QQmlComponent::Status SharedQmlEngine::status() const
{
    if (!d->engine()) {
        return QQmlComponent::Error;
    }

    if (!d->component) {
        return QQmlComponent::Null;
    }

    return QQmlComponent::Status(d->component->status());
}

void SharedQmlEngine::completeInitialization(const QVariantHash &initialProperties)
{
    d->executionEndTimer->stop();

    if (!d->component) {
        qWarning(LOG_PLASMAQUICK) << "No component for" << source();
        return;
    }

    if (d->component->status() != QQmlComponent::Ready || d->component->isError()) {
        d->errorPrint(d->component);
        return;
    }

    for (auto it = initialProperties.constBegin(); it != initialProperties.constEnd(); ++it) {
        d->rootObject->setProperty(it.key().toUtf8().data(), it.value());
    }

    d->component->completeCreate();
    Q_EMIT finished();
}

QObject *SharedQmlEngine::createObjectFromSource(const QUrl &source, QQmlContext *context, const QVariantHash &initialProperties)
{
    QQmlComponent *component = new QQmlComponent(d->engine().get(), this);
    component->loadUrl(source);

    return createObjectFromComponent(component, context, initialProperties);
}

QObject *SharedQmlEngine::createObjectFromComponent(QQmlComponent *component, QQmlContext *context, const QVariantHash &initialProperties)
{
    QObject *object = component->beginCreate(context ? context : d->rootContext);

    for (auto it = initialProperties.constBegin(); it != initialProperties.constEnd(); ++it) {
        object->setProperty(it.key().toUtf8().data(), it.value());
    }
    component->completeCreate();

    if (!component->isError() && object) {
        // memory management
        component->setParent(object);
        // reparent to root object if wasn't specified otherwise by initialProperties
        if (!initialProperties.contains(QLatin1String("parent"))) {
            if (qobject_cast<QQuickItem *>(rootObject())) {
                object->setProperty("parent", QVariant::fromValue(rootObject()));
            } else {
                object->setParent(rootObject());
            }
        }

        return object;

    } else {
        d->errorPrint(component);
        delete object;
        return nullptr;
    }
}
}

#include "moc_sharedqmlengine.cpp"
