#include <fstream>

#include <gtest/gtest.h>

#include <basic.h>
#include <worldengine/generation.h>
#include <worldengine/plates.h>

namespace WorldEngine
{

static float MeanElevationAtBorders(const World& world);

TEST(PlatesTest, WorldGenTest)
{
   std::shared_ptr<World> world = WorldGen("Dummy", 32, 16, 1);

   EXPECT_NE(world, nullptr);
}

TEST(GenerationTest, CenterLandTest)
{
   static const std::string testDataDir   = WORLDENGINE_TEST_DATA_DIR;
   const std::string        worldFilename = testDataDir + "/data/seed_1618.world";

   std::shared_ptr<World> w = std::make_shared<World>();

   std::ifstream input(worldFilename,
                       std::ios_base::in | std::ios_base::binary);
   bool          success = w->ProtobufDeserialize(input);

   EXPECT_EQ(success, true) << "Unable to parse " << worldFilename;

   if (success)
   {
      // We want to have less land than before at the borders
      float elBefore = MeanElevationAtBorders(*w);
      CenterLand(*w);
      float elAfter = MeanElevationAtBorders(*w);
      EXPECT_LE(elAfter, elBefore);
   }
}

TEST(GenerationTest, SeaDepthTest)
{
   static const size_t extent     = 11u;
   static const float  oceanLevel = 1.0f;

   static const float desiredResult[extent][extent] = {
      {0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f},
      {0.9f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.3f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.3f, 0.0f, -1.0f, 0.0f, 0.3f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.3f, 0.0f, 0.0f, 0.0f, 0.3f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.3f, 0.3f, 0.3f, 0.3f, 0.3f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.7f, 0.7f},
      {0.9f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f, 0.7f},
      {0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f, 0.9f}};
   static const float* desiredResultBegin = &desiredResult[0][0];
   static const float* desiredResultEnd =
      desiredResultBegin + (extent * extent);

   SeaDepthArrayType expectedSeaDepth(boost::extents[extent][extent]);
   expectedSeaDepth.assign(desiredResultBegin, desiredResultEnd);

   // This part is verbatim from the function.  Some refactoring should be done
   // in order to increase test quality
   AntiAlias(expectedSeaDepth, 10);

   auto        minmax     = std::minmax_element(expectedSeaDepth.data(),
                                     expectedSeaDepth.data() +
                                        expectedSeaDepth.num_elements());
   const float minDepth   = *minmax.first;
   const float maxDepth   = *minmax.second;
   const float deltaDepth = maxDepth - minDepth;
   std::transform(expectedSeaDepth.data(),
                  expectedSeaDepth.data() + expectedSeaDepth.num_elements(),
                  expectedSeaDepth.data(),
                  [&minDepth, &maxDepth, &deltaDepth](const float& a) -> float {
                     return (a - minDepth) / (deltaDepth);
                  });
   // End verbatim

   std::shared_ptr<World> w = std::make_shared<World>(
      "seaDepth",
      Size(extent, extent),
      0,
      GenerationParameters(0, oceanLevel, StepType::Full));

   ElevationArrayType& elevation = w->GetElevationData();
   OceanArrayType&     ocean     = w->GetOceanData();

   elevation.resize(boost::extents[extent][extent]);
   ocean.resize(boost::extents[extent][extent]);

   std::fill(ocean.data(), ocean.data() + ocean.num_elements(), true);
   ocean[5][5] = false;

   std::fill(
      elevation.data(), elevation.data() + elevation.num_elements(), 0.0f);
   elevation[5][5] = 2.0f;

   SeaDepth(*w, oceanLevel);

   const SeaDepthArrayType& seaDepth = w->GetSeaDepthData();

   for (size_t y = 0; y < extent; y++)
   {
      for (size_t x = 0; x < extent; x++)
      {
         EXPECT_NEAR(seaDepth[y][x], expectedSeaDepth[y][x], 0.07f)
            << "(x, y) = (" << x << ", " << y << ")";
      }
   }
}

static float MeanElevationAtBorders(const World& world)
{
   float totalElevation = 0.0f;

   for (size_t y = 0; y < world.height(); y++)
   {
      totalElevation += world.GetElevationAt(0, y);
      totalElevation += world.GetElevationAt(world.width() - 1, y);
   }
   for (size_t x = 1; x < world.width() - 1; x++)
   {
      totalElevation += world.GetElevationAt(x, 0);
      totalElevation += world.GetElevationAt(x, world.height() - 1);
   }

   size_t totalCells = (world.width() + world.height() - 2) * 2;

   return totalElevation / totalCells;
}

} // namespace WorldEngine
