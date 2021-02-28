#include "world.h"

namespace WorldEngine
{

World::World(const std::string&          name,
             Size                        size,
             uint32_t                    seed,
             const GenerationParameters& generationParams,
             const std::vector<float>&   temps,
             const std::vector<float>&   humids,
             float                       gammaCurve,
             float                       curveOffset) :
    name_(name),
    size_(size),
    seed_(seed),
    generationParams_(generationParams),
    temps_(temps),
    humids_(humids),
    gammaCurve_(gammaCurve),
    curveOffset_(curveOffset)
{
}

World::~World() {}

} // namespace WorldEngine
