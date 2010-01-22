/*
 *   Copyright 2009 Mehmet Ali Akmanalp <makmanalp@wpi.edu>
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

/**
 * @file This file contains the abstract base class for all singular
 * animations.
 */

#ifndef PLASMA_ANIMATION_H
#define PLASMA_ANIMATION_H

#include <QtGui/QGraphicsWidget>
#include <QtCore/QObject>
#include <QtCore/QPropertyAnimation>
#include <QtCore/QAbstractAnimation>
#include <QtCore/QEasingCurve>

#include <plasma/plasma_export.h>
#include <plasma/plasma.h>

namespace Plasma
{

class AnimationPrivate;

/**
 * @brief Abstract representation of a single animation.
 * @since 4.4
 */
class PLASMA_EXPORT Animation : public QAbstractAnimation
{

    Q_OBJECT
    Q_ENUMS(Reference)
    Q_ENUMS(MovementDirection)
    Q_PROPERTY(int duration READ duration WRITE setDuration)
    Q_PROPERTY(QEasingCurve easingCurve READ easingCurve WRITE setEasingCurve)
    Q_PROPERTY(QGraphicsWidget *targetWidget READ targetWidget WRITE setTargetWidget)

public:
    /**
     * Get the animation duration. It can be set using the property duration.
     * @return duration in ms.
     */
    int duration() const;

    /**
     * Animation movement reference (used by \ref RotationAnimation).
     */
    enum Reference{
        Center,
        Up,
        Down,
        Left,
        Right
    };

    /**
     * The movement direction of an animation.
     */
    enum MovementDirection {
        MoveUp = 0,     /**< Move up */
        MoveUpRight,    /**< Move up and right */
        MoveRight,      /**< Move right */
        MoveDownRight,  /**< Move down and right */
        MoveDown,       /**< Move down */
        MoveDownLeft,   /**< Move down and left */
        MoveLeft,       /**< Move left */
        MoveUpLeft,     /**< Move up and left */
        MoveAny         /**< Move in any direction*/
    };


    /**
     * Default constructor.
     *
     * @param parent Object parent (might be set when using
     * \ref Animator::create factory).
     *
     */
    explicit Animation(QObject* parent = 0);

    /**
     * Destructor.
     */
    ~Animation() = 0;

    /**
     * Set the widget on which the animation is to be performed.
     * @arg widget The QGraphicsWidget to be animated.
     */
    void setTargetWidget(QGraphicsWidget* widget);

    /**
     * @return The widget that the animation will be performed upon
     */
    QGraphicsWidget *targetWidget() const;

    /**
     * Set the animation easing curve type
     */
    void setEasingCurve(const QEasingCurve &curve);

    /**
     * Get the animation easing curve type
     */
    QEasingCurve easingCurve() const;

protected:
    /**
     * Change the animation duration. Default is 250ms.
     * @arg duration The new duration of the animation.
     */
    virtual void setDuration(int duration = 250);

    /**
     * QAbstractAnimation will call this method while the animation
     * is running. Each specialized animation class should implement
     * the correct behavior for it.
     * @param currentTime Slapsed time using the \ref duration as reference
     * (it will be from duration up to zero if the animation is running
     * backwards).
     */
    virtual void updateCurrentTime(int currentTime);

private:

    /**
     * Internal pimple (actually is used as a data structure, see
     * \ref AnimationPrivate).
     */
    AnimationPrivate *const d;

};

} //namespace Plasma

#endif
