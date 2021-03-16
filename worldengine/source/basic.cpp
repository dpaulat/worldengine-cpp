#include "basic.h"

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/log/trivial.hpp>

#include <OpenSimplexNoise.h>

namespace ba = boost::accumulators;

namespace WorldEngine
{

float FindThresholdF(const boost::multi_array<float, 2>& mapData,
                     float                               landPercentage,
                     const OceanArrayType*               ocean)
{
   typedef ba::accumulator_set<float, ba::stats<ba::tag::p_square_quantile>>
      accumulator_t;

   uint32_t width    = mapData.shape()[1];
   uint32_t height   = mapData.shape()[2];
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

float InterpolateF(float x, const std::vector<std::pair<float, float>>& points)
{
   if (x <= points[0].first)
   {
      return points[0].second;
   }

   for (auto it = points.begin() + 1; it != points.end(); it++)
   {
      if (x <= it->first)
      {
         float t = (x - (it - 1)->first) / (it->first - (it - 1)->first);
         float y = (it - 1)->second + t * (it->second - (it - 1)->second);
         return y;
      }
   }

   return points.back().second;
}

double Noise(const OpenSimplexNoise::Noise& noise,
             double                         x,
             double                         y,
             uint32_t                       octaves)
{
   static const double persistence = 0.5;
   static const double lacunarity  = 2.0;

   double freq  = 1.0;
   double amp   = 1.0;
   double max   = 1.0;
   double total = noise.eval(x, y);

   for (uint32_t i = 1; i < octaves; i++)
   {
      freq *= lacunarity;
      amp *= persistence;
      max += amp;
      total += noise.eval(x * freq, y * freq) * amp;
   }

   return total / max;
}

} // namespace WorldEngine
