#include "basic.h"

#if defined(__GNUC__)
#pragma GCC diagnostic push
#if __GNUC_PREREQ(9, 0)
#pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <boost/accumulators/accumulators.hpp>
#include <boost/accumulators/statistics.hpp>
#include <boost/log/trivial.hpp>

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#if defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4554)
#pragma warning(disable : 26450)
#pragma warning(disable : 26451)
#pragma warning(disable : 26454)
#pragma warning(disable : 26495)
#pragma warning(disable : 26812)
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif

#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#if defined(_MSC_VER)
#pragma warning(pop)
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

#include <OpenSimplexNoise.h>

namespace ba = boost::accumulators;

namespace WorldEngine
{

void AntiAlias(boost::multi_array<float, 2>& mapData, size_t steps)
{
   typedef Eigen::Tensor<float, 2>    Tensor2D;
   typedef Eigen::TensorMap<Tensor2D> Tensor2DMap;

   static const float w = 1.0f / 11.0f;

   Tensor2D kernel(3, 3);
   kernel.setValues({{w, w, w}, {w, w, w}, {w, w, w}});

   const uint32_t width  = static_cast<uint32_t>(mapData.shape()[1]);
   const uint32_t height = static_cast<uint32_t>(mapData.shape()[0]);

   Tensor2DMap map(mapData.data(), width, height);

   Tensor2D mapPart = map * (2.0f / 11.0f);

   auto AntiAliasStep = [&width,
                         &height,
                         &mapPart = std::as_const(mapPart),
                         &kernel  = std::as_const(kernel)](Tensor2DMap& map) {
      // Specify first and second dimension for convolution
      static const Eigen::array<int, 2> dimensions({0, 1});

      // Create a circular boundary
      const Eigen::array<std::pair<int, int>, 2> paddings(
         {std::make_pair(1, 1), std::make_pair(1, 1)});
      Tensor2D paddedMap = map.pad(paddings);

      for (uint32_t x = 0; x <= width + 1; x++)
      {
         paddedMap(x, 0)          = paddedMap(x, height);
         paddedMap(x, height + 1) = paddedMap(x, 1);
      }
      for (uint32_t y = 0; y <= height + 1; y++)
      {
         paddedMap(0, y)         = paddedMap(width, y);
         paddedMap(width + 1, y) = paddedMap(1, y);
      }

      map = paddedMap.convolve(kernel, dimensions);

      map += mapPart;
   };

   for (size_t i = 0; i < steps; i++)
   {
      AntiAliasStep(map);
   }
}

template<typename T>
boost::multi_array<uint32_t, 2>
CountNeighbors(const boost::multi_array<T, 2>& mask, int32_t radius)
{
   const int32_t width  = static_cast<int32_t>(mask.shape()[1]);
   const int32_t height = static_cast<int32_t>(mask.shape()[0]);

   boost::multi_array<uint32_t, 2> neighbors(boost::extents[height][width]);

   std::fill(neighbors.data(), neighbors.data() + neighbors.num_elements(), 0u);

   for (int32_t y = 0; y < height; y++)
   {
      for (int32_t x = 0; x < width; x++)
      {
         for (int32_t ny = y - radius; ny <= y + radius; ny++)
         {
            if (0 <= ny && ny < height)
            {
               for (int32_t nx = x - radius; nx <= x + radius; nx++)
               {
                  if ((nx != x || ny != y) && 0 <= nx && nx < width)
                  {
                     if (mask[ny][nx])
                     {
                        neighbors[y][x]++;
                     }
                  }
               }
            }
         }
      }
   }

   return neighbors;
}
template boost::multi_array<uint32_t, 2>
CountNeighbors<bool>(const boost::multi_array<bool, 2>& mask, int32_t radius);
template boost::multi_array<uint32_t, 2>
CountNeighbors<float>(const boost::multi_array<float, 2>& mask, int32_t radius);

float FindThresholdF(const boost::multi_array<float, 2>& mapData,
                     float                               landPercentage,
                     const OceanArrayType*               ocean)
{
   typedef ba::accumulator_set<float, ba::stats<ba::tag::p_square_quantile>>
      accumulator_t;

   const uint32_t width    = static_cast<uint32_t>(mapData.shape()[1]);
   const uint32_t height   = static_cast<uint32_t>(mapData.shape()[0]);
   float          quantile = 1.0f - landPercentage;

   accumulator_t accumulator(ba::quantile_probability = quantile);

   if (ocean == nullptr || ocean->size() != mapData.size())
   {
      BOOST_LOG_TRIVIAL(trace) << "Calculating threshold (" << landPercentage
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
      BOOST_LOG_TRIVIAL(trace) << "Calculating threshold (" << landPercentage
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

   BOOST_LOG_TRIVIAL(trace) << "Threshold: " << threshold;

   return threshold;
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
