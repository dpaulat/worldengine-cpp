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
   explicit TemperatureImage(const World& world);
   ~TemperatureImage();

protected:
   /**
    * @brief Draw a grayscale temperature image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::gray8_image_t::view_t& target) override;

   /**
    * @brief Draw a temperature image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;
};
} // namespace WorldEngine
