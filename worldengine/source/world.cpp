#include "world.h"

#include <boost/log/trivial.hpp>
#include <boost/tokenizer.hpp>

#include <World.pb.h>

namespace std
{
template<typename T>
std::ostream& operator<<(std::ostream& os, const boost::multi_array<T, 2>& a)
{
   boost::multi_array<T, 2>::const_iterator i;

   os << "[";
   for (uint32_t y = 0; y < a.shape()[0]; y++)
   {
      if (y > 0)
      {
         os << std::endl << " ";
      }
      os << "[";
      for (uint32_t x = 0; x < a.shape()[1]; x++)
      {
         os << a[y][x];

         if (x == 2 && x < a.shape()[1] - 4)
         {
            x = a.shape()[1] - 4;
            os << " ...";
         }

         if (x < a.shape()[1] - 1)
         {
            os << " ";
         }
      }

      os << "]";

      if (y == 2 && y < a.shape()[0] - 4)
      {
         y = a.shape()[0] - 4;
         os << std::endl << " ...";
      }
   }
   os << "]";

   return os;
}
} // namespace std

namespace WorldEngine
{

World::World(const std::string&          name,
             Size                        size,
             uint32_t                    seed,
             const GenerationParameters& generationParams,
             const std::vector<float>&   temps,
             const std::vector<float>&   humids,
             float                       gammaCurve,
             float                       curveOffset) :
    name_(name),
    size_(size),
    seed_(seed),
    generationParams_(generationParams),
    temps_(temps),
    humids_(humids),
    gammaCurve_(gammaCurve),
    curveOffset_(curveOffset),
    elevation_(),
    plates_(),
    ocean_()
{
}

World::~World() {}

const std::string& World::name() const
{
   return name_;
}

uint32_t World::width() const
{
   return size_.width_;
}

uint32_t World::height() const
{
   return size_.height_;
}

uint32_t World::seed() const
{
   return seed_;
}

uint32_t World::numPlates() const
{
   return generationParams_.numPlates_;
}

float World::oceanLevel() const
{
   return generationParams_.oceanLevel_;
}

const Step& World::step() const
{
   return generationParams_.step_;
}

bool World::HasBiome() const
{
   return false;
}

bool World::HasHumidity() const
{
   return false;
}

bool World::HasIrrigiation() const
{
   return false;
}

bool World::HasPermeability() const
{
   return false;
}

bool World::HasWatermap() const
{
   return false;
}

bool World::HasPrecipitations() const
{
   return false;
}

bool World::HasTemperature() const
{
   return false;
}

ElevationArrayType& World::GetElevationData()
{
   return elevation_;
}

OceanArrayType& World::GetOceanData()
{
   return ocean_;
}

PlateArrayType& World::GetPlateData()
{
   return plates_;
}

void World::SetElevationData(const float* heightmap)
{
   SetArrayData(heightmap, elevation_);

   BOOST_LOG_TRIVIAL(debug) << "Elevation multi_array:" << std::endl
                            << elevation_;
}

void World::SetPlatesData(const uint32_t* platesmap)
{
   SetArrayData(platesmap, plates_);

   BOOST_LOG_TRIVIAL(debug) << "Platesmap multi_array:" << std::endl << plates_;
}

bool World::ProtobufSerialize(std::string& output) const
{
   bool success = false;

   ::World::World pbWorld;

   ::World::World_GenerationData* pbGenerationData =
      new ::World::World_GenerationData();
   ::World::World_DoubleMatrix* pbHeightmapData =
      new ::World::World_DoubleMatrix();
   ::World::World_IntegerMatrix* pbPlates = new ::World::World_IntegerMatrix();
   ::World::World_BooleanMatrix* pbOcean  = new ::World::World_BooleanMatrix();
   ::World::World_DoubleMatrix*  pbSeaDepth = new ::World::World_DoubleMatrix();

   pbWorld.set_worldengine_tag(WorldengineTag());
   pbWorld.set_worldengine_version(VersionHashcode());

   pbWorld.set_name(name_);
   pbWorld.set_width(size_.width_);
   pbWorld.set_height(size_.height_);

   pbGenerationData->set_seed(seed_);
   pbGenerationData->set_n_plates(generationParams_.numPlates_);
   pbGenerationData->set_ocean_level(generationParams_.oceanLevel_);
   pbGenerationData->set_step(generationParams_.step_.name());
   pbWorld.set_allocated_generationdata(pbGenerationData);

   // Elevation
   // TODO
   pbWorld.set_allocated_heightmapdata(pbHeightmapData);
   pbWorld.set_heightmapth_sea(0.0);
   pbWorld.set_heightmapth_plain(0.0);
   pbWorld.set_heightmapth_hill(0.0);

   // Plates
   // TODO
   pbWorld.set_allocated_plates(pbPlates);

   // Ocean
   // TODO
   pbWorld.set_allocated_ocean(pbOcean);
   pbWorld.set_allocated_sea_depth(pbSeaDepth);

   try
   {
      success = pbWorld.SerializeToString(&output);
   }
   catch (const std::exception& ex)
   {
      BOOST_LOG_TRIVIAL(error) << ex.what();
   }

   return success;
}

int32_t World::WorldengineTag()
{
   return ('W' << 24) | ('o' << 16) | ('e' << 8) | 'n';
}

int32_t World::VersionHashcode()
{
   int hashcode = 0;

   boost::char_separator<char>                   sep(".");
   boost::tokenizer<boost::char_separator<char>> t(WORLDENGINE_VERSION, sep);
   for (auto it = t.begin(); it != t.end(); it++)
   {
      // 3 version components
      hashcode = (hashcode << 8) | std::stoi(*it);
   }

   // 4th component (0)
   hashcode <<= 8;

   return hashcode;
}

template<typename T, typename U>
void World::SetArrayData(const U* source, boost::multi_array<T, 2>& dest)
{
   dest.resize(boost::extents[size_.height_][size_.width_]);

   for (uint32_t y = 0; y < size_.height_; y++)
   {
      uint32_t rowOffset = y * size_.width_;

      for (uint32_t x = 0; x < size_.width_; x++)
      {
         dest[y][x] = static_cast<T>(source[x + rowOffset]);
      }
   }
}

} // namespace WorldEngine
