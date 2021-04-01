#include "world.h"

#include <random>

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
    ocean_(),
    biome_(),
    humidity_(),
    icecap_(),
    irrigation_(),
    precipitation_(),
    temperature_(),
    waterMap_(),
    elevationThresholds_ {0.0f, 0.0f, 0.0f},
    humidityThresholds_ {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    precipitationThresholds_ {0.0f, 0.0f, 0.0f},
    temperatureThresholds_ {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
    waterThresholds_ {0.0f, 0.0f, 0.0f}
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

const std::vector<float>& World::temps() const
{
   return temps_;
}

const std::vector<float>& World::humids() const
{
   return humids_;
}

float World::gammaCurve() const
{
   return gammaCurve_;
}

float World::curveOffset() const
{
   return curveOffset_;
}

bool World::Contains(int32_t x, int32_t y) const
{
   return (0 <= x && x < width() && 0 <= y && y < height());
}

bool World::HasBiome() const
{
   return false;
}

bool World::HasHumidity() const
{
   return !humidity_.empty();
}

bool World::HasIrrigiation() const
{
   return !irrigation_.empty();
}

bool World::HasPermeability() const
{
   return false;
}

bool World::HasWatermap() const
{
   return !waterMap_.empty();
}

bool World::HasPrecipitations() const
{
   return !precipitation_.empty();
}

bool World::HasTemperature() const
{
   return !temperature_.empty();
}

const ElevationArrayType& World::GetElevationData() const
{
   return elevation_;
}

const OceanArrayType& World::GetOceanData() const
{
   return ocean_;
}

const BiomeArrayType& World::GetBiomeData() const
{
   return biome_;
}

const HumidityArrayType& World::GetHumidityData() const
{
   return humidity_;
}

const IrrigationArrayType& World::GetIrrigationData() const
{
   return irrigation_;
}

const LakeMapArrayType& World::GetLakeMapData() const
{
   return lakeMap_;
}

const PrecipitationArrayType& World::GetPrecipitationData() const
{
   return precipitation_;
}

const RiverMapArrayType& World::GetRiverMapData() const
{
   return riverMap_;
}

const TemperatureArrayType& World::GetTemperatureData() const
{
   return temperature_;
}

const WaterMapArrayType& World::GetWaterMapData() const
{
   return waterMap_;
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

BiomeArrayType& World::GetBiomeData()
{
   return biome_;
}

HumidityArrayType& World::GetHumidityData()
{
   return humidity_;
}

IrrigationArrayType& World::GetIrrigationData()
{
   return irrigation_;
}

LakeMapArrayType& World::GetLakeMapData()
{
   return lakeMap_;
}

PrecipitationArrayType& World::GetPrecipitationData()
{
   return precipitation_;
}

RiverMapArrayType& World::GetRiverMapData()
{
   return riverMap_;
}

TemperatureArrayType& World::GetTemperatureData()
{
   return temperature_;
}

WaterMapArrayType& World::GetWaterMapData()
{
   return waterMap_;
}

float World::GetThreshold(ElevationThresholdType type) const
{
   if (type > ElevationThresholdType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   return elevationThresholds_[static_cast<uint32_t>(type)];
}

float World::GetThreshold(HumidityLevels type) const
{
   if (type > HumidityLevels::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   return humidityThresholds_[static_cast<uint32_t>(type)];
}

float World::GetThreshold(PrecipitationLevelType type) const
{
   if (type > PrecipitationLevelType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   return precipitationThresholds_[static_cast<uint32_t>(type)];
}

float World::GetThreshold(TemperatureType type) const
{
   if (type > TemperatureType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   return temperatureThresholds_[static_cast<uint32_t>(type)];
}

float World::GetThreshold(WaterThresholds type) const
{
   if (type > WaterThresholds::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   return waterThresholds_[static_cast<uint32_t>(type)];
}

bool World::IsLand(uint32_t x, uint32_t y) const
{
   return !ocean_[y][x];
}

bool World::IsOcean(uint32_t x, uint32_t y) const
{
   return ocean_[y][x];
}

bool World::IsOcean(Point p) const
{
   return IsOcean(p.first, p.second);
}

bool World::IsMountain(uint32_t x, uint32_t y) const
{
   if (IsOcean(x, y))
   {
      return false;
   }

   return elevation_[y][x] > GetThreshold(ElevationThresholdType::Mountain);
}

TemperatureType World::GetTemperatureType(uint32_t x, uint32_t y) const
{
   uint32_t width  = temperature_.shape()[1];
   uint32_t height = temperature_.shape()[0];

   if (x >= width || y >= height)
   {
      throw std::invalid_argument("Coordinates out of bounds");
   }

   float temperature = temperature_[y][x];

   for (uint32_t i = static_cast<uint32_t>(TemperatureType::First);
        i < static_cast<uint32_t>(TemperatureType::Last);
        i++)
   {
      TemperatureType type = static_cast<TemperatureType>(i);

      if (temperature < GetThreshold(type))
      {
         return type;
      }
   }

   return TemperatureType::Last;
}

HumidityLevels World::GetHumidityLevel(uint32_t x, uint32_t y) const
{
   uint32_t width  = precipitation_.shape()[1];
   uint32_t height = precipitation_.shape()[0];

   if (x >= width || y >= height)
   {
      throw std::invalid_argument("Coordinates out of bounds");
   }

   float humidity = humidity_[y][x];

   for (uint32_t i = static_cast<uint32_t>(HumidityLevels::First);
        i < static_cast<uint32_t>(HumidityLevels::Last);
        i++)
   {
      HumidityLevels type = static_cast<HumidityLevels>(i);

      if (humidity < GetThreshold(type))
      {
         return type;
      }
   }

   return HumidityLevels::Last;
}

void World::GetRandomLand(
   std::vector<std::pair<uint32_t, uint32_t>>& landSamples,
   uint32_t                                    numSamples,
   uint32_t                                    seed) const
{
   std::vector<std::pair<uint32_t, uint32_t>> land;

   uint32_t width  = ocean_.shape()[1];
   uint32_t height = ocean_.shape()[0];

   for (uint32_t y = 0; y < height; y++)
   {
      for (uint32_t x = 0; x < width; x++)
      {
         if (!ocean_[y][x])
         {
            land.push_back({x, y});
         }
      }
   }

   if (land.size() == 0)
   {
      return;
   }

   std::default_random_engine              generator(seed);
   std::uniform_int_distribution<uint32_t> distribution(0, land.size() - 1);

   for (uint32_t i = 0; i < numSamples; i++)
   {
      landSamples.push_back(land[distribution(generator)]);
   }
}

void World::GetTilesAround(std::vector<std::pair<uint32_t, uint32_t>>& tiles,
                           uint32_t                                    x,
                           uint32_t                                    y) const
{
   int32_t radius = 1;
   for (int32_t dx = -radius; dx <= radius; dx++)
   {
      int32_t nx = static_cast<int32_t>(x) + dx;
      if (0 <= nx && nx < width())
      {
         for (int32_t dy = -radius; dy <= radius; dy++)
         {
            int32_t ny = static_cast<int32_t>(y) + dy;
            if (0 <= ny && ny < height())
            {
               tiles.push_back({nx, ny});
            }
         }
      }
   }
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

void World::SetThreshold(ElevationThresholdType type, float value)
{
   if (type > ElevationThresholdType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   elevationThresholds_[static_cast<uint32_t>(type)] = value;
}

void World::SetThreshold(HumidityLevels type, float value)
{
   if (type > HumidityLevels::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   humidityThresholds_[static_cast<uint32_t>(type)] = value;
}

void World::SetThreshold(PrecipitationLevelType type, float value)
{
   if (type > PrecipitationLevelType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   precipitationThresholds_[static_cast<uint32_t>(type)] = value;
}

void World::SetThreshold(TemperatureType type, float value)
{
   if (type > TemperatureType::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   temperatureThresholds_[static_cast<uint32_t>(type)] = value;
}

void World::SetThreshold(WaterThresholds type, float value)
{
   if (type > WaterThresholds::Last)
   {
      throw std::invalid_argument("Invalid threshold type");
   }

   waterThresholds_[static_cast<uint32_t>(type)] = value;
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
   pbWorld.set_heightmapth_sea(GetThreshold(ElevationThresholdType::Sea));
   pbWorld.set_heightmapth_plain(GetThreshold(ElevationThresholdType::Hill));
   pbWorld.set_heightmapth_hill(GetThreshold(ElevationThresholdType::Mountain));

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
