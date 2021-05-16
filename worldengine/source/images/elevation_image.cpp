#include "worldengine/images/elevation_image.h"

#include <boost/iterator/zip_iterator.hpp>
#include <boost/log/trivial.hpp>

namespace WorldEngine
{
ElevationImage::ElevationImage(const World& world, bool shadow) :
    Image(world, false), shadow_(shadow)
{
}
ElevationImage::~ElevationImage() {}

void ElevationImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const ElevationArrayType& e     = world_.GetElevationData();
   const OceanArrayType&     ocean = world_.GetOceanData();

   float seaLevel = world_.oceanLevel();
   bool  hasOcean = (seaLevel != NAN) && (!ocean.empty());
   float minElev  = std::numeric_limits<float>::infinity();
   float maxElev  = -std::numeric_limits<float>::infinity();

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (!hasOcean || !ocean[y][x])
         {
            if (minElev > e[y][x])
            {
               minElev = e[y][x];
            }
            if (maxElev < e[y][x])
            {
               maxElev = e[y][x];
            }
         }
      }
   }

   BOOST_LOG_TRIVIAL(debug) << "minElev = " << minElev;
   BOOST_LOG_TRIVIAL(debug) << "maxElev = " << maxElev;

   float elevDelta = maxElev - minElev;
   float elevation;

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (hasOcean && ocean[y][x])
         {
            target(x, y) = boost::gil::rgb8_pixel_t(0, 0, 255);
         }
         else
         {
            elevation = ((e[y][x] - minElev) / elevDelta);
            int32_t c = static_cast<int32_t>(255 - elevation * 255);

            if (shadow_ && y > 2 && x > 2)
            {
               if (e[y - 1][x - 1] > e[y][x])
               {
                  c -= 15;
               }
               if (e[y - 2][x - 2] > e[y][x] &&
                   e[y - 2][x - 2] > e[y - 1][x - 1])
               {
                  c -= 10;
               }
               if (e[y - 3][x - 3] > e[y][x] &&
                   e[y - 3][x - 3] > e[y - 1][x - 1] &&
                   e[y - 3][x - 3] > e[y - 2][x - 2])
               {
                  c -= 5;
               }
               if (c < 0)
               {
                  c = 0;
               }
            }

            target(x, y) = boost::gil::rgb8_pixel_t(c, c, c);
         }
      }
   }
}
} // namespace WorldEngine