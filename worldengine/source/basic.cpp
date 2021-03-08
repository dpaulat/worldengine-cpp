#include "basic.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/log/trivial.hpp>

namespace ba = boost::accumulators;

namespace WorldEngine
{

float FindThresholdF(const boost::multi_array<float, 2>& mapData,
                     float                               landPercentage,
                     OceanArrayType*                     ocean)
{
   typedef ba::accumulator_set<float, ba::stats<ba::tag::p_square_quantile>>
      accumulator_t;

   uint32_t width  = mapData.shape()[1];
   uint32_t height = mapData.shape()[2];
   float    quantile = 1.0f - landPercentage;

   accumulator_t accumulator(ba::quantile_probability = quantile);

   if (ocean == nullptr || ocean->size() != mapData.size())
   {
      BOOST_LOG_TRIVIAL(debug) << "Calculating threshold (" << landPercentage
                               << ") without ocean data...";

      for (uint32_t y = 0; y < height; y++)
      {
         for (uint32_t x = 0; x < width; x++)
         {
            accumulator(mapData[y][x]);
         }
      }
   }
   else
   {
      BOOST_LOG_TRIVIAL(debug) << "Calculating threshold (" << landPercentage
                               << ") with ocean data...";

      for (uint32_t y = 0; y < height; y++)
      {
         for (uint32_t x = 0; x < width; x++)
         {
            if (!(*ocean)[y][x])
            {
               accumulator(mapData[y][x]);
            }
         }
      }
   }

   float threshold = ba::p_square_quantile(accumulator);

   BOOST_LOG_TRIVIAL(debug) << "Threshold: " << threshold;

   return threshold;
}

} // namespace WorldEngine
