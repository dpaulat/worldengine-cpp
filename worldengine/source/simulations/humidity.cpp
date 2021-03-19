#include "humidity.h"
#include "../basic.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

static void HumidityCalculation(World& world);

void HumiditySimulation(World& world)
{
   BOOST_LOG_TRIVIAL(debug) << "Humidity simulation start";

   const OceanArrayType&    ocean = world.GetOceanData();
   const HumidityArrayType& h     = world.GetHumidityData();

   HumidityCalculation(world);

   world.SetThreshold(HumidityLevels::Superarid,
                      FindThresholdF(h, world.humids()[6], &ocean));
   world.SetThreshold(HumidityLevels::Perarid,
                      FindThresholdF(h, world.humids()[5], &ocean));
   world.SetThreshold(HumidityLevels::Arid,
                      FindThresholdF(h, world.humids()[4], &ocean));
   world.SetThreshold(HumidityLevels::Semiarid,
                      FindThresholdF(h, world.humids()[3], &ocean));
   world.SetThreshold(HumidityLevels::Subhumid,
                      FindThresholdF(h, world.humids()[2], &ocean));
   world.SetThreshold(HumidityLevels::Humid,
                      FindThresholdF(h, world.humids()[1], &ocean));
   world.SetThreshold(HumidityLevels::Perhumid,
                      FindThresholdF(h, world.humids()[0], &ocean));
   world.SetThreshold(HumidityLevels::Superhumid, 0.0f);

   BOOST_LOG_TRIVIAL(debug) << "Humidity simulation finish";
}

static void HumidityCalculation(World& world)
{
   uint32_t width  = world.width();
   uint32_t height = world.height();

   const std::vector<float>& humids = world.humids();

   float precipitationWeight = 1.0f;
   float irrigationWeight    = 3.0f;

   PrecipitationArrayType& p = world.GetPrecipitationData();
   HumidityArrayType&      h = world.GetHumidityData();
   // TODO:
   // IrrigationArrayType&    i = world.GetIrrigationData();
   PrecipitationArrayType& i = world.GetPrecipitationData();
   h.resize(boost::extents[height][width]);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < height; x++)
      {
         h[y][x] =
            (p[y][x] * precipitationWeight - i[y][x] * irrigationWeight) /
            (precipitationWeight + irrigationWeight);
      }
   }
}

} // namespace WorldEngine
