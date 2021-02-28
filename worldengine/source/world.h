#pragma once

#include "common.h"

#include <string>
#include <vector>

namespace WorldEngine
{

class World
{
private:
   explicit World();

public:
   World(const std::string&          name,
         Size                        size,
         uint32_t                    seed,
         const GenerationParameters& generationParams,
         const std::vector<float>&   temps,
         const std::vector<float>&   humids,
         float                       gammaCurve  = DEFAULT_GAMMA_CURVE,
         float                       curveOffset = DEFAULT_CURVE_OFFSET);
   ~World();

private:
   std::string          name_;
   Size                 size_;
   uint32_t             seed_;
   GenerationParameters generationParams_;
   std::vector<float>   temps_;
   std::vector<float>   humids_;
   float                gammaCurve_;
   float                curveOffset_;
};

} // namespace WorldEngine
