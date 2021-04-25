#include "worldengine/images/river_image.h"

namespace WorldEngine
{
RiverImage::RiverImage(const World& world) : Image(world, false) {}
RiverImage::~RiverImage() {}

void RiverImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   static const boost::gil::rgb8_pixel_t oceanColor(255, 255, 255);
   static const boost::gil::rgb8_pixel_t landColor(0, 0, 0);

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (world_.IsOcean(x, y))
         {
            target(x, y) = oceanColor;
         }
         else
         {
            target(x, y) = landColor;
         }
      }
   }

   DrawRivers(target);
}

} // namespace WorldEngine
