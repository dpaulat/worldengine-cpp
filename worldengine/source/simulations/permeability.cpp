#include "temperature.h"
#include "../basic.h"

#include <random>

#include <boost/log/trivial.hpp>
#include <boost/random.hpp>

#include <OpenSimplexNoise.h>

namespace WorldEngine
{

static void PermeabilityCalculation(World& world, uint32_t seed);

void PermeabilitySimulation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(info) << "Permeability simulation start";

   const PermeabilityArrayType& perm  = world.GetPermeabilityData();
   const OceanArrayType&        ocean = world.GetOceanData();

   PermeabilityCalculation(world, seed);

   world.SetThreshold(PermeabilityLevel::Low,
                      FindThresholdF(perm, 0.75f, &ocean));
   world.SetThreshold(PermeabilityLevel::Medium,
                      FindThresholdF(perm, 0.25f, &ocean));
   world.SetThreshold(PermeabilityLevel::High,
                      std::numeric_limits<float>::max());

   BOOST_LOG_TRIVIAL(info) << "Permeability simulation finish";
}

static void PermeabilityCalculation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(debug) << "Seed: " << seed;

   std::mt19937                                      generator(seed);
   boost::random::uniform_int_distribution<uint32_t> distribution;

   OpenSimplexNoise::Noise noise(distribution(generator));

   uint32_t width  = world.width();
   uint32_t height = world.height();

   PermeabilityArrayType& perm = world.GetPermeabilityData();
   perm.resize(boost::extents[height][width]);

   const uint32_t octaves = 6u;
   const float    freq    = 64.0f * octaves;
   const float    nScale  = 1.0f;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         double n   = Noise(noise, //
                          x * nScale / freq,
                          y * nScale / freq,
                          octaves);
         perm[y][x] = static_cast<float>(n);
      }
   }
}

} // namespace WorldEngine
