#pragma once

#include "window/Window.h"
#include "states/StateManager.h"
#include "resources/TextureManager.h"
#include "utilities/RandomGenerator.h"
#include "resources/FontManager.h"
#include "resources/AudioManager.h"
#include "sounds/SoundManager.h"
#include "map/Map.h"
#include "ecs/core/SystemManager.h"
#include "ecs/core/EntityManager.h"
#include "gui/GUIManager.h"
#include "particles/ParticleSystem.h"
#include "SharedContext.h"

class Game
{
    public:
    Game();
    ~Game();

    void Update();
    void Render();
    void LateUpdate();

    sf::Time GetElapsed() const;

    ragnarok::Window* GetWindow();

    private:
    void SetUpClasses();
    void SetUpECS();
    void SetUpStates();
    void RestartClock();
    sf::Clock m_clock;
    sf::Time m_elapsed;
    ragnarok::SharedContext m_context;
    ragnarok::RandomGenerator m_rand;
    ragnarok::Window m_window;
    ragnarok::TextureManager m_textureManager;
    ragnarok::FontManager m_fontManager;
    ragnarok::AudioManager m_audioManager;
    ragnarok::SoundManager m_soundManager;
    ragnarok::GUIManager m_guiManager;
    ragnarok::SystemManager m_systemManager;
    ragnarok::EntityManager m_entityManager;
    ragnarok::Map m_gameMap;
    std::unique_ptr<ragnarok::ParticleSystem> m_particles;
    std::unique_ptr<ragnarok::StateManager> m_stateManager;
};
