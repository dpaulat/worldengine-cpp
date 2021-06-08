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
   explicit HeightmapImage(const World& world);
   ~HeightmapImage();

protected:
   /**
    * @brief Draw a grayscale heightmap image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::gray8_image_t::view_t& target) override;
};
} // namespace WorldEngine
