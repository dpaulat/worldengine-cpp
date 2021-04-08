#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Ocean image
 */
class OceanImage : public Image
{
public:
   explicit OceanImage(const World& world);
   ~OceanImage();

protected:
   /**
    * @brief Draw an ocean image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;
};
} // namespace WorldEngine
