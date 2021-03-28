#pragma once

#include "world.h"

namespace WorldEngine
{

std::list<Point> FindPath(const World& world, Point source, Point destination);

} // namespace WorldEngine
