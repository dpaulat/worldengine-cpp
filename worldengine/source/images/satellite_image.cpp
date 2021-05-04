#include "worldengine/images/satellite_image.h"
#include "../basic.h"

#include <algorithm>

template<typename T>
std::tuple<T, T, T>& operator+=(std::tuple<T, T, T>&       lhs,
                                const std::tuple<T, T, T>& rhs)
{
   std::get<0>(lhs) += std::get<0>(rhs);
   std::get<1>(lhs) += std::get<1>(rhs);
   std::get<2>(lhs) += std::get<2>(rhs);
   return lhs;
}

template<typename T>
std::tuple<T, T, T> operator+(const std::tuple<T, T, T>& lhs,
                              const std::tuple<T, T, T>& rhs)
{
   return std::tuple<T, T, T>(std::get<0>(lhs) + std::get<0>(rhs),
                              std::get<1>(lhs) + std::get<1>(rhs),
                              std::get<2>(lhs) + std::get<2>(rhs));
}

template<typename T>
boost::gil::rgb8_pixel_t& operator+=(boost::gil::rgb8_pixel_t&  lhs,
                                     const std::tuple<T, T, T>& rhs)
{
   lhs[0] = std::clamp<T>(static_cast<T>(lhs[0]) + std::get<0>(rhs), 0, 255);
   lhs[1] = std::clamp<T>(static_cast<T>(lhs[1]) + std::get<1>(rhs), 0, 255);
   lhs[2] = std::clamp<T>(static_cast<T>(lhs[2]) + std::get<2>(rhs), 0, 255);
   return lhs;
}

template<typename T>
boost::gil::rgb8_pixel_t& operator+=(boost::gil::rgb8_pixel_t& lhs,
                                     const T&                  rhs)
{
   lhs[0] = std::clamp<T>(static_cast<T>(lhs[0]) + rhs, 0, 255);
   lhs[1] = std::clamp<T>(static_cast<T>(lhs[1]) + rhs, 0, 255);
   lhs[2] = std::clamp<T>(static_cast<T>(lhs[2]) + rhs, 0, 255);
   return lhs;
}

boost::multi_array<bool, 2> operator!(const boost::multi_array<bool, 2>& rhs)
{
   const uint32_t width  = rhs.shape()[1];
   const uint32_t height = rhs.shape()[0];

   boost::multi_array<bool, 2> result(boost::extents[height][width]);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         result[y][x] = !rhs[y][x];
      }
   }

   return result;
}

namespace WorldEngine
{

typedef std::tuple<int32_t, int32_t, int32_t> RgbValue;

// A random value between -NOISE_RANGE and NOISE_RANGE will be added to the RGB
// of each pixel.
static const int32_t NOISE_RANGE = 15;

// These are arbitrarily chosen elevation cutoffs for 4 different height levels.
// Some color modifiers will be applied at each level.
static const int32_t HIGH_MOUNTAIN_ELEV = 215;
static const int32_t MOUNTAIN_ELEV      = 175;
static const int32_t HIGH_HILL_ELEV     = 160;
static const int32_t HILL_ELEV          = 145;

// These are RGB color values which will be added to the noise, if the elevation
// is greater than the heigh specified. These are not cumulative.
static const RgbValue HIGH_MOUNTAIN_NOISE_MODIFIER(10, 6, 10);
static const RgbValue MOUNTAIN_NOISE_MODIFIER(-4, 12, 4);
static const RgbValue HIGH_HILL_NOISE_MODIFIER(-3, -10, -3);
static const RgbValue HILL_NOISE_MODIFIER(-2, -6, -2);

// This is the base "mountain color". Elevations above this size will have their
// colors interpolated with this color in order to give a more mountainous
// appearance.
static const boost::gil::rgb8_pixel_t MOUNTAIN_COLOR(50, 57, 28);

// If a tile is a river or a lake, the color of the tile will change by this
// amount.
static const RgbValue RIVER_COLOR_CHANGE(-12, -12, 4);
static const RgbValue LAKE_COLOR_CHANGE(-12, -12, 10);

// The normalized (0-255) value of an elevation of a tile gets divided by this
// amount, and added to a tile's color.
static const int32_t BASE_ELEVATION_INTENSITY_MODIFIER = 10;

// How many tiles to average together when comparing this tile's elevation to
// the previous tiles.
static const uint32_t SAT_SHADOW_SIZE = 5u;

// How much to multiply the difference in elevation between this tile and the
// previous tile. Higher will result in starker contrast between high and low
// areas.
static const int32_t SAT_SHADOW_DISTANCE_MULTIPLIER = 9;

// How much to vary ice color. 0 means perfectly white ice. Must be in [0, 255].
// Only affects R and G channels.
static const int32_t ICE_COLOR_VARIATION = 30;

static const std::unordered_map<Biome, boost::gil::rgb8_pixel_t>
   biomeSatelliteColors_ = {{Biome::Ocean, {23, 94, 145}},
                            {Biome::Sea, {23, 94, 145}},
                            {Biome::Ice, {255, 255, 255}},
                            {Biome::SubpolarDryTundra, {186, 199, 206}},
                            {Biome::SubpolarMoistTundra, {186, 195, 202}},
                            {Biome::SubpolarWetTundra, {186, 195, 204}},
                            {Biome::SubpolarRainTundra, {186, 200, 210}},
                            {Biome::PolarDesert, {182, 195, 201}},
                            {Biome::BorealDesert, {132, 146, 143}},
                            {Biome::CoolTemperateDesert, {183, 163, 126}},
                            {Biome::WarmTemperateDesert, {166, 142, 104}},
                            {Biome::SubtropicalDesert, {205, 181, 137}},
                            {Biome::TropicalDesert, {203, 187, 153}},
                            {Biome::BorealRainForest, {21, 29, 8}},
                            {Biome::CoolTemperateRainForest, {25, 34, 15}},
                            {Biome::WarmTemperateRainForest, {19, 28, 7}},
                            {Biome::SubtropicalRainForest, {48, 60, 24}},
                            {Biome::TropicalRainForest, {21, 38, 6}},
                            {Biome::BorealWetForest, {6, 17, 11}},
                            {Biome::CoolTemperateWetForest, {6, 17, 11}},
                            {Biome::WarmTemperateWetForest, {44, 48, 19}},
                            {Biome::SubtropicalWetForest, {23, 36, 10}},
                            {Biome::TropicalWetForest, {23, 36, 10}},
                            {Biome::BorealMoistForest, {31, 39, 18}},
                            {Biome::CoolTemperateMoistForest, {31, 39, 18}},
                            {Biome::WarmTemperateMoistForest, {36, 42, 19}},
                            {Biome::SubtropicalMoistForest, {23, 31, 10}},
                            {Biome::TropicalMoistForest, {24, 36, 11}},
                            {Biome::WarmTemperateDryForest, {52, 51, 30}},
                            {Biome::SubtropicalDryForest, {53, 56, 30}},
                            {Biome::TropicalDryForest, {54, 60, 30}},
                            {Biome::BorealDryScrub, {73, 70, 61}},
                            {Biome::CoolTemperateDesertScrub, {80, 58, 44}},
                            {Biome::WarmTemperateDesertScrub, {92, 81, 49}},
                            {Biome::SubtropicalDesertScrub, {68, 57, 35}},
                            {Biome::TropicalDesertScrub, {107, 87, 60}},
                            {Biome::CoolTemperateSteppe, {95, 82, 50}},
                            {Biome::WarmTemperateThornScrub, {77, 81, 48}},
                            {Biome::SubtropicalThornWoodland, {27, 40, 12}},
                            {Biome::TropicalThornWoodland, {40, 62, 15}},
                            {Biome::TropicalVeryDryForest, {87, 81, 49}},
                            {Biome::BareRock, {96, 96, 96}}};

SatelliteImage::SatelliteImage(const World& world, uint32_t seed) :
    Image(world, false), seed_(seed), generator_(seed)
{
}
SatelliteImage::~SatelliteImage() {}

void SatelliteImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const ElevationArrayType& elevation = world_.GetElevationData();
   const BiomeArrayType&     biomes    = world_.GetBiomeData();
   const IcecapArrayType&    icecap    = world_.GetIcecapData();
   const RiverMapArrayType&  rivermap  = world_.GetRiverMapData();
   const LakeMapArrayType&   lakemap   = world_.GetLakeMapData();

   const uint32_t width  = biomes.shape()[1];
   const uint32_t height = biomes.shape()[0];

   auto minmaxElevation = std::minmax_element(
      elevation.data(), elevation.data() + elevation.num_elements());
   const float minElevation = *minmaxElevation.first;
   const float maxElevation = *minmaxElevation.second;

   // Re-seed the engine
   generator_.seed(seed_);

   // Get an elevation mask where heights are normalized between 0 and 255
   boost::multi_array<uint8_t, 2> normalElevation(
      GetNormalizedElevationArray());

   // All land shall be smoothed (other tiles can be included by setting them to
   // true)
   boost::multi_array<bool, 2> smoothMask(!world_.GetOceanData());

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         // Set the initial pixel color based on normalized elevation
         target(x, y) = GetBiomeColor(normalElevation[y][x], x, y);
      }
   }

   // Paint frozen areas
   std::uniform_int_distribution<int32_t> variationGenerator(
      0, ICE_COLOR_VARIATION);
   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (icecap[y][x] > 0.0f)
         {
            // Smooth the frozen areas
            smoothMask[y][x]  = true;
            int32_t variation = variationGenerator(generator_);

            target(x, y) =
               boost::gil::rgb8_pixel_t(255 - ICE_COLOR_VARIATION + variation,
                                        255 - ICE_COLOR_VARIATION + variation,
                                        255);
         }
      }
   }

   // Loop through and average a pixel with its neighbors to smooth transitions
   // between biomes
   for (uint32_t y = 1; y < height - 1; y++)
   {
      for (uint32_t x = 1; x < width - 1; x++)
      {
         // Only smooth land and frozen tiles
         if (smoothMask[y][x])
         {
            // Lists to hold the separated RGB values of the neighboring pixels
            std::list<uint32_t> r;
            std::list<uint32_t> g;
            std::list<uint32_t> b;

            // Loop through this pixel and all neighboring pixels
            for (uint32_t j = y - 1; j <= y + 1; j++)
            {
               for (uint32_t i = x - 1; i <= x + 1; i++)
               {
                  // Don't include ocean in smoothing, if this tile happens to
                  // border an ocean
                  if (smoothMask[j][i])
                  {
                     // Grab each RGB value and append to the list
                     r.push_back(target(i, j)[0]);
                     g.push_back(target(i, j)[1]);
                     b.push_back(target(i, j)[2]);
                  }
               }
            }

            // Make sure there is at least one valid tile to be smoothed before
            // we attempt to average the values
            if (!r.empty())
            {
               int32_t avgR = std::accumulate(r.begin(), r.end(), 0) / r.size();
               int32_t avgG = std::accumulate(g.begin(), g.end(), 0) / g.size();
               int32_t avgB = std::accumulate(b.begin(), b.end(), 0) / b.size();

               // Set the color of the pixel again
               target(x, y) = boost::gil::rgb8_pixel_t(avgR, avgG, avgB);
            }
         }
      }
   }

   // After smoothing, draw rivers
   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         // Color rivers
         if (world_.IsLand(x, y) && rivermap[y][x] > 0.0f)
         {
            target(x, y) += RIVER_COLOR_CHANGE;
         }

         // Color lakes
         if (world_.IsLand(x, y) && lakemap[y][x] > 0.0f)
         {
            target(x, y) += LAKE_COLOR_CHANGE;
         }
      }
   }

   // "Shade" the map by sending beams of light west to east, and increasing or
   // decreasing value of pixel based on elevation difference
   for (uint32_t y = SAT_SHADOW_SIZE; y < height; y++)
   {
      for (uint32_t x = SAT_SHADOW_SIZE; x < width; x++)
      {
         if (world_.IsLand(x, y))
         {
            std::list<float> prevElevs;

            // Build up list of elevations in the previous n tiles, where n is
            // the shadow size. This goes northwest to southeast.
            for (uint32_t n = 1; n <= SAT_SHADOW_SIZE; n++)
            {
               prevElevs.push_back(elevation[y - n][x - n]);
            }

            // Take the average of the height of the previous n tiles
            float avgPrevElev =
               std::accumulate(prevElevs.begin(), prevElevs.end(), 0.0f) /
               prevElevs.size();

            // Find the difference between this tile's elevation, and the
            // average of the previous elevations
            float difference = elevation[y][x] - avgPrevElev;

            // Amplify the difference
            int32_t adjustedDifference =
               difference * SAT_SHADOW_DISTANCE_MULTIPLIER;

            // The amplified difference is now translated into the RGB of the
            // tile. This adds light to tiles higher than the previous average,
            // and shadow to tiles lower than the previous average.
            target(x, y) += adjustedDifference;
         }
      }
   }
}

boost::gil::rgb8_pixel_t
SatelliteImage::AverageColors(boost::gil::rgb8_pixel_t c1,
                              boost::gil::rgb8_pixel_t c2)
{
   return boost::gil::rgb8_pixel_t(
      (c1[0] + c2[0]) / 2, (c1[1] + c2[1]) / 2, (c1[2] + c2[2]) / 2);
}

boost::gil::rgb8_pixel_t SatelliteImage::GetBiomeColor(
   const uint32_t normalElevation, const uint32_t x, const uint32_t y)
{
   Biome biome = world_.GetBiome(x, y);

   boost::gil::rgb8_pixel_t biomeColor = biomeSatelliteColors_.at(biome);

   // Default is no noise
   RgbValue noise(0, 0, 0);

   std::uniform_int_distribution<int32_t> noiseDistribution(-NOISE_RANGE,
                                                            NOISE_RANGE);

   if (world_.IsLand(x, y))
   {
      // Generate some random noise to apply to this pixel. Ther eis noise for
      // each element of the rgb value. This noise will be furhter modified by
      // the height of this tile.

      // Draw three random numbers at once
      noise = std::make_tuple(noiseDistribution(generator_),
                              noiseDistribution(generator_),
                              noiseDistribution(generator_));

      if (normalElevation > HIGH_MOUNTAIN_ELEV)
      {
         // Elevation is very high
         noise += HIGH_MOUNTAIN_NOISE_MODIFIER;
         biomeColor = AverageColors(biomeColor, MOUNTAIN_COLOR);
      }
      else if (normalElevation > MOUNTAIN_ELEV)
      {
         // Elevation is high
         noise += MOUNTAIN_NOISE_MODIFIER;
         biomeColor = AverageColors(biomeColor, MOUNTAIN_COLOR);
      }
      else if (normalElevation > HIGH_HILL_ELEV)
      {
         // Elevation is somewhat high
         noise += HIGH_HILL_NOISE_MODIFIER;
      }
      else if (normalElevation > HILL_ELEV)
      {
         // Elevation is a little bit high
         noise += HILL_NOISE_MODIFIER;
      }
   }

   // There is also a minor base modifier to the pixel's RGB value based on
   // height
   int32_t modificationAmount =
      static_cast<int>(normalElevation / BASE_ELEVATION_INTENSITY_MODIFIER);
   RgbValue baseElevationModifier(
      modificationAmount, modificationAmount, modificationAmount);

   biomeColor += noise + baseElevationModifier;

   return biomeColor;
}

boost::multi_array<uint8_t, 2>
SatelliteImage::GetNormalizedElevationArray() const
{
   const ElevationArrayType& e     = world_.GetElevationData();
   const OceanArrayType&     ocean = world_.GetOceanData();

   float minElevLand = 10.0f;
   float maxElevLand = -10.0f;
   float minElevSea  = 10.0f;
   float maxElevSea  = -10.0f;

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (ocean[y][x])
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

   float elevDeltaLand = maxElevLand - minElevLand;
   float elevDeltaSea  = maxElevSea - minElevSea;

   boost::multi_array<uint8_t, 2> c(
      boost::extents[world_.height()][world_.width()]);

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (ocean[y][x])
         {
            c[y][x] = std::roundf((e[y][x] - minElevSea) * 127 / elevDeltaSea);
         }
         else
         {
            c[y][x] =
               std::roundf((e[y][x] - minElevLand) * 127 / elevDeltaLand + 128);
         }
      }
   }

   return c;
}

} // namespace WorldEngine
