#include "image.h"
#include "../basic.h"

#include <boost/gil/extension/io/png.hpp>
#include <boost/log/trivial.hpp>

namespace WorldEngine
{
Image::Image(bool hasColor, bool hasBlackAndWhite) :
    hasColor_(hasColor), hasBlackAndWhite_(hasBlackAndWhite)
{
}

Image::Image(bool hasBlackAndWhite) : Image(true, hasBlackAndWhite) {}

Image::~Image() {}

void Image::DrawGrayscaleFromArray(
   const boost::multi_array<float, 2>& array,
   boost::gil::gray8_image_t::view_t&  target) const
{
   auto minmax =
      std::minmax_element(array.data(), array.data() + array.num_elements());
   const float low  = *minmax.first;
   const float high = *minmax.second;

   DrawGrayscaleFromArray(array, low, high, target);
}

void Image::DrawGrayscaleFromArray(
   const boost::multi_array<float, 2>& array,
   const float                         low,
   const float                         high,
   boost::gil::gray8_image_t::view_t&  target) const
{
   const uint32_t width   = array.shape()[1];
   const uint32_t height  = array.shape()[0];
   const uint32_t floor   = 0u;
   const uint32_t ceiling = UINT8_MAX;

   const std::vector<std::pair<float, uint32_t>> points = {{low, floor},
                                                           {high, ceiling}};

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         uint32_t color = Interpolate(array[y][x], points);
         target(x, y)   = boost::gil::gray8_pixel_t(color);
      }
   }
}

void Image::DrawImage(const World&                       world,
                      boost::gil::gray8_image_t::view_t& target)
{
   // Empty grayscale implementation
}

void Image::DrawImage(const World&                      world,
                      boost::gil::rgb8_image_t::view_t& target)
{
   // Empty color implementation
}

void Image::Draw(const World&       world,
                 const std::string& filename,
                 bool               blackAndWhite)
{
   if ((!blackAndWhite || !hasBlackAndWhite_) && hasColor_)
   {
      boost::gil::rgb8_image_t         image(world.width(), world.height());
      boost::gil::rgb8_image_t::view_t view = boost::gil::view(image);

      DrawImage(world, view);

      try
      {
         boost::gil::write_view(filename, view, boost::gil::png_tag());
      }
      catch (const std::exception& ex)
      {
         BOOST_LOG_TRIVIAL(error) << ex.what();
      }
   }
   else
   {
      boost::gil::gray8_image_t         image(world.width(), world.height());
      boost::gil::gray8_image_t::view_t view = boost::gil::view(image);

      DrawImage(world, view);

      try
      {
         boost::gil::write_view(filename, view, boost::gil::png_tag());
      }
      catch (const std::exception& ex)
      {
         BOOST_LOG_TRIVIAL(error) << ex.what();
      }
   }
}
} // namespace WorldEngine
