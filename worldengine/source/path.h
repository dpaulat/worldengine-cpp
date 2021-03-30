#pragma once

#include "world.h"

namespace WorldEngine
{

/**
 * @brief Find the best path between two points
 * @param world 
 * @param source 
 * @param destination 
 * @return 
*/
std::list<Point> FindPath(const World& world, Point source, Point destination);

} // namespace WorldEngine