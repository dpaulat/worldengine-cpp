#include "generation.h"
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

#include <OpenSimplexNoise.h>

namespace WorldEngine
{

typedef std::pair<uint32_t, uint32_t> CoordType;

static void Around(std::vector<CoordType>& coordList,
                   uint32_t                x,
                   uint32_t                y,
                   uint32_t                width,
                   uint32_t                height);

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
         elevation[y][x] += n;
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
   int32_t yWithMinSum =
      std::min_element(rowSums.cbegin(), rowSums.cend()) - rowSums.cbegin();
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
   int32_t xWithMinSum =
      std::min_element(colSums.cbegin(), colSums.cend()) - colSums.cbegin();
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

   std::default_random_engine              generator(seed);
   std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);

   // Seed map should be appended to to maximize compatibility between versions
   std::unordered_map<Simulations, uint32_t> seedMap;
   seedMap.insert({Simulations::Precipitation, distribution(generator)});
   seedMap.insert({Simulations::Erosion, distribution(generator)});
   seedMap.insert({Simulations::Watermap, distribution(generator)});
   seedMap.insert({Simulations::Irrigation, distribution(generator)});
   seedMap.insert({Simulations::Temperature, distribution(generator)});
   seedMap.insert({Simulations::Humidity, distribution(generator)});
   seedMap.insert({Simulations::Permeability, distribution(generator)});
   seedMap.insert({Simulations::Biome, distribution(generator)});
   seedMap.insert({Simulations::Icecap, distribution(generator)});

   TemperatureSimulation(world, seedMap[Simulations::Temperature]);
   PrecipitationSimulation(world, seedMap[Simulations::Precipitation]);

   if (!step.includeErosion_)
   {
      return;
   }

   ErosionSimulation(world);
   WatermapSimulation(world, seedMap[Simulations::Watermap]);
   IrrigationSimulation(world);
   HumiditySimulation(world);
   PermeabilitySimulation(world, seedMap[Simulations::Permeability]);
   BiomeSimulation(world);
   IcecapSimulation(world, seedMap[Simulations::Icecap]);
}

void InitializeOceanAndThresholds(World& world, float oceanLevel)
{
   ElevationArrayType& e     = world.GetElevationData();
   OceanArrayType&     ocean = world.GetOceanData();

   FillOcean(ocean, e, oceanLevel);

   float hl = FindThresholdF(e, 0.10); // Highest 10% of land is hills
   float ml = FindThresholdF(e, 0.03); // Highest 3% of land is mountains
   world.SetThreshold(ElevationThresholdType::Sea, oceanLevel);
   world.SetThreshold(ElevationThresholdType::Hill, hl);
   world.SetThreshold(ElevationThresholdType::Mountain, ml);

   HarmonizeOcean(ocean, e, oceanLevel);

   // TODO: Finish
   // SeaDepth(world, oceanLevel);
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
                   uint32_t                x,
                   uint32_t                y,
                   uint32_t                width,
                   uint32_t                height)
{
   for (int dy = -1; dy <= 1; dy++)
   {
      int ny = y + dy;
      if (0 <= ny && ny < height)
      {
         for (int dx = -1; dx <= 1; dx++)
         {
            int nx = x + dx;
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
   uint32_t height = elevation.shape()[0];
   uint32_t width  = elevation.shape()[1];

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
   uint32_t width  = ocean.shape()[1];
   uint32_t height = ocean.shape()[0];

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

} // namespace WorldEngine
