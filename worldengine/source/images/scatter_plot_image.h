#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Scatter plot image
 */
class ScatterPlotImage : public Image
{
public:
   ScatterPlotImage(size_t size);
   ~ScatterPlotImage();

protected:
   /**
    * @brief Draw a scatter plot image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World& world, boost::gil::rgb8_image_t::view_t& target);

private:
   const size_t size_;
};
} // namespace WorldEngine
