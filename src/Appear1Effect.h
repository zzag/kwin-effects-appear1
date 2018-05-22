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

#ifndef EFFECTS_APPEAR1_H
#define EFFECTS_APPEAR1_H

// own
#include "Timeline.h"

// kwineffects
#include <kwineffects.h>

// Qt
#include <QHash>
#include <QSet>
#include <QString>

class Appear1Effect : public KWin::Effect {
public:
    Appear1Effect();
    ~Appear1Effect() override;

    void reconfigure(ReconfigureFlags flags) override;
    int requestedEffectChainPosition() const override;

    void prePaintScreen(KWin::ScreenPrePaintData& data, int time) override;
    void prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, int time) override;
    void paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data) override;
    void postPaintScreen() override;

    bool isActive() const override;

public:
    static bool supported();

private:
    bool shouldAnimate(const KWin::EffectWindow* w) const;
    void start(KWin::EffectWindow* w);
    void stop(KWin::EffectWindow* w);

private:
    QSet<QString> m_blacklist;
    QHash<const KWin::EffectWindow*, Timeline> m_animations;
    int m_duration;
    qreal m_opacity;
    qreal m_scale;
};

inline int Appear1Effect::requestedEffectChainPosition() const
{
    return 50;
}

#endif // EFFECTS_APPEAR1_H
