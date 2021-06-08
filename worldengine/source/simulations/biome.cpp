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

   std::map<Biome, uint32_t> biomeCounts;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (world.IsOcean(x, y))
         {
            biome[y][x] = Biome::Ocean;
         }
         else
         {
            TemperatureLevel temperature   = world.GetTemperatureLevel(x, y);
            HumidityLevel    humidityLevel = world.GetHumidityLevel(x, y);

            switch (temperature)
            {
            case TemperatureLevel::Polar:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::PolarDesert;
                  break;
               default: //
                  biome[y][x] = Biome::Ice;
                  break;
               }
               break;

            case TemperatureLevel::Alpine:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::SubpolarDryTundra;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::SubpolarMoistTundra;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::SubpolarWetTundra;
                  break;
               default: //
                  biome[y][x] = Biome::SubpolarRainTundra;
                  break;
               }
               break;

            case TemperatureLevel::Boreal:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::BorealDesert;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::BorealDryScrub;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::BorealMoistForest;
                  break;
               case HumidityLevel::Semiarid:
                  biome[y][x] = Biome::BorealWetForest;
                  break;
               default: //
                  biome[y][x] = Biome::BorealRainForest;
                  break;
               }
               break;

            case TemperatureLevel::Cool:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::CoolTemperateDesert;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::CoolTemperateDesertScrub;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::CoolTemperateSteppe;
                  break;
               case HumidityLevel::Semiarid:
                  biome[y][x] = Biome::CoolTemperateMoistForest;
                  break;
               case HumidityLevel::Subhumid:
                  biome[y][x] = Biome::CoolTemperateWetForest;
                  break;
               default: //
                  biome[y][x] = Biome::CoolTemperateRainForest;
                  break;
               }
               break;

            case TemperatureLevel::Warm:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::WarmTemperateDesert;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::WarmTemperateDesertScrub;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::WarmTemperateThornScrub;
                  break;
               case HumidityLevel::Semiarid:
                  biome[y][x] = Biome::WarmTemperateDryForest;
                  break;
               case HumidityLevel::Subhumid:
                  biome[y][x] = Biome::WarmTemperateMoistForest;
                  break;
               case HumidityLevel::Humid:
                  biome[y][x] = Biome::WarmTemperateWetForest;
                  break;
               default: //
                  biome[y][x] = Biome::WarmTemperateRainForest;
                  break;
               }
               break;

            case TemperatureLevel::Subtropical:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::SubtropicalDesert;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::SubtropicalDesertScrub;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::SubtropicalThornWoodland;
                  break;
               case HumidityLevel::Semiarid:
                  biome[y][x] = Biome::SubtropicalDryForest;
                  break;
               case HumidityLevel::Subhumid:
                  biome[y][x] = Biome::SubtropicalMoistForest;
                  break;
               case HumidityLevel::Humid:
                  biome[y][x] = Biome::SubtropicalWetForest;
                  break;
               default: //
                  biome[y][x] = Biome::SubtropicalRainForest;
                  break;
               }
               break;

            case TemperatureLevel::Tropical:
               switch (humidityLevel)
               {
               case HumidityLevel::Superarid:
                  biome[y][x] = Biome::TropicalDesert;
                  break;
               case HumidityLevel::Perarid:
                  biome[y][x] = Biome::TropicalDesertScrub;
                  break;
               case HumidityLevel::Arid:
                  biome[y][x] = Biome::TropicalThornWoodland;
                  break;
               case HumidityLevel::Semiarid:
                  biome[y][x] = Biome::TropicalVeryDryForest;
                  break;
               case HumidityLevel::Subhumid:
                  biome[y][x] = Biome::TropicalDryForest;
                  break;
               case HumidityLevel::Humid:
                  biome[y][x] = Biome::TropicalMoistForest;
                  break;
               case HumidityLevel::Perhumid:
                  biome[y][x] = Biome::TropicalWetForest;
                  break;
               default: //
                  biome[y][x] = Biome::TropicalRainForest;
                  break;
               }
               break;

            default: // Invalid
               biome[y][x] = Biome::BareRock;
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
