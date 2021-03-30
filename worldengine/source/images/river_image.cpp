#include "river_image.h"

namespace WorldEngine
{
RiverImage::RiverImage() : Image(false) {}
RiverImage::~RiverImage() {}

void RiverImage::DrawImage(const World&                      world,
                           boost::gil::rgb8_image_t::view_t& target) const
{
   static const boost::gil::rgb8_pixel_t oceanColor(255, 255, 255);
   static const boost::gil::rgb8_pixel_t landColor(0, 0, 0);

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         if (world.IsOcean(x, y))
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

void RiverImage::DrawRivers(const World&                      world,
                            boost::gil::rgb8_image_t::view_t& target,
                            uint32_t                          factor) const
{
   static const boost::gil::rgb8_pixel_t riverColor(0, 0, 128);
   static const boost::gil::rgb8_pixel_t lakeColor(0, 100, 128);

   const RiverMapArrayType& riverMap = world.GetRiverMapData();
   const LakeMapArrayType&  lakeMap  = world.GetLakeMapData();

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         if (world.IsLand(x, y) && riverMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < factor; dy++)
            {
               for (uint32_t dx = 0; dx < factor; dx++)
               {
                  target(x * factor + dx, y * factor + dy) = riverColor;
               }
            }
         }
         if (world.IsLand(x, y) && lakeMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < factor; dy++)
            {
               for (uint32_t dx = 0; dx < factor; dx++)
               {
                  target(x * factor + dx, y * factor + dy) = lakeColor;
               }
            }
         }
      }
   }
}

} // namespace WorldEngine
