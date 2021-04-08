#include "icecap.h"
#include "../basic.h"

#include <random>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

typedef bool                                 SolidDataType;
typedef boost::multi_array<SolidDataType, 2> SolidArrayType;

// Primary constants, all values should be in [0, 1]

// Only the coldest % of cold area will freeze (0 = no ice, 1 = all ice)
static const float MAX_FREEZE_PERCENTAGE = 0.6f;

// The warmest % of freezable area won't completely freeze
static const float FREEZE_CHANCE_WINDOW = 0.2f;

// Chance modifier to freeze a slightly warm tile when neighbors are frozen
static const float SURROUNDING_TILE_INFLUENCE = 0.5f;

static const uint32_t NUM_SURROUNDING_TILES = 8u;

void IcecapSimulation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(info) << "Icecap simulation start";

   std::default_random_engine            generator(seed);
   std::uniform_real_distribution<float> distribution(0.0f, 1.0f);

   const uint32_t width  = world.width();
   const uint32_t height = world.height();

   const OceanArrayType&       ocean       = world.GetOceanData();
   const TemperatureArrayType& temperature = world.GetTemperatureData();

   // Secondary constants
   // Coldest spot in the world
   const float minTemp = *std::min_element(
      temperature.data(), temperature.data() + temperature.num_elements());

   // Upper temperature-limit for freezing effects
   const float freezeLimit = world.GetThreshold(TemperatureLevel::Polar);

   // Derived constants
   // Freeze threshold above minimum
   const float freezeThreshold =
      (freezeLimit - minTemp) * MAX_FREEZE_PERCENTAGE;
   const float freezeChanceThreshold =
      freezeThreshold * (1.0f - FREEZE_CHANCE_WINDOW);

   IcecapArrayType& icecap = world.GetIcecapData();
   icecap.resize(boost::extents[height][width]);

   // Map that is true whenever there is land or (certain) ice around
   SolidArrayType solidMap(boost::extents[height][width]);
   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         solidMap[y][x] = !ocean[y][x] || (temperature[y][x] <=
                                           freezeChanceThreshold + minTemp);
      }
   }

   // Will freeze: [minTemp, freezeChanceThreshold]
   // Can freeze:  (freezeChanceThreshold, freezeThreshold)
   static const std::vector<std::pair<float, float>> freezePoints(
      {{minTemp, 1.0f},
       {freezeChanceThreshold, 1.0f},
       {freezeThreshold, 0.0f}});
   static const std::vector<std::pair<uint32_t, float>> chancePoints(
      {{0, -1.0f}, {NUM_SURROUNDING_TILES, 1.0f}});

   for (int32_t y = 0; y < height; y++)
   {
      for (int32_t x = 0; x < width; x++)
      {
         if (world.IsOcean(x, y))
         {
            float t = temperature[y][x];

            if (t - minTemp < freezeThreshold)
            {
               // Map temperature to freeze-chance (linear interpolation)
               float chance = Interpolate(t, freezePoints);

               // Count number of frozen/solid tiles around this one
               if (0 < x && x < width - 1 && 0 < y &&
                   y < height - 1) // Exclude borders
               {
                  // Count number of frozen/solid tiles around this one
                  uint32_t frozenTiles = 0;
                  for (int32_t ny = y - 1; ny <= y + 1; ny++)
                  {
                     for (int32_t nx = x - 1; nx <= x + 1; nx++)
                     {
                        if ((nx != x || ny != y) && solidMap[ny][nx])
                        {
                           frozenTiles++;
                        }
                     }
                  }

                  // Map number of tiles to chance-modifier
                  float chanceMod = Interpolate(frozenTiles, chancePoints);
                  chance += chanceMod * SURROUNDING_TILE_INFLUENCE;
               }

               if (distribution(generator) <= chance)
               {
                  solidMap[y][x] = true; // Mark tile as frozen
                  icecap[y][x] =
                     freezeThreshold - (t - minTemp); // Ice thickness
               }
            }
         }
      }
   }

   BOOST_LOG_TRIVIAL(info) << "Icecap simulation finish";
}

} // namespace WorldEngine
