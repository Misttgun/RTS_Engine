#include "states/StateManager.h"
#include "window/Window.h"
#include "resources/TextureManager.h"
#include "resources/FontManager.h"
#include "states/StateLoading.h"
#include "particles/ParticleSystem.h"
#include "../../include/states/StateIntro.h"

StateIntro::StateIntro(ragnarok::StateManager* t_stateManager) : BaseState(t_stateManager)
{}

StateIntro::~StateIntro() = default;

void StateIntro::OnCreate()
{
    m_timePassed = 0.0f;

    auto context = m_stateMgr->GetContext();
    const sf::Vector2u windowSize = context->m_wind->GetRenderWindow()->getSize();

    ragnarok::TextureManager* textureMgr = context->m_textureManager;
    textureMgr->RequireResource("Intro");
    m_introSprite.setTexture(*textureMgr->GetResource("Intro"));
    m_introSprite.setOrigin(textureMgr->GetResource("Intro")->getSize().x / 2.0f, textureMgr->GetResource("Intro")->getSize().y / 2.0f);
    m_introSprite.setPosition(windowSize.x / 2.0f, windowSize.y / 2.0f - 75.f);

    ragnarok::FontManager* fontMgr = context->m_fontManager;
    m_text.setFont(*fontMgr->GetResource("Main"));
    m_text.setString(sf::String("Press SPACE to continue"));
    m_text.setCharacterSize(20);
    const sf::FloatRect textRect = m_text.getLocalBounds();
    m_text.setOrigin(textRect.left + textRect.width / 2.0f, textRect.top + textRect.height / 2.0f);
    m_text.setPosition(m_introSprite.getPosition().x,
                       m_introSprite.getPosition().y + textureMgr->GetResource("Intro")->getSize().y / 1.75f);

    ragnarok::EventManager* evMgr = context->m_eventManager;
    evMgr->AddCallback("Intro_Continue", &StateIntro::Continue, this);

    auto loading = m_stateMgr->GetState<ragnarok::StateLoading>(ragnarok::StateType::Loading);
    loading->SetManualContinue(false);
    LoadFiles();
}

void StateIntro::LoadFiles()
{
    auto context = m_stateMgr->GetContext();
    const std::string dir = "res/Particles/";
    auto fileList = ragnarok::Utils::GetFileList(ragnarok::Utils::GetWorkingDirectory() + dir, "*.particle");
    for (auto& file : fileList)
    {
        context->m_particles->AddFile(ragnarok::Utils::GetWorkingDirectory() + (dir + file.first));
    }

    auto loading = m_stateMgr->GetState<ragnarok::StateLoading>(ragnarok::StateType::Loading);
    loading->AddLoader(context->m_particles);
    loading->SetManualContinue(false);
}

void StateIntro::OnDestroy()
{
    ragnarok::TextureManager* textureMgr = m_stateMgr->GetContext()->m_textureManager;
    textureMgr->ReleaseResource("Intro");

    ragnarok::EventManager* evMgr = m_stateMgr->GetContext()->m_eventManager;
    evMgr->RemoveCallback(ragnarok::StateType::Intro, "Intro_Continue");
}

void StateIntro::Draw()
{
    sf::RenderWindow* window = m_stateMgr->GetContext()->m_wind->GetRenderWindow();

    window->draw(m_introSprite);
    window->draw(m_text);
}

void StateIntro::Continue(ragnarok::EventDetails* t_details)
{
    m_stateMgr->SwitchTo(ragnarok::StateType::MainMenu);
    m_stateMgr->Remove(ragnarok::StateType::Intro);
}

void StateIntro::Update(const sf::Time& t_time)
{}

void StateIntro::Activate()
{}

void StateIntro::Deactivate()
{}
