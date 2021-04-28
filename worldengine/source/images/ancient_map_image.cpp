#include "worldengine/images/ancient_map_image.h"
#include "../basic.h"

#include <random>

#include <boost/log/trivial.hpp>
#include <boost/multiprecision/integer.hpp>

namespace bm = boost::multiprecision;

namespace WorldEngine
{
typedef std::function<void(boost::gil::rgb8_image_t::view_t& target,
                           uint32_t                          x,
                           uint32_t                          y,
                           uint32_t                          w,
                           uint32_t                          h)>
   DrawFunction;

static void CreateBiomeGroupMasks(
   const World&                                                 world,
   std::unordered_map<BiomeGroup, boost::multi_array<bool, 2>>& biomeMasks,
   uint32_t                                                     scale);
static void CreateMountainMask(const World&                  world,
                               boost::multi_array<float, 2>& mountainMask,
                               uint32_t                      scale);

static void DrawDesertPattern(boost::gil::rgb8_image_t::view_t& target,
                              int32_t                           x,
                              int32_t                           y,
                              boost::gil::rgb8_pixel_t          c);
static void DrawForestPattern1(boost::gil::rgb8_image_t::view_t& target,
                               int32_t                           x,
                               int32_t                           y,
                               boost::gil::rgb8_pixel_t          c1,
                               boost::gil::rgb8_pixel_t          c2);
static void DrawForestPattern2(boost::gil::rgb8_image_t::view_t& target,
                               int32_t                           x,
                               int32_t                           y,
                               boost::gil::rgb8_pixel_t          c1,
                               boost::gil::rgb8_pixel_t          c2);

static void DrawPixelCheck(boost::gil::rgb8_image_t::view_t& target,
                           int32_t                           x,
                           int32_t                           y,
                           boost::gil::rgb8_pixel_t          c);
static void DrawShadedPixel(boost::gil::rgb8_image_t::view_t& target,
                            uint32_t                          x,
                            uint32_t                          y,
                            uint8_t                           r,
                            uint8_t                           g,
                            uint8_t                           b);
static void DrawBorealForest(boost::gil::rgb8_image_t::view_t& target,
                             uint32_t                          x,
                             uint32_t                          y,
                             uint32_t                          w,
                             uint32_t                          h);
static void DrawChaparral(boost::gil::rgb8_image_t::view_t& target,
                          uint32_t                          x,
                          uint32_t                          y,
                          uint32_t                          w,
                          uint32_t                          h);
static void DrawCoolDesert(boost::gil::rgb8_image_t::view_t& target,
                           uint32_t                          x,
                           uint32_t                          y,
                           uint32_t                          w,
                           uint32_t                          h);
static void DrawColdParklands(boost::gil::rgb8_image_t::view_t& target,
                              uint32_t                          x,
                              uint32_t                          y,
                              uint32_t                          w,
                              uint32_t                          h);
static void DrawGlacier(boost::gil::rgb8_image_t::view_t& target,
                        uint32_t                          x,
                        uint32_t                          y,
                        uint32_t                          w,
                        uint32_t                          h);
static void DrawHotDesert(boost::gil::rgb8_image_t::view_t& target,
                          uint32_t                          x,
                          uint32_t                          y,
                          uint32_t                          w,
                          uint32_t                          h);
static void DrawJungle(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h);
static void DrawSavanna(boost::gil::rgb8_image_t::view_t& target,
                        uint32_t                          x,
                        uint32_t                          y,
                        uint32_t                          w,
                        uint32_t                          h);
static void DrawSteppe(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h);
static void DrawTemperateForest1(boost::gil::rgb8_image_t::view_t& target,
                                 uint32_t                          x,
                                 uint32_t                          y,
                                 uint32_t                          w,
                                 uint32_t                          h);
static void DrawTemperateForest2(boost::gil::rgb8_image_t::view_t& target,
                                 uint32_t                          x,
                                 uint32_t                          y,
                                 uint32_t                          w,
                                 uint32_t                          h);
static void DrawTropicalDryForest(boost::gil::rgb8_image_t::view_t& target,
                                  uint32_t                          x,
                                  uint32_t                          y,
                                  uint32_t                          w,
                                  uint32_t                          h);
static void DrawTundra(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h);
static void DrawWarmTemperateForest(boost::gil::rgb8_image_t::view_t& target,
                                    uint32_t                          x,
                                    uint32_t                          y,
                                    uint32_t                          w,
                                    uint32_t                          h);

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
   std::unordered_map<int32_t, boost::multi_array<int32_t, 2>> borderNeighbors;
   borderNeighbors[6].resize(boost::extents[sHeight][sWidth]);
   borderNeighbors[9].resize(boost::extents[sHeight][sWidth]);
   borderNeighbors[6] = CountNeighbors(borders, 6);
   borderNeighbors[9] = CountNeighbors(borders, 9);

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

   std::function<void(
      BiomeGroup, DrawFunction, uint32_t, uint32_t, int32_t, DrawFunction)>
      DrawBiome;
   if (drawBiome_)
   {
      CreateBiomeGroupMasks(world_, biomeMasks, scale_);

      DrawBiome = [this,
                   &biomeMasks,
                   &generator,
                   &target,
                   &borderNeighbors = std::as_const(borderNeighbors),
                   &borders = std::as_const(borders)](BiomeGroup   group,
                                                      DrawFunction Draw,
                                                      uint32_t     w,
                                                      uint32_t     h,
                                                      int32_t      r,
                                                      DrawFunction AltDraw) {
         BOOST_LOG_TRIVIAL(debug)
            << "Ancient map: Drawing biome group " << group;

         static const std::uniform_real_distribution<float> random(0.0f, 1.0f);

         const uint32_t width   = world_.width();
         const uint32_t height  = world_.height();
         const uint32_t sWidth  = width * scale_;
         const uint32_t sHeight = height * scale_;

         for (int32_t sy = 0; sy < sHeight; sy++)
         {
            for (int32_t sx = 0; sx < sWidth; sx++)
            {
               if (biomeMasks.at(group)[sy][sx])
               {
                  if (group == BiomeGroup::Iceland)
                  {
                     if (!borders[sy][sx])
                     {
                        Draw(target, sx, sy, 0, 0);
                     }
                  }
                  else
                  {
                     if (r == 0 || borderNeighbors.at(r)[sy][sx] <= 2)
                     {
                        if (AltDraw != nullptr && random(generator) >= 0.5f)
                        {
                           AltDraw(target, sx, sy, w, h);
                        }
                        else
                        {
                           Draw(target, sx, sy, w, h);
                        }

                        for (int32_t dy = -r; dy <= r; dy++)
                        {
                           const int32_t yp = sy + dy;
                           for (int32_t dx = -r; dx <= r; dx++)
                           {
                              const int32_t xp = sx + dx;
                              if (0 <= yp && yp < sHeight && 0 <= xp &&
                                  xp < sWidth)
                              {
                                 biomeMasks.at(group)[yp][xp] = false;
                              }
                           }
                        }
                     }
                  }
               }
            }
         }
      };
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
      DrawBiome(BiomeGroup::Iceland, DrawGlacier, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::Tundra, DrawTundra, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::ColdParklands, DrawColdParklands, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::Steppe, DrawSteppe, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::Chaparral, DrawChaparral, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::Savanna, DrawSavanna, 0, 0, 0, nullptr);
      DrawBiome(BiomeGroup::CoolDesert, DrawCoolDesert, 8, 2, 9, nullptr);
      DrawBiome(BiomeGroup::HotDesert, DrawHotDesert, 8, 2, 9, nullptr);
      DrawBiome(BiomeGroup::BorealForest, DrawBorealForest, 4, 5, 6, nullptr);
      DrawBiome(BiomeGroup::CoolTemperateForest,
                DrawTemperateForest1,
                4,
                5,
                6,
                DrawTemperateForest2);
      DrawBiome(BiomeGroup::WarmTemperateForest,
                DrawWarmTemperateForest,
                4,
                5,
                6,
                nullptr);
      DrawBiome(BiomeGroup::TropicalDryForest,
                DrawTropicalDryForest,
                4,
                5,
                6,
                nullptr);
      DrawBiome(BiomeGroup::Jungle, DrawJungle, 4, 5, 6, nullptr);
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
   const World&                                                 world,
   std::unordered_map<BiomeGroup, boost::multi_array<bool, 2>>& masks,
   uint32_t                                                     scale)
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

static void CreateMountainMask(const World&                  world,
                               boost::multi_array<float, 2>& mask,
                               uint32_t                      scale)
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

static void DrawDesertPattern(boost::gil::rgb8_image_t::view_t& target,
                              int32_t                           x,
                              int32_t                           y,
                              boost::gil::rgb8_pixel_t          c)
{
   static const std::vector<Point> points = {
      {-1, -2}, {0, -2}, {1, -2}, {2, -2}, {-2, -1}, {-1, -1}, {0, -1},
      {4, -1},  {-4, 0}, {-3, 0}, {-2, 0}, {-1, 0},  {1, 0},   {2, 0},
      {6, 0},   {-5, 1}, {0, 1},  {7, 1},  {8, 1},   {-8, 2},  {-7, 2}};

   for (Point p : points)
   {
      DrawPixelCheck(target, x + p.first, y + p.second, c);
   }
}

static void DrawForestPattern1(boost::gil::rgb8_image_t::view_t& target,
                               int32_t                           x,
                               int32_t                           y,
                               boost::gil::rgb8_pixel_t          c1,
                               boost::gil::rgb8_pixel_t          c2)
{
   static const std::vector<Point> c1Points = {
      {0, -4}, {0, -3}, {-1, -2}, {1, -2}, {-1, -1}, {1, -1}, {-2, 0}, {1, 0},
      {2, 0},  {-2, 1}, {2, 1},   {-3, 2}, {-1, 2},  {3, 2},  {-3, 3}, {-2, 3},
      {-1, 3}, {0, 3},  {1, 3},   {2, 3},  {3, 3},   {0, 4}};
   static const std::vector<Point> c2Points = {{0, -2},
                                               {0, -1},
                                               {-1, 0},
                                               {0, 0},
                                               {-1, 1},
                                               {0, 1},
                                               {1, 1},
                                               {-2, 2},
                                               {0, 2},
                                               {1, 2},
                                               {2, 2}};

   for (Point p : c1Points)
   {
      DrawPixelCheck(target, x + p.first, y + p.second, c1);
   }
   for (Point p : c2Points)
   {
      DrawPixelCheck(target, x + p.first, y + p.second, c2);
   }
}

static void DrawForestPattern2(boost::gil::rgb8_image_t::view_t& target,
                               int32_t                           x,
                               int32_t                           y,
                               boost::gil::rgb8_pixel_t          c1,
                               boost::gil::rgb8_pixel_t          c2)
{
   static const std::vector<Point> c1Points = {
      {-1, -4}, {0, -4}, {1, -4}, {-2, -3}, {-1, -3}, {2, -3},
      {-2, -2}, {1, -2}, {2, -2}, {-2, -1}, {2, -1},  {-2, 0},
      {-1, 0},  {2, 0},  {-2, 1}, {1, 1},   {2, 1},   {-1, 2},
      {0, 2},   {1, 2},  {0, 3},  {0, 4}};
   static const std::vector<Point> c2Points = {{0, -3},
                                               {1, -3},
                                               {-1, -2},
                                               {0, -2},
                                               {-1, -1},
                                               {0, -1},
                                               {1, -1},
                                               {0, 0},
                                               {1, 0},
                                               {-1, 1},
                                               {0, 1}};

   for (Point p : c1Points)
   {
      DrawPixelCheck(target, x + p.first, y + p.second, c1);
   }
   for (Point p : c2Points)
   {
      DrawPixelCheck(target, x + p.first, y + p.second, c2);
   }
}

static void DrawPixelCheck(boost::gil::rgb8_image_t::view_t& target,
                           int32_t                           x,
                           int32_t                           y,
                           boost::gil::rgb8_pixel_t          c)
{
   if (0 <= x && x < target.width() && 0 <= y && y < target.height())
   {
      target(x, y) = c;
   }
}

static void DrawShadedPixel(boost::gil::rgb8_image_t::view_t& target,
                            uint32_t                          x,
                            uint32_t                          y,
                            uint8_t                           r,
                            uint8_t                           g,
                            uint8_t                           b)
{
   const uint8_t db =
      (bm::powm(x, y / 5, 75) + x * 23 + y * 37 + (x * y) * 13) % 75;
   const uint8_t nr = r - db;
   const uint8_t ng = g - db;
   const uint8_t nb = b - db;

   target(x, y) = boost::gil::rgb8_pixel_t(nr, ng, nb);
}

static void DrawBorealForest(boost::gil::rgb8_image_t::view_t& target,
                             uint32_t                          x,
                             uint32_t                          y,
                             uint32_t                          w,
                             uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(0, 32, 0);
   const boost::gil::rgb8_pixel_t c2(0, 64, 0);
   DrawForestPattern1(target, x, y, c1, c2);
}

static void DrawChaparral(boost::gil::rgb8_image_t::view_t& target,
                          uint32_t                          x,
                          uint32_t                          y,
                          uint32_t                          w,
                          uint32_t                          h)
{
   DrawShadedPixel(target, x, y, 180, 171, 113);
}

static void DrawCoolDesert(boost::gil::rgb8_image_t::view_t& target,
                           uint32_t                          x,
                           uint32_t                          y,
                           uint32_t                          w,
                           uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c(72, 72, 53);
   DrawDesertPattern(target, x, y, c);
}

static void DrawColdParklands(boost::gil::rgb8_image_t::view_t& target,
                              uint32_t                          x,
                              uint32_t                          y,
                              uint32_t                          w,
                              uint32_t                          h)
{
   const uint8_t db =
      (bm::powm(x, y / 5, 75) + x * 23 + y * 37 + (x * y) * 13) % 75;
   const uint8_t r = 105 - db;
   const uint8_t g = 96 - db;
   const uint8_t b = 38 - db / 2;
   target(x, y)    = boost::gil::rgb8_pixel_t(r, g, b);
}

static void DrawGlacier(boost::gil::rgb8_image_t::view_t& target,
                        uint32_t                          x,
                        uint32_t                          y,
                        uint32_t                          w,
                        uint32_t                          h)
{
   const uint8_t rg =
      255 - (bm::powm(x, y / 5, 75) + x * 23 + y * 37 + (x * y) * 13) % 75;
   target(x, y) = boost::gil::rgb8_pixel_t(rg, rg, 255);
}

static void DrawHotDesert(boost::gil::rgb8_image_t::view_t& target,
                          uint32_t                          x,
                          uint32_t                          y,
                          uint32_t                          w,
                          uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c(72, 72, 53);
   DrawDesertPattern(target, x, y, c);
}

static void DrawJungle(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(0, 128, 0);
   const boost::gil::rgb8_pixel_t c2(0, 255, 0);
   DrawForestPattern2(target, x, y, c1, c2);
}

static void DrawSavanna(boost::gil::rgb8_image_t::view_t& target,
                        uint32_t                          x,
                        uint32_t                          y,
                        uint32_t                          w,
                        uint32_t                          h)
{
   DrawShadedPixel(target, x, y, 255, 246, 188);
}

static void DrawSteppe(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h)
{
   DrawShadedPixel(target, x, y, 96, 192, 96);
}

static void DrawTemperateForest1(boost::gil::rgb8_image_t::view_t& target,
                                 uint32_t                          x,
                                 uint32_t                          y,
                                 uint32_t                          w,
                                 uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(0, 64, 0);
   const boost::gil::rgb8_pixel_t c2(0, 96, 0);
   DrawForestPattern1(target, x, y, c1, c2);
}

static void DrawTemperateForest2(boost::gil::rgb8_image_t::view_t& target,
                                 uint32_t                          x,
                                 uint32_t                          y,
                                 uint32_t                          w,
                                 uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(0, 32, 0);
   const boost::gil::rgb8_pixel_t c2(0, 112, 0);
   DrawForestPattern2(target, x, y, c1, c2);
}

static void DrawTropicalDryForest(boost::gil::rgb8_image_t::view_t& target,
                                  uint32_t                          x,
                                  uint32_t                          y,
                                  uint32_t                          w,
                                  uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(51, 36, 3);
   const boost::gil::rgb8_pixel_t c2(139, 204, 58);
   DrawForestPattern2(target, x, y, c1, c2);
}

static void DrawTundra(boost::gil::rgb8_image_t::view_t& target,
                       uint32_t                          x,
                       uint32_t                          y,
                       uint32_t                          w,
                       uint32_t                          h)
{
   DrawShadedPixel(target, x, y, 166, 148, 75);
}

static void DrawWarmTemperateForest(boost::gil::rgb8_image_t::view_t& target,
                                    uint32_t                          x,
                                    uint32_t                          y,
                                    uint32_t                          w,
                                    uint32_t                          h)
{
   const boost::gil::rgb8_pixel_t c1(0, 96, 0);
   const boost::gil::rgb8_pixel_t c2(0, 192, 0);
   DrawForestPattern2(target, x, y, c1, c2);
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
