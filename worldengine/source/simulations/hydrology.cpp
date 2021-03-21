#include "hydrology.h"
#include "../basic.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

static void Droplet(World& world, uint32_t x, uint32_t y, float q);
static void WatermapExecute(World& world, uint32_t numSamples);

static const uint32_t NUM_SAMPLES = 20000u;

void WatermapSimulation(World& world)
{
   BOOST_LOG_TRIVIAL(info) << "Watermap simulation start";

   const WaterMapArrayType& watermap = world.GetWaterMapData();
   const OceanArrayType&    ocean    = world.GetOceanData();

   WatermapExecute(world, NUM_SAMPLES);

   world.SetThreshold(WaterThresholds::Creek,
                      FindThresholdF(watermap, 0.05f, &ocean));
   world.SetThreshold(WaterThresholds::River,
                      FindThresholdF(watermap, 0.02f, &ocean));
   world.SetThreshold(WaterThresholds::MainRiver,
                      FindThresholdF(watermap, 0.007f, &ocean));

   BOOST_LOG_TRIVIAL(info) << "Watermap simulation finish";
}

static void Droplet(World& world, uint32_t x, uint32_t y, float q)
{
   if (q < 0)
   {
      return;
   }

   const ElevationArrayType&     elevation      = world.GetElevationData();
   const PrecipitationArrayType& precipitations = world.GetPrecipitationData();
   WaterMapArrayType&            watermap       = world.GetWaterMapData();

   std::vector<std::pair<uint32_t, uint32_t>>            tilesAround;
   std::vector<std::tuple<uint32_t, uint32_t, uint32_t>> lowers;

   float    posElev   = elevation[y][x] + watermap[y][x];
   float    minLower  = std::numeric_limits<float>::max();
   uint32_t totLowers = 0;

   world.GetTilesAround(tilesAround, x, y);

   for (auto it = tilesAround.begin(); it != tilesAround.end(); it++)
   {
      uint32_t px = it->first;
      uint32_t py = it->second;
      float    e  = elevation[py][px] + watermap[py][px];

      if (e < posElev)
      {
         uint32_t dq = static_cast<uint32_t>(posElev - e) << 2;
         if (e < minLower)
         {
            minLower = e;
            if (dq == 0)
            {
               dq = 1;
            }
         }
         lowers.push_back({dq, px, py});
         totLowers += dq;
      }
   }

   if (!lowers.empty())
   {
      float f = q / static_cast<float>(totLowers);

      for (auto it = lowers.begin(); it != lowers.end(); it++)
      {
         uint32_t s;
         uint32_t px;
         uint32_t py;

         std::tie(s, px, py) = *it;

         if (!world.IsOcean(px, py))
         {
            float ql    = f * s;
            bool  going = ql > 0.05f;

            watermap[py][px] += ql;

            if (going)
            {
               Droplet(world, px, py, ql);
            }
         }
      }
   }
   else
   {
      watermap[y][x] += q;
   }
}

static void WatermapExecute(World& world, uint32_t numSamples)
{
   uint32_t width  = world.width();
   uint32_t height = world.height();

   const ElevationArrayType&     elevation      = world.GetElevationData();
   const PrecipitationArrayType& precipitations = world.GetPrecipitationData();
   WaterMapArrayType&            watermap       = world.GetWaterMapData();
   watermap.resize(boost::extents[height][width]);

   std::fill(watermap.data(), watermap.data() + watermap.num_elements(), 0.0f);

   /*
    * This indirectly calls the global RNG. We want different implementations of
    * watermap and internally called functions (especially RandomLand) to show
    * the same RNG behavior and not contaminate the state of the global RNG
    * should anyone else happen to rely on it.
    */

   std::vector<std::pair<uint32_t, uint32_t>> landSamples;

   world.GetRandomLand(landSamples, numSamples);

   for (uint32_t i = 0; i < landSamples.size(); i++)
   {
      uint32_t x = landSamples[i].first;
      uint32_t y = landSamples[i].second;
      float    q = precipitations[y][x];

      if (q > 0)
      {
         Droplet(world, x, y, q);
      }
   }
}

} // namespace WorldEngine
