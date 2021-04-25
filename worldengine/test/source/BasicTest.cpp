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

TEST(BasicTest, AntiAliasTest)
{
   boost::multi_array<float, 2> map(boost::extents[3][3]);
   map[0][0] = 0;
   map[0][1] = 0;
   map[0][2] = 1;
   map[1][0] = 0;
   map[1][1] = 1;
   map[1][2] = 1;
   map[2][0] = 0;
   map[2][1] = 1;
   map[2][2] = 2;

   AntiAlias(map);

   EXPECT_FLOAT_EQ(map[0][0], 0.54545455f);
   EXPECT_FLOAT_EQ(map[0][1], 0.54545455f);
   EXPECT_FLOAT_EQ(map[0][2], 0.72727273f);
   EXPECT_FLOAT_EQ(map[1][0], 0.54545455f);
   EXPECT_FLOAT_EQ(map[1][1], 0.72727273f);
   EXPECT_FLOAT_EQ(map[1][2], 0.72727273f);
   EXPECT_FLOAT_EQ(map[2][0], 0.54545455f);
   EXPECT_FLOAT_EQ(map[2][1], 0.72727273f);
   EXPECT_FLOAT_EQ(map[2][2], 0.90909091f);
}

TEST(BasicTest, CountNeighborsTest)
{
   boost::multi_array<bool, 2> map(boost::extents[3][3]);
   map[0][0] = 0;
   map[0][1] = 0;
   map[0][2] = true;
   map[1][0] = 0;
   map[1][1] = true;
   map[1][2] = true;
   map[2][0] = 0;
   map[2][1] = true;
   map[2][2] = true;

   boost::multi_array<uint32_t, 2> n = CountNeighbors(map);

   EXPECT_EQ(n[0][0], 1);
   EXPECT_EQ(n[0][1], 3);
   EXPECT_EQ(n[0][2], 2);
   EXPECT_EQ(n[1][0], 2);
   EXPECT_EQ(n[1][1], 4);
   EXPECT_EQ(n[1][2], 4);
   EXPECT_EQ(n[2][0], 2);
   EXPECT_EQ(n[2][1], 3);
   EXPECT_EQ(n[2][2], 3);
}

} // namespace WorldEngine
