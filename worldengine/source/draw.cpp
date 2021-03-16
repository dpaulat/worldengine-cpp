#include "draw.h"
#include "world.h"

#include <cstdint>
#include <string>

#include <boost/gil.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/log/trivial.hpp>

namespace WorldEngine
{
//
// For draw_satellite
//

// A random value between -NOISE_RANGE and NOISE_RANGE will be added to the RGB
// of each pixel
const int32_t NOISE_RANGE = 15;

// These are arbitrarily-chosen elevation cutoffs for 4 different height levels.
// Some color modifiers will be applied at each level.
const int32_t HIGH_MOUNTAIN_ELEV = 215;
const int32_t MOUNTAIN_ELEV      = 175;
const int32_t HIGH_HILL_ELEV     = 160;
const int32_t HILL_ELEV          = 145;

// These are RGB color values which will be added to the noise, if the elevation
// is greater than the height specified.  These are not cumulative.

// This is the base "mountain color".  Elevations above this size will have
// their colors interpolated with this color in order to give a more mountainous
// appearance.

// If a tile is a river or a lake, the color of the tile will change by this
// amount.

// The normalized (0-255) value of an elevation of a tile gets divided by this
// amount, and added to a tile's color.
const uint32_t BASE_ELEVATION_INTENSITY_MODIFIER = 10;

// How many tiles to average together when comparing this tile's elevation to
// the previous tiles.
const uint32_t SAT_SHADOW_SIZE = 5;

// How much to multiply the difference in elevation between this tile and the
// previous tile. Higher will result in starker contrast between high and low
// areeas.
const uint32_t SAT_SHADOW_DISTANCE_MULTIPLIER = 9;

//
// End values for draw_satellite
//

static void DrawOcean(const OceanArrayType&              ocean,
                      boost::gil::rgba8_image_t::view_t& target);
static void DrawSimpleElevation(const World&                       world,
                                float                              seaLevel,
                                boost::gil::rgba8_image_t::view_t& target);
static void DrawTemperatureLevels(const World&                       world,
                                  boost::gil::rgba8_image_t::view_t& target,
                                  bool blackAndWhite = false);

static boost::gil::rgba8_pixel_t       ElevationColor(float elevation,
                                                      float seaLevel);
static std::tuple<float, float, float> ElevationColorF(float elevation,
                                                       float seaLevel = 1.0f);
static void                            SatureColorComponent(float& component);

void DrawBiomeOnFile(const World& world, const std::string& filename)
{
   // TODO
}

void DrawOceanOnFile(const OceanArrayType& ocean, const std::string& filename)
{
   uint32_t width  = ocean.shape()[1];
   uint32_t height = ocean.shape()[0];

   boost::gil::rgba8_image_t         image(width, height);
   boost::gil::rgba8_image_t::view_t view = boost::gil::view(image);

   DrawOcean(ocean, view);

   try
   {
      boost::gil::write_view(filename, view, boost::gil::png_tag());
   }
   catch (const std::exception& ex)
   {
      BOOST_LOG_TRIVIAL(error) << ex.what();
   }
}

void DrawPrecipitationOnFile(const World&       world,
                             const std::string& filename,
                             bool               blackAndWhite)
{
   // TODO
}

void DrawSimpleElevationOnFile(const World&       world,
                               const std::string& filename,
                               float              seaLevel)
{
   boost::gil::rgba8_image_t         image(world.width(), world.height());
   boost::gil::rgba8_image_t::view_t view = boost::gil::view(image);

   DrawSimpleElevation(world, seaLevel, view);

   try
   {
      boost::gil::write_view(filename, view, boost::gil::png_tag());
   }
   catch (const std::exception& ex)
   {
      BOOST_LOG_TRIVIAL(error) << ex.what();
   }
}

void DrawTemperatureLevelsOnFile(const World&       world,
                                 const std::string& filename,
                                 bool               blackAndWhite)
{
   boost::gil::rgba8_image_t         image(world.width(), world.height());
   boost::gil::rgba8_image_t::view_t view = boost::gil::view(image);

   DrawTemperatureLevels(world, view, blackAndWhite);

   try
   {
      boost::gil::write_view(filename, view, boost::gil::png_tag());
   }
   catch (const std::exception& ex)
   {
      BOOST_LOG_TRIVIAL(error) << ex.what();
   }
}

static void DrawOcean(const OceanArrayType&              ocean,
                      boost::gil::rgba8_image_t::view_t& target)
{
   static const boost::gil::rgba8_pixel_t oceanColor(0, 0, 255, 255);
   static const boost::gil::rgba8_pixel_t landColor(0, 255, 255, 255);

   uint32_t width  = ocean.shape()[1];
   uint32_t height = ocean.shape()[0];

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (ocean[y][x])
         {
            target(x, y) = oceanColor;
         }
         else
         {
            target(x, y) = landColor;
         }
      }
   }
}

static void DrawSimpleElevation(const World&                       world,
                                float                              seaLevel,
                                boost::gil::rgba8_image_t::view_t& target)
{
   const ElevationArrayType& e     = world.GetElevationData();
   const OceanArrayType&     ocean = world.GetOceanData();

   bool  hasOcean    = (seaLevel != NAN) && (!ocean.empty());
   float minElevLand = 10.0f;
   float maxElevLand = -10.0f;
   float minElevSea  = 10.0f;
   float maxElevSea  = -10.0f;

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
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

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
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

static void DrawTemperatureLevels(const World&                       world,
                                  boost::gil::rgba8_image_t::view_t& target,
                                  bool blackAndWhite)
{
   if (blackAndWhite)
   {
      // TODO
   }
   else
   {
      for (uint32_t y = 0; y < world.height(); y++)
      {
         for (uint32_t x = 0; x < world.width(); x++)
         {
            switch (world.GetTemperatureType(x, y))
            {
            case TemperatureType::Polar:
               target(x, y) = boost::gil::rgba8_pixel_t(0, 0, 255, 255);
               break;

            case TemperatureType::Alpine:
               target(x, y) = boost::gil::rgba8_pixel_t(42, 0, 213, 255);
               break;

            case TemperatureType::Boreal:
               target(x, y) = boost::gil::rgba8_pixel_t(85, 0, 170, 255);
               break;

            case TemperatureType::Cool:
               target(x, y) = boost::gil::rgba8_pixel_t(128, 0, 128, 255);
               break;

            case TemperatureType::Warm:
               target(x, y) = boost::gil::rgba8_pixel_t(170, 0, 85, 255);
               break;

            case TemperatureType::Subtropical:
               target(x, y) = boost::gil::rgba8_pixel_t(213, 0, 42, 255);
               break;

            case TemperatureType::Tropical:
            default:
               target(x, y) = boost::gil::rgba8_pixel_t(255, 0, 0, 255);
               break;
            }
         }
      }
   }
}

static boost::gil::rgba8_pixel_t ElevationColor(float elevation, float seaLevel)
{
   float r, g, b;
   std::tie(r, g, b) = ElevationColorF(elevation, seaLevel);
   SatureColorComponent(r);
   SatureColorComponent(g);
   SatureColorComponent(b);

   boost::gil::rgba8_pixel_t color(static_cast<uint8_t>(r * 255),
                                   static_cast<uint8_t>(g * 255),
                                   static_cast<uint8_t>(b * 255),
                                   255);

   return color;
}

static std::tuple<float, float, float> ElevationColorF(float elevation,
                                                       float seaLevel)
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

static void SatureColorComponent(float& component)
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
