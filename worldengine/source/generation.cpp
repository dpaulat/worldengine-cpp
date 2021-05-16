#include "worldengine/generation.h"
#include "basic.h"
#include "simulations/biome.h"
#include "simulations/erosion.h"
#include "simulations/humidity.h"
#include "simulations/hydrology.h"
#include "simulations/icecap.h"
#include "simulations/irrigation.h"
#include "simulations/permeability.h"
#include "simulations/precipitation.h"
#include "simulations/temperature.h"

#include <queue>
#include <random>

#include <boost/log/trivial.hpp>
#include <boost/random.hpp>

#include <OpenSimplexNoise.h>

namespace WorldEngine
{

typedef std::pair<uint32_t, uint32_t> CoordType;

static void Around(std::vector<CoordType>& coordList,
                   int32_t                 x,
                   int32_t                 y,
                   int32_t                 width,
                   int32_t                 height);

/**
 * @brief Fill the ocean from the borders of the map
 * @param ocean Ocean data
 * @param elevation Elevation data
 * @param seaLevel The elevation representing the sea level
 */
static void FillOcean(OceanArrayType&           ocean,
                      const ElevationArrayType& elevation,
                      float                     seaLevel);

/**
 * @brief Make the ocean floor less noisy. The underwater erosion should cause
 * the ocean floor to be more uniform.
 * @param ocean Ocean data
 * @param elevation Elevation data
 * @param oceanLevel The elevation representing the ocean level
 */
static void HarmonizeOcean(const OceanArrayType& ocean,
                           ElevationArrayType&   elevation,
                           float                 oceanLevel);

/**
 * @brief A dynamic programming approach to gather how far the next land is from
 * a given coordinate up to a maximum distance of max_radius. Result is 0 for
 * land coordinates and -1 for coordinates further than max_radius away from
 * land.
 * @param ocean
 * @param maxRadius
 */
static boost::multi_array<int32_t, 2>
NextLandDynamic(const OceanArrayType& ocean, int32_t maxRadius = 5);

void AddNoiseToElevation(World& world, uint32_t seed)
{
   uint32_t octaves = 8;
   double   freq    = 16.0 * octaves;

   ElevationArrayType& elevation = world.GetElevationData();

   OpenSimplexNoise::Noise noise(seed);

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         double n = Noise(noise, x / freq * 2.0, y / freq * 2.0, octaves);
         elevation[y][x] += static_cast<float>(n);
      }
   }
}

void CenterLand(World& world)
{
   ElevationArrayType& elevation = world.GetElevationData();
   PlateArrayType&     plates    = world.GetPlateData();

   std::vector<ElevationDataType> rowSums;
   std::vector<ElevationDataType> colSums;

   // Find row with the lowest elevation
   for (uint32_t y = 0; y < world.height(); y++)
   {
      ElevationDataType sum =
         std::accumulate(elevation[y].begin(), elevation[y].end(), 0.0f);
      rowSums.push_back(sum);
   }
   int32_t yWithMinSum = static_cast<int32_t>(
      std::min_element(rowSums.cbegin(), rowSums.cend()) - rowSums.cbegin());
   BOOST_LOG_TRIVIAL(debug)
      << "CenterLand(): Height complete (min y = " << yWithMinSum << ")";

   // Find column with the lowest elevation
   for (uint32_t x = 0; x < world.width(); x++)
   {
      ElevationArrayType::array_view<1>::type colView =
         elevation[boost::indices[ElevationArrayType::index_range()][x]];
      ElevationDataType sum =
         std::accumulate(colView.begin(), colView.end(), 0.0f);
      colSums.push_back(sum);
   }
   int32_t xWithMinSum = static_cast<int32_t>(
      std::min_element(colSums.cbegin(), colSums.cend()) - colSums.cbegin());
   BOOST_LOG_TRIVIAL(debug)
      << "CenterLand(): Width complete (min x = " << xWithMinSum << ")";

   int32_t latShift = 0;
   int32_t xOffset  = xWithMinSum;
   int32_t yOffset  = yWithMinSum - latShift;
   if (yOffset < 0)
      yOffset += world.height();

   // Rotate so the column with the lowest elevation is at the left edge
   for (uint32_t y = 0; y < world.height(); y++)
   {
      std::rotate(elevation[y].begin(),
                  elevation[y].begin() + xOffset,
                  elevation[y].end());
      std::rotate(
         plates[y].begin(), plates[y].begin() + xOffset, plates[y].end());
   }

   // Rotate so the row with the lowest elevation is at the top
   for (uint32_t x = 0; x < world.width(); x++)
   {
      ElevationArrayType::array_view<1>::type elevationColView =
         elevation[boost::indices[ElevationArrayType::index_range()][x]];
      PlateArrayType::array_view<1>::type platesColView =
         plates[boost::indices[ElevationArrayType::index_range()][x]];

      std::rotate(elevationColView.begin(),
                  elevationColView.begin() + yOffset,
                  elevationColView.end());
      std::rotate(platesColView.begin(),
                  platesColView.begin() + yOffset,
                  platesColView.end());
   }

   BOOST_LOG_TRIVIAL(debug) << "CenterLand(): Rotate complete";
}

void GenerateWorld(World& world, const Step& step, uint32_t seed)
{
   if (!step.includePrecipitations_)
   {
      return;
   }

   std::mt19937                                      generator(seed);
   boost::random::uniform_int_distribution<uint32_t> distribution(0,
                                                                  UINT32_MAX);

   // Seed map should be appended to to maximize compatibility between versions
   std::unordered_map<Simulation, uint32_t> seedMap;
   seedMap.insert({Simulation::Precipitation, distribution(generator)});
   seedMap.insert({Simulation::Erosion, distribution(generator)});
   seedMap.insert({Simulation::Watermap, distribution(generator)});
   seedMap.insert({Simulation::Irrigation, distribution(generator)});
   seedMap.insert({Simulation::Temperature, distribution(generator)});
   seedMap.insert({Simulation::Humidity, distribution(generator)});
   seedMap.insert({Simulation::Permeability, distribution(generator)});
   seedMap.insert({Simulation::Biome, distribution(generator)});
   seedMap.insert({Simulation::Icecap, distribution(generator)});

   TemperatureSimulation(world, seedMap[Simulation::Temperature]);
   PrecipitationSimulation(world, seedMap[Simulation::Precipitation]);

   if (!step.includeErosion_)
   {
      return;
   }

   ErosionSimulation(world);
   WatermapSimulation(world, seedMap[Simulation::Watermap]);
   IrrigationSimulation(world);
   HumiditySimulation(world);
   PermeabilitySimulation(world, seedMap[Simulation::Permeability]);
   BiomeSimulation(world);
   IcecapSimulation(world, seedMap[Simulation::Icecap]);
}

void InitializeOceanAndThresholds(World& world, float oceanLevel)
{
   ElevationArrayType& e     = world.GetElevationData();
   OceanArrayType&     ocean = world.GetOceanData();

   FillOcean(ocean, e, oceanLevel);

   float hl = FindThresholdF(e, 0.10f); // Highest 10% of land is hills
   float ml = FindThresholdF(e, 0.03f); // Highest 3% of land is mountains
   world.SetThreshold(ElevationThreshold::Sea, oceanLevel);
   world.SetThreshold(ElevationThreshold::Hill, hl);
   world.SetThreshold(ElevationThreshold::Mountain, ml);

   HarmonizeOcean(ocean, e, oceanLevel);

   SeaDepth(world, oceanLevel);
}

void PlaceOceansAtMapBorders(World& world)
{
   // Lower the elevation near the border of the map
   uint32_t oceanBorder =
      std::min(30u, std::max(world.width() / 5, world.height() / 5));
   ElevationArrayType& elevation = world.GetElevationData();

   auto PlaceOcean = [&](uint32_t x, uint32_t y, uint32_t i) {
      elevation[y][x] = elevation[y][x] * i / oceanBorder;
   };

   for (uint32_t x = 0; x < world.width(); x++)
   {
      for (uint32_t i = 0; i < oceanBorder; i++)
      {
         PlaceOcean(x, i, i);
         PlaceOcean(x, world.height() - i - 1, i);
      }
   }

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t i = 0; i < oceanBorder; i++)
      {
         PlaceOcean(i, y, i);
         PlaceOcean(world.width() - i - 1, y, i);
      }
   }
}

static void Around(std::vector<CoordType>& coordList,
                   int32_t                 x,
                   int32_t                 y,
                   int32_t                 width,
                   int32_t                 height)
{
   for (int32_t dy = -1; dy <= 1; dy++)
   {
      int32_t ny = y + dy;
      if (0 <= ny && ny < height)
      {
         for (int32_t dx = -1; dx <= 1; dx++)
         {
            int32_t nx = x + dx;
            if (0 <= nx && nx < width && (dx != 0 || dy != 0))
            {
               coordList.push_back(CoordType(nx, ny));
            }
         }
      }
   }
}

static void FillOcean(OceanArrayType&           ocean,
                      const ElevationArrayType& elevation,
                      float                     seaLevel)
{
   const uint32_t height = static_cast<uint32_t>(elevation.shape()[0]);
   const uint32_t width  = static_cast<uint32_t>(elevation.shape()[1]);

   ocean.resize(boost::extents[height][width]);

   std::queue<CoordType> toExpand;

   // Handle top and bottom border of the map
   for (uint32_t x = 0; x < width; x++)
   {
      if (elevation[0][x] <= seaLevel)
      {
         toExpand.push(CoordType(x, 0));
      }
      if (elevation[height - 1][x] <= seaLevel)
      {
         toExpand.push(CoordType(x, height - 1));
      }
   }

   // Handle left- and rightmost border of the map
   for (uint32_t y = 0; y < height; y++)
   {
      if (elevation[y][0] <= seaLevel)
      {
         toExpand.push(CoordType(0, y));
      }
      if (elevation[y][width - 1] <= seaLevel)
      {
         toExpand.push(CoordType(width - 1, y));
      }
   }

   while (toExpand.size() > 0)
   {
      const CoordType& coord = toExpand.front();
      uint32_t         x     = coord.first;
      uint32_t         y     = coord.second;
      toExpand.pop();

      if (!ocean[y][x])
      {
         std::vector<CoordType> around;
         ocean[y][x] = true;

         Around(around, x, y, width, height);

         for (const CoordType& p : around)
         {
            int px = p.first;
            int py = p.second;
            if (!ocean[py][px] && elevation[py][px] <= seaLevel)
            {
               toExpand.push(p);
            }
         }
      }
   }
}

static void HarmonizeOcean(const OceanArrayType& ocean,
                           ElevationArrayType&   elevation,
                           float                 oceanLevel)
{
   const uint32_t width  = static_cast<uint32_t>(ocean.shape()[1]);
   const uint32_t height = static_cast<uint32_t>(ocean.shape()[0]);

   float shallowSea = oceanLevel * 0.85f;
   float midpoint   = shallowSea / 2.0f;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (ocean[y][x] && elevation[y][x] < shallowSea)
         {
            if (elevation[y][x] < midpoint)
            {
               elevation[y][x] =
                  midpoint - ((midpoint - elevation[y][x]) / 5.0f);
            }
            else if (elevation[y][x] > midpoint)
            {
               elevation[y][x] =
                  midpoint + ((elevation[y][x] - midpoint) / 5.0f);
            }
         }
      }
   }
}

static boost::multi_array<int32_t, 2>
NextLandDynamic(const OceanArrayType& ocean, int32_t maxRadius)
{
   const int32_t width  = static_cast<int32_t>(ocean.shape()[1]);
   const int32_t height = static_cast<int32_t>(ocean.shape()[0]);

   boost::multi_array<int32_t, 2> nextLand(boost::extents[height][width]);

   std::transform(ocean.data(),
                  ocean.data() + ocean.num_elements(),
                  nextLand.data(),
                  [](const bool& ocean) -> int32_t {
                     // Non-ocean tiles are zero distance away from next land
                     return (ocean) ? -1 : 0;
                  });

   for (int32_t distance = 0; distance < maxRadius; distance++)
   {
      for (int32_t y = 0; y < height; y++)
      {
         for (int32_t x = 0; x < width; x++)
         {
            if (nextLand[y][x] != distance)
            {
               continue;
            }

            for (int32_t dy = -1; dy <= 1; dy++)
            {
               int32_t ny = y + dy;
               if (0 <= ny && ny < height)
               {
                  for (int32_t dx = -1; dx <= 1; dx++)
                  {
                     int32_t nx = x + dx;
                     if (0 <= nx && nx < width)
                     {
                        if (nextLand[ny][nx] == -1)
                        {
                           nextLand[ny][nx] = distance + 1;
                        }
                     }
                  }
               }
            }
         }
      }
   }

   return nextLand;
}

void SeaDepth(World& world, float seaLevel)
{
   // We want to multiply the raw sea depth by one of these factors depending on
   // the distance from the next land
   const std::vector<float> factors({0.0f, 0.3f, 0.5f, 0.7f, 0.9f});

   const ElevationArrayType& elevation = world.GetElevationData();
   SeaDepthArrayType&        seaDepth  = world.GetSeaDepthData();

   seaDepth.resize(boost::extents[world.height()][world.width()]);

   boost::multi_array<int32_t, 2> nextLand =
      NextLandDynamic(world.GetOceanData());

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         seaDepth[y][x] = seaLevel - elevation[y][x];

         int32_t distToNextLand = nextLand[y][x];
         if (distToNextLand > 0)
         {
            seaDepth[y][x] *= factors[distToNextLand - 1];
         }
      }
   }

   AntiAlias(seaDepth, 10);

   auto minmax = std::minmax_element(seaDepth.data(),
                                     seaDepth.data() + seaDepth.num_elements());

   const float minDepth = *minmax.first;
   const float maxDepth = *minmax.second;

   std::transform(seaDepth.data(),
                  seaDepth.data() + seaDepth.num_elements(),
                  seaDepth.data(),
                  [&minDepth, &maxDepth](const float& a) -> float {
                     return (a - minDepth) / (maxDepth - minDepth);
                  });
}

} // namespace WorldEngine
