#include <gtest/gtest.h>

#include <basic.h>

namespace WorldEngine
{

TEST(BasicTest, InterpolateTest)
{
   const std::vector<std::pair<float, float>> points_({{-0.5f, 0.0f}, //
                                                       {0.0f, 1.0f},
                                                       {0.5f, 0.0f}});
   EXPECT_EQ(Interpolate(-0.55f, points_), 0.0f);
   EXPECT_EQ(Interpolate(0.55f, points_), 0.0f);
   EXPECT_EQ(Interpolate(0.0f, points_), 1.0f);
   EXPECT_EQ(Interpolate(-0.1f, points_), 0.8f);
}

} // namespace WorldEngine
