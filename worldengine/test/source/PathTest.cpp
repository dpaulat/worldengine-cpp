#include <gtest/gtest.h>

#include <path.h>

namespace WorldEngine
{

TEST(PathTest, TraversalTest)
{
   ElevationArrayType testMap(boost::extents[20][20]);

   std::fill(testMap.data(), testMap.data() + testMap.num_elements(), 0.0f);

   for (uint32_t i = 0; i < 20; i++)
   {
      testMap[10][i] = 1.0f;
   }
   testMap[10][18] = 0.0f;

   Point source({0, 0});
   Point destination({19, 19});

   static const std::list<Point> pathData = {
      {0, 1},   {0, 2},   {0, 3},   {0, 4},   {0, 5},   {0, 6},   {0, 7},
      {0, 8},   {0, 9},   {1, 9},   {2, 9},   {3, 9},   {4, 9},   {5, 9},
      {6, 9},   {7, 9},   {8, 9},   {9, 9},   {10, 9},  {11, 9},  {12, 9},
      {13, 9},  {14, 9},  {15, 9},  {16, 9},  {17, 9},  {18, 9},  {18, 10},
      {18, 11}, {18, 12}, {18, 13}, {18, 14}, {18, 15}, {18, 16}, {18, 17},
      {18, 18}, {18, 19}, {19, 19}};
   std::list<Point> shortestPath = FindPath(testMap, source, destination);

   EXPECT_EQ(pathData, shortestPath);
}

} // namespace WorldEngine
