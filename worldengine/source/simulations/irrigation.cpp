#include "irrigation.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

static void IrrigationExecute(World& world);

void IrrigationSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Irrigation simulation start";

   IrrigationExecute(world);

   BOOST_LOG_TRIVIAL(info) << "Irrigation simulation finish";
}

static void IrrigationExecute(World& world)
{
   const int32_t width  = world.width();
   const int32_t height = world.height();
   const int32_t radius = 10;

   const WaterMapArrayType& watermap   = world.GetWaterMapData();
   IrrigationArrayType&     irrigation = world.GetIrrigationData();
   irrigation.resize(boost::extents[height][width]);

   std::fill(
      irrigation.data(), irrigation.data() + irrigation.num_elements(), 0.0f);

   // Create array of pre-calculated values
   boost::multi_array<float, 2> logs(
      boost::extents[radius * 2 + 1][radius * 2 + 1]);
   for (int32_t y = 0; y <= radius * 2; y++)
   {
      float dy = y - radius; // Y distance to center: [-10, 10]
      for (int32_t x = 0; x <= radius * 2; x++)
      {
         float dx = x - radius; // X distance to center: [-10, 10]

         // Calculate final matrix: ln(sqrt(x^2 + y^2) + 1) + 1
         logs[y][x] = log1pf(sqrtf(dx * dx + dy * dy)) + 1;
      }
   }

   for (int32_t y = 0; y < height; y++)
   {
      for (int32_t x = 0; x < width; x++)
      {
         if (world.IsOcean(x, y))
         {
            // Coordinates (top-left / bottom-right) used for the values slice
            uint32_t tlXV = std::max(x - radius, 0);
            uint32_t tlYV = std::max(y - radius, 0);
            uint32_t brXV = std::min(x + radius, width - 1);
            uint32_t brYV = std::min(y + radius, height - 1);

            // Coordinates (top-left / bottom-right) used for the logs slice
            uint32_t tlXL = std::max(radius - x, 0);
            uint32_t tlYL = std::max(radius - y, 0);
            uint32_t brXL = std::min(radius - x + width - 1, 2 * radius);
            uint32_t brYL = std::min(radius - y + height - 1, 2 * radius);

            // Values slice and logs slice should be the same size and
            // dimensions

            for (uint32_t vy = tlYV, ly = tlYL; //
                 vy <= brYV && ly <= brYL;
                 vy++, ly++)
            {
               for (uint32_t vx = tlXV, lx = tlXL; //
                    vx <= brXV && lx <= brXL;
                    vx++, lx++)
               {
                  irrigation[vy][vx] += watermap[y][x] / logs[ly][lx];
               }
            }
         }
      }
   }
}

} // namespace WorldEngine
