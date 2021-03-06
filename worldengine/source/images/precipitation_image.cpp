#include "worldengine/images/precipitation_image.h"
#include "../basic.h"

namespace WorldEngine
{
PrecipitationImage::PrecipitationImage(const World& world) : Image(world, true)
{
}
PrecipitationImage::~PrecipitationImage() {}

void PrecipitationImage::DrawImage(boost::gil::gray8_image_t::view_t& target)
{
   DrawGrayscaleFromArray(world_.GetPrecipitationData(), target);
}

void PrecipitationImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   if (!world_.HasHumidity())
   {
      return;
   }

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         switch (world_.GetHumidityLevel(x, y))
         {
         case HumidityLevel::Superarid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 32, 32);
            break;

         case HumidityLevel::Perarid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 64, 64);
            break;

         case HumidityLevel::Arid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 96, 96);
            break;

         case HumidityLevel::Semiarid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 128, 128);
            break;

         case HumidityLevel::Subhumid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 160, 160);
            break;

         case HumidityLevel::Humid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 192, 192);
            break;

         case HumidityLevel::Perhumid:
            target(x, y) = boost::gil::rgb8_pixel_t(0, 224, 224);
            break;

         case HumidityLevel::Superhumid:
         default: //
            target(x, y) = boost::gil::rgb8_pixel_t(0, 255, 255);
            break;
         }
      }
   }
}
} // namespace WorldEngine
