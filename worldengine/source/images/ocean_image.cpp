#include "worldengine/images/ocean_image.h"

namespace WorldEngine
{
OceanImage::OceanImage(const World& world) : Image(world, false) {}
OceanImage::~OceanImage() {}

void OceanImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   static const boost::gil::rgb8_pixel_t oceanColor(0, 0, 255);
   static const boost::gil::rgb8_pixel_t landColor(0, 255, 255);

   const OceanArrayType& ocean = world_.GetOceanData();

   const uint32_t width  = static_cast<uint32_t>(ocean.shape()[1]);
   const uint32_t height = static_cast<uint32_t>(ocean.shape()[0]);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (ocean[y][x])
         {
            target(x, y) = oceanColor;
         }
         else
         {
            target(x, y) = landColor;
         }
      }
   }
}
} // namespace WorldEngine