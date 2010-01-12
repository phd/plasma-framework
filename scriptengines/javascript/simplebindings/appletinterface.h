/*
 *   Copyright 2008 Chani Armitage <chani@kde.org>
 *   Copyright 2008, 2009 Aaron Seigo <aseigo@kde.org>
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef APPLETINTERFACE_H
#define APPLETINTERFACE_H

#include <QAbstractAnimation>
#include <QEasingCurve>
#include <QObject>
#include <QSizePolicy>
#include <QScriptValue>

#include <Plasma/Applet>
#include <Plasma/PopupApplet>
#include <Plasma/DataEngine>

#include "simplejavascriptapplet.h"

class QAction;
class SimpleJavaScriptApplet;
class QSignalMapper;
class QSizeF;


namespace Plasma
{
    class ConfigLoader;
} // namespace Plasa

class AppletInterface : public QObject
{
    Q_OBJECT
    Q_ENUMS(FormFactor)
    Q_ENUMS(Location)
    Q_ENUMS(AspectRatioMode)
    Q_ENUMS(BackgroundHints)
    Q_ENUMS(QtOrientation)
    Q_ENUMS(QtAnchorPoint)
    Q_ENUMS(QtCorner)
    Q_ENUMS(QtSizePolicy)
    Q_ENUMS(QtAlignment)
    Q_ENUMS(AnimationDirection)
    Q_ENUMS(EasingCurveType)
    Q_PROPERTY(AspectRatioMode aspectRatioMode READ aspectRatioMode WRITE setAspectRatioMode)
    Q_PROPERTY(FormFactor formFactor READ formFactor)
    Q_PROPERTY(Location location READ location)
    Q_PROPERTY(QString currentActivity READ currentActivity)
    Q_PROPERTY(bool shouldConserveResources READ shouldConserveResources)
    Q_PROPERTY(QString activeConfig WRITE setActiveConfig READ activeConfig)
    Q_PROPERTY(bool busy WRITE setBusy READ isBusy)
    Q_PROPERTY(BackgroundHints backgroundHints WRITE setBackgroundHints READ backgroundHints)
    Q_PROPERTY(QGraphicsLayout *layout WRITE setLayout READ layout)
    Q_PROPERTY(bool immutable READ immutable)
    Q_PROPERTY(int apiVersion READ apiVersion)
    Q_PROPERTY(QObject *sender READ sender)

public:
    AppletInterface(SimpleJavaScriptApplet *parent);
    ~AppletInterface();

//------------------------------------------------------------------
//enums copy&pasted from plasma.h because qtscript is evil

enum FormFactor {
    Planar = 0,  /**< The applet lives in a plane and has two
                    degrees of freedom to grow. Optimize for
                    desktop, laptop or tablet usage: a high
                    resolution screen 1-3 feet distant from the
                    viewer. */
    MediaCenter, /**< As with Planar, the applet lives in a plane
                    but the interface should be optimized for
                    medium-to-high resolution screens that are
                    5-15 feet distant from the viewer. Sometimes
                    referred to as a "ten foot interface".*/
    Horizontal,  /**< The applet is constrained vertically, but
                    can expand horizontally. */
    Vertical     /**< The applet is constrained horizontally, but
                    can expand vertically. */
};

enum Location {
    Floating = 0, /**< Free floating. Neither geometry or z-ordering
                     is described precisely by this value. */
    Desktop,      /**< On the planar desktop layer, extending across
                     the full screen from edge to edge */
    FullScreen,   /**< Full screen */
    TopEdge,      /**< Along the top of the screen*/
    BottomEdge,   /**< Along the bottom of the screen*/
    LeftEdge,     /**< Along the left side of the screen */
    RightEdge     /**< Along the right side of the screen */
};

enum AspectRatioMode {
    InvalidAspectRatioMode = -1, /**< Unsetted mode used for dev convenience
                                    when there is a need to store the
                                    aspectRatioMode somewhere */
    IgnoreAspectRatio = 0,       /**< The applet can be freely resized */
    KeepAspectRatio = 1,         /**< The applet keeps a fixed aspect ratio */
    Square = 2,                  /**< The applet is always a square */
    ConstrainedSquare = 3,       /**< The applet is no wider (in horizontal
                                    formfactors) or no higher (in vertical
                                    ones) than a square */
    FixedSize = 4                /** The applet cannot be resized */
};

enum EasingCurveType {
    LinearEasingCurve = QEasingCurve::Linear,

    InQuadEasingCurve = QEasingCurve::InQuad,
    OutQuadEasingCurve = QEasingCurve::OutQuad,
    InOutQuadEasingCurve = QEasingCurve::InOutQuad,
    OutInQuadEasingCurve = QEasingCurve::OutInQuad,

    InCubicEasingCurve = QEasingCurve::InCubic,
    OutCubicEasingCurve = QEasingCurve::OutCubic,
    InOutCubicEasingCurve = QEasingCurve::InOutCubic,
    OutInCubicEasingCurve = QEasingCurve::OutInCubic,

    InQuartEasingCurve = QEasingCurve::InQuart,
    OutQuartEasingCurve = QEasingCurve::OutQuart,
    InOutQuartEasingCurve = QEasingCurve::InOutQuart,
    OutInQuartEasingCurve = QEasingCurve::OutInQuart,

    InQuintEasingCurve = QEasingCurve::InQuint,
    OutQuintEasingCurve = QEasingCurve::OutQuint,
    InOutQuintEasingCurve = QEasingCurve::InOutQuint,
    OutInQuintEasingCurve = QEasingCurve::OutInQuint,

    InSizeEasingCurve = QEasingCurve::InSine,
    OutSineEasingCurve = QEasingCurve::OutSine,
    InOutSineEasingCurve = QEasingCurve::InOutSine,
    OutInSineEasingCurve = QEasingCurve::OutInSine,

    InExpoEasingCurve = QEasingCurve::InExpo,
    OutExpoEasingCurve = QEasingCurve::OutExpo,
    InOutExpoEasingCurve = QEasingCurve::InOutExpo,
    OutInExpoEasingCurve = QEasingCurve::OutInExpo,

    InCircEasingCurve = QEasingCurve::InCirc,
    OutCircEasingCurve = QEasingCurve::OutCirc,
    InOutCircEasingCurve = QEasingCurve::InOutCirc,
    OutInCircEasingCurve = QEasingCurve::OutInCirc,

    InElasticEasingCurve = QEasingCurve::InElastic,
    OutElasticEasingCurve = QEasingCurve::OutElastic,
    InOutElasticEasingCurve = QEasingCurve::InOutElastic,
    OutInElasticEasingCurve = QEasingCurve::OutInElastic,

    InBackEasingCurve = QEasingCurve::InBack,
    OutBackEasingCurve = QEasingCurve::OutBack,
    InOutBackEasingCurve = QEasingCurve::InOutBack,
    OutInBackEasingCurve = QEasingCurve::OutInBack,

    InBouncEasingCurve = QEasingCurve::InBounce,
    OutBounceEasingCurve = QEasingCurve::OutBounce,
    InOutBounceEasingCurve = QEasingCurve::InOutBounce,
    OutInBounceEasingCurve = QEasingCurve::OutInBounce,

    InCurveEasingCurve = QEasingCurve::InCurve,
    OutCurveEasingCurve = QEasingCurve::OutCurve,
    SineCurveEasingCurve = QEasingCurve::SineCurve,
    CosineCurveEasingCurve = QEasingCurve::CosineCurve
};

//From Qt namespace
enum QtOrientation {
    QtHorizontal= Qt::Horizontal,
    QtVertical = Qt::Vertical
};

enum QtAnchorPoint {
    QtAnchorLeft = Qt::AnchorLeft,
    QtAnchorRight = Qt::AnchorRight,
    QtAnchorBottom = Qt::AnchorBottom,
    QtAnchorTop = Qt::AnchorTop,
    QtAnchorHorizontalCenter = Qt::AnchorHorizontalCenter,
    QtAnchorVerticalCenter = Qt::AnchorVerticalCenter
};

enum QtCorner {
    QtTopLeftCorner = Qt::TopLeftCorner,
    QtTopRightCorner = Qt::TopRightCorner,
    QtBottomLeftCorner = Qt::BottomLeftCorner,
    QtBottomRightCorner = Qt::BottomRightCorner
};

enum QtSizePolicy {
    QSizePolicyFixed = QSizePolicy::Fixed,
    QSizePolicyMinimum = QSizePolicy::Minimum,
    QSizePolicyMaximum = QSizePolicy::Maximum,
    QSizePolicyPreferred = QSizePolicy::Preferred,
    QSizePolicyExpanding = QSizePolicy::Expanding,
    QSizePolicyMinimumExpanding = QSizePolicy::MinimumExpanding,
    QSizePolicyIgnored = QSizePolicy::Ignored
};

enum BackgroundHints {
    NoBackground = Plasma::Applet::NoBackground,
    StandardBackground = Plasma::Applet::StandardBackground,
    TranslucentBackground = Plasma::Applet::TranslucentBackground,
    DefaultBackground = Plasma::Applet::DefaultBackground
};

enum QtAlignment {
    QtAlignLeft = 0x0001,
    QtAlignRight = 0x0002,
    QtAlignHCenter = 0x0004,
    QtAlignJustify = 0x0005,
    QtAlignTop = 0x0020,
    QtAlignBottom = 0x0020,
    QtAlignVCenter = 0x0080
};

enum QtScrollBarPolicy {
    QtScrollBarAsNeeded = Qt::ScrollBarAsNeeded,
    QtScrollBarAlwaysOff = Qt::ScrollBarAlwaysOff,
    QtScrollBarAlwaysOn = Qt::ScrollBarAlwaysOn
};

enum AnimationDirection {
    AnimationForward = QAbstractAnimation::Forward,
    AnimationBackward = QAbstractAnimation::Backward
};
//-------------------------------------------------------------------

    Q_INVOKABLE void gc();
    Q_INVOKABLE FormFactor formFactor() const;

    Location location() const;
    QString currentActivity() const;
    bool shouldConserveResources() const;

    Q_INVOKABLE AspectRatioMode aspectRatioMode() const;
    Q_INVOKABLE void setAspectRatioMode(AspectRatioMode mode);


    Q_INVOKABLE void setFailedToLaunch(bool failed, const QString &reason = QString());

    Q_INVOKABLE bool isBusy() const;
    Q_INVOKABLE void setBusy(bool busy);

    Q_INVOKABLE BackgroundHints backgroundHints() const;
    Q_INVOKABLE void setBackgroundHints(BackgroundHints hint);

    Q_INVOKABLE void setConfigurationRequired(bool needsConfiguring, const QString &reason = QString());

    Q_INVOKABLE QSizeF size() const;
    Q_INVOKABLE QRectF rect() const;

    Q_INVOKABLE void setAction(const QString &name, const QString &text,
                               const QString &icon = QString(), const QString &shortcut = QString());

    Q_INVOKABLE void removeAction(const QString &name);

    Q_INVOKABLE void resize(qreal w, qreal h);

    Q_INVOKABLE void setMinimumSize(qreal w, qreal h);

    Q_INVOKABLE void setPreferredSize(qreal w, qreal h);

    Q_INVOKABLE void update(const QRectF &rect = QRectF());

    Q_INVOKABLE QString activeConfig() const;

    Q_INVOKABLE void setActiveConfig(const QString &name);

    Q_INVOKABLE QScriptValue readConfig(const QString &entry) const;

    Q_INVOKABLE void writeConfig(const QString &entry, const QVariant &value);

    Q_INVOKABLE QString file(const QString &fileType);
    Q_INVOKABLE QString file(const QString &fileType, const QString &filePath);

    Q_INVOKABLE bool include(const QString &script);
    Q_INVOKABLE bool hasExtension(const QString &extension) const;

    Q_INVOKABLE void debug(const QString &msg);
    Q_INVOKABLE QObject *findChild(const QString &name) const;

    Plasma::DataEngine *dataEngine(const QString &name);

    const Plasma::Package *package() const;
    QList<QAction*> contextualActions() const;
    QGraphicsLayout *layout() const;
    void setLayout(QGraphicsLayout *);
    bool immutable() const;
    int apiVersion() const;

    inline Plasma::Applet *applet() const { return m_appletScriptEngine->applet(); }

Q_SIGNALS:
    void releaseVisualFocus();
    void configNeedsSaving();

public Q_SLOTS:
    void dataUpdated(QString source, Plasma::DataEngine::Data data);

protected:
    SimpleJavaScriptApplet *m_appletScriptEngine;

private:
    QSet<QString> m_actions;
    QSignalMapper *m_actionSignals;
    QString m_currentConfig;
    QMap<QString, Plasma::ConfigLoader*> m_configs;
};

class PopupAppletInterface : public AppletInterface
{
    Q_OBJECT
    Q_PROPERTY(QIcon popupIcon READ popupIcon WRITE setPopupIcon)

public:
    PopupAppletInterface(SimpleJavaScriptApplet *parent);

    void setPopupIcon(const QIcon &icon);
    QIcon popupIcon();

    inline Plasma::PopupApplet *popupApplet() const { return static_cast<Plasma::PopupApplet *>(m_appletScriptEngine->applet()); }

public Q_SLOTS:
    void setPopupIconByName(const QString &name);
};

#endif
