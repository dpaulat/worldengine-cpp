#include "world.h"

#include <boost/log/trivial.hpp>
#include <boost/python/numpy.hpp>

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

} // namespace WorldEngine
