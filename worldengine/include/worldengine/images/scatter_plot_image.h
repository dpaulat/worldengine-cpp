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
   explicit ScatterPlotImage(const World& world, uint32_t size);
   ~ScatterPlotImage();

protected:
   /**
    * @brief Draw a scatter plot image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

private:
   const uint32_t size_;
};
} // namespace WorldEngine
