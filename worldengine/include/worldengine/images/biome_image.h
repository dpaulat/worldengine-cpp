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
   explicit BiomeImage(const World& world);
   ~BiomeImage();

protected:
   /**
    * @brief Draw a biome image
    * @param target Destination filename
    */
   virtual void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

   /**
    * @brief Get the color of a biome
    * @param biome A biome
    * @return The corresponding color
   */
   static boost::gil::rgb8_pixel_t BiomeColor(Biome biome);
};
} // namespace WorldEngine
