#include "Functions.h"

#include <gtest/gtest.h>

#include <boost/filesystem.hpp>
#include <boost/gil/extension/io/png.hpp>

#include <worldengine/images/ancient_map_image.h>
#include <worldengine/images/biome_image.h>
#include <worldengine/images/elevation_image.h>
#include <worldengine/images/heightmap_image.h>
#include <worldengine/images/ocean_image.h>
#include <worldengine/images/precipitation_image.h>
#include <worldengine/images/river_image.h>
#include <worldengine/images/satellite_image.h>
#include <worldengine/images/scatter_plot_image.h>
#include <worldengine/images/simple_elevation_image.h>
#include <worldengine/images/temperature_image.h>
#include <worldengine/images/world_image.h>

namespace WorldEngine
{

class ImageTest : public ::testing::Test
{
protected:
   void SetUp() override
   {
      const std::string worldFilename = "/data/seed_1618.world";

      seed_     = 1618;
      filename_ = GenerateTemporaryFilename("ImageTest-", ".png");

      w_           = std::make_shared<World>();
      bool success = LoadWorld(*w_, worldFilename);

      ASSERT_EQ(success, true);
   }

   void TearDown() override
   {
      w_.reset();
      std::remove(filename_.c_str());
   }

   uint32_t               seed_;
   std::shared_ptr<World> w_;
   std::string            filename_;
};

template<typename ImageType = boost::gil::rgb8_image_t>
static void CompareImages(const std::string& file1, const std::string& file2);
static std::string GoldenImagePath(const std::string& filename);

TEST(ImageFunctionTest, GradientTest)
{
   const boost::gil::rgb8_pixel_t c1(10, 20, 40);
   const boost::gil::rgb8_pixel_t c2(0, 128, 240);

   boost::gil::rgb8_pixel_t g1 = Gradient(0.0f, 0.0f, 1.0f, c1, c2);
   boost::gil::rgb8_pixel_t g2 = Gradient(1.0f, 0.0f, 1.0f, c1, c2);
   boost::gil::rgb8_pixel_t g3 = Gradient(0.5f, 0.0f, 1.0f, c1, c2);

   EXPECT_EQ(g1, boost::gil::rgb8_pixel_t(10, 20, 40));
   EXPECT_EQ(g2, boost::gil::rgb8_pixel_t(0, 128, 240));
   EXPECT_EQ(g3, boost::gil::rgb8_pixel_t(5, 74, 140));
}

TEST_F(ImageTest, AncientMapTest)
{
   const uint32_t  scale = 3u;
   AncientMapImage image(*w_, seed_, scale);

   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("ancient_map_seed_1618_factor3.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, AncientMapBorderTest)
{
   const uint32_t  scale               = 1u;
   const bool      drawBiome           = true;
   const bool      drawRivers          = true;
   const bool      drawMountains       = true;
   const bool      drawOuterLandBorder = true;
   AncientMapImage image(*w_,
                         seed_,
                         scale,
                         SeaColor::Brown,
                         drawBiome,
                         drawRivers,
                         drawMountains,
                         drawOuterLandBorder);

   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("ancient_map_seed_1618_outer_border.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, BiomeTest)
{
   BiomeImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_biome.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, ElevationNoShadowTest)
{
   ElevationImage image(*w_, false);
   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("seed_1618_elevation_no_shadow.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, ElevationShadowTest)
{
   ElevationImage image(*w_, true);
   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("seed_1618_elevation_shadow.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, HeightmapTest)
{
   HeightmapImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_grayscale.png");

   CompareImages<boost::gil::gray8_image_t>(filename_, goldenImage);
}

TEST_F(ImageTest, OceanTest)
{
   OceanImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_ocean.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, PrecipitationTest)
{
   PrecipitationImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("seed_1618_precipitation.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, RiverMapTest)
{
   RiverImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_rivers.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, SatelliteTest)
{
   SatelliteImage image(*w_, seed_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_satellite.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, ScatterPlotTest)
{
   ScatterPlotImage image(*w_, 512u);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_scatter.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, SimpleElevationTest)
{
   SimpleElevationImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_elevation.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, TemperatureTest)
{
   TemperatureImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_temperature.png");

   CompareImages(filename_, goldenImage);
}

TEST_F(ImageTest, WorldTest)
{
   WorldImage image(*w_);
   image.Draw(filename_);

   const std::string goldenImage = GoldenImagePath("seed_1618_world.png");

   CompareImages(filename_, goldenImage);
}

template<typename ImageType>
static void CompareImages(const std::string& file1, const std::string& file2)
{
   ASSERT_EQ(boost::filesystem::exists(file2), true);

   ImageType                                            image1;
   ImageType                                            image2;
   boost::gil::image_read_settings<boost::gil::png_tag> readSettings;

   boost::gil::read_image(file1, image1, readSettings);
   boost::gil::read_image(file2, image2, readSettings);

   ASSERT_EQ(image1.width(), image2.width());
   ASSERT_EQ(image1.height(), image2.height());

   typename ImageType::const_view_t view1 = boost::gil::const_view(image1);
   typename ImageType::const_view_t view2 = boost::gil::const_view(image2);

   for (size_t y = 0; y < image1.height(); y++)
   {
      for (size_t x = 0; x < image1.width(); x++)
      {
         ASSERT_EQ(view1(x, y), view2(x, y))
            << "Image differs at (" << x << ", " << y << ")";
      }
   }
}

static std::string GoldenImagePath(const std::string& filename)
{
   return TEST_DATA_DIR + "/images/" + filename;
}

} // namespace WorldEngine
