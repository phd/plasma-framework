/*
    SPDX-FileCopyrightText: 2023 David Edmundson <davidedmundson@kde.org>
    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "plasmawindow.h"

#include "dialogshadows_p.h"
#include "private/dialogbackground_p.h"

#include <QMarginsF>
#include <QQuickItem>

#include <KWindowEffects>
#include <KX11Extras>

using namespace Plasma;

namespace PlasmaQuick
{
class PlasmaWindowPrivate
{
public:
    PlasmaWindowPrivate(PlasmaWindow *_q)
        : q(_q)
    {
    }
    void handleFrameChanged();
    void updateMainItemGeometry();
    PlasmaWindow *q;
    QPointer<QQuickItem> mainItem;
    DialogBackground *dialogBackground;
};

PlasmaWindow::PlasmaWindow(QWindow *parent)
    : QQuickWindow(parent)
    , d(new PlasmaWindowPrivate(this))
{
    setColor(QColor(Qt::transparent));
    setFlags(Qt::FramelessWindowHint);

    d->dialogBackground = new DialogBackground(contentItem());
    d->dialogBackground->setImagePath(QStringLiteral("dialogs/background"));
    connect(d->dialogBackground, &DialogBackground::fixedMarginsChanged, this, [this]() {
        d->updateMainItemGeometry();
        Q_EMIT marginsChanged();
    });
    connect(d->dialogBackground, &DialogBackground::maskChanged, this, [this]() {
        d->handleFrameChanged();
    });

    DialogShadows::self()->addWindow(this, d->dialogBackground->enabledBorders());
}

PlasmaWindow::~PlasmaWindow()
{
}

void PlasmaWindow::setMainItem(QQuickItem *mainItem)
{
    if (d->mainItem == mainItem)
        return;

    d->mainItem = mainItem;

    if (d->mainItem) {
        mainItem->setParentItem(contentItem());
        d->updateMainItemGeometry();
    }
    Q_EMIT mainItemChanged();
}

QQuickItem *PlasmaWindow::mainItem() const
{
    return d->mainItem;
}

static FrameSvg::EnabledBorders edgeToBorder(Qt::Edges edges)
{
    FrameSvg::EnabledBorders borders = FrameSvg::EnabledBorders(0);

    if (edges & Qt::Edge::TopEdge) {
        borders |= FrameSvg::EnabledBorder::TopBorder;
    }
    if (edges & Qt::Edge::BottomEdge) {
        borders |= FrameSvg::EnabledBorder::BottomBorder;
    }
    if (edges & Qt::Edge::LeftEdge) {
        borders |= FrameSvg::EnabledBorder::LeftBorder;
    }
    if (edges & Qt::Edge::RightEdge) {
        borders |= FrameSvg::EnabledBorder::RightBorder;
    }

    return borders;
}

void PlasmaWindow::setBorders(Qt::Edges bordersToShow)
{
    d->dialogBackground->setEnabledBorders(edgeToBorder(bordersToShow));
    DialogShadows::self()->setEnabledBorders(this, d->dialogBackground->enabledBorders());
}

void PlasmaWindow::resizeEvent(QResizeEvent *e)
{
    QQuickWindow::resizeEvent(e);

    const QSize windowSize = e->size();
    d->dialogBackground->setSize(windowSize);
    if (d->mainItem) {
        const QSize contentSize = windowSize.shrunkBy(margins());
        d->mainItem->setSize(contentSize);
    }
}

void PlasmaWindowPrivate::handleFrameChanged()
{
    auto theme = Plasma::Theme();
    const QRegion mask = dialogBackground->mask();
    KWindowEffects::enableBlurBehind(q, theme.blurBehindEnabled(), mask);
    KWindowEffects::enableBackgroundContrast(q,
                                             theme.backgroundContrastEnabled(),
                                             theme.backgroundContrast(),
                                             theme.backgroundIntensity(),
                                             theme.backgroundSaturation(),
                                             mask);

    if (KX11Extras::compositingActive()) {
        q->setMask(QRegion());
    } else {
        q->setMask(mask);
    }
}

void PlasmaWindowPrivate::updateMainItemGeometry()
{
    if (!mainItem) {
        return;
    }
    const QMargins frameMargins = q->margins();
    const QSize windowSize = q->size();

    mainItem->setX(frameMargins.left());
    mainItem->setY(frameMargins.top());

    const QSize contentSize = windowSize.shrunkBy(frameMargins);
    mainItem->setSize(contentSize);
}

QMargins PlasmaWindow::margins() const
{
    return QMargins(d->dialogBackground->leftMargin(),
                    d->dialogBackground->topMargin(),
                    d->dialogBackground->rightMargin(),
                    d->dialogBackground->bottomMargin());
}
}

#include "moc_plasmawindow.cpp"
