#pragma once

#include "world.h"

namespace WorldEngine
{
void DrawBiomeOnFile(const World& world, const std::string& filename);
void DrawOceanOnFile(const OceanArrayType& ocean, const std::string& filename);
void DrawPrecipitationOnFile(const World&       world,
                             const std::string& filename,
                             bool               blackAndWhite = false);
void DrawSimpleElevationOnFile(const World&       world,
                               const std::string& filename,
                               float              seaLevel);
void DrawTemperatureLevelsOnFile(const World&       world,
                                 const std::string& filename,
                                 bool               blackAndWhite = false);
} // namespace WorldEngine
