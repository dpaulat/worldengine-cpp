#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Biome image
*/
class BiomeImage : public Image
{
public:
   BiomeImage();
   ~BiomeImage();

protected:
   /**
    * @brief Draw a biome image
    * @param world A world
    * @param target Destination filename
   */
   void DrawImage(const World&                       world,
                  boost::gil::rgba8_image_t::view_t& target) const;
};
} // namespace WorldEngine
