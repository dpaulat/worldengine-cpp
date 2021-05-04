#include "worldengine/plates.h"
#include "worldengine/world.h"
#include "generation.h"

#include <chrono>
#include <random>

#include <boost/log/trivial.hpp>

#include <platecapi.hpp>

namespace WorldEngine
{

/**
 * @brief Perform an initial plates simulation using the Plate Tectonics library
 * @param heightmap Elevation map
 * @param platesmap Plates map
 * @param seed Random seed value
 * @param width Width in pixels
 * @param height Height in pixels
 * @param seaLevel The elevation representing the ocean level
 * @param erosionPeriod
 * @param foldingRatio
 * @param aggrOverlapAbs
 * @param aggrOverlapRel
 * @param cycleCount
 * @param numPlates Number of plates
 * @return Handle to a Plate Tectonics library object
 */
static void*
GeneratePlatesSimulation(float**    heightmap,
                         uint32_t** platesmap,
                         long       seed,
                         uint32_t   width,
                         uint32_t   height,
                         float      seaLevel       = DEFAULT_SEA_LEVEL,
                         uint32_t   erosionPeriod  = DEFAULT_EROSION_PERIOD,
                         float      foldingRatio   = DEFAULT_FOLDING_RATIO,
                         uint32_t   aggrOverlapAbs = DEFAULT_AGGR_OVERLAP_ABS,
                         float      aggrOverlapRel = DEFAULT_AGGR_OVERLAP_REL,
                         uint32_t   cycleCount     = DEFAULT_CYCLE_COUNT,
                         uint32_t   numPlates      = DEFAULT_NUM_PLATES);

/**
 * @brief Create a new world based on an initial plates simulation
 * @param name World name
 * @param width Width in pixels
 * @param height Height in pixels
 * @param seed Random seed value
 * @param temps A list of six temperatures
 * @param humids A list of seven humidity values
 * @param gammaCurve Gamma value for temperature and precipitation on gamma
 * correction curve
 * @param curveOffset Adjustment value for temperature and precipitation gamma
 * correction curve
 * @param numPlates Number of plates
 * @param oceanLevel The elevation representing the ocean level
 * @param step Generation steps to perform
 * @return A new world
 */
static std::shared_ptr<World>
PlatesSimulation(const std::string&        name,
                 uint32_t                  width,
                 uint32_t                  height,
                 uint32_t                  seed,
                 const std::vector<float>& temps       = DEFAULT_TEMPS,
                 const std::vector<float>& humids      = DEFAULT_HUMIDS,
                 float                     gammaCurve  = DEFAULT_GAMMA_CURVE,
                 float                     curveOffset = DEFAULT_CURVE_OFFSET,
                 uint32_t                  numPlates   = DEFAULT_NUM_PLATES,
                 float                     oceanLevel  = DEFAULT_OCEAN_LEVEL,
                 const Step&               step        = DEFAULT_STEP);

std::shared_ptr<World> WorldGen(const std::string&        name,
                                uint32_t                  width,
                                uint32_t                  height,
                                uint32_t                  seed,
                                const std::vector<float>& temps,
                                const std::vector<float>& humids,
                                float                     gammaCurve,
                                float                     curveOffset,
                                uint32_t                  numPlates,
                                uint32_t                  oceanLevel,
                                const Step&               step,
                                bool                      fadeBorders)
{
   std::chrono::steady_clock::time_point startTime;
   std::chrono::steady_clock::time_point endTime;

   startTime = std::chrono::steady_clock::now();

   std::shared_ptr<World> world = PlatesSimulation(name,
                                                   width,
                                                   height,
                                                   seed,
                                                   temps,
                                                   humids,
                                                   gammaCurve,
                                                   curveOffset,
                                                   numPlates,
                                                   oceanLevel,
                                                   step);

   CenterLand(*world);

   endTime = std::chrono::steady_clock::now();
   auto elapsedTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
         .count();

   BOOST_LOG_TRIVIAL(debug)
      << "WorldGen(): setElevation, setPlates, centerLand complete. "
      << "Elapsed time " << elapsedTime << "ms.";

   startTime = std::chrono::steady_clock::now();

   std::mt19937                            generator(seed);
   std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);
   AddNoiseToElevation(*world, distribution(generator));

   endTime = std::chrono::steady_clock::now();
   elapsedTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
         .count();

   BOOST_LOG_TRIVIAL(debug) << "WorldGen(): elevation noise added. "
                            << "Elapsed time " << elapsedTime << "ms.";

   startTime = std::chrono::steady_clock::now();

   if (fadeBorders)
      PlaceOceansAtMapBorders(*world);
   InitializeOceanAndThresholds(*world);

   endTime = std::chrono::steady_clock::now();
   elapsedTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
         .count();

   BOOST_LOG_TRIVIAL(debug) << "WorldGen(): oceans initialized. "
                            << "Elapsed time " << elapsedTime << "ms.";

   GenerateWorld(*world, step, distribution(generator));

   return world;
}

static void* GeneratePlatesSimulation(float**    heightmap,
                                      uint32_t** platesmap,
                                      long       seed,
                                      uint32_t   width,
                                      uint32_t   height,
                                      float      seaLevel,
                                      uint32_t   erosionPeriod,
                                      float      foldingRatio,
                                      uint32_t   aggrOverlapAbs,
                                      float      aggrOverlapRel,
                                      uint32_t   cycleCount,
                                      uint32_t   numPlates)
{
   std::chrono::steady_clock::time_point startTime;
   std::chrono::steady_clock::time_point endTime;

   startTime = std::chrono::steady_clock::now();

   void* p = platec_api_create(seed,
                               width,
                               height,
                               seaLevel,
                               erosionPeriod,
                               foldingRatio,
                               aggrOverlapAbs,
                               aggrOverlapRel,
                               cycleCount,
                               numPlates);

   // Note: To rescale the world's heightmap to roughly Earth's scale, multiply
   // by 2000

   while (!platec_api_is_finished(p))
   {
      platec_api_step(p);
   }

   *heightmap = platec_api_get_heightmap(p);
   *platesmap = platec_api_get_platesmap(p);

   endTime = std::chrono::steady_clock::now();
   auto elapsedTime =
      std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime)
         .count();

   BOOST_LOG_TRIVIAL(debug) << "GeneratePlatesSimulation() complete. "
                            << "Elapsed time " << elapsedTime << "ms.";

   return p;
}

static std::shared_ptr<World> PlatesSimulation(const std::string&        name,
                                               uint32_t                  width,
                                               uint32_t                  height,
                                               uint32_t                  seed,
                                               const std::vector<float>& temps,
                                               const std::vector<float>& humids,
                                               float       gammaCurve,
                                               float       curveOffset,
                                               uint32_t    numPlates,
                                               float       oceanLevel,
                                               const Step& step)
{
   float*    heightmap;
   uint32_t* platesmap;

   void* p = GeneratePlatesSimulation(&heightmap,
                                      &platesmap,
                                      seed,
                                      width,
                                      height,
                                      DEFAULT_SEA_LEVEL,
                                      DEFAULT_EROSION_PERIOD,
                                      DEFAULT_FOLDING_RATIO,
                                      DEFAULT_AGGR_OVERLAP_ABS,
                                      DEFAULT_AGGR_OVERLAP_REL,
                                      DEFAULT_CYCLE_COUNT,
                                      numPlates);

   std::shared_ptr<World> world = std::shared_ptr<World>(
      new World(name,
                Size(width, height),
                seed,
                GenerationParameters(numPlates, oceanLevel, step),
                temps,
                humids,
                gammaCurve,
                curveOffset));

   world->SetElevationData(heightmap);
   world->SetPlatesData(platesmap);

   platec_api_destroy(p);

   return world;
}

} // namespace WorldEngine
