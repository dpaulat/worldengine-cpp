#pragma once

#include "worldengine/world.h"

#include <boost/gil.hpp>

namespace WorldEngine
{
/**
 * @brief Base abstract image class
 */
class Image
{
private:
   explicit Image();

protected:
   /**
    * @brief Construct an image
    * @param world A world
    */
   explicit Image(const World& world);

   /**
    * @brief Construct an image
    * @param world A world
    * @param scale Image scale
    */
   explicit Image(const World& world, size_t scale);

   /**
    * @brief Construct an image
    * @param world A world
    * @param hasBlackAndWhite Derived image has a black and white option.
    * DrawImage should be overridden for the grayscale target.
    */
   explicit Image(const World& world, bool hasBlackAndWhite);

   /**
    * @brief Construct an image
    * @param world A world
    * @param hasColor Derived image has a color option
    * @param hasBlackAndWhite Derived image has a black and white option.
    */
   explicit Image(const World& world, bool hasColor, bool hasBlackAndWhite);

   /**
    * @brief Construct an image
    * @param world A world
    * @param hasColor Derived image has a color option
    * @param hasBlackAndWhite Derived image has a black and white option.
    * @param scale Image scale
    */
   explicit Image(const World& world,
                  bool         hasColor,
                  bool         hasBlackAndWhite,
                  size_t       scale);
   virtual ~Image();

   const World& world_;
   const bool   hasColor_;
   const bool   hasBlackAndWhite_;
   const size_t scale_;

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
    * @param target Target image view
    */
   virtual void DrawImage(boost::gil::gray8_image_t::view_t& target);

   /**
    * @brief Draw a color image
    * @param target Target image view
    */
   virtual void DrawImage(boost::gil::rgb8_image_t::view_t& target);

   /**
    * @brief Draw rivers on top of an existing background
    * @param target
    */
   void DrawRivers(boost::gil::rgb8_image_t::view_t& target) const;

public:
   /**
    * @brief Draw an image
    * @param filename Destination filename
    * @param blackAndWhite Draw image in black and white
    */
   void Draw(const std::string& filename, bool blackAndWhite = false);
};
} // namespace WorldEngine
