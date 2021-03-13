#pragma once

#include "common.h"
#include "world.h"

#include <memory>

namespace WorldEngine
{

/**
 * @brief Add random noise to elevation using OpenSimplex noise
 * @param world A world having elevation
 * @param seed Random seed value
 */
void AddNoiseToElevation(World& world, uint32_t seed);

/**
 * @brief Translate the map horizontally and vertically to put as much ocean as
 * possible at the borders, operating on elevation and plates map
 * @param world A world having elevation and oceans
 */
void CenterLand(World& world);

/**
 * @brief Generate a world, performing simulations according to the enabled
 * generation steps
 * @param world A world having elevation, oceans and thresholds
 * @param step Generation steps to perform
 */
void GenerateWorld(World& world, const Step& step);

/**
 * @brief Calculate the ocean, the sea depth and the elevation thresholds
 * @param world A world having elevation but not thresholds
 * @param oceanLevel The elevation representing the ocean level
 */
void InitializeOceanAndThresholds(World& world,
                                  float  oceanLevel = DEFAULT_OCEAN_LEVEL);

/**
 * @brief Lower the elevation near the border of the map
 * @param world A world having elevation
 */
void PlaceOceansAtMapBorders(World& world);

} // namespace WorldEngine
