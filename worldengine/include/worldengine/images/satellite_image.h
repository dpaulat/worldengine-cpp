#pragma once

#include "image.h"

#include <random>

namespace WorldEngine
{
/**
 * @brief Satellite image
 */
class SatelliteImage : public Image
{
public:
   explicit SatelliteImage(const World& world, uint32_t seed);
   ~SatelliteImage();

protected:
   /**
    * @brief Draw a satellite image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

private:
   std::default_random_engine generator_;

   /**
    * @brief Take the average of two colors
    * @param c1
    * @param c2
    * @return
    */
   static boost::gil::rgb8_pixel_t AverageColors(boost::gil::rgb8_pixel_t c1,
                                                 boost::gil::rgb8_pixel_t c2);

   /**
    * @brief This is the "business logic" for determining the base biome color
    * in satellite view. This includes generating some "noise" at each spot in a
    * pixels' rgb value, potentially modifying the noise based on elevation, and
    * finally incorporating this with the base biome color.
    *
    * The basic rules regarding noise generation are:
    * - Oceans have no noise added
    * - Land tiles start with noise somewhere inside (-NOISE_RANGE, NOISE_RANGE)
    * for each rgb value
    * - Land tiles with high elevations further modify the noise by set amounts
    * to drain some of the color and make the map look more like mountains)
    *
    * The biome's base color may be interpolated with a predefined mountain
    * brown color if the elevation is high enough.
    *
    * Finally, the noise plus the biome color are added and returned.
    * @param normalElevation
    * @param x
    * @param y
    * @return
    */
   boost::gil::rgb8_pixel_t GetBiomeColor(const uint32_t normalElevation,
                                          const uint32_t x,
                                          const uint32_t y);

   /**
    * @brief Convert raw elevation into normalized values between 0 and 255
    * @return
    */
   boost::multi_array<uint8_t, 2>
   GetNormalizedElevationArray() const;
};
} // namespace WorldEngine
