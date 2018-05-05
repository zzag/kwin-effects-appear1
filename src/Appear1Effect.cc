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

#include "Appear1Effect.h"

// KConfigSkeleton
#include "appear1config.h"

Appear1Effect::Appear1Effect()
{
    initConfig<Appear1Config>();
    reconfigure(ReconfigureAll);

    connect(KWin::effects, &KWin::EffectsHandler::windowAdded,
        this, &Appear1Effect::start);
    connect(KWin::effects, &KWin::EffectsHandler::windowClosed,
        this, &Appear1Effect::stop);
    connect(KWin::effects, &KWin::EffectsHandler::windowDeleted,
        this, &Appear1Effect::stop);
    connect(KWin::effects, &KWin::EffectsHandler::windowMinimized,
        this, &Appear1Effect::stop);
}

Appear1Effect::~Appear1Effect()
{
}

void Appear1Effect::reconfigure(ReconfigureFlags flags)
{
    Q_UNUSED(flags);

    Appear1Config::self()->read();
    m_blacklist = Appear1Config::blacklist().toSet();
    m_duration = animationTime(Appear1Config::duration() > 0
            ? Appear1Config::duration()
            : 160);
    m_opacity = Appear1Config::opacity();
    m_shift = Appear1Config::shift();
    m_distance = Appear1Config::distance();
}

void Appear1Effect::prePaintScreen(KWin::ScreenPrePaintData& data, int time)
{
    auto it = m_animations.begin();
    while (it != m_animations.end()) {
        Timeline& t = *it;
        t.update(time);
        if (t.done()) {
            it = m_animations.erase(it);
        } else {
            ++it;
        }
    }

    if (!m_animations.isEmpty()) {
        data.mask |= PAINT_SCREEN_WITH_TRANSFORMED_WINDOWS;
    }

    KWin::effects->prePaintScreen(data, time);
}

void Appear1Effect::prePaintWindow(KWin::EffectWindow* w, KWin::WindowPrePaintData& data, int time)
{
    if (m_animations.contains(w)) {
        data.setTransformed();
    }

    KWin::effects->prePaintWindow(w, data, time);
}

void Appear1Effect::paintWindow(KWin::EffectWindow* w, int mask, QRegion region, KWin::WindowPaintData& data)
{
    const auto it = m_animations.constFind(w);
    if (it != m_animations.cend()) {
        const qreal t = (*it).value();

        data.setYTranslation(interpolate(m_shift, 0, t));
        data.setZTranslation(interpolate(m_distance, 0, t));
        data.multiplyOpacity(interpolate(m_opacity, 1, t));
    }

    KWin::effects->paintWindow(w, mask, region, data);
}

void Appear1Effect::postPaintScreen()
{
    if (!m_animations.isEmpty()) {
        KWin::effects->addRepaintFull();
    }

    KWin::effects->postPaintScreen();
}

bool Appear1Effect::isActive() const
{
    return !m_animations.isEmpty();
}

bool Appear1Effect::supported()
{
    return KWin::effects->isOpenGLCompositing()
        && KWin::effects->animationsSupported();
}

bool Appear1Effect::shouldAnimate(const KWin::EffectWindow* w) const
{
    if (KWin::effects->activeFullScreenEffect()) {
        return false;
    }

    const auto* addGrab = w->data(KWin::WindowAddedGrabRole).value<void*>();
    if (addGrab != nullptr && addGrab != this) {
        return false;
    }

    if (!w->isManaged()) {
        return false;
    }

    if (m_blacklist.contains(w->windowClass())) {
        return false;
    }

    return w->isNormalWindow()
        || w->isDialog();
}

void Appear1Effect::start(KWin::EffectWindow* w)
{
    if (!shouldAnimate(w)) {
        return;
    }

    // Tell other effects(like fade, for example) to ignore this window.
    w->setData(KWin::WindowAddedGrabRole, QVariant::fromValue(static_cast<void*>(this)));

    Timeline& t = m_animations[w];
    t.setDuration(m_duration);
    t.setEasingCurve(QEasingCurve::InCurve);
}

void Appear1Effect::stop(KWin::EffectWindow* w)
{
    m_animations.remove(w);
}
