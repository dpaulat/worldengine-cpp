#include "image.h"

#include <boost/gil/extension/io/png.hpp>
#include <boost/log/trivial.hpp>

namespace WorldEngine
{
Image::Image(bool hasBlackAndWhite) :
    hasBlackAndWhite_(hasBlackAndWhite)
{
}

Image::~Image() {}

void Image::DrawImage(const World&                       world,
                      boost::gil::gray8_image_t::view_t& target) const
{
   // Empty grayscale implementation
}

void Image::Draw(const World&       world,
                 const std::string& filename,
                 bool               blackAndWhite) const
{
   if (!blackAndWhite || !hasBlackAndWhite_)
   {
      boost::gil::rgba8_image_t         image(world.width(), world.height());
      boost::gil::rgba8_image_t::view_t view = boost::gil::view(image);

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
