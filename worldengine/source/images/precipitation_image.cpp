#include "precipitation_image.h"
#include "../basic.h"

namespace WorldEngine
{
PrecipitationImage::PrecipitationImage() : Image(true) {}
PrecipitationImage::~PrecipitationImage() {}

void PrecipitationImage::DrawImage(
   const World& world, boost::gil::gray8_image_t::view_t& target) const
{
   const PrecipitationArrayType& precipitation = world.GetPrecipitationData();

   auto minmax =
      std::minmax(precipitation.data(),
                  precipitation.data() + precipitation.num_elements());
   float    low     = *minmax.first;
   float    high    = *minmax.second;
   uint32_t floor   = 0;
   uint32_t ceiling = 255;

   const std::vector<std::pair<float, uint32_t>> points = {{low, floor},
                                                           {high, ceiling}};

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         uint32_t color = Interpolate(precipitation[y][x], points);
         target(x, y)   = boost::gil::gray8_pixel_t(color);
      }
   }
}

void PrecipitationImage::DrawImage(
   const World& world, boost::gil::rgba8_image_t::view_t& target) const
{
   if (!world.HasHumidity())
   {
      return;
   }

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         switch (world.GetHumidityLevel(x, y))
         {
         case HumidityLevels::Superarid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 32, 32, 255);
            break;

         case HumidityLevels::Perarid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 64, 64, 255);
            break;

         case HumidityLevels::Arid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 96, 96, 255);
            break;

         case HumidityLevels::Semiarid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 128, 128, 255);
            break;

         case HumidityLevels::Subhumid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 160, 160, 255);
            break;

         case HumidityLevels::Humid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 192, 192, 255);
            break;

         case HumidityLevels::Perhumid:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 224, 224, 255);
            break;

         case HumidityLevels::Superhumid:
         default:
            target(x, y) = boost::gil::rgba8_pixel_t(0, 255, 255, 255);
            break;
         }
      }
   }
}
} // namespace WorldEngine
