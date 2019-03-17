#pragma once

#include "../core/S_Base.h"
#include "../../types/Directions.h"

namespace ragnarok
{
    /**
     * \brief Control system
     */
    class S_Control : public S_Base
    {
        public:
        S_Control(SystemManager* t_systemMgr);
        ~S_Control() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        private:
        void MoveEntity(const EntityId& t_entity, const Direction& t_dir) const;
    };
}