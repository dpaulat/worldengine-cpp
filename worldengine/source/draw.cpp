#include <cstdint>

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

} // namespace WorldEngine
