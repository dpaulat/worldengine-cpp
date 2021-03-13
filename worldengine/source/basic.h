#pragma once

#include "world.h"

namespace WorldEngine
{

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
                     OceanArrayType*                     ocean = nullptr);

} // namespace WorldEngine
