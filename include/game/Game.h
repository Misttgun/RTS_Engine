#pragma once

#include "../ragnarok/window/Window.h"
#include "../ragnarok/events/EventManager.h"
#include "../ragnarok/states/StateManager.h"
#include "../ragnarok/resources/TextureManager.h"
#include "../ragnarok/utilities/RandomGenerator.h"
#include "../ragnarok/resources/FontManager.h"
#include "../ragnarok/resources/AudioManager.h"
#include "../ragnarok/sounds/SoundManager.h"
#include "../ragnarok/map/Map.h"
#include "../ragnarok/ecs/core/SystemManager.h"
#include "../ragnarok/ecs/core/EntityManager.h"
#include "../ragnarok/gui/GUIManager.h"
#include "../ragnarok/particles/ParticleSystem.h"

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
