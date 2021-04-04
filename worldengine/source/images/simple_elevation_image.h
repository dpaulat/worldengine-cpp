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
   explicit SimpleElevationImage(const World& world);
   ~SimpleElevationImage();

protected:
   /**
    * @brief Draw a simple elevation image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

private:
   static boost::gil::rgb8_pixel_t ElevationColor(float elevation,
                                                  float seaLevel);
   static std::tuple<float, float, float>
               ElevationColorF(float elevation, float seaLevel = 1.0f);
   static void SatureColorComponent(float& component);
};
} // namespace WorldEngine
