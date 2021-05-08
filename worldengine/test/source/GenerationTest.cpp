#include <gtest/gtest.h>

#include <basic.h>
#include <worldengine/generation.h>
#include <worldengine/plates.h>

namespace WorldEngine
{

TEST(PlatesTest, WorldGenTest)
{
   std::shared_ptr<World> world = WorldGen("Dummy", 32, 16, 1);

   EXPECT_NE(world, nullptr);
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
         EXPECT_NEAR(seaDepth[y][x], expectedSeaDepth[y][x], 0.07f);
      }
   }
}

} // namespace WorldEngine
