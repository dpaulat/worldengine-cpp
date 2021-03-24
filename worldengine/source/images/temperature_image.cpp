#include "temperature_image.h"
#include "../basic.h"

namespace WorldEngine
{
TemperatureImage::TemperatureImage() : Image(true) {}
TemperatureImage::~TemperatureImage() {}

void TemperatureImage::DrawImage(
   const World& world, boost::gil::gray8_image_t::view_t& target) const
{
   float    low     = world.GetThreshold(TemperatureType::Polar);
   float    high    = world.GetThreshold(TemperatureType::Subtropical);
   uint32_t floor   = 0;
   uint32_t ceiling = 255;

   const std::vector<std::pair<float, uint32_t>> points = {{low, floor},
                                                           {high, ceiling}};

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         uint32_t color = Interpolate(world.GetTemperatureData()[y][x], points);
         target(x, y)   = boost::gil::gray8_pixel_t(color);
      }
   }
}

void TemperatureImage::DrawImage(const World&                      world,
                                 boost::gil::rgb8_image_t::view_t& target) const
{
   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         switch (world.GetTemperatureType(x, y))
         {
         case TemperatureType::Polar:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 0, 255);
            break;

         case TemperatureType::Alpine:
            target(x, y) = boost::gil::rgb8_pixel_t(42, 0, 213);
            break;

         case TemperatureType::Boreal:
            target(x, y) = boost::gil::rgb8_pixel_t(85, 0, 170);
            break;

         case TemperatureType::Cool:
            target(x, y) = boost::gil::rgb8_pixel_t(128, 0, 128);
            break;

         case TemperatureType::Warm:
            target(x, y) = boost::gil::rgb8_pixel_t(170, 0, 85);
            break;

         case TemperatureType::Subtropical:
            target(x, y) = boost::gil::rgb8_pixel_t(213, 0, 42);
            break;

         case TemperatureType::Tropical:
         default: //
            target(x, y) = boost::gil::rgb8_pixel_t(255, 0, 0);
            break;
         }
      }
   }
}
} // namespace WorldEngine
