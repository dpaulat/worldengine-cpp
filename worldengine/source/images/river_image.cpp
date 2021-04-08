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

void RiverImage::DrawRivers(boost::gil::rgb8_image_t::view_t& target,
                            uint32_t                          factor) const
{
   static const boost::gil::rgb8_pixel_t riverColor(0, 0, 128);
   static const boost::gil::rgb8_pixel_t lakeColor(0, 100, 128);

   const RiverMapArrayType& riverMap = world_.GetRiverMapData();
   const LakeMapArrayType&  lakeMap  = world_.GetLakeMapData();

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (world_.IsLand(x, y) && riverMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < factor; dy++)
            {
               for (uint32_t dx = 0; dx < factor; dx++)
               {
                  target(x * factor + dx, y * factor + dy) = riverColor;
               }
            }
         }
         if (world_.IsLand(x, y) && lakeMap[y][x] > 0.0f)
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
