#include <gtest/gtest.h>

#include <worldengine/world.h>
#include <simulations/hydrology.h>

namespace WorldEngine
{

TEST(SimulationTest, RandomLandTest)
{
   const Size size(100, 90);

   std::shared_ptr<World> w = std::make_shared<World>(
      "RandomLand", size, 0, GenerationParameters(0, 1.0f, StepType::Full));

   OceanArrayType& ocean = w->GetOceanData();
   ocean.resize(boost::extents[size.height_][size.width_]);

   for (size_t y = 0; y < size.height_; y++)
   {
      for (size_t x = 0; x < size.width_; x++)
      {
         ocean[y][x] = (y >= x);
      }
   }

   const size_t       numSamples = 1000u;
   std::vector<Point> landSamples;
   w->GetRandomLand(landSamples, numSamples, 0);
   EXPECT_EQ(landSamples.size(), numSamples);

   for (Point p : landSamples)
   {
      EXPECT_EQ(w->IsOcean(p), false) << p << " is ocean";
   }
}

TEST(SimulationTest, WatermapNoLandTest)
{
   const uint32_t seed = 12345;
   const Size     size(16, 8);

   std::shared_ptr<World> w = std::make_shared<World>(
      "Watermap", size, 0, GenerationParameters(0, 1.0f, StepType::Full));

   OceanArrayType& ocean = w->GetOceanData();
   std::fill(ocean.data(), ocean.data() + ocean.num_elements(), true);

   WatermapSimulation(*w, 200);

   EXPECT_EQ(w->GetWaterMapData().num_elements(), size.width_ * size.height_);
}

} // namespace WorldEngine
