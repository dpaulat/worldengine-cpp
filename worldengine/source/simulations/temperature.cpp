#include "temperature.h"
#include "../basic.h"

#include <algorithm>
#include <random>

#include <boost/log/trivial.hpp>

#include <OpenSimplexNoise.h>

namespace WorldEngine
{

static const float SQRT_2XLN2 = sqrtf(2 * log(2));

static void TemperatureCalculation(World&              world,
                                   uint32_t            seed,
                                   ElevationArrayType& elevation,
                                   float               mountainLevel);

void TemperatureSimulation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(info) << "Temperature simulation start";

   ElevationArrayType& elevation = world.GetElevationData();
   float mountainLevel = world.GetThreshold(ElevationThresholdType::Mountain);
   const OceanArrayType&       ocean = world.GetOceanData();
   const TemperatureArrayType& t     = world.GetTemperatureData();

   TemperatureCalculation(world, seed, elevation, mountainLevel);

   world.SetThreshold(TemperatureType::Polar,
                      FindThresholdF(t, world.temps()[5], &ocean));
   world.SetThreshold(TemperatureType::Alpine,
                      FindThresholdF(t, world.temps()[4], &ocean));
   world.SetThreshold(TemperatureType::Boreal,
                      FindThresholdF(t, world.temps()[3], &ocean));
   world.SetThreshold(TemperatureType::Cool,
                      FindThresholdF(t, world.temps()[2], &ocean));
   world.SetThreshold(TemperatureType::Warm,
                      FindThresholdF(t, world.temps()[1], &ocean));
   world.SetThreshold(TemperatureType::Subtropical,
                      FindThresholdF(t, world.temps()[0], &ocean));
   world.SetThreshold(TemperatureType::Tropical,
                      std::numeric_limits<float>::max());

   BOOST_LOG_TRIVIAL(info) << "Temperature simulation finish";
}

static void TemperatureCalculation(World&              world,
                                   uint32_t            seed,
                                   ElevationArrayType& elevation,
                                   float               mountainLevel)
{
   BOOST_LOG_TRIVIAL(debug) << "Seed: " << seed;

   std::default_random_engine              generator(seed);
   std::uniform_int_distribution<uint32_t> distribution;

   OpenSimplexNoise::Noise noise(distribution(generator));

   uint32_t width  = world.width();
   uint32_t height = world.height();

   TemperatureArrayType& temperature = world.GetTemperatureData();
   temperature.resize(boost::extents[height][width]);

   /*
    * Set up variables to take care of some orbital paramters:
    * distanceToSun:
    *     - Earth-like planet = 1.0
    *     - Valid range between ~0.7 and ~1.3
    *       See https://en.wikipedia.org/wiki/Circumstellar_habitable_zone
    *     - Random value chosen via normal (Gaussian) distribution
    *       See https://en.wikipedia.org/wiki/Gaussian_function
    *     - Width of distrubiton around 1.0 is determined by HWHM (half width at
    *       half maximum)
    *     - HWHM is used to calculate the random scale
    *       sigma = HWHM / sqrt(2*ln(2))
    *     - *only HWHM* should be considered a parameter
    *     - most likely outcomes can be estimated
    *       HWHM * sqrt(2*ln(10)) / sqrt(2*ln(2)) = HWHM * 1.822615728
    *       e.g., for HWHM = 0.12: 0.78 < distanceToSun < 1.22
    * axialTilt:
    *     - The world/planet may move around its star at an angle
    *       See https://en.wikipedia.org/wiki/Axial_tilt
    *     - A value of 0.5 here would refer to an angle of 90 degrees,
    *       Uranus-style
    *       See https://en.wikipedia.org/wiki/Uranus
    *     - This value should usually be in the range -0.15 < axialTilt < 0.15
    *       for a habitable planet
    */

   const float distanceToSunHWHM = 0.12f;
   const float axialTiltHWHM     = 0.07f;

   // Derive parameters
   float distanceToSun = std::normal_distribution<float>(
      1.0f, distanceToSunHWHM / SQRT_2XLN2)(generator);
   // Clamp value, no planets inside the star
   distanceToSun = std::max(0.1f, distanceToSun);
   // Prepare for later usage, use inverse-square law
   distanceToSun *= distanceToSun;
   // An atmosphere would soften the effect of distanceToSun by *some* factor

   float axialTilt = std::normal_distribution<float>(
      0.0f, axialTiltHWHM / SQRT_2XLN2)(generator);
   axialTilt = std::clamp(axialTilt, -0.5f, 0.5f);

   BOOST_LOG_TRIVIAL(debug) << "Axial tilt: " << axialTilt;

   int32_t  border  = width / 4;
   uint32_t octaves = 8; // Number of passes for noise generator
   float    freq    = 16.0f * octaves;
   float    nScale  = 1024.0f / static_cast<float>(height);

   std::vector<std::pair<float, float>> points = {{axialTilt - 0.5f, 0.0f}, //
                                                  {axialTilt, 1.0f},
                                                  {axialTilt + 0.5f, 0.0f}};

   for (uint32_t y = 0; y < height; y++)
   {
      // yScaled = -0.5..0.5
      float yScaled = static_cast<float>(y) / static_cast<float>(height) - 0.5f;

      // Map/linearly interpolate yScaled to latitude measured from where the
      // most sunlight hits the world:
      //     1.0 = hottest zone
      //     0.0 = coldest zone
      float latitudeFactor = Interpolate(yScaled, points);

      for (uint32_t x = 0; x < width; x++)
      {
         float n = Noise(noise, //
                         x * nScale / freq,
                         y * nScale / freq,
                         octaves);

         // Added to allow noise pattern to wrap around right and left
         if (x <= border)
         {
            n *= static_cast<float>(x) / border;
            n += Noise(noise,
                       (x * nScale + width) / freq,
                       y * nScale / freq,
                       octaves) *
                 (border - x) / border;
         }

         float t = (latitudeFactor * 12 + n) / 13.0f / distanceToSun;

         // Vary temperature based on height
         if (elevation[y][x] > mountainLevel)
         {
            float altitudeFactor;

            if (elevation[y][x] > mountainLevel + 29)
            {
               altitudeFactor = 0.033f;
            }
            else
            {
               altitudeFactor =
                  1.0f - (elevation[y][x] - mountainLevel) / 30.0f;
            }

            t *= altitudeFactor;
         }

         temperature[y][x] = t;
      }
   }
}

} // namespace WorldEngine
