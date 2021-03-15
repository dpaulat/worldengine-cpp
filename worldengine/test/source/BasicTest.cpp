#include <gtest/gtest.h>

#include <basic.h>

namespace WorldEngine
{

TEST(BasicTest, InterpolateFTest)
{
   const std::vector<std::pair<float, float>> points_({{-0.5f, 0.0f}, //
                                                       {0.0f, 1.0f},
                                                       {0.5f, 0.0f}});
   EXPECT_EQ(InterpolateF(-0.55, points_), 0.0f);
   EXPECT_EQ(InterpolateF(0.55, points_), 0.0f);
   EXPECT_EQ(InterpolateF(0.0f, points_), 1.0f);
   EXPECT_EQ(InterpolateF(-0.1f, points_), 0.8f);
}

} // namespace WorldEngine
