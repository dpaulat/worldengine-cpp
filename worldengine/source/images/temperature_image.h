#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Temperature image
 */
class TemperatureImage : public Image
{
public:
   TemperatureImage();
   ~TemperatureImage();

protected:
   /**
    * @brief Draw a grayscale temperature image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                       world,
                  boost::gil::gray8_image_t::view_t& target);

   /**
    * @brief Draw a temperature image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                      world,
                  boost::gil::rgb8_image_t::view_t& target);
};
} // namespace WorldEngine
