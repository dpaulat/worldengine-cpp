#include "world.h"

#include <boost/log/trivial.hpp>
#include <boost/python/numpy.hpp>
#include <boost/tokenizer.hpp>

#include <World.pb.h>

namespace py = boost::python;
namespace np = boost::python::numpy;

namespace WorldEngine
{

typedef float    ElevationType;
typedef uint32_t PlateType;

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
    elevation_(
       np::empty(py::make_tuple(0), np::dtype::get_builtin<ElevationType>())),
    plates_(np::empty(py::make_tuple(0), np::dtype::get_builtin<PlateType>()))
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

void World::SetElevationData(const float* heightmap)
{
   // TODO: Can we optimize by making a 2D ndarray instead of reshaping?
   py::tuple  shape  = py::make_tuple(size_.height_ * size_.width_);
   py::tuple  stride = py::make_tuple(sizeof(float));
   py::tuple  size   = py::make_tuple(size_.height_, size_.width_);
   py::object own;

   elevation_ =
      np::from_data(
         heightmap, np::dtype::get_builtin<ElevationType>(), shape, stride, own)
         .reshape(size);

   BOOST_LOG_TRIVIAL(debug) << "Elevation ndarray:" << std::endl
                            << py::extract<const char*>(py::str(elevation_));
}

void World::SetPlatesData(const uint32_t* platesmap)
{
   // TODO: Can we optimize by making a 2D ndarray instead of reshaping?
   py::tuple  shape  = py::make_tuple(size_.height_ * size_.width_);
   py::tuple  stride = py::make_tuple(sizeof(float));
   py::tuple  size   = py::make_tuple(size_.height_, size_.width_);
   py::object own;

   plates_ =
      np::from_data(
         platesmap, np::dtype::get_builtin<PlateType>(), shape, stride, own)
         .reshape(size);

   BOOST_LOG_TRIVIAL(debug) << "Platesmap ndarray:" << std::endl
                            << py::extract<const char*>(py::str(plates_));
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

} // namespace WorldEngine
