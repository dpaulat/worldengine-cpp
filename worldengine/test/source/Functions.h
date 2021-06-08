#pragma once

#include <worldengine/world.h>

namespace WorldEngine
{
const std::string TEST_DATA_DIR = WORLDENGINE_TEST_DATA_DIR;

bool LoadWorld(World& world, const std::string& filename);
} // namespace WorldEngine
