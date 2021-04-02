#include <gtest/gtest.h>

#include <basic.h>

namespace WorldEngine
{

TEST(BasicTest, InterpolateFFTest)
{
   const std::vector<std::pair<float, float>> points_({{-0.5f, 0.0f}, //
                                                       {0.0f, 1.0f},
                                                       {0.5f, 0.0f}});
   EXPECT_EQ(Interpolate(-0.55f, points_), 0.0f);
   EXPECT_EQ(Interpolate(0.55f, points_), 0.0f);
   EXPECT_EQ(Interpolate(0.0f, points_), 1.0f);
   EXPECT_EQ(Interpolate(-0.1f, points_), 0.8f);
}

TEST(BasicTest, InterpolateUFTest)
{
   static const std::vector<std::pair<uint32_t, float>> points_({{0u, -1.0f}, //
                                                                 {8u, 1.0f}});
   EXPECT_EQ(Interpolate(0u, points_), -1.0f);
   EXPECT_EQ(Interpolate(4u, points_), 0.0f);
   EXPECT_EQ(Interpolate(6u, points_), 0.5f);
   EXPECT_EQ(Interpolate(8u, points_), 1.0f);
}

TEST(BasicTest, InterpolateFUTest)
{
   static const std::vector<std::pair<float, uint32_t>> points_({{-1.0f, 0u}, //
                                                                 {1.0f, 255u}});
   EXPECT_EQ(Interpolate(-1.0f, points_), 0u);
   EXPECT_EQ(Interpolate(0.0f, points_), 127u);
   EXPECT_EQ(Interpolate(0.5f, points_), 191u);
   EXPECT_EQ(Interpolate(1.0f, points_), 255u);
}

} // namespace WorldEngine
