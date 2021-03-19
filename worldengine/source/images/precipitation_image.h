#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Precipitation image
 */
class PrecipitationImage : public Image
{
public:
   PrecipitationImage();
   ~PrecipitationImage();

protected:
   /**
    * @brief Draw a grayscale precipitation image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                       world,
                  boost::gil::gray8_image_t::view_t& target) const;

   /**
    * @brief Draw a precipitation image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                       world,
                  boost::gil::rgba8_image_t::view_t& target) const;
};
} // namespace WorldEngine
