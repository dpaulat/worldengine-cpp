#include "humidity.h"
#include "../basic.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

static void HumidityCalculation(World& world);

void HumiditySimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Humidity simulation start";

   const OceanArrayType&    ocean = world.GetOceanData();
   const HumidityArrayType& h     = world.GetHumidityData();

   HumidityCalculation(world);

   world.SetThreshold(HumidityLevel::Superarid,
                      FindThresholdF(h, world.humids()[0], &ocean));
   world.SetThreshold(HumidityLevel::Perarid,
                      FindThresholdF(h, world.humids()[1], &ocean));
   world.SetThreshold(HumidityLevel::Arid,
                      FindThresholdF(h, world.humids()[2], &ocean));
   world.SetThreshold(HumidityLevel::Semiarid,
                      FindThresholdF(h, world.humids()[3], &ocean));
   world.SetThreshold(HumidityLevel::Subhumid,
                      FindThresholdF(h, world.humids()[4], &ocean));
   world.SetThreshold(HumidityLevel::Humid,
                      FindThresholdF(h, world.humids()[5], &ocean));
   world.SetThreshold(HumidityLevel::Perhumid,
                      FindThresholdF(h, world.humids()[6], &ocean));
   world.SetThreshold(HumidityLevel::Superhumid,
                      std::numeric_limits<float>::max());

   BOOST_LOG_TRIVIAL(info) << "Humidity simulation finish";
}

static void HumidityCalculation(World& world)
{
   const uint32_t width  = world.width();
   const uint32_t height = world.height();

   float precipitationWeight = 1.0f;
   float irrigationWeight    = 3.0f;

   const PrecipitationArrayType& p = world.GetPrecipitationData();
   const IrrigationArrayType&    i = world.GetIrrigationData();
   HumidityArrayType&            h = world.GetHumidityData();
   h.resize(boost::extents[height][width]);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         h[y][x] =
            (p[y][x] * precipitationWeight - i[y][x] * irrigationWeight) /
            (precipitationWeight + irrigationWeight);
      }
   }
}

} // namespace WorldEngine
