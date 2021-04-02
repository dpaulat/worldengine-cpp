#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Heightmap image
 */
class HeightmapImage : public Image
{
public:
   HeightmapImage();
   ~HeightmapImage();

protected:
   /**
    * @brief Draw a grayscale heightmap image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                       world,
                  boost::gil::gray8_image_t::view_t& target) const;
};
} // namespace WorldEngine
