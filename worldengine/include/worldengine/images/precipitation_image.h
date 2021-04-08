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
   explicit PrecipitationImage(const World& world);
   ~PrecipitationImage();

protected:
   /**
    * @brief Draw a grayscale precipitation image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::gray8_image_t::view_t& target) override;

   /**
    * @brief Draw a precipitation image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;
};
} // namespace WorldEngine
