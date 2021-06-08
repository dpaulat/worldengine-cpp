#include "worldengine/images/temperature_image.h"
#include "../basic.h"

namespace WorldEngine
{
TemperatureImage::TemperatureImage(const World& world) : Image(world, true) {}
TemperatureImage::~TemperatureImage() {}

void TemperatureImage::DrawImage(boost::gil::gray8_image_t::view_t& target)
{
   float low  = world_.GetThreshold(TemperatureLevel::Polar);
   float high = world_.GetThreshold(TemperatureLevel::Subtropical);

   DrawGrayscaleFromArray(world_.GetTemperatureData(), low, high, target);
}

void TemperatureImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         switch (world_.GetTemperatureLevel(x, y))
         {
         case TemperatureLevel::Polar:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 0, 255);
            break;

         case TemperatureLevel::Alpine:
            target(x, y) = boost::gil::rgb8_pixel_t(42, 0, 213);
            break;

         case TemperatureLevel::Boreal:
            target(x, y) = boost::gil::rgb8_pixel_t(85, 0, 170);
            break;

         case TemperatureLevel::Cool:
            target(x, y) = boost::gil::rgb8_pixel_t(128, 0, 128);
            break;

         case TemperatureLevel::Warm:
            target(x, y) = boost::gil::rgb8_pixel_t(170, 0, 85);
            break;

         case TemperatureLevel::Subtropical:
            target(x, y) = boost::gil::rgb8_pixel_t(213, 0, 42);
            break;

         case TemperatureLevel::Tropical:
         default: //
            target(x, y) = boost::gil::rgb8_pixel_t(255, 0, 0);
            break;
         }
      }
   }
}
} // namespace WorldEngine
