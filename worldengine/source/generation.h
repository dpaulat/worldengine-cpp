#pragma once

#include "common.h"
#include "world.h"

#include <memory>

namespace WorldEngine
{

void CenterLand(World& world);
void PlaceOceansAtMapBorders(World& world);
void AddNoiseToElevation(World& world, uint32_t seed);
void InitializeOceanAndThresholds(World& world,
                                  float  oceanLevel = DEFAULT_OCEAN_LEVEL);

} // namespace WorldEngine
