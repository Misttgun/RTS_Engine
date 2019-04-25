#pragma once

#include <memory>
#include <functional>
#include "ParticleContainer.h"
#include "BaseUpdater.h"
#include "BaseGenerator.h"
#include "Emitter.h"
#include "ForceApplicator.h"
#include "../window/Window.h"
#include "../utilities/RandomGenerator.h"
#include "../states/StateTypes.h"
#include "../states/StateDependent.h"
#include "../threads/FileLoader.h"
#include "../map/MapLoadee.h"

namespace ragnarok
{
    using Updaters = std::unordered_map<std::string, std::unique_ptr<BaseUpdater>>;
    using EmitterList = std::vector<std::unique_ptr<Emitter>>;
    using Emitters = std::unordered_map<StateType, EmitterList>;
    using GeneratorList = std::vector<std::unique_ptr<BaseGenerator>>;
    using Generators = std::unordered_map < std::string, GeneratorList >;
    using RemovedEmitters = std::vector<Emitter*>;
    using Particles = std::unordered_map<StateType, std::unique_ptr<ParticleContainer>>;
    using ForceApplicatorList = std::vector<ForceApplicator>;
    using ForceApplicators = std::unordered_map<StateType, ForceApplicatorList>;

    using GeneratorFactory = std::unordered_map<std::string, std::function<BaseGenerator*(void)>>;

    class Map;

    class ParticleSystem : public FileLoader, public StateDependent, public MapLoadee
    {
    public:
        ParticleSystem(StateManager* t_stateManager, TextureManager* t_textureMgr, RandomGenerator* t_rand, Map* t_map);
        Emitter* AddEmitter(std::unique_ptr<Emitter> t_emitter, const StateType& t_state = StateType(0));
        Emitter* AddEmitter(const sf::Vector3f& t_position, std::string& t_generators, unsigned int t_emitRate, const StateType& t_state = StateType(0));
        void AddForce(ForceApplicator t_force, const StateType& t_state = StateType(0));
        void RemoveEmitter(Emitter* t_emitter);

        Emitter* FindEmitter(const sf::Vector2f& t_point, const sf::Vector2f& t_area, unsigned int t_layerMin = 0, unsigned int t_layerMax = -1);

        GeneratorList* GetGenerators(const std::string& t_name);

        TextureManager* GetTextureManager() const;
        RandomGenerator* GetRand() const;

        void CreateState(const StateType& t_state) override;
        void ChangeState(const StateType& t_state) override;
        void RemoveState(const StateType& t_state) override;

        void PurgeCurrentState() const;

        void ReadMapLine(const std::string& t_type, std::stringstream& t_stream) override;
        void SaveMap(std::ofstream& t_fileStream) override;

        void Update(float t_dT);
        void ApplyForce(const sf::Vector3f& t_center, const sf::Vector3f& t_force, float t_radius);
        void Draw(Window* t_window, int t_elevation);

    private:
        bool ProcessLine(std::stringstream& t_stream) override;
        void ResetForNextFile() override;

        template<class T>
        void RegisterGenerator(const std::string& t_name)
        {
            m_factory[t_name] = []() -> BaseGenerator*
            {
                return new T();
            };
        }

        std::string m_loadingGenerator;
        Particles m_container;
        Particles::iterator m_stateItr;
        Emitters::iterator m_emitterItr;
        Updaters m_updaters;
        Emitters m_emitters;
        Generators m_generators;
        GeneratorFactory m_factory;
        ForceApplicators m_forces;
        RemovedEmitters m_removedEmitters;
        TextureManager* m_textureMgr;
        StateManager* m_stateManager;
        RandomGenerator* m_rand;
        Map* m_map;
    };
}