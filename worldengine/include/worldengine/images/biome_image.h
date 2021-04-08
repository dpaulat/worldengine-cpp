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
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;
};
} // namespace WorldEngine
