#include "precipitation.h"
#include "../basic.h"

#include <random>

#include <boost/log/trivial.hpp>

#include <OpenSimplexNoise.h>

namespace WorldEngine
{

static void PrecipitationCalculation(World& world, uint32_t seed);

void PrecipitationSimulation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(info) << "Precipitation simulation start";

   const OceanArrayType&         ocean = world.GetOceanData();
   const PrecipitationArrayType& p     = world.GetPrecipitationData();

   PrecipitationCalculation(world, seed);

   world.SetThreshold(PrecipitationLevel::Low,
                      FindThresholdF(p, 0.75f, &ocean));
   world.SetThreshold(PrecipitationLevel::Medium,
                      FindThresholdF(p, 0.3f, &ocean));
   world.SetThreshold(PrecipitationLevel::High, 0.0f);

   BOOST_LOG_TRIVIAL(info) << "Precipitation simulation finish";
}

static void PrecipitationCalculation(World& world, uint32_t seed)
{
   BOOST_LOG_TRIVIAL(debug) << "Seed: " << seed;

   // Precipitation is a value in [-1, 1]
   std::mt19937                            generator(seed);
   std::uniform_int_distribution<uint32_t> distribution(0, UINT32_MAX);

   OpenSimplexNoise::Noise noise(distribution(generator));

   uint32_t width  = world.width();
   uint32_t height = world.height();
   int32_t  border = width / 4;

   float curveGamma = world.gammaCurve();
   float curveBonus = world.curveOffset();

   const TemperatureArrayType& temperature   = world.GetTemperatureData();
   PrecipitationArrayType&     precipitation = world.GetPrecipitationData();
   precipitation.resize(boost::extents[height][width]);

   uint32_t octaves = 6;
   float    freq    = 64.0f * octaves;
   float    nScale  = 1024.0f / static_cast<float>(height);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         float n = Noise(noise, //
                         x * nScale / freq,
                         y * nScale / freq,
                         octaves);

         if (x <= border)
         {
            n *= static_cast<float>(x) / border;
            n += Noise(noise,
                       (x * nScale + width) / freq,
                       y * nScale / freq,
                       octaves) *
                 (border - x) / border;
         }

         precipitation[y][x] = n;
      }
   }

   // Find ranges
   std::pair<PrecipitationDataType*, PrecipitationDataType*> minmaxPrecip =
      std::minmax_element(precipitation.data(),
                          precipitation.data() + precipitation.num_elements());
   float minPrecip = *minmaxPrecip.first;
   float maxPrecip = *minmaxPrecip.second;

   auto minmaxTemp =
      std::minmax_element(temperature.data(), //
                          temperature.data() + temperature.num_elements());
   float minTemp = *minmaxTemp.first;
   float maxTemp = *minmaxTemp.second;

   float precipDelta = maxPrecip - minPrecip;
   float tempDelta   = maxTemp - minTemp;

   BOOST_LOG_TRIVIAL(debug)
      << "Precipitation minmax: " << minPrecip << ", " << maxPrecip;
   BOOST_LOG_TRIVIAL(debug)
      << "Temperature minmax: " << minTemp << ", " << maxTemp;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         // Normalize temperature and precipitation arrays
         float t = (temperature[y][x] - minTemp) / tempDelta;
         float p = (precipitation[y][x] - minPrecip) / precipDelta;

         /*
          * Ok, some explanation here because why the formula is doing htis may
          * be a little confusing. We are going to generate a modified gamma
          * curve based on normalized temperature and multiply our precipitation
          * amounts by it.
          *
          * std::pow(t, curveGamma) generates a standard gamma curve. However
          * we probably don't want to be multiplying precipitation by 0 at the
          * far side of the curve. To avoid this we multiply the curve by (1 -
          * curveBonus) and then add back curveBonus. Thus, if we have a curve
          * bonus of 0.2 then the range of our modified gamma curve goes from
          * 0-1 to 0-0.8 after we multiply and then to 0.2-1 after we add back
          * the curveBonus.
          *
          * Because we renormalize there is not much point to offsetting the
          * opposite end of the curve so it is less than or more than 1. We are
          * trying to avoid setting the start of the curve to 0 because f(t) * p
          * would equal 0 when t equals 0. However f(t) * p does not
          * automatically equal 1 when t equals 1 and if we raise or lower the
          * value for f(t) at 1 it would have negligible impact after
          * renormalizing.
          */

         // Modify precipitation based on temperature
         float curve = std::pow(t, curveGamma) * (1 - curveBonus) + curveBonus;
         precipitation[y][x] = p * curve;
      }
   }

   // Renormalize precipitation because the precipitiation changes will probably
   // not fully extend from -1 to 1
   minmaxPrecip =
      std::minmax_element(precipitation.data(),
                          precipitation.data() + precipitation.num_elements());
   minPrecip   = *minmaxPrecip.first;
   maxPrecip   = *minmaxPrecip.second;
   precipDelta = maxPrecip - minPrecip;

   BOOST_LOG_TRIVIAL(debug)
      << "Precipitation minmax (modified): " << minPrecip << ", " << maxPrecip;

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         precipitation[y][x] =
            (precipitation[y][x] - minPrecip) / precipDelta * 2 - 1;
      }
   }
}

} // namespace WorldEngine
