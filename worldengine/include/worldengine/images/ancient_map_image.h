#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Ancient map image
 */
class AncientMapImage : public Image
{
public:
   explicit AncientMapImage(const World& world,
                            uint32_t     seed,
                            uint32_t     scale               = 1u,
                            SeaColor     seaColor            = SeaColor::Brown,
                            bool         drawBiome           = true,
                            bool         drawRivers          = true,
                            bool         drawMountains       = true,
                            bool         drawOuterLandBorder = false);
   ~AncientMapImage();

protected:
   /**
    * @brief Draw an ancient map image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

private:
   uint32_t seed_;
   SeaColor seaColor_;
   bool     drawBiome_;
   bool     drawRivers_;
   bool     drawMountains_;
   bool     drawOuterLandBorder_;
};

boost::gil::rgb8_pixel_t Gradient(float                    value,
                                  float                    low,
                                  float                    high,
                                  boost::gil::rgb8_pixel_t lowColor,
                                  boost::gil::rgb8_pixel_t highColor);

} // namespace WorldEngine
