#pragma once

#include "../core/S_Base.h"
#include "../components/C_Position.h"
#include "../components/C_Collidable.h"

namespace ragnarok
{
    /**
     * \brief Dead entity remover system
     */
    class S_DeadRemover : public S_Base
    {
    public:
        explicit S_DeadRemover(SystemManager* t_systemMgr);
        ~S_DeadRemover() override = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

    private:
        bool m_msgSent;
        std::vector<unsigned int> m_dead;
    };
}