#pragma once

#include "AnimBase.h"
#include "../types/Directions.h"

namespace ragnarok
{
	/**
	 * \brief Base class to setup certain elements of the animations
	 */
    class AnimDirectional : public AnimBase
    {
        protected:
		/**
		 * \brief Prepare the next frame value if the model is moving
		 */
        void FrameStep();
		/**
		 * \brief Crop the sprite sheet for the right sprite
		 */
        void CropSprite();
		/**
		 * \brief Read frame information from stream
		 */
        void ReadIn(std::stringstream& t_stream);
    };
}