/*
 * Copyright (C) 2018 Vlad Zagorodniy <vladzzag@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef EFFECTS_TIMELINE_H
#define EFFECTS_TIMELINE_H

#include <QEasingCurve>

class Timeline {
public:
    enum Direction {
        Forward,
        Backward
    };

public:
    explicit Timeline(int duration = 1000, Direction direction = Forward)
        : m_duration(duration)
        , m_direction(direction)
    {
    }

    qreal progress() const
    {
        Q_ASSERT(m_duration > 0);
        auto t = static_cast<qreal>(m_elapsed) / m_duration;
        return m_direction == Backward ? 1 - t : t;
    }

    qreal value() const
    {
        return m_easingCurve.valueForProgress(progress());
    }

    qreal rprogress() const
    {
        Q_ASSERT(m_duration > 0);
        auto t = static_cast<qreal>(m_elapsed) / m_duration;
        return m_direction == Forward ? 1 - t : t;
    }

    qreal rvalue() const
    {
        return m_easingCurve.valueForProgress(rprogress());
    }

    void update(int delta)
    {
        Q_ASSERT(delta >= 0);
        if (m_finished) {
            return;
        }
        m_elapsed += delta;
        if (m_elapsed >= m_duration) {
            m_finished = true;
            m_elapsed = m_duration;
        }
    }

    int elapsed() const
    {
        return m_elapsed;
    }

    void setElapsed(int elapsed)
    {
        reset();
        update(elapsed);
    }

    int duration() const
    {
        return m_duration;
    }

    void setDuration(int duration)
    {
        Q_ASSERT(duration > 0);
        m_duration = duration;
    }

    Direction direction() const
    {
        return m_direction;
    }

    void setDirection(Direction direction)
    {
        if (m_direction == direction) {
            return;
        }
        if (m_elapsed > 0) {
            m_elapsed = m_duration - m_elapsed;
        }
        m_direction = direction;
    }

    void toggleDirection()
    {
        setDirection(m_direction == Forward ? Backward : Forward);
    }

    QEasingCurve easingCurve() const
    {
        return m_easingCurve;
    }

    void setEasingCurve(const QEasingCurve& easingCurve)
    {
        m_easingCurve = easingCurve;
    }

    void setEasingCurve(QEasingCurve::Type type)
    {
        m_easingCurve.setType(type);
    }

    bool done() const
    {
        return m_finished;
    }

    void reset()
    {
        m_elapsed = 0;
        m_finished = false;
    }

private:
    int m_duration;
    Direction m_direction;
    QEasingCurve m_easingCurve;

    int m_elapsed = 0;
    bool m_finished = false;
};

#endif // EFFECTS_TIMELINE_H
