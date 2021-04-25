#pragma once

#include "worldengine/world.h"

namespace OpenSimplexNoise
{
class Noise;
}

namespace WorldEngine
{

/**
 * @brief Execute the anti-alias operation on the given data
 * @param data Data to anti-alias
 * @param steps Number of times to run the anti-alias operation
 */
void AntiAlias(boost::multi_array<float, 2>& data, size_t steps = 1);

/**
 * @brief Count how many neighbors of a coordinate are set to true.
 * @tparam T
 * @param mask
 * @param radius
 * @return Map of number of neighbors
 */
template<typename T>
boost::multi_array<uint32_t, 2>
CountNeighbors(const boost::multi_array<T, 2>& mask, int32_t radius = 1);

/**
 * @brief Estimate the elevation threshold that is lower than a given percentage
 * of land
 * @param mapData Elevation data
 * @param landPercentage Percentage of land higher than threshold
 * @param ocean Optional ocean data to exclude in threshold calculation
 * @return Elevation threshold
 */
float FindThresholdF(const boost::multi_array<float, 2>& mapData,
                     float                               landPercentage,
                     const OceanArrayType*               ocean = nullptr);

/**
 * @brief Perform a linear interpolation over the given points
 * @tparam T
 * @tparam U
 * @param x X coordinate to evaluate
 * @param points A list of (x, y) points, sorted along the x-axis
 * @return Evaluated y coordinate
 */
template<typename T, typename U>
U Interpolate(T x, const std::vector<std::pair<T, U>>& points)
{
   if (x <= points[0].first)
   {
      return points[0].second;
   }

   for (auto it = points.begin() + 1; it != points.end(); it++)
   {
      if (x <= it->first)
      {
         double t = static_cast<double>(x - (it - 1)->first) /
                    (it->first - (it - 1)->first);
         U y = (it - 1)->second + t * (it->second - (it - 1)->second);
         return y;
      }
   }

   return points.back().second;
}

/**
 * @brief OpenSimplex noise value for specified 2D coordinate
 * @param noise Seeded OpenSimplex noise generator
 * @param x X coordinate
 * @param y Y coordinate
 * @param octaves Number of passes, defaults to 1 (simple noise).
 * @return OpenSimplex noise value
 */
double Noise(const OpenSimplexNoise::Noise& noise,
             double                         x,
             double                         y,
             uint32_t                       octaves = 1);

} // namespace WorldEngine
