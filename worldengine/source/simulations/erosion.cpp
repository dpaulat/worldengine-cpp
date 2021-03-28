#include "erosion.h"

#include <map>

#include <boost/graph/astar_search.hpp>
#include <boost/graph/adjacency_list.hpp> //?
#include <boost/graph/filtered_graph.hpp> //?
#include <boost/graph/grid_graph.hpp> //?
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

typedef std::pair<int32_t, int32_t> Point;

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
static std::tuple<bool, bool, int32_t, int32_t>
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
static std::vector<Point> RiverSources(const World&              world,
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
static std::vector<Point> RiverFlow(World&                           world,
                                    Point                            source,
                                    std::vector<std::vector<Point>>& riverList,
                                    std::vector<Point>&              lakeList);

void ErosionSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Erosion simulation start";

   uint32_t width  = world.width();
   uint32_t height = world.height();

   WaterFlowArrayType waterFlow(world.GetPrecipitationData());
   WaterPathArrayType waterPath(boost::extents[height][width]);

   RiverMapArrayType& riverMap = world.GetRiverMapData();
   LakeMapArrayType&  lakeMap  = world.GetLakeMapData();

   riverMap.resize(boost::extents[height][width]);
   lakeMap.resize(boost::extents[height][width]);

   std::vector<Point> riverSources;

   std::vector<std::vector<Point>> riverList;
   std::vector<Point>              lakeList;

   // Step 1: Water flow per cell based on rainfall
   FindWaterFlow(world, waterPath);

   // Step 2: Find river sources (seeds)
   riverSources = RiverSources(world, waterPath, waterFlow);

   // Step 3: For each source, find a path to sea
   for (Point source : riverSources)
   {
      std::vector<Point> river = RiverFlow(world, source, riverList, lakeList);
   }

   // Step 4: Simulate erosion and update river map
   // TODO

   // Step 5: Rivers with no paths to sea form lakes
   // TODO

   BOOST_LOG_TRIVIAL(info) << "Erosion simulation finish";
}

static std::tuple<bool, bool, int32_t, int32_t>
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

   return std::tie(found, isWrapped, destX, destY);
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

static std::vector<Point> RiverSources(const World&              world,
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

   std::vector<Point> riverSources;

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

static std::vector<Point> RiverFlow(World&                           world,
                                    Point                            source,
                                    std::vector<std::vector<Point>>& riverList,
                                    std::vector<Point>&              lakeList)
{
   Point              currentLocation = source;
   std::vector<Point> path;

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

         for (std::vector<Point> river : riverList)
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

      bool    foundLowerElevation;
      bool    isWrapped;
      int32_t lx;
      int32_t ly;

      std::tie(foundLowerElevation, isWrapped, lx, ly) =
         FindLowerElevation(world, x, y);
      if (foundLowerElevation && !isWrapped)
      {
         // TODO: Finish
         break; // TODO: Remove break
      }
      else if (foundLowerElevation && isWrapped)
      {
         const int32_t maxRadius = 40;

         int nx;
         int ny;

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
         // TODO

         // Find our way to the lowest position originally found
         // TODO
         break; // TODO: Remove break
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

} // namespace WorldEngine
