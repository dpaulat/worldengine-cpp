#include "Functions.h"

#include <gtest/gtest.h>

#include <boost/gil/extension/io/png.hpp>

#include <worldengine/images/ancient_map_image.h>

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

static void CompareImages(const std::string& file1, const std::string& file2);
static std::string GoldenImagePath(const std::string& filename);

TEST_F(ImageTest, AncientMapTest)
{
   const uint32_t  scale = 3u;
   AncientMapImage image(*w_, seed_, scale);

   image.Draw(filename_);

   const std::string goldenImage =
      GoldenImagePath("ancient_map_seed_1618_factor3.png");

   CompareImages(filename_, goldenImage);
}

static void CompareImages(const std::string& file1, const std::string& file2)
{
   boost::gil::rgb8_image_t                             image1;
   boost::gil::rgb8_image_t                             image2;
   boost::gil::image_read_settings<boost::gil::png_tag> readSettings;

   boost::gil::read_image(file1, image1, readSettings);
   boost::gil::read_image(file2, image2, readSettings);

   ASSERT_EQ(image1.width(), image2.width());
   ASSERT_EQ(image1.height(), image2.height());

   boost::gil::rgb8_image_t::const_view_t view1 =
      boost::gil::const_view(image1);
   boost::gil::rgb8_image_t::const_view_t view2 =
      boost::gil::const_view(image2);

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
