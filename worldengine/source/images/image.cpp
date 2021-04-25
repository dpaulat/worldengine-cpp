#include "worldengine/images/image.h"
#include "../basic.h"

#include <boost/gil/extension/io/png.hpp>
#include <boost/log/trivial.hpp>

namespace WorldEngine
{
Image::Image(const World& world, bool hasColor, bool hasBlackAndWhite) :
    world_(world), hasColor_(hasColor), hasBlackAndWhite_(hasBlackAndWhite)
{
}

Image::Image(const World& world, bool hasBlackAndWhite) :
    Image(world, true, hasBlackAndWhite)
{
}

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

void Image::DrawImage(boost::gil::gray8_image_t::view_t& target)
{
   // Empty grayscale implementation
}

void Image::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   // Empty color implementation
}

void Image::DrawRivers(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          scale) const
{
   static const boost::gil::rgb8_pixel_t riverColor(0, 0, 128);
   static const boost::gil::rgb8_pixel_t lakeColor(0, 100, 128);

   const RiverMapArrayType& riverMap = world_.GetRiverMapData();
   const LakeMapArrayType&  lakeMap  = world_.GetLakeMapData();

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (world_.IsLand(x, y) && riverMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < scale; dy++)
            {
               for (uint32_t dx = 0; dx < scale; dx++)
               {
                  target(x * scale + dx, y * scale + dy) = riverColor;
               }
            }
         }
         if (world_.IsLand(x, y) && lakeMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < scale; dy++)
            {
               for (uint32_t dx = 0; dx < scale; dx++)
               {
                  target(x * scale + dx, y * scale + dy) = lakeColor;
               }
            }
         }
      }
   }
}

void Image::Draw(const std::string& filename, bool blackAndWhite)
{
   if ((!blackAndWhite || !hasBlackAndWhite_) && hasColor_)
   {
      boost::gil::rgb8_image_t         image(world_.width(), world_.height());
      boost::gil::rgb8_image_t::view_t view = boost::gil::view(image);

      DrawImage(view);

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
      boost::gil::gray8_image_t         image(world_.width(), world_.height());
      boost::gil::gray8_image_t::view_t view = boost::gil::view(image);

      DrawImage(view);

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
