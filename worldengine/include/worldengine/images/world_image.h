#pragma once

#include "biome_image.h"

namespace WorldEngine
{
/**
 * @brief World image
 */
class WorldImage : public BiomeImage
{
public:
   explicit WorldImage(const World& world);
   ~WorldImage();

protected:
   /**
    * @brief Draw a world image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;
};
} // namespace WorldEngine
