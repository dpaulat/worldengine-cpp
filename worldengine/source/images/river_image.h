#pragma once

#include "image.h"

namespace WorldEngine
{
/**
 * @brief River image
 */
class RiverImage : public Image
{
public:
   RiverImage();
   ~RiverImage();

protected:
   /**
    * @brief Draw a river image
    * @param world A world
    * @param target Destination filename
    */
   void DrawImage(const World&                      world,
                  boost::gil::rgb8_image_t::view_t& target) const;

   /**
    * @brief Draw rivers on top of an existing background
    * @param world
    * @param target
    * @param factor
    */
   void DrawRivers(const World&                      world,
                   boost::gil::rgb8_image_t::view_t& target,
                   uint32_t                          factor = 1u) const;
};
} // namespace WorldEngine
