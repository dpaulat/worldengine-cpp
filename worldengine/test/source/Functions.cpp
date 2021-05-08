#include "Functions.h"

#include <fstream>

#include <boost/log/trivial.hpp>

namespace WorldEngine
{

bool LoadWorld(World& world, const std::string& filename)
{
   const std::string worldFilename = TEST_DATA_DIR + filename;

   BOOST_LOG_TRIVIAL(info) << "Loading " << worldFilename;

   std::ifstream input(worldFilename,
                       std::ios_base::in | std::ios_base::binary);
   bool          success = world.ProtobufDeserialize(input);

   return success;
}

} // namespace WorldEngine
