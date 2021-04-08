#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief Icecap image
 */
class IcecapImage : public Image
{
public:
   explicit IcecapImage(const World& world);
   ~IcecapImage();

protected:
   /**
    * @brief Draw a grayscale icecap image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::gray8_image_t::view_t& target) override;
};
} // namespace WorldEngine
