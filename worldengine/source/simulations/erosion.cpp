#include "erosion.h"
#include "../path.h"

#include <map>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

enum class Direction
{
   Center,
   North,
   East,
   South,
   West
};

typedef float     WaterFlowDataType;
typedef Direction WaterPathDataType;

typedef boost::multi_array<WaterFlowDataType, 2> WaterFlowArrayType;
typedef boost::multi_array<WaterPathDataType, 2> WaterPathArrayType;

static const std::unordered_map<Direction, Point> directionMap_ = {
   {Direction::Center, {0, 0}},
   {Direction::North, {0, -1}},
   {Direction::East, {1, 0}},
   {Direction::South, {0, 1}},
   {Direction::West, {-1, 0}}};

static const Direction dirNeighbors_[] = {Direction::North, //
                                          Direction::East,
                                          Direction::South,
                                          Direction::West};

static const Direction dirNeighborsCenter_[] = {Direction::Center, //
                                                Direction::North,  //
                                                Direction::East,
                                                Direction::South,
                                                Direction::West};

static const bool wrap_ = true;

static const float RIVER_THRESHOLD = 0.02f;

/**
 * @brief Try to find a lower elevation within a range of an increasing circle's
 * radius, try to find the best path, and return it
 * @param world
 * @param x
 * @param y
 * @return
 */
static std::tuple<bool, bool, Point>
FindLowerElevation(const World& world, int32_t x, int32_t y);

/**
 * @brief Water flows based on cost, seeking the highest elevation difference.
 * Highest positive number is the path of least resistance (lowest point).
 *
 * Cost
 * *** 1,0 ***
 * 0,1 *** 2,1
 * *** 1,2 ***
 *
 * @param world
 * @param x
 * @param y
 * @return
 */
static std::tuple<Direction, int32_t, int32_t>
FindQuickPath(const World& world, int32_t x, int32_t y);

/**
 * @brief Determine whether or not point (x, y) is in a circle with center
 * (centerX, centerY) and a given radius
 * @param radius
 * @param centerX
 * @param centerY
 * @param x
 * @param y
 * @return
 */
static bool InCircle(
   int32_t radius, int32_t centerX, int32_t centerY, int32_t x, int32_t y);

/**
 * @brief Find the flow direction for each cell in heightmap
 * @param world
 * @param waterPath
 */
static void FindWaterFlow(const World& world, WaterPathArrayType& waterPath);

/**
 * @brief Find places on map where sources of river can be found
 * @param world
 * @param waterPath
 * @param waterFlow
 * @return River sources
 */
static std::list<Point> RiverSources(const World&              world,
                                     const WaterPathArrayType& waterPath,
                                     WaterFlowArrayType&       waterFlow);

/**
 * @brief Simulate fluid dynamics by using starting point and flowing to the
 * lowest available point
 * @param world
 * @param source
 * @param riverList
 * @param lakeList
 * @return River flow path
 */
static std::list<Point> RiverFlow(const World&                       world,
                                  Point                              source,
                                  const std::list<std::list<Point>>& riverList,
                                  std::list<Point>&                  lakeList);

/**
 * @brief Validate that for each point in river is equal to or lower than the
 * last
 * @param world
 * @param river
 */
static void CleanUpFlow(World& world, std::list<Point>& river);

/**
 * @brief Simulate erosion in heightmap based on river path.
 * - Current location must be less than or equal to previous location
 * - Riverbed is carved out by % of volume/flow
 * - Sides of river are also eroded to slope into riverbed
 * @param world
 * @param river
 */
static void RiverErosion(World& world, std::list<Point>& river);

/**
 * @brief Update the river map with rainfall that is to become the waterflow
 * @param waterFlow
 * @param precipitations
 * @param river
 * @param riverMap
 */
static void RiverMapUpdate(const WaterFlowArrayType&     waterFlow,
                           const PrecipitationArrayType& precipitations,
                           const std::list<Point>&       river,
                           RiverMapArrayType&            riverMap);

void ErosionSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Erosion simulation start";

   uint32_t width  = world.width();
   uint32_t height = world.height();

   const PrecipitationArrayType& precipitations = world.GetPrecipitationData();

   WaterFlowArrayType waterFlow(precipitations);
   WaterPathArrayType waterPath(boost::extents[height][width]);

   RiverMapArrayType& riverMap = world.GetRiverMapData();
   LakeMapArrayType&  lakeMap  = world.GetLakeMapData();

   riverMap.resize(boost::extents[height][width]);
   lakeMap.resize(boost::extents[height][width]);

   std::list<Point> riverSources;

   std::list<std::list<Point>> riverList;
   std::list<Point>            lakeList;

   // Step 1: Water flow per cell based on rainfall
   FindWaterFlow(world, waterPath);

   // Step 2: Find river sources (seeds)
   riverSources = RiverSources(world, waterPath, waterFlow);

   // Step 3: For each source, find a path to sea
   for (Point source : riverSources)
   {
      std::list<Point> river = RiverFlow(world, source, riverList, lakeList);
      if (!river.empty())
      {
         riverList.push_back(river);
         CleanUpFlow(world, river);

         Point riverEnd = river.back();
         if (!world.IsOcean(riverEnd) && lakeList.back() != riverEnd)
         {
            lakeList.push_back(riverEnd);
         }
      }
   }

   // Step 4: Simulate erosion and update river map
   for (std::list<Point> river : riverList)
   {
      RiverErosion(world, river);
      RiverMapUpdate(waterFlow, precipitations, river, riverMap);
   }

   // Step 5: Rivers with no paths to sea form lakes
   for (Point lake : lakeList)
   {
      int32_t lx      = lake.first;
      int32_t ly      = lake.second;
      lakeMap[ly][lx] = 0.1f; // TODO: Make this based on rainfall/flow

      BOOST_LOG_TRIVIAL(debug) << "Found lake at (" << lx << ", " << ly << ")";
   }

   BOOST_LOG_TRIVIAL(info) << "Erosion simulation finish";
}

static std::tuple<bool, bool, Point>
FindLowerElevation(const World& world, int32_t x, int32_t y)
{
   const ElevationArrayType& e         = world.GetElevationData();
   const int32_t             maxRadius = 40;

   int32_t currentRadius   = 1;
   float   lowestElevation = e[y][x];
   bool    found           = false;
   bool    isWrapped       = false;
   int32_t destX;
   int32_t destY;

   std::vector<Point> wrapped;
   while (!found && currentRadius <= maxRadius)
   {
      for (int32_t cy = -currentRadius; cy <= currentRadius; cy++)
      {
         for (int32_t cx = -currentRadius; cx <= currentRadius; cx++)
         {
            int32_t rx = x + cx;
            int32_t ry = y + cy;

            // Are we within bounds?
            if (!wrap_ && !world.Contains(rx, ry))
            {
               continue;
            }

            // Are we within a circle?
            if (!InCircle(currentRadius, x, y, rx, ry))
            {
               continue;
            }

            rx %= world.width();
            ry %= world.height();

            float elevation = e[ry][rx];

            // Have we found a lower elevation?
            if (elevation < lowestElevation)
            {
               lowestElevation = elevation;
               destX           = rx;
               destY           = ry;
               found           = true;
               if (!world.Contains(x + cx, y + cy))
               {
                  wrapped.push_back({rx, ry});
               }
            }
         }
      }
   }

   if (found && std::find(wrapped.begin(),
                          wrapped.end(),
                          std::make_pair(destX, destY)) != wrapped.end())
   {
      isWrapped = true;
   }

   return std::tie(found, isWrapped, std::make_pair(destX, destY));
}

static std::tuple<Direction, int32_t, int32_t>
FindQuickPath(const World& world, int32_t x, int32_t y)
{
   const ElevationArrayType& elevation = world.GetElevationData();

   float     lowestElevation = elevation[y][x];
   Direction newDirection    = Direction::Center;
   int32_t   newPathX        = 0;
   int32_t   newPathY        = 0;

   for (Direction direction : dirNeighbors_)
   {
      int32_t dx;
      int32_t dy;
      std::tie(dx, dy) = directionMap_.at(direction);

      int32_t tx = x + dx;
      int32_t ty = y + dy;

      if (!wrap_ && !world.Contains(tx, ty))
      {
         continue;
      }

      tx %= world.width();
      ty %= world.height();

      float e = elevation[ty][tx];

      if (e < lowestElevation)
      {
         lowestElevation = e;
         newDirection    = direction;
         newPathX        = tx;
         newPathY        = ty;
      }
   }

   return std::tie(newDirection, newPathX, newPathY);
}

static bool
InCircle(int32_t radius, int32_t centerX, int32_t centerY, int32_t x, int32_t y)
{
   int32_t dx         = centerX - x;
   int32_t dy         = centerY - y;
   int32_t squareDist = (dx * dx + dy * dy);
   return squareDist <= radius * radius;
}

static void FindWaterFlow(const World& world, WaterPathArrayType& waterPath)
{
   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         std::tie(waterPath[y][x], std::ignore, std::ignore) =
            FindQuickPath(world, x, y);
      }
   }
}

static std::list<Point> RiverSources(const World&              world,
                                     const WaterPathArrayType& waterPath,
                                     WaterFlowArrayType&       waterFlow)
{
   /*
    * Using the wind and rainfall data, create river "seeds" by flowing rainfall
    * along paths until a "flow" threshold is reached and we ahve a beginning of
    * a river.
    * Trickle -> Stream -> River -> Sea
    */
   const PrecipitationArrayType& precipitation = world.GetPrecipitationData();

   std::list<Point> riverSources;

   // Step 1: Using flow direction, follow the path for each cell adding the
   // previous cell's flow to the current cell's flow.

   // Step 2: We loop through the water flow map looking for cells above the
   // water flow threshold.  These are our river sources and we mark them as
   // rivers.  While looking, the cells with no outgoing flow, above water flow
   // threshold are still above sea level and marked as "sources".
   for (int32_t y = 0; y < world.height(); y++)
   {
      for (int32_t x = 0; x < world.width(); x++)
      {
         if (waterPath[y][x] == Direction::Center)
         {
            // Ignore cells without flow direction
            continue;
         }

         // Begin with starting location
         int32_t cx = x;
         int32_t cy = y;

         float rainfall = precipitation[y][x];

         // Follow flow path to where it may lead
         while (true)
         {
            // Have we found a seed?
            if (world.IsMountain(cx, cy) &&
                waterFlow[cy][cx] >= RIVER_THRESHOLD)
            {
               // Try not to create seeds around other seeds
               for (Point seed : riverSources)
               {
                  int32_t sx = seed.first;
                  int32_t sy = seed.second;

                  if (InCircle(9, cx, cy, sx, sy))
                  {
                     // We do not want seeds for neighbors
                     break;
                  }
               }

               riverSources.push_back({cx, cy});
            }

            // No path means dead end
            if (waterPath[cy][cx] == Direction::Center)
            {
               break;
            }

            // Follow path, add water flow from previous cell
            int32_t dx;
            int32_t dy;
            std::tie(dx, dy) = directionMap_.at(waterPath[y][x]);

            // Calculate next cell
            int32_t nx = cx + dx;
            int32_t ny = cy + dy;

            waterFlow[ny][nx] += rainfall;

            // Set current cell to next cell
            cx = nx;
            cy = ny;
         }
      }
   }

   return riverSources;
}

static std::list<Point> RiverFlow(const World&                       world,
                                  Point                              source,
                                  const std::list<std::list<Point>>& riverList,
                                  std::list<Point>&                  lakeList)
{
   Point            currentLocation = source;
   std::list<Point> path;

   path.push_back(source);

   // Start the flow
   while (true)
   {
      int32_t x = currentLocation.first;
      int32_t y = currentLocation.second;

      // If there is a nearby river, flow into it
      for (Direction direction : dirNeighbors_)
      {
         int32_t dx;
         int32_t dy;
         std::tie(dx, dy) = directionMap_.at(direction);

         int32_t ax = x + dx;
         int32_t ay = y + dy;

         if (wrap_)
         {
            ax %= world.width();
            ay %= world.height();
         }

         for (std::list<Point> river : riverList)
         {
            if (std::find(river.begin(), river.end(), std::make_pair(ax, ay)) !=
                river.end())
            {
               bool merge = false;

               for (Point r : river)
               {
                  if (r == std::make_pair(ax, ay) || merge)
                  {
                     merge = true;
                     path.push_back(r);
                  }
               }

               // Skip the rest, return path
               return path;
            }
         }
      }

      // Found at sea?
      if (world.IsOcean(x, y))
      {
         break;
      }

      // Find our immediate lowest elevationand flow there
      Direction quickDirection;
      int32_t   quickX;
      int32_t   quickY;
      std::tie(quickDirection, quickX, quickY) = FindQuickPath(world, x, y);
      if (quickDirection != Direction::Center)
      {
         path.push_back({quickX, quickY});
         currentLocation = {quickX, quickY};

         // Stop here and enter back into loop
         continue;
      }

      bool  foundLowerElevation;
      bool  isWrapped;
      Point lowerElevation;

      std::tie(foundLowerElevation, isWrapped, lowerElevation) =
         FindLowerElevation(world, x, y);
      if (foundLowerElevation && !isWrapped)
      {
         std::list<Point> lowerPath =
            FindPath(world, currentLocation, lowerElevation);
         if (!lowerPath.empty())
         {
            path.splice(path.end(), lowerPath);
            currentLocation = path.back();
         }
         else
         {
            break;
         }
      }
      else if (foundLowerElevation && isWrapped)
      {
         const int32_t maxRadius = 40;

         int32_t lx = lowerElevation.first;
         int32_t ly = lowerElevation.second;
         int32_t nx;
         int32_t ny;

         if (!InCircle(maxRadius, x, y, lx, y))
         {
            // Are we wrapping on X axis?
            if (x - lx < 0)
            {
               lx = 0;                 // Move to left edge
               nx = world.width() - 1; // Next step is wrapped around
            }
            else
            {
               lx = world.width() - 1; // Move to right edge
               nx = 0;                 // Next step is wrapped around
            }

            ly = ny = (y + ly) / 2; // Move halfway
         }
         else if (!InCircle(maxRadius, x, y, x, ly))
         {
            // Are we wrapping on Y axis?
            if (y - ly < 0)
            {
               ly = 0;                  // Move to top edge
               ny = world.height() - 1; // Next step is wrapped around;
            }
            else
            {
               ly = world.height() - 1; // Move to bottom edge
               ny = 0;                  // Next step is wrapped around
            }

            lx = nx = (x + lx) / 2; // Move halfway
         }
         else
         {
            BOOST_LOG_TRIVIAL(error)
               << "RiverFlow: Unexpected condition, unable to continue flow";
            break;
         }

         // Find our way to the edge
         std::list<Point> edgePath = FindPath(world, currentLocation, {lx, ly});
         if (edgePath.empty())
         {
            // Can't find a path, make it a lake
            lakeList.push_back(currentLocation);
            break;
         }
         path.splice(path.end(), edgePath); // Add our newly found path
         path.push_back({nx, ny}); // Finally add our overflow to the other side
         currentLocation = path.back();

         // Find our way to the lowest position originally found
         std::list<Point> lowerPath =
            FindPath(world, currentLocation, lowerElevation);
         path.splice(path.end(), edgePath);
         currentLocation = path.back();
      }
      else
      {
         // Can't find any other path, make it a lake
         lakeList.push_back(currentLocation);
         break; // End of river
      }

      if (!world.Contains(x, y))
      {
         BOOST_LOG_TRIVIAL(warning)
            << "RiverFlow: Out of bounds coordinates detected";
      }
   }

   return path;
}

static void CleanUpFlow(World& world, std::list<Point>& river)
{
   ElevationArrayType e = world.GetElevationData();

   float cElevation = 1.0;

   for (Point r : river)
   {
      int32_t rx         = r.first;
      int32_t ry         = r.second;
      float&  rElevation = e[ry][rx];
      if (rElevation <= cElevation)
      {
         cElevation = rElevation;
      }
      else
      {
         rElevation = cElevation;
      }
   }
}

static void RiverErosion(World& world, std::list<Point>& river)
{
   const int32_t radius = 2;

   ElevationArrayType& e = world.GetElevationData();

   // Erosion around river, create river valley
   for (Point r : river)
   {
      int32_t rx = r.first;
      int32_t ry = r.second;

      for (int32_t y = ry - radius; y <= ry + radius; y++)
      {
         for (int32_t x = rx - radius; x <= rx + radius; x++)
         {
            if (!wrap_ && !world.Contains(x, y))
            {
               // Ignore edges of map
               continue;
            }

            int32_t wx = x % world.width();
            int32_t wy = y % world.width();

            if (std::find(river.begin(), river.end(), std::make_pair(wx, wy)) !=
                river.end())
            {
               // Ignore points within the river
               continue;
            }

            if (e[wy][wx] <= e[ry][rx])
            {
               // Ignore points lower than the river
               continue;
            }

            if (!InCircle(radius, rx, ry, wx, wy))
            {
               // Ignore points outside a circle
               continue;
            }

            // Determine curve
            float   curve = 1.0f;
            int32_t adx   = std::abs(rx - wx);
            int32_t ady   = std::abs(ry - wy);
            if (adx == 1 || ady == 1)
            {
               curve = 0.2f;
            }
            else if (adx == 2 || ady == 2)
            {
               curve = 0.05f;
            }

            // Update elevation
            float diff         = e[ry][rx] - e[wy][wx];
            float newElevation = e[wy][wx] + diff * curve;

            if (newElevation < e[ry][rx])
            {
               BOOST_LOG_TRIVIAL(warning)
                  << "RiverErosion: New elevation is lower than river, fixing";
               newElevation = e[ry][rx];
            }

            e[wy][wx] = newElevation;
         }
      }
   }
}

static void RiverMapUpdate(const WaterFlowArrayType&     waterFlow,
                           const PrecipitationArrayType& precipitations,
                           const std::list<Point>&       river,
                           RiverMapArrayType&            riverMap)
{
   bool    isSeed = true;
   int32_t px     = 0;
   int32_t py     = 0;

   for (Point p : river)
   {
      int32_t x = p.first;
      int32_t y = p.second;

      if (isSeed)
      {
         riverMap[y][x] = waterFlow[y][x];
         isSeed         = false;
      }
      else
      {
         riverMap[y][x] = precipitations[y][x] + riverMap[py][px];
      }
      px = x;
      py = y;
   }
}

} // namespace WorldEngine
