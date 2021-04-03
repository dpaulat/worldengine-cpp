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
   IcecapImage();
   ~IcecapImage();

protected:
   /**
    * @brief Draw a grayscale icecap image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                       world,
                  boost::gil::gray8_image_t::view_t& target);
};
} // namespace WorldEngine
