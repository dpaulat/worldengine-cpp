#include "biome_image.h"

namespace WorldEngine
{

static std::unordered_map<Biomes, boost::gil::rgba8_pixel_t> biomeColors_ = {
   {Biomes::Ocean, {23, 94, 145, 255}},
   {Biomes::Sea, {23, 94, 145, 255}},
   {Biomes::Ice, {255, 255, 255, 255}},
   {Biomes::SubpolarDryTundra, {128, 128, 128, 255}},
   {Biomes::SubpolarMoistTundra, {96, 128, 128, 255}},
   {Biomes::SubpolarWetTundra, {64, 128, 128, 255}},
   {Biomes::SubpolarRainTundra, {32, 128, 192, 255}},
   {Biomes::PolarDesert, {192, 192, 192, 255}},
   {Biomes::BorealDesert, {160, 160, 128, 255}},
   {Biomes::CoolTemperateDesert, {192, 192, 128, 255}},
   {Biomes::WarmTemperateDesert, {224, 224, 128, 255}},
   {Biomes::SubtropicalDesert, {240, 240, 128, 255}},
   {Biomes::TropicalDesert, {255, 255, 128, 255}},
   {Biomes::BorealRainForest, {32, 160, 192, 255}}, // TODO: Colors
   {Biomes::CoolTemperateRainForest, {32, 192, 192, 255}},
   {Biomes::WarmTemperateRainForest, {32, 224, 192, 255}},
   {Biomes::SubtropicalRainForest, {32, 240, 176, 255}},
   {Biomes::TropicalRainForest, {32, 255, 160, 255}},
   {Biomes::BorealWetForest, {64, 160, 144, 255}},
   {Biomes::CoolTemperateWetForest, {64, 192, 144, 255}},
   {Biomes::WarmTemperateWetForest, {64, 224, 144, 255}},
   {Biomes::SubtropicalWetForest, {64, 240, 144, 255}},
   {Biomes::TropicalWetForest, {64, 255, 144, 255}},
   {Biomes::BorealMoistForest, {96, 160, 128, 255}},
   {Biomes::CoolTemperateMoistForest, {96, 192, 128, 255}},
   {Biomes::WarmTemperateMoistForest, {96, 224, 128, 255}},
   {Biomes::SubtropicalMoistForest, {96, 240, 128, 255}},
   {Biomes::TropicalMoistForest, {96, 255, 128, 255}},
   {Biomes::WarmTemperateDryForest, {128, 224, 128, 255}},
   {Biomes::SubtropicalDryForest, {128, 240, 128, 255}},
   {Biomes::TropicalDryForest, {128, 255, 128, 255}},
   {Biomes::BorealDryScrub, {128, 160, 128, 255}},
   {Biomes::CoolTemperateDesertScrub, {160, 192, 128, 255}},
   {Biomes::WarmTemperateDesertScrub, {192, 224, 128, 255}},
   {Biomes::SubtropicalDesertScrub, {208, 240, 128, 255}},
   {Biomes::TropicalDesertScrub, {224, 255, 128, 255}},
   {Biomes::CoolTemperateSteppe, {128, 192, 128, 255}},
   {Biomes::WarmTemperateThornScrub, {160, 224, 128, 255}},
   {Biomes::SubtropicalThornWoodland, {176, 240, 128, 255}},
   {Biomes::TropicalThornWoodland, {192, 255, 128, 255}},
   {Biomes::TropicalVeryDryForest, {192, 255, 128, 255}},
   {Biomes::BareRock, {96, 96, 96, 255}}};

BiomeImage::BiomeImage() : Image(false) {}
BiomeImage::~BiomeImage() {}

void BiomeImage::DrawImage(const World&                       world,
                           boost::gil::rgba8_image_t::view_t& target) const
{
   const BiomeArrayType& biomes = world.GetBiomeData();

   const uint32_t width  = biomes.shape()[1];
   const uint32_t height = biomes.shape()[0];

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         target(x, y) = biomeColors_[biomes[y][x]];
      }
   }
}
} // namespace WorldEngine