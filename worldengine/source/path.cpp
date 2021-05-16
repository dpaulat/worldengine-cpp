#include "path.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

struct Node;
typedef std::shared_ptr<Node> NodePtr;
typedef std::list<NodePtr>    NodeList;

const uint32_t MAX_PATH_ITERATIONS = 10000u;

/**
 * @brief The basic unit, pixel or location
 */
struct Node
{
   Point   location_;     /**< Where the node is located */
   float   movementCost_; /**< Total move cost to reach this node */
   float   score_;        /**< Calculated score for this node */
   NodePtr parent_;       /**< Parent node */

   Node(Point location, float movementCost, NodePtr parent = nullptr) :
       location_(location),
       movementCost_(movementCost),
       parent_(parent),
       score_(0.0f)
   {
   }
};

struct Path
{
   NodeList nodes_;
   float    totalCost_;

   Path(NodeList nodes, float totalCost) : nodes_(nodes), totalCost_(totalCost)
   {
   }
};

bool operator==(const Node& lhs, const Node& rhs)
{
   return lhs.location_ == rhs.location_;
}

/**
 * @brief A simple square map implementation
 */
struct SQMapHandler
{
   const ElevationArrayType& mapData_;
   const int32_t             width_;
   const int32_t             height_;

   SQMapHandler(const ElevationArrayType& mapData,
                int32_t                   width,
                int32_t                   height) :
       mapData_(mapData), width_(width), height_(height)
   {
   }

   NodeList GetAdjacentNodes(NodePtr currentNode, Point destination) const
   {
      NodeList result;

      const Point& cl = currentNode->location_;
      const Point& dl = destination;

      NodePtr n;

      n = HandleNode(cl.first + 1, cl.second, currentNode, dl.first, dl.second);
      if (n != nullptr)
      {
         result.push_back(n);
      }

      n = HandleNode(cl.first - 1, cl.second, currentNode, dl.first, dl.second);
      if (n != nullptr)
      {
         result.push_back(n);
      }

      n = HandleNode(cl.first, cl.second + 1, currentNode, dl.first, dl.second);
      if (n != nullptr)
      {
         result.push_back(n);
      }

      n = HandleNode(cl.first, cl.second - 1, currentNode, dl.first, dl.second);
      if (n != nullptr)
      {
         result.push_back(n);
      }

      return result;
   }

   NodePtr GetNode(Point location) const
   {
      int32_t x = location.first;
      int32_t y = location.second;
      if (x < 0 || x >= width_ || y < 0 || y >= height_)
      {
         return nullptr;
      }
      float d = mapData_[y][x];
      return std::make_shared<Node>(location, d);
   }

   NodePtr HandleNode(int32_t x,
                      int32_t y,
                      NodePtr fromNode,
                      int32_t destX,
                      int32_t destY) const
   {
      NodePtr n = GetNode({x, y});

      if (n != nullptr)
      {
         int32_t dx     = std::max(x, destX) - std::min(x, destX);
         int32_t dy     = std::max(y, destY) - std::min(y, destY);
         int32_t emCost = dx + dy;
         n->movementCost_ += fromNode->movementCost_;
         n->score_  = n->movementCost_ + emCost;
         n->parent_ = fromNode;
      }

      return n;
   }
};

/**
 * @brief The A* Search Algorithm
 *
 * https://en.wikipedia.org/wiki/A*_search_algorithm
 *
 * TODO: Use Boost implementation
 */
struct AStar
{
   const SQMapHandler& mapHandler_;
   NodeList            openSet;
   NodeList            closedSet;

   AStar(const SQMapHandler& mapHandler) : mapHandler_(mapHandler) {}

   NodePtr GetBestOpenNode()
   {
      NodePtr bestNode = nullptr;

      for (NodePtr n : openSet)
      {
         if (bestNode == nullptr || n->score_ <= bestNode->score_)
         {
            bestNode = n;
         }
      }

      return bestNode;
   }

   static Path TracePath(NodePtr n)
   {
      NodeList nodes;
      NodePtr  p         = n->parent_;
      float    totalCost = n->movementCost_;

      nodes.push_front(n);

      while (p->parent_ != nullptr)
      {
         nodes.push_front(p);
         p = p->parent_;
      }

      return Path(nodes, totalCost);
   }

   NodePtr HandleNode(NodePtr node, Point end)
   {
      openSet.remove_if([&node](const NodePtr o) { return *node == *o; });
      closedSet.push_back(node);

      NodeList nodes = mapHandler_.GetAdjacentNodes(node, end);

      NodeList::iterator openNode;

      for (NodePtr n : nodes)
      {
         if (n->location_ == end)
         {
            // Reached the destination
            return n;
         }
         else if (std::find_if(closedSet.begin(), //
                               closedSet.end(),
                               [&n](const NodePtr o) { return *n == *o; }) !=
                  closedSet.end())
         {
            // Already in closed set, skip this
            continue;
         }
         else if ((openNode = std::find_if(openSet.begin(), //
                                           openSet.end(),
                                           [&n](const NodePtr o) {
                                              return *n == *o;
                                           })) != openSet.end())
         {
            // Already in open set, check if better score
            if (n->movementCost_ < (*openNode)->movementCost_)
            {
               openSet.erase(openNode);
               openSet.push_back(n);
            }
         }
         else
         {
            // New node, append to open set
            openSet.push_back(n);
         }
      }

      return nullptr;
   }

   Path FindPath(Point fromLocation, Point toLocation)
   {
      NodePtr nextNode = mapHandler_.GetNode(fromLocation);
      openSet.push_back(nextNode);

      uint32_t counter = 0;

      while (nextNode != nullptr)
      {
         if (counter > MAX_PATH_ITERATIONS)
         {
            BOOST_LOG_TRIVIAL(warning) << "FindPath: Exceeded trial limit";
            break;
         }

         NodePtr finish = HandleNode(nextNode, toLocation);
         if (finish != nullptr)
         {
            return TracePath(finish);
         }

         nextNode = GetBestOpenNode();
         counter++;
      }

      return Path(NodeList(), 0.0f);
   }
};

std::list<Point>
FindPath(const ElevationArrayType& elevation, Point source, Point destination)
{
   std::list<Point> path;

   const int32_t width  = static_cast<int32_t>(elevation.shape()[1]);
   const int32_t height = static_cast<int32_t>(elevation.shape()[0]);

   SQMapHandler mapHandler(elevation, width, height);
   AStar        pathFinder(mapHandler);
   Path         p = pathFinder.FindPath(source, destination);

   for (NodePtr n : p.nodes_)
   {
      path.push_back(n->location_);
   }

   return path;
}

} // namespace WorldEngine
