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
   OceanImage();
   ~OceanImage();

protected:
   /**
    * @brief Draw an ocean image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                      world,
                  boost::gil::rgb8_image_t::view_t& target) const;
};
} // namespace WorldEngine
