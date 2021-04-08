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
   explicit RiverImage(const World& world);
   ~RiverImage();

protected:
   /**
    * @brief Draw a river image
    * @param target Destination filename
    */
   void DrawImage(boost::gil::rgb8_image_t::view_t& target) override;

   /**
    * @brief Draw rivers on top of an existing background
    * @param target
    * @param factor
    */
   void DrawRivers(boost::gil::rgb8_image_t::view_t& target,
                   uint32_t                          factor = 1u) const;
};
} // namespace WorldEngine
