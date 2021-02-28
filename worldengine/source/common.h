#pragma once

#include <string>

namespace WorldEngine
{

struct Step
{
   std::string name_;
   bool        includePlates_;
   bool        includePrecipitations_;
   bool        includeErosion_;
   bool        includeBiome_;

   Step(const std::string& name,
        bool               includePlates,
        bool               includePrecipitations,
        bool               includeErosion,
        bool               includeBiome) :
       name_(name),
       includePlates_(includePlates),
       includePrecipitations_(includePrecipitations),
       includeErosion_(includeErosion),
       includeBiome_(includeBiome)
   {
   }
};

struct Size
{
   uint32_t width_;
   uint32_t height_;

   Size(uint32_t width, uint32_t height) : width_(width), height_(height) {}
};

struct GenerationParameters
{
   uint32_t numPlates_;
   float    oceanLevel_;
   Step     step_;

   GenerationParameters(uint32_t    numPlates,
                        float       oceanLevel,
                        const Step& step) :
       numPlates_(numPlates), oceanLevel_(oceanLevel), step_(step)
   {
   }
};

const Step STEP_PLATES("plates", true, false, false, false);
const Step STEP_PRECIPITATIONS("precipitations", true, true, false, false);
const Step STEP_FULL("full", true, true, true, true);

const float    DEFAULT_SEA_LEVEL        = 0.65f;
const uint32_t DEFAULT_EROSION_PERIOD   = 60u;
const float    DEFAULT_FOLDING_RATIO    = 0.02f;
const uint32_t DEFAULT_AGGR_OVERLAP_ABS = 1000000u;
const float    DEFAULT_AGGR_OVERLAP_REL = 0.33f;
const uint32_t DEFAULT_CYCLE_COUNT      = 2u;
const uint32_t DEFAULT_NUM_PLATES       = 10u;
const float    DEFAULT_GAMMA_CURVE      = 1.25f;
const float    DEFAULT_CURVE_OFFSET     = 0.2f;
const float    DEFAULT_OCEAN_LEVEL      = 1.0f;
const Step     DEFAULT_STEP             = STEP_FULL;
const bool     DEFAULT_FADE_BORDERS     = true;

} // namespace WorldEngine
