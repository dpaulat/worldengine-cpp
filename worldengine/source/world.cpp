#include "world.h"

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
    curveOffset_(curveOffset)
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

void World::SetElevationData(const boost::python::numpy::ndarray& data) {}
void World::SetPlatesData(const boost::python::numpy::ndarray& data) {}

} // namespace WorldEngine
