#include "generation.h"

#include <queue>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

typedef std::pair<uint32_t, uint32_t> CoordType;

static void Around(std::vector<CoordType>& coordList,
                   uint32_t                x,
                   uint32_t                y,
                   uint32_t                width,
                   uint32_t                height);
static void FillOcean(OceanArrayType&           ocean,
                      const ElevationArrayType& elevation,
                      float                     seaLevel);
static void HarmonizeOcean(const OceanArrayType&     ocean,
                           const ElevationArrayType& elevation,
                           float                     oceanLevel);

void AddNoiseToElevation(World& world, uint32_t seed) {}

void CenterLand(World& world) {}

void InitializeOceanAndThresholds(World& world, float oceanLevel)
{
   const ElevationArrayType& e     = world.GetElevationData();
   OceanArrayType&           ocean = world.GetOceanData();

   FillOcean(ocean, e, oceanLevel);

   // TODO: Finish
   // hl = FindThresholdF(e, 0.10); // Highest 10% of land is hills
   // ml = FindThresholdF(e, 0.03); // Highest 3% of land is mountains

   HarmonizeOcean(ocean, e, oceanLevel);

   // TODO: Finish
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

   BOOST_LOG_TRIVIAL(trace) << "FillOcean(): " << width << "x" << height;

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

static void HarmonizeOcean(const OceanArrayType&     ocean,
                           const ElevationArrayType& elevation,
                           float                     oceanLevel)
{
}

} // namespace WorldEngine
