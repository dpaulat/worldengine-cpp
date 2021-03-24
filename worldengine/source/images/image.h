#pragma once

#include "../world.h"

#include <boost/gil.hpp>

namespace WorldEngine
{
/**
 * @brief Base abstract image class
 */
class Image
{
protected:
   /**
    * @brief Construct an image
    * @param hasBlackAndWhite Derived image has a black and white option.
    * DrawImage should be overridden for the grayscale target.
    */
   explicit Image(bool hasBlackAndWhite);
   virtual ~Image();

   const bool hasBlackAndWhite_;

   /**
    * @brief Draw a grayscale image (optional)
    * @param world A world
    * @param target Target image view
    */
   virtual void DrawImage(const World&                       world,
                          boost::gil::gray8_image_t::view_t& target) const;

   /**
    * @brief Draw a color image
    * @param world A world
    * @param target Target image view
    */
   virtual void DrawImage(const World&                      world,
                          boost::gil::rgb8_image_t::view_t& target) const = 0;

public:
   /**
    * @brief Draw an image
    * @param world A world
    * @param filename Destination filename
    * @param blackAndWhite Draw image in black and white
    */
   void Draw(const World&       world,
             const std::string& filename,
             bool               blackAndWhite = false) const;
};
} // namespace WorldEngine
