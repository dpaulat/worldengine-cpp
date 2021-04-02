#include "precipitation_image.h"
#include "../basic.h"

namespace WorldEngine
{
PrecipitationImage::PrecipitationImage() : Image(true) {}
PrecipitationImage::~PrecipitationImage() {}

void PrecipitationImage::DrawImage(
   const World& world, boost::gil::gray8_image_t::view_t& target) const
{
   DrawGrayscaleFromArray(world.GetPrecipitationData(), target);
}

void PrecipitationImage::DrawImage(
   const World& world, boost::gil::rgb8_image_t::view_t& target) const
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
            target(x, y) = boost::gil::rgb8_pixel_t(0, 32, 32);
            break;

         case HumidityLevels::Perarid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 64, 64);
            break;

         case HumidityLevels::Arid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 96, 96);
            break;

         case HumidityLevels::Semiarid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 128, 128);
            break;

         case HumidityLevels::Subhumid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 160, 160);
            break;

         case HumidityLevels::Humid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 192, 192);
            break;

         case HumidityLevels::Perhumid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 224, 224);
            break;

         case HumidityLevels::Superhumid:
         default: //
            target(x, y) = boost::gil::rgb8_pixel_t(0, 255, 255);
            break;
         }
      }
   }
}
} // namespace WorldEngine
