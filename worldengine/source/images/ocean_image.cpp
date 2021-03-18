#include "ocean_image.h"

namespace WorldEngine
{
OceanImage::OceanImage() : Image(false) {}
OceanImage::~OceanImage() {}

void OceanImage::DrawImage(const World&                       world,
                           boost::gil::rgba8_image_t::view_t& target) const
{
   static const boost::gil::rgba8_pixel_t oceanColor(0, 0, 255, 255);
   static const boost::gil::rgba8_pixel_t landColor(0, 255, 255, 255);

   const OceanArrayType& ocean = world.GetOceanData();

   uint32_t width  = ocean.shape()[1];
   uint32_t height = ocean.shape()[0];

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