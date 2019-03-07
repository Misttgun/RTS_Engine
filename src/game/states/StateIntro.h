#pragma once
#ifndef STATEINTRO_H
#define STATEINTRO_H

#include "BaseState.h"
#include "EventManager.h"

namespace SExE
{
    class StateIntro : public BaseState
    {
        public:
        StateIntro(StateManager* t_stateManager);
        ~StateIntro();

        void onCreate();
        void onDestroy();

        void activate();
        void deactivate();

        void update(const sf::Time& t_time);
        void draw();

        void carryOn(EventDetails* t_details);

        private:
        sf::Texture m_introTexture;
        sf::Sprite m_introSprite;
        sf::Font m_font;
        sf::Text m_text;

        float m_timePassed;
    };
}

#endif // !STATEINTRO_H