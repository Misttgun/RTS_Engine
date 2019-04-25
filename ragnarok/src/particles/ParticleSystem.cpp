#include "../../include/particles/ParticleSystem.h"
#include "../../include/particles/updaters/Lifespan.h"
#include "../../include/particles/updaters/Spatial.h"
#include "../../include/particles/updaters/Forces.h"
#include "../../include/particles/updaters/Collisions.h"
#include "../../include/particles/updaters/Interpolator.h"
#include "../../include/particles/updaters/Drawables.h"
#include "../../include/particles/generators/PointPosition.h"
#include "../../include/particles/generators/LinePosition.h"
#include "../../include/particles/generators/AreaPosition.h"
#include "../../include/particles/generators/RandomColor.h"
#include "../../include/particles/generators/RandomVelocity.h"
#include "../../include/particles/generators/RandomLifespan.h"
#include "../../include/particles/generators/RandomSize.h"
#include "../../include/particles/generators/SizeRange.h"
#include "../../include/particles/generators/ColorRange.h"
#include "../../include/particles/generators/RotationRange.h"
#include "../../include/particles/generators/Texture.h"
#include "../../include/particles/generators/Properties.h"
#include "../../include/map/Map.h"
#include "../../include/states/StateManager.h"

namespace ragnarok
{
    ParticleSystem::ParticleSystem(StateManager* t_stateManager, TextureManager* t_textureMgr, RandomGenerator* t_rand, Map* t_map)
        : m_textureMgr(t_textureMgr), m_stateManager(t_stateManager), m_rand(t_rand), m_map(t_map)
    {
        m_updaters.emplace("Lifespan", std::make_unique<LifespanUpdater>());
        m_updaters.emplace("Spatial", std::make_unique<SpatialUpdater>());
        m_updaters.emplace("Force", std::make_unique<ForceUpdater>());
        m_updaters.emplace("Collision", std::make_unique<CollisionUpdater>());
        m_updaters.emplace("Interpolator", std::make_unique<Interpolator>());
        m_updaters.emplace("Drawable", std::make_unique<DrawableUpdater>());

        RegisterGenerator<PointPosition>("PointPosition");
        RegisterGenerator<LinePosition>("LinePosition");
        RegisterGenerator<AreaPosition>("AreaPosition");
        RegisterGenerator<RandomColor>("RandomColor");
        RegisterGenerator<RandomVelocity>("RandomVelocity");
        RegisterGenerator<RandomLifespan>("RandomLifespan");
        RegisterGenerator<RandomSize>("RandomSize");
        RegisterGenerator<SizeRange>("SizeRange");
        RegisterGenerator<ColorRange>("ColorRange");
        RegisterGenerator<RotationRange>("RotationRange");
        RegisterGenerator<TextureGenerator>("Texture");
        RegisterGenerator<PropGenerator>("Properties");
    }

    Emitter* ParticleSystem::AddEmitter(std::unique_ptr<Emitter> t_emitter, const StateType& t_state)
    {
        const auto ptr = t_emitter.get();
        t_emitter->SetParticleSystem(this);

        if (!GetGenerators(t_emitter->GetGenerators()))
        {
            std::cout << "Failed to add emitter due to a missing resource: " << t_emitter->GetGenerators() << std::endl;
            return nullptr;
        }

        if (t_state == StateType(0))
        {
            if (m_emitterItr == m_emitters.end())
            {
                std::cout << "Cannot add an emitter to a state without particles." << std::endl; return nullptr;
            }

            m_emitterItr->second.emplace_back(std::move(t_emitter));
            return ptr;
        }

        auto itr = m_emitters.find(t_state);
        if (itr == m_emitters.end())
        {
            std::cout << "Cannot add an emitter to a state without particles." << std::endl; return nullptr;
        }

        itr->second.emplace_back(std::move(t_emitter));
        return ptr;
    }

    Emitter* ParticleSystem::AddEmitter(const sf::Vector3f& t_position, std::string& t_generators, const unsigned int t_emitRate, const StateType& t_state)
    {
        auto emitter = std::make_unique<Emitter>(t_position);
        emitter->SetGenerators(t_generators);
        emitter->SetEmitRate(t_emitRate);

        return AddEmitter(std::move(emitter), t_state);
    }

    void ParticleSystem::AddForce(ForceApplicator t_force, const StateType& t_state)
    {
        if (t_state == StateType(0))
        {
            if (m_stateItr == m_container.end())
            {
                std::cout << "Cannot add a force to a state without particles." << std::endl; return;
            }

            m_forces[m_currentState].emplace_back(t_force);
            return;
        }

        auto itr = m_forces.find(t_state);
        if (itr == m_forces.end())
        {
            std::cout << "Cannot add a force to a state without particles." << std::endl; return;
        }

        itr->second.emplace_back(t_force);
    }

    void ParticleSystem::RemoveEmitter(Emitter* t_emitter)
    {
        m_removedEmitters.push_back(t_emitter);
    }

    Emitter* ParticleSystem::FindEmitter(const sf::Vector2f& t_point, const sf::Vector2f& t_area, const unsigned int t_layerMin, const unsigned int t_layerMax)
    {
        if (m_emitterItr == m_emitters.end())
        {
            return nullptr;
        }

        for (auto& emitter : m_emitterItr->second)
        {
            const auto position = emitter->GetPosition();
            const auto elevation = std::floor(static_cast<int>(position.z / Sheet::Tile_Size));

            if (t_layerMax != -1 && (elevation < t_layerMin || elevation > t_layerMax))
            {
                continue;
            }

            sf::FloatRect box;
            box.left = position.x - (t_area.x / 2.f);
            box.top = position.y - (t_area.y / 2.f);
            box.width = t_area.x;
            box.height = t_area.y;

            if (box.contains(t_point))
            {
                return emitter.get();
            }
        }

        return nullptr;
    }

    GeneratorList* ParticleSystem::GetGenerators(const std::string& t_name)
    {
        auto& itr = m_generators.find(t_name);
        if (itr == m_generators.end())
        {
            return nullptr;
        }

        return &itr->second;
    }

    TextureManager* ParticleSystem::GetTextureManager() const
    {
        return m_textureMgr;
    }

    RandomGenerator* ParticleSystem::GetRand() const
    {
        return m_rand;
    }

    void ParticleSystem::CreateState(const StateType& t_state)
    {
        if (m_container.find(t_state) != m_container.end())
        {
            return;
        }

        m_container.emplace(t_state, std::make_unique<ParticleContainer>(m_textureMgr));
        m_emitters.emplace(t_state, EmitterList());
        m_forces.emplace(t_state, ForceApplicatorList());
        ChangeState(t_state);
    }

    void ParticleSystem::ChangeState(const StateType& t_state)
    {
        SetState(t_state);
        m_stateItr = m_container.find(m_currentState);
        m_emitterItr = m_emitters.find(m_currentState);

        auto c = dynamic_cast<CollisionUpdater*>(m_updaters["Collision"].get());
        if (t_state == StateType::Game || t_state == StateType::MapEditor)
        {
            c->SetMap(m_map);
        }
        else
        {
            c->SetMap(nullptr);
        }

        auto f = dynamic_cast<ForceUpdater*>(m_updaters["Force"].get());
        auto& forceItr = m_forces.find(m_currentState);
        if (forceItr == m_forces.end())
        {
            f->SetApplicators(nullptr); return;
        }

        f->SetApplicators(&forceItr->second);
    }

    void ParticleSystem::RemoveState(const StateType& t_state)
    {
        if (m_stateItr->first == t_state)
        {
            m_stateItr = m_container.end();
            m_emitterItr = m_emitters.end();
        }

        m_emitters.erase(t_state);
        m_forces.erase(t_state);
        m_container.erase(t_state);
    }

    void ParticleSystem::PurgeCurrentState() const
    {
        m_emitterItr->second.clear();
    }

    bool ParticleSystem::ProcessLine(std::stringstream& t_stream)
    {
        std::string type;
        t_stream >> type;
        if (type == "Name")
        {
            if (!(t_stream >> m_loadingGenerator))
            {
                std::cout << "Faulty particle file format." << std::endl; return false;
            }

            const auto generators = GetGenerators(m_loadingGenerator);
            if (generators)
            {
                std::cout << "A generator set with the name '" << m_loadingGenerator << "' already exists." << std::endl; return false;
            }
        }
        else
        {
            if (m_loadingGenerator.empty())
            {
                std::cout << "Faulty particle file format." << std::endl; return false;
            }

            const auto itr = m_factory.find(type);
            if (itr == m_factory.end())
            {
                std::cout << "Unknown generator type: " << type << std::endl; return true;
            }

            std::unique_ptr<BaseGenerator> generator(itr->second());
            t_stream >> *generator;
            m_generators[m_loadingGenerator].emplace_back(std::move(generator));
        }

        return true;
    }

    void ParticleSystem::ReadMapLine(const std::string& t_type, std::stringstream& t_stream)
    {
        if (t_type == "ParticleEmitter")
        {
            sf::Vector3f position;
            size_t emitRate;
            std::string generatorType;
            t_stream >> generatorType >> position.x >> position.y >> position.z >> emitRate;
            AddEmitter(position, generatorType, emitRate, m_stateManager->GetNextToLast());
        }
        else if (t_type == "ForceApplicator")
        {
            sf::Vector3f position;
            sf::Vector3f force;
            float radius;
            t_stream >> position.x >> position.y >> position.z >> force.x >> force.y >> force.z >> radius;
            AddForce(ForceApplicator(position, force, radius), m_stateManager->GetNextToLast());
        }
    }

    void ParticleSystem::SaveMap(std::ofstream& t_fileStream)
    {
        for (auto& emitter : m_emitterItr->second)
        {
            t_fileStream << "ParticleEmitter " << emitter->GetGenerators() << " "
                << emitter->GetPosition().x << " " << emitter->GetPosition().y << " "
                << emitter->GetPosition().z << " " << emitter->GetEmitRate() << std::endl;
        }

        for (auto& force : m_forces[m_currentState])
        {
            t_fileStream << "ForceApplicator " << force.m_center.x << " " << force.m_center.y
                << " " << force.m_center.z << " " << force.m_force.x << " " << force.m_force.y
                << " " << force.m_force.z << " " << force.m_radius << std::endl;
        }
    }

    void ParticleSystem::ResetForNextFile()
    {
        m_loadingGenerator.clear();
    }

    void ParticleSystem::Update(const float t_dT)
    {
        if (m_stateItr == m_container.end())
        {
            return;
        }

        for (auto& emitter : m_emitterItr->second)
        {
            emitter->Update(t_dT, m_stateItr->second.get());
        }

        for (auto& updater : m_updaters)
        {
            updater.second->Update(t_dT, m_stateItr->second.get());
        }

        if (m_removedEmitters.empty())
        {
            return;
        }

        for (auto& removed : m_removedEmitters)
        {
            auto itr = std::remove_if(m_emitterItr->second.begin(), m_emitterItr->second.end(),[removed](std::unique_ptr<Emitter>& emitter)
            {
                return emitter.get() == removed;
            });

            if (itr == m_emitterItr->second.end())
            {
                continue;
            }

            m_emitterItr->second.erase(itr);
        }

        m_removedEmitters.clear();
    }

    void ParticleSystem::ApplyForce(const sf::Vector3f& t_center, const sf::Vector3f& t_force, const float t_radius)
    {
        if (m_stateItr == m_container.end())
        {
            return;
        }

        const auto f = dynamic_cast<ForceUpdater*>(m_updaters["Force"].get());
        auto container = m_stateItr->second.get();
        auto& positions = container->m_position;
        auto& velocities = container->m_velocity;

        for (size_t i = 0; i < container->m_countAlive; ++i)
        {
            f->Force(t_center, t_force, t_radius, positions[i], velocities[i]);
        }
    }

    void ParticleSystem::Draw(Window* t_window, const int t_elevation)
    {
        if (m_stateItr == m_container.end())
        {
            return;
        }

        auto container = m_stateItr->second.get();
        auto& drawables = container->m_drawable;
        auto& positions = container->m_position;
        auto& blendModes = container->m_addBlend;
        auto view = t_window->GetRenderWindow()->getView();
        auto renderer = t_window->GetRenderer();

        const auto state = m_stateManager->GetCurrentStateType();
        if (state == StateType::Game || state == StateType::MapEditor)
        {
            renderer->UseShader("default");
        }
        else
        {
            renderer->DisableShader();
        }

        for (size_t i = 0; i < container->m_countAlive; ++i)
        {
            if (t_elevation >= 0)
            {
                if (positions[i].z < t_elevation * Sheet::Tile_Size)
                {
                    continue;
                }
                if (positions[i].z >= (t_elevation + 1) * Sheet::Tile_Size)
                {
                    continue;
                }
            }
            else if (positions[i].z < Sheet::Num_Layers * Sheet::Tile_Size)
            {
                continue;
            }
            renderer->AdditiveBlend(blendModes[i]);
            renderer->Draw(drawables[i]);
        }
        renderer->AdditiveBlend(false);
    }
}
