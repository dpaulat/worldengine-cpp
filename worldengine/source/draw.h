#pragma once

#include "world.h"

namespace WorldEngine
{
/**
 * @brief Draw a biome image file
 * @param world World object
 * @param filename Destination filename
 */
void DrawBiomeOnFile(const World& world, const std::string& filename);

/**
 * @brief Draw an ocean image file
 * @param ocean Ocean data
 * @param filename Destination filename
 */
void DrawOceanOnFile(const OceanArrayType& ocean, const std::string& filename);

/**
 * @brief Draw a precipitation image file
 * @param world World object
 * @param filename Destination filename
 * @param blackAndWhite Draw image in black and white
 */
void DrawPrecipitationOnFile(const World&       world,
                             const std::string& filename,
                             bool               blackAndWhite = false);

/**
 * @brief Draw a simple elevation map image file
 * @param world World object
 * @param filename Destination filename
 * @param seaLevel Sea level
 */
void DrawSimpleElevationOnFile(const World&       world,
                               const std::string& filename,
                               float              seaLevel);

/**
 * @brief Draw a temperature level image file
 * @param world World object
 * @param filename Destination filename
 * @param blackAndWhite Draw image in black and white
 */
void DrawTemperatureLevelsOnFile(const World&       world,
                                 const std::string& filename,
                                 bool               blackAndWhite = false);
} // namespace WorldEngine
