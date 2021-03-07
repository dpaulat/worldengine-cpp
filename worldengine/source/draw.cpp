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

void DrawOcean(const OceanArrayType&              ocean,
               boost::gil::rgba8_image_t::view_t& target);

void DrawOcean(const OceanArrayType&              ocean,
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

} // namespace WorldEngine
