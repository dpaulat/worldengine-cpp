#pragma once

#include "common.h"
#include "world.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace WorldEngine
{

std::shared_ptr<World>
WorldGen(const std::string&        name,
         uint32_t                  width,
         uint32_t                  height,
         uint32_t                  seed,
         const std::vector<float>& temps       = DEFAULT_TEMPS,
         const std::vector<float>& humids      = DEFAULT_HUMIDS,
         float                     gammaCurve  = DEFAULT_GAMMA_CURVE,
         float                     curveOffset = DEFAULT_CURVE_OFFSET,
         uint32_t                  numPlates   = DEFAULT_NUM_PLATES,
         uint32_t                  oceanLevel  = DEFAULT_OCEAN_LEVEL,
         const Step&               step        = DEFAULT_STEP,
         bool                      fadeBorders = DEFAULT_FADE_BORDERS);

} // namespace WorldEngine
