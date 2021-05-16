#include "worldengine/images/image.h"
#include "../basic.h"

#pragma warning(push)
#pragma warning(disable : 4996)
#include <boost/gil/extension/io/png.hpp>
#pragma warning(pop)

#include <boost/log/trivial.hpp>

namespace WorldEngine
{
static const bool     DEFAULT_HAS_COLOR           = true;
static const bool     DEFAULT_HAS_BLACK_AND_WHITE = false;
static const uint32_t DEFAULT_SCALE               = 1u;

static Size CalculateSize(const World& world, uint32_t scale);

Image::Image(const World& world) : Image(world, DEFAULT_SCALE) {}

Image::Image(const World& world, Size size) :
    Image(world,
          DEFAULT_HAS_COLOR,
          DEFAULT_HAS_BLACK_AND_WHITE,
          size,
          DEFAULT_SCALE)
{
}

Image::Image(const World& world, uint32_t scale) :
    Image(world, DEFAULT_HAS_COLOR, DEFAULT_HAS_BLACK_AND_WHITE, scale)
{
}

Image::Image(const World& world, bool hasBlackAndWhite) :
    Image(world, DEFAULT_HAS_COLOR, hasBlackAndWhite)
{
}

Image::Image(const World& world, bool hasColor, bool hasBlackAndWhite) :
    Image(world, hasColor, hasBlackAndWhite, DEFAULT_SCALE)
{
}

Image::Image(const World& world,
             bool         hasColor,
             bool         hasBlackAndWhite,
             uint32_t     scale) :
    Image(world, hasColor, hasBlackAndWhite, CalculateSize(world, scale), scale)
{
}

Image::Image(const World& world,
             bool         hasColor,
             bool         hasBlackAndWhite,
             Size         size,
             uint32_t     scale) :
    world_(world),
    hasColor_(hasColor),
    hasBlackAndWhite_(hasBlackAndWhite),
    size_(size),
    scale_(scale)
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
   const uint32_t width   = static_cast<uint32_t>(array.shape()[1]);
   const uint32_t height  = static_cast<uint32_t>(array.shape()[0]);
   const uint32_t floor   = 0u;
   const uint32_t ceiling = UINT8_MAX;

   const std::vector<std::pair<float, uint32_t>> points = {{low, floor},
                                                           {high, ceiling}};

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         uint32_t color = Interpolate(array[y][x], points);

         for (uint32_t dy = 0; dy < scale_; dy++)
         {
            for (uint32_t dx = 0; dx < scale_; dx++)
            {
               target(x * scale_ + dx, y * scale_ + dy) =
                  boost::gil::gray8_pixel_t(color);
            }
         }
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

void Image::DrawRivers(boost::gil::rgb8_image_t::view_t& target) const
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
            for (uint32_t dy = 0; dy < scale_; dy++)
            {
               for (uint32_t dx = 0; dx < scale_; dx++)
               {
                  target(x * scale_ + dx, y * scale_ + dy) = riverColor;
               }
            }
         }
         if (world_.IsLand(x, y) && lakeMap[y][x] > 0.0f)
         {
            for (uint32_t dy = 0; dy < scale_; dy++)
            {
               for (uint32_t dx = 0; dx < scale_; dx++)
               {
                  target(x * scale_ + dx, y * scale_ + dy) = lakeColor;
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
      boost::gil::rgb8_image_t         image(size_.width_, size_.height_);
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
      boost::gil::gray8_image_t         image(world_.width() * scale_,
                                      world_.height() * scale_);
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

static Size CalculateSize(const World& world, uint32_t scale)
{
   return Size(world.width() * scale, world.height() * scale);
}
} // namespace WorldEngine
