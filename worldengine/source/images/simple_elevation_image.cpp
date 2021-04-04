#include "simple_elevation_image.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{
SimpleElevationImage::SimpleElevationImage(const World& world) :
    Image(world, false)
{
}
SimpleElevationImage::~SimpleElevationImage() {}

void SimpleElevationImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const ElevationArrayType& e     = world_.GetElevationData();
   const OceanArrayType&     ocean = world_.GetOceanData();

   float seaLevel    = world_.oceanLevel();
   bool  hasOcean    = (seaLevel != NAN) && (!ocean.empty());
   float minElevLand = 10.0f;
   float maxElevLand = -10.0f;
   float minElevSea  = 10.0f;
   float maxElevSea  = -10.0f;

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (hasOcean && ocean[y][x])
         {
            if (minElevSea > e[y][x])
            {
               minElevSea = e[y][x];
            }
            if (maxElevSea < e[y][x])
            {
               maxElevSea = e[y][x];
            }
         }
         else
         {

            if (minElevLand > e[y][x])
            {
               minElevLand = e[y][x];
            }
            if (maxElevLand < e[y][x])
            {
               maxElevLand = e[y][x];
            }
         }
      }
   }

   if (hasOcean)
   {
      BOOST_LOG_TRIVIAL(debug) << "minElevSea = " << minElevSea;
      BOOST_LOG_TRIVIAL(debug) << "maxElevSea = " << maxElevSea;
   }

   BOOST_LOG_TRIVIAL(debug) << "minElevLand = " << minElevLand;
   BOOST_LOG_TRIVIAL(debug) << "maxElevLand = " << maxElevLand;

   float elevDeltaLand = (maxElevLand - minElevLand) / 11.0f;
   float elevDeltaSea  = (maxElevSea - minElevSea);
   float elevation;

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (hasOcean && ocean[y][x])
         {
            elevation = ((e[y][x] - minElevSea) / elevDeltaSea);
         }
         else
         {
            elevation = ((e[y][x] - minElevLand) / elevDeltaLand) + 1;
         }

         target(x, y) = ElevationColor(elevation, seaLevel);
      }
   }
}

boost::gil::rgb8_pixel_t SimpleElevationImage::ElevationColor(float elevation,
                                                              float seaLevel)
{
   float r, g, b;
   std::tie(r, g, b) = ElevationColorF(elevation, seaLevel);
   SatureColorComponent(r);
   SatureColorComponent(g);
   SatureColorComponent(b);

   boost::gil::rgb8_pixel_t color(static_cast<uint8_t>(r * 255),
                                  static_cast<uint8_t>(g * 255),
                                  static_cast<uint8_t>(b * 255));

   return color;
}

std::tuple<float, float, float>
SimpleElevationImage::ElevationColorF(float elevation, float seaLevel)
{
   float colorStep = 1.5f;

   if (seaLevel == NAN)
   {
      seaLevel = -1.0f;
   }

   if (elevation < seaLevel / 2.0f)
   {
      elevation /= seaLevel;
      return std::make_tuple(0.0f, 0.0f, 0.75f + 0.5f * elevation);
   }
   if (elevation < seaLevel)
   {
      elevation /= seaLevel;
      return std::make_tuple(0.0f, 2.0f * (elevation - 0.5f), 1.0f);
   }

   elevation -= seaLevel;

   if (elevation < 1.0f * colorStep)
   {
      return std::make_tuple(0.0f, 0.5f + 0.5f * elevation / colorStep, 0.0f);
   }
   if (elevation < 1.5f * colorStep)
   {
      return std::make_tuple(
         2.0f * (elevation - 1.0f * colorStep) / colorStep, 1.0f, 0.0f);
   }
   if (elevation < 2.0f * colorStep)
   {
      return std::make_tuple(
         1.0f, 1.0f - (elevation - 1.5f * colorStep) / colorStep, 0.0f);
   }
   if (elevation < 3.0f * colorStep)
   {
      return std::make_tuple(
         1.0f - 0.5f * (elevation - 2.0f * colorStep) / colorStep,
         0.5f - 0.25f * (elevation - 2.0f * colorStep) / colorStep,
         0.0f);
   }
   if (elevation < 5.0f * colorStep)
   {
      return std::make_tuple(
         0.5f - 0.125f * (elevation - 3.0f * colorStep) / (2.0f * colorStep),
         0.25f + 0.125f * (elevation - 3.0f * colorStep) / (2.0f * colorStep),
         0.375f * (elevation - 3.0f * colorStep) / 2.0f * colorStep);
   }
   if (elevation < 8.0f * colorStep)
   {
      return std::make_tuple(
         0.375f + 0.625f * (elevation - 5.0f * colorStep) / (3.0f * colorStep),
         0.375f + 0.625f * (elevation - 5.0f * colorStep) / (3.0f * colorStep),
         0.375f + 0.625f * (elevation - 5.0f * colorStep) / (3.0f * colorStep));
   }

   elevation -= 8.0f * colorStep;
   while (elevation > 2.0f * colorStep)
   {
      elevation -= 2.0f * colorStep;
   }
   return std::make_tuple(1.0f, 1.0f - elevation / 4.0f, 1.0f);
}

void SimpleElevationImage::SatureColorComponent(float& component)
{
   if (component < 0.0f)
   {
      component = 0.0f;
   }
   if (component > 1.0f)
   {
      component = 1.0f;
   }
}
} // namespace WorldEngine