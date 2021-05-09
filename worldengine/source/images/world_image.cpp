#include "worldengine/images/world_image.h"

namespace WorldEngine
{

WorldImage::WorldImage(const World& world) : BiomeImage(world) {}
WorldImage::~WorldImage() {}

void WorldImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const BiomeArrayType&    biomes   = world_.GetBiomeData();
   const SeaDepthArrayType& seaDepth = world_.GetSeaDepthData();

   const uint32_t width  = biomes.shape()[1];
   const uint32_t height = biomes.shape()[0];

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (world_.IsLand(x, y))
         {
            target(x, y) = BiomeColor(biomes[y][x]);
         }
         else
         {
            uint8_t c    = std::clamp<int>(seaDepth[y][x] * 200 + 50, 0, 255);
            target(x, y) = boost::gil::rgb8_pixel_t(0, 0, 255 - c);
         }
      }
   }
}
} // namespace WorldEngine