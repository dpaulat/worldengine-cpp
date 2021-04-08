#include "worldengine/images/biome_image.h"

namespace WorldEngine
{

static std::unordered_map<Biome, boost::gil::rgb8_pixel_t> biomeColors_ = {
   {Biome::Ocean, {23, 94, 145}},
   {Biome::Sea, {23, 94, 145}},
   {Biome::Ice, {255, 255, 255}},
   {Biome::SubpolarDryTundra, {128, 128, 128}},
   {Biome::SubpolarMoistTundra, {96, 128, 128}},
   {Biome::SubpolarWetTundra, {64, 128, 128}},
   {Biome::SubpolarRainTundra, {32, 128, 192}},
   {Biome::PolarDesert, {192, 192, 192}},
   {Biome::BorealDesert, {160, 160, 128}},
   {Biome::CoolTemperateDesert, {192, 192, 128}},
   {Biome::WarmTemperateDesert, {224, 224, 128}},
   {Biome::SubtropicalDesert, {240, 240, 128}},
   {Biome::TropicalDesert, {255, 255, 128}},
   {Biome::BorealRainForest, {32, 160, 192}},
   {Biome::CoolTemperateRainForest, {32, 192, 192}},
   {Biome::WarmTemperateRainForest, {32, 224, 192}},
   {Biome::SubtropicalRainForest, {32, 240, 176}},
   {Biome::TropicalRainForest, {32, 255, 160}},
   {Biome::BorealWetForest, {64, 160, 144}},
   {Biome::CoolTemperateWetForest, {64, 192, 144}},
   {Biome::WarmTemperateWetForest, {64, 224, 144}},
   {Biome::SubtropicalWetForest, {64, 240, 144}},
   {Biome::TropicalWetForest, {64, 255, 144}},
   {Biome::BorealMoistForest, {96, 160, 128}},
   {Biome::CoolTemperateMoistForest, {96, 192, 128}},
   {Biome::WarmTemperateMoistForest, {96, 224, 128}},
   {Biome::SubtropicalMoistForest, {96, 240, 128}},
   {Biome::TropicalMoistForest, {96, 255, 128}},
   {Biome::WarmTemperateDryForest, {128, 224, 128}},
   {Biome::SubtropicalDryForest, {128, 240, 128}},
   {Biome::TropicalDryForest, {128, 255, 128}},
   {Biome::BorealDryScrub, {128, 160, 128}},
   {Biome::CoolTemperateDesertScrub, {160, 192, 128}},
   {Biome::WarmTemperateDesertScrub, {192, 224, 128}},
   {Biome::SubtropicalDesertScrub, {208, 240, 128}},
   {Biome::TropicalDesertScrub, {224, 255, 128}},
   {Biome::CoolTemperateSteppe, {128, 192, 128}},
   {Biome::WarmTemperateThornScrub, {160, 224, 128}},
   {Biome::SubtropicalThornWoodland, {176, 240, 128}},
   {Biome::TropicalThornWoodland, {192, 255, 128}},
   {Biome::TropicalVeryDryForest, {192, 255, 128}},
   {Biome::BareRock, {96, 96, 96}}};

BiomeImage::BiomeImage(const World& world) : Image(world, false) {}
BiomeImage::~BiomeImage() {}

void BiomeImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const BiomeArrayType& biomes = world_.GetBiomeData();

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