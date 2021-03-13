#pragma once

#include "common.h"
#include "world.h"

#include <memory>

namespace WorldEngine
{

void CenterLand(World& world);
void AddNoiseToElevation(World& world, uint32_t seed);
void GenerateWorld(World& world, const Step& step);
void InitializeOceanAndThresholds(World& world,
                                  float  oceanLevel = DEFAULT_OCEAN_LEVEL);
void PlaceOceansAtMapBorders(World& world);

} // namespace WorldEngine
