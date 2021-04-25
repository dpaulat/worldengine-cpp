#include "worldengine/images/ancient_map_image.h"
#include "../basic.h"

#include <random>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{
static void CreateBiomeGroupMasks(
   const World&                                                world,
   std::unordered_map<BiomeGroup, boost::multi_array<bool, 2>> biomeMasks,
   uint32_t                                                    scale);
static void CreateMountainMask(const World&                 world,
                               boost::multi_array<float, 2> mountainMask,
                               uint32_t                     scale);

static boost::gil::rgb8_pixel_t Gradient(float                    value,
                                         float                    low,
                                         float                    high,
                                         boost::gil::rgb8_pixel_t lowColor,
                                         boost::gil::rgb8_pixel_t highColor);
template<typename T>
static void ScaleArray(const boost::multi_array<T, 2>& input,
                       boost::multi_array<T, 2>&       output,
                       uint32_t                        scale);

AncientMapImage::AncientMapImage(const World& world,
                                 uint32_t     seed,
                                 uint32_t     scale,
                                 SeaColor     seaColor,
                                 bool         drawBiome,
                                 bool         drawRivers,
                                 bool         drawMountains,
                                 bool         drawOuterLandBorder) :
    Image(world, false),
    seed_(seed),
    scale_(scale),
    seaColor_(seaColor),
    drawBiome_(drawBiome),
    drawRivers_(drawRivers),
    drawMountains_(drawMountains),
    drawOuterLandBorder_(drawOuterLandBorder)
{
   // TODO: Resize factor should change the image size
}

AncientMapImage::~AncientMapImage() {}

void AncientMapImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   BOOST_LOG_TRIVIAL(debug) << "Ancient map: Initializing";

   std::default_random_engine generator(seed_);

   const uint32_t width   = world_.width();
   const uint32_t height  = world_.height();
   const uint32_t sWidth  = width * scale_;
   const uint32_t sHeight = height * scale_;

   const boost::gil::rgb8_pixel_t seaColor =
      (seaColor_ == SeaColor::Blue) ? boost::gil::rgb8_pixel_t(142, 162, 179) :
                                      boost::gil::rgb8_pixel_t(212, 198, 169);
   static const boost::gil::rgb8_pixel_t landColor =
      boost::gil::rgb8_pixel_t(181, 166, 127);

   OceanArrayType scaledOcean;
   ScaleArray(world_.GetOceanData(), scaledOcean, scale_);
   boost::multi_array<uint32_t, 2> neighbors = CountNeighbors(scaledOcean);

   boost::multi_array<bool, 2> borders(boost::extents[sHeight][sWidth]);
   std::transform(scaledOcean.data(),
                  scaledOcean.data() + scaledOcean.num_elements(),
                  neighbors.data(),
                  borders.data(),
                  [](const bool& ocean, const uint32_t& neighbors) -> bool {
                     return (ocean || neighbors == 0) ? false : true;
                  });

   // Cache neighbors count at different radii
   boost::multi_array<uint32_t, 2> borderNeighbors6 =
      CountNeighbors(borders, 6);
   boost::multi_array<uint32_t, 2> borderNeighbors9 =
      CountNeighbors(borders, 9);

   boost::multi_array<bool, 2> outerBorders;
   if (drawOuterLandBorder_)
   {
      outerBorders.resize(boost::extents[sHeight][sWidth]);

      auto GenerateOuterBorders =
         [&sWidth, &sHeight, &scaledOcean = std::as_const(scaledOcean)](
            const boost::multi_array<bool, 2>& innerBorders,
            boost::multi_array<bool, 2>&       outerBorders) {
            boost::multi_array<uint32_t, 2> neighbors =
               CountNeighbors(innerBorders);

            for (uint32_t y = 0; y < sHeight; y++)
            {
               for (uint32_t x = 0; x < sWidth; x++)
               {
                  outerBorders[y][x] = !innerBorders[y][x] &&
                                       scaledOcean[y][x] && neighbors[y][x] > 0;
               }
            }
         };

      GenerateOuterBorders(borders, outerBorders);
      GenerateOuterBorders(outerBorders, outerBorders);
   }

   boost::multi_array<float, 2> mountainMask;
   if (drawMountains_)
   {
      CreateMountainMask(world_, mountainMask, scale_);
   }

   std::unordered_map<BiomeGroup, boost::multi_array<bool, 2>> biomeMasks;
   if (drawBiome_)
   {
      CreateBiomeGroupMasks(world_, biomeMasks, scale_);
   }

   BOOST_LOG_TRIVIAL(debug) << "Ancient map: Coloring oceans and borders";

   static const boost::gil::rgb8_pixel_t borderColor(0, 0, 0);
   const boost::gil::rgb8_pixel_t        outerBorderColor =
      Gradient(0.5f, 0.0f, 1.0f, borderColor, seaColor);

   for (uint32_t y = 0; y < sHeight; y++)
   {
      for (uint32_t x = 0; x < sWidth; x++)
      {
         if (borders[y][x])
         {
            target(x, y) = borderColor;
         }
         else if (drawOuterLandBorder_ && outerBorders[y][x])
         {
            target(x, y) = outerBorderColor;
         }
         else if (scaledOcean[y][x])
         {
            target(x, y) = seaColor;
         }
         else
         {
            target(x, y) = landColor;
         }
      }
   }

   BOOST_LOG_TRIVIAL(debug) << "Ancient map: Anti-aliasing image";

   // TODO: Anti-alias channel

   if (drawBiome_)
   {
      BOOST_LOG_TRIVIAL(debug) << "Ancient map: Drawing glacier";
      // TODO
   }

   if (drawRivers_)
   {
      BOOST_LOG_TRIVIAL(debug) << "Ancient map: Drawing rivers";
      DrawRivers(target, scale_);
   }

   if (drawMountains_)
   {
      BOOST_LOG_TRIVIAL(debug) << "Ancient map: Drawing mountains";
      // TODO
   }

   BOOST_LOG_TRIVIAL(debug) << "Ancient map: Complete";
}

static void CreateBiomeGroupMasks(
   const World&                                                world,
   std::unordered_map<BiomeGroup, boost::multi_array<bool, 2>> masks,
   uint32_t                                                    scale)
{
   const uint32_t width  = world.width();
   const uint32_t height = world.height();

   for (BiomeGroup group : BiomeGroupIterator())
   {
      boost::multi_array<bool, 2>& mask = masks[group];
      mask.resize(boost::extents[height][width]);
      std::fill(mask.data(), mask.data() + mask.num_elements(), false);

      for (uint32_t y = 0; y < height; y++)
      {
         for (uint32_t x = 0; x < width; x++)
         {
            if (group == world.GetBiomeGroup(x, y))
            {
               mask[y][x] = true;
            }
         }
      }

      const boost::multi_array<uint32_t, 2> neighbors = CountNeighbors(mask);

      std::transform(mask.data(),
                     mask.data() + mask.num_elements(),
                     neighbors.data(),
                     mask.data(),
                     [](const bool& mask, const uint32_t& neighbors) -> bool {
                        return (mask && neighbors > 5);
                     });

      ScaleArray(mask, mask, scale);
   }
}

static void CreateMountainMask(const World&                 world,
                               boost::multi_array<float, 2> mask,
                               uint32_t                     scale)
{
   const uint32_t width  = world.width();
   const uint32_t height = world.height();

   const ElevationArrayType& elevation = world.GetElevationData();
   const OceanArrayType&     ocean     = world.GetOceanData();

   mask.resize(boost::extents[height][width]);
   std::fill(mask.data(), mask.data() + mask.num_elements(), 0.0f);

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (world.IsMountain(x, y))
         {
            mask[y][x] = 1.0f;
         }
      }
   }

   const boost::multi_array<uint32_t, 2> neighbors = CountNeighbors(mask, 3);

   std::transform(mask.data(),
                  mask.data() + mask.num_elements(),
                  neighbors.data(),
                  mask.data(),
                  [](const float& mask, const uint32_t& neighbors) -> float {
                     if (mask > 0.0f && neighbors > 32)
                     {
                        return neighbors / 4.0f;
                     }
                     else
                     {
                        return 0.0f;
                     }
                  });

   ScaleArray(mask, mask, scale);
}

static boost::gil::rgb8_pixel_t Gradient(float                    value,
                                         float                    low,
                                         float                    high,
                                         boost::gil::rgb8_pixel_t lowColor,
                                         boost::gil::rgb8_pixel_t highColor)
{
   if (low == high)
   {
      return lowColor;
   }

   const float range = high - low;
   const float x     = (value - low) / range;
   const float ix    = 1.0f - x;

   const uint8_t lr = lowColor[0];
   const uint8_t lg = lowColor[1];
   const uint8_t lb = lowColor[2];
   const uint8_t hr = highColor[0];
   const uint8_t hg = highColor[1];
   const uint8_t hb = highColor[2];

   const uint8_t r = lr * ix + hr * x;
   const uint8_t g = lg * ix + hg * x;
   const uint8_t b = lb * ix + hb * x;

   return boost::gil::rgb8_pixel_t(r, g, b);
}

template<typename T>
static void ScaleArray(const boost::multi_array<T, 2>& input,
                       boost::multi_array<T, 2>&       output,
                       uint32_t                        scale)
{
   const uint32_t width   = input.shape()[1];
   const uint32_t height  = input.shape()[0];
   const uint32_t sWidth  = width * scale;
   const uint32_t sHeight = height * scale;

   output.resize(boost::extents[sHeight][sWidth]);

   if (scale == 1)
   {
      output = input;
   }
   else
   {
      for (int32_t y = height - 1; y >= 0; y--)
      {
         for (uint32_t dy = 0; dy < scale; dy++)
         {
            const uint32_t yp = y * scale + dy;

            for (int32_t x = width - 1; x >= 0; x--)
            {
               for (uint32_t dx = 0; dx < scale; dx++)
               {
                  const uint32_t xp = x * scale + dx;

                  output[yp][xp] = input[y][x];
               }
            }
         }
      }
   }
}

} // namespace WorldEngine
