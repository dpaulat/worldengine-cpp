#include "biome.h"

#include <iomanip>
#include <map>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

void BiomeSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Biome simulation start";

   uint32_t width  = world.width();
   uint32_t height = world.height();

   BiomeArrayType& biome = world.GetBiomeData();
   biome.resize(boost::extents[height][width]);

   std::map<Biomes, uint32_t> biomeCounts;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (world.IsOcean(y, x))
         {
            biome[y][x] = Biomes::Ocean;
         }
         else
         {
            TemperatureType temperature   = world.GetTemperatureType(x, y);
            HumidityLevels  humidityLevel = world.GetHumidityLevel(x, y);

            switch (temperature)
            {
            case TemperatureType::Polar:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::PolarDesert;
                  break;
               default: //
                  biome[y][x] = Biomes::Ice;
                  break;
               }
               break;

            case TemperatureType::Alpine:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::SubpolarDryTundra;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::SubpolarMoistTundra;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::SubpolarWetTundra;
                  break;
               default: //
                  biome[y][x] = Biomes::SubpolarRainTundra;
                  break;
               }
               break;

            case TemperatureType::Boreal:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::BorealDesert;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::BorealDryScrub;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::BorealMoistForest;
                  break;
               case HumidityLevels::Semiarid:
                  biome[y][x] = Biomes::BorealWetForest;
                  break;
               default: //
                  biome[y][x] = Biomes::BorealRainForest;
                  break;
               }
               break;

            case TemperatureType::Cool:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::CoolTemperateDesert;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::CoolTemperateDesertScrub;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::CoolTemperateSteppe;
                  break;
               case HumidityLevels::Semiarid:
                  biome[y][x] = Biomes::CoolTemperateMoistForest;
                  break;
               case HumidityLevels::Subhumid:
                  biome[y][x] = Biomes::CoolTemperateWetForest;
                  break;
               default: //
                  biome[y][x] = Biomes::CoolTemperateRainForest;
                  break;
               }
               break;

            case TemperatureType::Warm:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::WarmTemperateDesert;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::WarmTemperateDesertScrub;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::WarmTemperateThornScrub;
                  break;
               case HumidityLevels::Semiarid:
                  biome[y][x] = Biomes::WarmTemperateDryForest;
                  break;
               case HumidityLevels::Subhumid:
                  biome[y][x] = Biomes::WarmTemperateMoistForest;
                  break;
               case HumidityLevels::Humid:
                  biome[y][x] = Biomes::WarmTemperateWetForest;
                  break;
               default: //
                  biome[y][x] = Biomes::WarmTemperateRainForest;
                  break;
               }
               break;

            case TemperatureType::Subtropical:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::SubtropicalDesert;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::SubtropicalDesertScrub;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::SubtropicalThornWoodland;
                  break;
               case HumidityLevels::Semiarid:
                  biome[y][x] = Biomes::SubtropicalDryForest;
                  break;
               case HumidityLevels::Subhumid:
                  biome[y][x] = Biomes::SubtropicalMoistForest;
                  break;
               case HumidityLevels::Humid:
                  biome[y][x] = Biomes::SubtropicalWetForest;
                  break;
               default: //
                  biome[y][x] = Biomes::SubtropicalRainForest;
                  break;
               }
               break;

            case TemperatureType::Tropical:
               switch (humidityLevel)
               {
               case HumidityLevels::Superarid:
                  biome[y][x] = Biomes::TropicalDesert;
                  break;
               case HumidityLevels::Perarid:
                  biome[y][x] = Biomes::TropicalDesertScrub;
                  break;
               case HumidityLevels::Arid:
                  biome[y][x] = Biomes::TropicalThornWoodland;
                  break;
               case HumidityLevels::Semiarid:
                  biome[y][x] = Biomes::TropicalVeryDryForest;
                  break;
               case HumidityLevels::Subhumid:
                  biome[y][x] = Biomes::TropicalDryForest;
                  break;
               case HumidityLevels::Humid:
                  biome[y][x] = Biomes::TropicalMoistForest;
                  break;
               case HumidityLevels::Perhumid:
                  biome[y][x] = Biomes::TropicalWetForest;
                  break;
               default: //
                  biome[y][x] = Biomes::TropicalRainForest;
                  break;
               }
               break;

            default: // Invalid
               biome[y][x] = Biomes::BareRock;
               break;
            }
         }

         biomeCounts[biome[y][x]]++;
      }
   }

   BOOST_LOG_TRIVIAL(debug) << "Biome obtained:";
   for (auto const& bc : biomeCounts)
   {
      BOOST_LOG_TRIVIAL(debug)
         << "  " << std::left << std::setw(30) << std::setfill(' ') << bc.first
         << ": " << std::right << std::setw(7) << std::setfill(' ')
         << bc.second;
   }

   BOOST_LOG_TRIVIAL(info) << "Biome simulation finish";
}

} // namespace WorldEngine
