#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Elevation image
 */
class ElevationImage : public Image
{
public:
   explicit ElevationImage(const World& world, bool shadow);
   ~ElevationImage();

protected:
   /**
    * @brief Draw an elevation image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

private:
   const bool shadow_;
};
} // namespace WorldEngine
