#pragma once

#include "world.h"

namespace WorldEngine
{
float FindThresholdF(const boost::multi_array<float, 2>& mapData,
                     float                               landPercentage,
                     OceanArrayType*                     ocean = nullptr);
}
