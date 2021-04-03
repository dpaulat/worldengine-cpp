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
    * @param hasColor Derived image has a color option
    * @param hasBlackAndWhite Derived image has a black and white option.
    */
   explicit Image(bool hasColor, bool hasBlackAndWhite);

   /**
    * @brief Construct an image
    * @param hasBlackAndWhite Derived image has a black and white option.
    * DrawImage should be overridden for the grayscale target.
    */
   explicit Image(bool hasBlackAndWhite);
   virtual ~Image();

   const bool hasColor_;
   const bool hasBlackAndWhite_;

   /**
    * @brief Draw a grayscale image from a multi_array
    * @param array Array from which to draw the image
    * @param target Target image view
    */
   void DrawGrayscaleFromArray(const boost::multi_array<float, 2>& array,
                               boost::gil::gray8_image_t::view_t& target) const;

   /**
    * @brief Draw a grayscale image from a multi_array
    * @param array Array from which to draw the image
    * @param low Minimum value to scale
    * @param high Maximum value to scale
    * @param target Target image view
    */
   void DrawGrayscaleFromArray(const boost::multi_array<float, 2>& array,
                               const float                         low,
                               const float                         high,
                               boost::gil::gray8_image_t::view_t& target) const;

   /**
    * @brief Draw a grayscale image (optional)
    * @param world A world
    * @param target Target image view
    */
   virtual void DrawImage(const World&                       world,
                          boost::gil::gray8_image_t::view_t& target);

   /**
    * @brief Draw a color image
    * @param world A world
    * @param target Target image view
    */
   virtual void DrawImage(const World&                      world,
                          boost::gil::rgb8_image_t::view_t& target);

public:
   /**
    * @brief Draw an image
    * @param world A world
    * @param filename Destination filename
    * @param blackAndWhite Draw image in black and white
    */
   void Draw(const World&       world,
             const std::string& filename,
             bool               blackAndWhite = false);
};
} // namespace WorldEngine
