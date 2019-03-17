#pragma once

#include "../core/S_Base.h"

namespace ragnarok
{
    /**
     * \brief Sheet animation system
     */
    class S_SheetAnimation : public S_Base
    {
        public:
        S_SheetAnimation(SystemManager* t_systemMgr);
        ~S_SheetAnimation() = default;

        void Update(float t_dT) override;
        void HandleEvent(const EntityId& t_entity, const EntityEvent& t_event) override;
        void Notify(const Message& t_message) override;

        private:
        /**
         * \brief Changes the current animation
         * \param t_entity the entity concerned
         * \param t_anim the animation name
         * \param t_play play the animation
         * \param t_loop loop the animation
         */
        void ChangeAnimation(const EntityId& t_entity, const std::string& t_anim, bool t_play, bool t_loop) const;
    };
}