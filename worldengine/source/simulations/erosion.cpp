#include "erosion.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

typedef float WaterFlowDataType;
typedef float WaterPathDataType;

typedef boost::multi_array<WaterFlowDataType, 2> WaterFlowArrayType;
typedef boost::multi_array<WaterPathDataType, 2> WaterPathArrayType;

void ErosionSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(debug) << "Erosion simulation start";

   uint32_t width  = world.width();
   uint32_t height = world.height();

   WaterFlowArrayType waterFlow(boost::extents[height][width]);
   WaterPathArrayType waterPath(boost::extents[height][width]);

   RiverMapArrayType& riverMap = world.GetRiverMapData();
   LakeMapArrayType&  lakeMap  = world.GetLakeMapData();

   //riverMap.resize(boost::extents[height][width]);
   //lakeMap.resize(boost::extents[height][width]);

   // Step 1: Water flow per cell based on rainfall
   // TODO

   // Step 2: Find river sources (seeds)
   // TODO

   // Step 3: For each source, find a path to sea
   // TODO

   // Step 4: Simulate erosion and update river map
   // TODO

   // Step 5: Rivers with no paths to sea form lakes
   // TODO

   BOOST_LOG_TRIVIAL(debug) << "Erosion simulation finish";
}

} // namespace WorldEngine
