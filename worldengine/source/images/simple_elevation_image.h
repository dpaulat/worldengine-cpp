#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Simple elevation image
 */
class SimpleElevationImage : public Image
{
public:
   SimpleElevationImage();
   ~SimpleElevationImage();

protected:
   /**
    * @brief Draw a simple elevation image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                      world,
                  boost::gil::rgb8_image_t::view_t& target) const;

private:
   static boost::gil::rgb8_pixel_t ElevationColor(float elevation,
                                                  float seaLevel);
   static std::tuple<float, float, float>
               ElevationColorF(float elevation, float seaLevel = 1.0f);
   static void SatureColorComponent(float& component);
};
} // namespace WorldEngine
