/*
    SPDX-FileCopyrightText: 2016 Marco Martin <mart@kde.org>
    SPDX-FileCopyrightText: 2022 ivan (@ratijas) tkachenko <me@ratijas.tk>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

import QtQuick 2.6
import QtQuick.Templates @QQC2_VERSION@ as T
import org.kde.ksvg 1.0 as KSvg
import org.kde.kirigami 2 as Kirigami
import "private" as P

T.Slider {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitHandleWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitHandleHeight + topPadding + bottomPadding)

    wheelEnabled: true
    snapMode: T.Slider.SnapOnRelease
    hoverEnabled: true

    layer.enabled: opacity < 1
    opacity: control.enabled ? 1 : 0.5

    KSvg.Svg {
        id: sliderSvg
        imagePath: "widgets/slider"
        // FIXME
        colorSet: control.Kirigami.Theme.colorSet
    }

    handle: Item {
        x: Math.round(control.leftPadding + (horizontal ? control.visualPosition * (control.availableWidth - width) : (control.availableWidth - width) / 2))
        y: Math.round(control.topPadding + (horizontal ? (control.availableHeight - height) / 2 : control.visualPosition * (control.availableHeight - height)))

        implicitWidth: sliderSvg.hasElement("hint-handle-size") ? sliderSvg.elementSize("hint-handle-size").width : firstHandle.implicitWidth
        implicitHeight: sliderSvg.hasElement("hint-handle-size") ? sliderSvg.elementSize("hint-handle-size").height : firstHandle.implicitHeight

        KSvg.SvgItem {
            id: shadow
            z: -1
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            svg: sliderSvg
            elementId: control.horizontal ? "horizontal-slider-shadow" : "vertical-slider-shadow"
            visible: enabled && !control.pressed
        }
        KSvg.SvgItem {
            id: firstHandle
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            svg: sliderSvg
            elementId: control.horizontal ? "horizontal-slider-handle" : "vertical-slider-handle"
        }
        KSvg.SvgItem {
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            svg: sliderSvg
            elementId: control.horizontal ? "horizontal-slider-focus" : "vertical-slider-focus"
            visible: opacity > 0
            opacity: control.visualFocus
            Behavior on opacity {
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        }
        KSvg.SvgItem {
            anchors.centerIn: parent
            implicitWidth: naturalSize.width
            implicitHeight: naturalSize.height
            svg: sliderSvg
            elementId: control.horizontal ? "horizontal-slider-hover" : "vertical-slider-hover"
            visible: opacity > 0
            opacity: control.hovered
            Behavior on opacity {
                enabled: control.hovered
                NumberAnimation {
                    duration: Kirigami.Units.longDuration
                    easing.type: Easing.OutCubic
                }
            }
        }
    }

    background: KSvg.FrameSvgItem {
        imagePath: "widgets/slider"
        prefix: "groove"
        implicitWidth: control.horizontal ? Kirigami.Units.gridUnit * 12 : fixedMargins.left + fixedMargins.right
        implicitHeight: control.vertical ? Kirigami.Units.gridUnit * 12 : fixedMargins.top + fixedMargins.bottom

        width: control.horizontal ? Math.max(fixedMargins.left + fixedMargins.right, control.availableWidth) : implicitWidth
        height: control.vertical ? Math.max(fixedMargins.top + fixedMargins.bottom, control.availableHeight) : implicitHeight
        x: control.leftPadding + (control.horizontal ? 0 : Math.round((control.availableWidth - width) / 2))
        y: control.topPadding + (control.vertical ? 0 : Math.round((control.availableHeight - height) / 2))

        KSvg.FrameSvgItem {
            id: grooveFill
            imagePath: "widgets/slider"
            prefix: "groove-highlight"

            LayoutMirroring.enabled: control.mirrored
            anchors.left: parent.left
            anchors.bottom: parent.bottom
            // The general idea is to extend the groove at least up to the middle of a handle, but don't overextend it at the end.
            width: control.horizontal ? Math.max(fixedMargins.left + fixedMargins.right, Math.round(control.position * (control.availableWidth - control.handle.width / 2) + (control.handle.width / 2))) : parent.width
            height: control.vertical ? Math.max(fixedMargins.top + fixedMargins.bottom, Math.round(control.position * (control.availableHeight - control.handle.height / 2) + (control.handle.height / 2))) : parent.height
        }

        Loader {
            id: tickLoader
            readonly property int stepCount: (control.to - control.from) / control.stepSize
            visible: stepCount > 0 && stepCount <= 20
            active: visible
            anchors {
                left: control.horizontal ? parent.left : parent.right
                top: control.vertical ? parent.top : parent.bottom
                leftMargin: control.horizontal ? Math.round(control.handle.width / 2) : 1
                topMargin: control.vertical ? Math.round(control.handle.height / 2) : 1
            }
            width: control.horizontal ? parent.width - control.handle.width : control.background.x
            height: control.vertical ? parent.height - control.handle.height : control.background.y
            sourceComponent: Grid {
                anchors.fill: parent
                rows: control.vertical ? tickLoader.stepCount + 1 : 1
                columns: control.horizontal ? tickLoader.stepCount + 1 : 1
                spacing: (control.vertical ? height : width) / tickLoader.stepCount - Kirigami.Units.devicePixelRatio
                LayoutMirroring.enabled: control.mirrored
                Repeater {
                    model: tickLoader.stepCount + 1
                    delegate: Rectangle {
                        property bool withinFill: (control.horizontal ? index : stepCount - index) <= control.position * tickLoader.stepCount
                        width: control.vertical ? parent.width : 1
                        height: control.horizontal ? parent.height : 1
                        opacity: withinFill ? 1 : 0.3
                        color: withinFill ? Kirigami.Theme.highlightColor : Kirigami.Theme.textColor
                    }
                }
            }
        }
    }
}
