#include "worldengine/world.h"

#include <random>

#include <boost/assign.hpp>
#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/log/trivial.hpp>
#include <boost/tokenizer.hpp>

#include <World.pb.h>

namespace std
{
template<typename T>
std::ostream& operator<<(std::ostream& os, const boost::multi_array<T, 2>& a)
{
   boost::multi_array<T, 2>::const_iterator i;

   const int32_t width  = a.shape()[1];
   const int32_t height = a.shape()[0];

   os << "[";
   for (int32_t y = 0; y < height; y++)
   {
      if (y > 0)
      {
         os << std::endl << " ";
      }
      os << "[";
      for (int32_t x = 0; x < width; x++)
      {
         os << a[y][x];

         if (x == 2 && x < width - 4)
         {
            x = width - 4;
            os << " ...";
         }

         if (x < width - 1)
         {
            os << " ";
         }
      }

      os << "]";

      if (y == 2 && y < height - 4)
      {
         y = height - 4;
         os << std::endl << " ...";
      }
   }
   os << "]";

   return os;
}
} // namespace std

namespace WorldEngine
{

template<class T, class U, class V = T>
static void ToProtobufMatrix(
   const boost::multi_array<T, 2>&   source,
   U*                                pbMatrix,
   const std::function<V(const T&)>& transform = [](const T& value) {
      return value;
   });

template<class T, class U, class V = U>
static void FromProtobufMatrix(
   const T&                          pbMatrix,
   boost::multi_array<U, 2>&         dest,
   const std::function<U(const V&)>& transform = [](const V& value) {
      return value;
   });

static int32_t WorldengineTag();
static int32_t VersionHashcode();

typedef boost::bimap<boost::bimaps::unordered_set_of<HumidityLevel>,
                     boost::bimaps::unordered_set_of<int>>
                                 HumidityQuantileMap;
static const HumidityQuantileMap humidityQuantiles_ =
   boost::assign::list_of<HumidityQuantileMap::relation> //
   (HumidityLevel::Superarid, 12)                        //
   (HumidityLevel::Perarid, 25)                          //
   (HumidityLevel::Arid, 37)                             //
   (HumidityLevel::Semiarid, 50)                         //
   (HumidityLevel::Subhumid, 62)                         //
   (HumidityLevel::Humid, 75)                            //
   (HumidityLevel::Perhumid, 87);

typedef boost::bimap<boost::bimaps::unordered_set_of<Biome>,
                     boost::bimaps::unordered_set_of<int>>
                           BiomeIndexMap;
static const BiomeIndexMap biomeIndices_ =
   boost::assign::list_of<BiomeIndexMap::relation> //
   (Biome::BorealDesert, 0)                        //
   (Biome::BorealDryScrub, 1)                      //
   (Biome::BorealMoistForest, 2)                   //
   (Biome::BorealRainForest, 3)                    //
   (Biome::BorealWetForest, 4)                     //
   (Biome::CoolTemperateDesert, 5)                 //
   (Biome::CoolTemperateDesertScrub, 6)            //
   (Biome::CoolTemperateMoistForest, 7)            //
   (Biome::CoolTemperateRainForest, 8)             //
   (Biome::CoolTemperateSteppe, 9)                 //
   (Biome::CoolTemperateWetForest, 10)             //
   (Biome::Ice, 11)                                //
   (Biome::Ocean, 12)                              //
   (Biome::PolarDesert, 13)                        //
   (Biome::Sea, 14)                                //
   (Biome::SubpolarDryTundra, 15)                  //
   (Biome::SubpolarMoistTundra, 16)                //
   (Biome::SubpolarRainTundra, 17)                 //
   (Biome::SubpolarWetTundra, 18)                  //
   (Biome::SubtropicalDesert, 19)                  //
   (Biome::SubtropicalDesertScrub, 20)             //
   (Biome::SubtropicalDryForest, 21)               //
   (Biome::SubtropicalMoistForest, 22)             //
   (Biome::SubtropicalRainForest, 23)              //
   (Biome::SubtropicalThornWoodland, 24)           //
   (Biome::SubtropicalWetForest, 25)               //
   (Biome::TropicalDesert, 26)                     //
   (Biome::TropicalDesertScrub, 27)                //
   (Biome::TropicalDryForest, 28)                  //
   (Biome::TropicalMoistForest, 29)                //
   (Biome::TropicalRainForest, 30)                 //
   (Biome::TropicalThornWoodland, 31)              //
   (Biome::TropicalVeryDryForest, 32)              //
   (Biome::TropicalWetForest, 33)                  //
   (Biome::WarmTemperateDesert, 34)                //
   (Biome::WarmTemperateDesertScrub, 35)           //
   (Biome::WarmTemperateDryForest, 36)             //
   (Biome::WarmTemperateMoistForest, 37)           //
   (Biome::WarmTemperateRainForest, 38)            //
   (Biome::WarmTemperateThornScrub, 39)            //
   (Biome::WarmTemperateWetForest, 40)             //
   (Biome::BareRock, -1);

static const std::unordered_map<Biome, BiomeGroup> biomeGroups_ = {
   {Biome::Ocean, BiomeGroup::None},
   {Biome::Sea, BiomeGroup::None},
   {Biome::PolarDesert, BiomeGroup::Iceland},
   {Biome::Ice, BiomeGroup::Iceland},
   {Biome::SubpolarDryTundra, BiomeGroup::ColdParklands},
   {Biome::SubpolarMoistTundra, BiomeGroup::Tundra},
   {Biome::SubpolarWetTundra, BiomeGroup::Tundra},
   {Biome::SubpolarRainTundra, BiomeGroup::Tundra},
   {Biome::BorealDesert, BiomeGroup::ColdParklands},
   {Biome::BorealDryScrub, BiomeGroup::ColdParklands},
   {Biome::BorealMoistForest, BiomeGroup::BorealForest},
   {Biome::BorealWetForest, BiomeGroup::BorealForest},
   {Biome::BorealRainForest, BiomeGroup::BorealForest},
   {Biome::CoolTemperateDesert, BiomeGroup::CoolDesert},
   {Biome::CoolTemperateDesertScrub, BiomeGroup::CoolDesert},
   {Biome::CoolTemperateSteppe, BiomeGroup::Steppe},
   {Biome::CoolTemperateMoistForest, BiomeGroup::CoolTemperateForest},
   {Biome::CoolTemperateWetForest, BiomeGroup::CoolTemperateForest},
   {Biome::CoolTemperateRainForest, BiomeGroup::CoolTemperateForest},
   {Biome::WarmTemperateDesert, BiomeGroup::HotDesert},
   {Biome::WarmTemperateDesertScrub, BiomeGroup::HotDesert},
   {Biome::WarmTemperateThornScrub, BiomeGroup::Chaparral},
   {Biome::WarmTemperateDryForest, BiomeGroup::Chaparral},
   {Biome::WarmTemperateMoistForest, BiomeGroup::WarmTemperateForest},
   {Biome::WarmTemperateWetForest, BiomeGroup::WarmTemperateForest},
   {Biome::WarmTemperateRainForest, BiomeGroup::WarmTemperateForest},
   {Biome::SubtropicalDesert, BiomeGroup::HotDesert},
   {Biome::SubtropicalDesertScrub, BiomeGroup::HotDesert},
   {Biome::SubtropicalThornWoodland, BiomeGroup::Savanna},
   {Biome::SubtropicalDryForest, BiomeGroup::TropicalDryForest},
   {Biome::SubtropicalMoistForest, BiomeGroup::Jungle},
   {Biome::SubtropicalWetForest, BiomeGroup::Jungle},
   {Biome::SubtropicalRainForest, BiomeGroup::Jungle},
   {Biome::TropicalDesert, BiomeGroup::HotDesert},
   {Biome::TropicalDesertScrub, BiomeGroup::HotDesert},
   {Biome::TropicalThornWoodland, BiomeGroup::Savanna},
   {Biome::TropicalVeryDryForest, BiomeGroup::Savanna},
   {Biome::TropicalDryForest, BiomeGroup::TropicalDryForest},
   {Biome::TropicalMoistForest, BiomeGroup::Jungle},
   {Biome::TropicalWetForest, BiomeGroup::Jungle},
   {Biome::TropicalRainForest, BiomeGroup::Jungle},
   {Biome::BareRock, BiomeGroup::None},
};

World::World() : seed_(0), gammaCurve_(0.0f), curveOffset_(0.0f) {}

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
    permeability_(),
    precipitation_(),
    seaDepth_(),
    temperature_(),
    waterMap_(),
    elevationThresholds_(),
    humidityThresholds_(),
    permeabilityThresholds_(),
    precipitationThresholds_(),
    temperatureThresholds_(),
    waterThresholds_()
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
   return !biome_.empty();
}

bool World::HasHumidity() const
{
   return !humidity_.empty();
}

bool World::HasIcecap() const
{
   return !icecap_.empty();
}

bool World::HasIrrigiation() const
{
   return !irrigation_.empty();
}

bool World::HasLakemap() const
{
   return !lakeMap_.empty();
}

bool World::HasPermeability() const
{
   return !permeability_.empty();
}

bool World::HasRivermap() const
{
   return !riverMap_.empty();
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

const IcecapArrayType& World::GetIcecapData() const
{
   return icecap_;
}

const IrrigationArrayType& World::GetIrrigationData() const
{
   return irrigation_;
}

const LakeMapArrayType& World::GetLakeMapData() const
{
   return lakeMap_;
}

const PermeabilityArrayType& World::GetPermeabilityData() const
{
   return permeability_;
}

const PrecipitationArrayType& World::GetPrecipitationData() const
{
   return precipitation_;
}

const RiverMapArrayType& World::GetRiverMapData() const
{
   return riverMap_;
}

const SeaDepthArrayType& World::GetSeaDepthData() const
{
   return seaDepth_;
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

IcecapArrayType& World::GetIcecapData()
{
   return icecap_;
}

IrrigationArrayType& World::GetIrrigationData()
{
   return irrigation_;
}

LakeMapArrayType& World::GetLakeMapData()
{
   return lakeMap_;
}

PermeabilityArrayType& World::GetPermeabilityData()
{
   return permeability_;
}

PrecipitationArrayType& World::GetPrecipitationData()
{
   return precipitation_;
}

RiverMapArrayType& World::GetRiverMapData()
{
   return riverMap_;
}

SeaDepthArrayType& World::GetSeaDepthData()
{
   return seaDepth_;
}

TemperatureArrayType& World::GetTemperatureData()
{
   return temperature_;
}

WaterMapArrayType& World::GetWaterMapData()
{
   return waterMap_;
}

float World::GetThreshold(ElevationThreshold type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (elevationThresholds_.find(type) != elevationThresholds_.end())
   {
      threshold = elevationThresholds_.at(type);
   }

   return threshold;
}

float World::GetThreshold(HumidityLevel type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (humidityThresholds_.find(type) != humidityThresholds_.end())
   {
      threshold = humidityThresholds_.at(type);
   }

   return threshold;
}

float World::GetThreshold(PermeabilityLevel type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (permeabilityThresholds_.find(type) != permeabilityThresholds_.end())
   {
      threshold = permeabilityThresholds_.at(type);
   }

   return threshold;
}

float World::GetThreshold(PrecipitationLevel type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (precipitationThresholds_.find(type) != precipitationThresholds_.end())
   {
      threshold = precipitationThresholds_.at(type);
   }

   return threshold;
}

float World::GetThreshold(TemperatureLevel type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (temperatureThresholds_.find(type) != temperatureThresholds_.end())
   {
      threshold = temperatureThresholds_.at(type);
   }

   return threshold;
}

float World::GetThreshold(WaterThreshold type) const
{
   float threshold = std::numeric_limits<float>::max();

   if (waterThresholds_.find(type) != waterThresholds_.end())
   {
      threshold = waterThresholds_.at(type);
   }

   return threshold;
}

Biome World::GetBiome(uint32_t x, uint32_t y) const
{
   return biome_[y][x];
}

BiomeGroup World::GetBiomeGroup(uint32_t x, uint32_t y) const
{
   return biomeGroups_.at(biome_[y][x]);
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

   return elevation_[y][x] > GetThreshold(ElevationThreshold::Mountain);
}

TemperatureLevel World::GetTemperatureLevel(uint32_t x, uint32_t y) const
{
   uint32_t width  = temperature_.shape()[1];
   uint32_t height = temperature_.shape()[0];

   if (x >= width || y >= height)
   {
      throw std::invalid_argument("Coordinates out of bounds");
   }

   float temperature = temperature_[y][x];

   for (TemperatureLevel type : TemperatureIterator())
   {
      if (temperature < GetThreshold(type))
      {
         return type;
      }
   }

   return TemperatureLevel::Last;
}

HumidityLevel World::GetHumidityLevel(uint32_t x, uint32_t y) const
{
   uint32_t width  = precipitation_.shape()[1];
   uint32_t height = precipitation_.shape()[0];

   if (x >= width || y >= height)
   {
      throw std::invalid_argument("Coordinates out of bounds");
   }

   float humidity = humidity_[y][x];

   for (HumidityLevel type : HumidityIterator())
   {
      if (humidity < GetThreshold(type))
      {
         return type;
      }
   }

   return HumidityLevel::Last;
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

void World::SetThreshold(ElevationThreshold type, float value)
{
   elevationThresholds_[type] = value;
}

void World::SetThreshold(HumidityLevel type, float value)
{
   humidityThresholds_[type] = value;
}

void World::SetThreshold(PermeabilityLevel type, float value)
{
   permeabilityThresholds_[type] = value;
}

void World::SetThreshold(PrecipitationLevel type, float value)
{
   precipitationThresholds_[type] = value;
}

void World::SetThreshold(TemperatureLevel type, float value)
{
   temperatureThresholds_[type] = value;
}

void World::SetThreshold(WaterThreshold type, float value)
{
   waterThresholds_[type] = value;
}

bool World::ProtobufSerialize(std::string& output) const
{
   bool success = false;

   ::World::World pbWorld;

   // Protobuf takes ownership of this data, and does not need deleted
   // manually
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
   ToProtobufMatrix(elevation_, pbHeightmapData);
   pbWorld.set_allocated_heightmapdata(pbHeightmapData);
   pbWorld.set_heightmapth_sea(GetThreshold(ElevationThreshold::Sea));
   pbWorld.set_heightmapth_plain(GetThreshold(ElevationThreshold::Hill));
   pbWorld.set_heightmapth_hill(GetThreshold(ElevationThreshold::Mountain));

   // Plates
   ToProtobufMatrix(plates_, pbPlates);
   pbWorld.set_allocated_plates(pbPlates);

   // Ocean
   ToProtobufMatrix(ocean_, pbOcean);
   pbWorld.set_allocated_ocean(pbOcean);
   ToProtobufMatrix(seaDepth_, pbSeaDepth);
   pbWorld.set_allocated_sea_depth(pbSeaDepth);

   if (HasBiome())
   {
      ::World::World_IntegerMatrix* pbBiome =
         new ::World::World_IntegerMatrix();
      ToProtobufMatrix<Biome, ::World::World_IntegerMatrix, int32_t>(
         biome_, pbBiome, [](const Biome& value) {
            return biomeIndices_.left.at(value);
         });
      pbWorld.set_allocated_biome(pbBiome);
   }

   if (HasHumidity())
   {
      ::World::World_DoubleMatrixWithQuantiles* pbHumidity =
         new ::World::World_DoubleMatrixWithQuantiles();
      ToProtobufMatrix(humidity_, pbHumidity);

      for (HumidityLevel h : HumidityIterator())
      {
         if (h != HumidityLevel::Last)
         {
            ::World::World_DoubleQuantile* entry = pbHumidity->add_quantiles();
            entry->set_key(humidityQuantiles_.left.at(h));
            entry->set_value(GetThreshold(h));
         }
      }

      pbWorld.set_allocated_humidity(pbHumidity);
   }

   if (HasIrrigiation())
   {
      ::World::World_DoubleMatrix* pbIrrigation =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(irrigation_, pbIrrigation);
      pbWorld.set_allocated_irrigation(pbIrrigation);
   }

   if (HasPermeability())
   {
      ::World::World_DoubleMatrix* pbPermeability =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(permeability_, pbPermeability);
      pbWorld.set_allocated_permeabilitydata(pbPermeability);
      pbWorld.set_permeability_low(GetThreshold(PermeabilityLevel::Low));
      pbWorld.set_permeability_med(GetThreshold(PermeabilityLevel::Medium));
   }

   if (HasWatermap())
   {
      ::World::World_DoubleMatrix* pbWatermap =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(waterMap_, pbWatermap);
      pbWorld.set_allocated_watermapdata(pbWatermap);
      pbWorld.set_watermap_creek(GetThreshold(WaterThreshold::Creek));
      pbWorld.set_watermap_river(GetThreshold(WaterThreshold::River));
      pbWorld.set_watermap_mainriver(GetThreshold(WaterThreshold::MainRiver));
   }

   if (HasLakemap())
   {
      ::World::World_DoubleMatrix* pbLakemap =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(lakeMap_, pbLakemap);
      pbWorld.set_allocated_lakemap(pbLakemap);
   }

   if (HasRivermap())
   {
      ::World::World_DoubleMatrix* pbRivermap =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(riverMap_, pbRivermap);
      pbWorld.set_allocated_rivermap(pbRivermap);
   }

   if (HasPrecipitations())
   {
      ::World::World_DoubleMatrix* pbPrecipitation =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(precipitation_, pbPrecipitation);
      pbWorld.set_allocated_precipitationdata(pbPrecipitation);
      pbWorld.set_precipitation_low(GetThreshold(PrecipitationLevel::Low));
      pbWorld.set_precipitation_med(GetThreshold(PrecipitationLevel::Medium));
   }

   if (HasTemperature())
   {
      ::World::World_DoubleMatrix* pbTemperature =
         new ::World::World_DoubleMatrix();
      ToProtobufMatrix(precipitation_, pbTemperature);
      pbWorld.set_allocated_temperaturedata(pbTemperature);
      pbWorld.set_temperature_polar(GetThreshold(TemperatureLevel::Polar));
      pbWorld.set_temperature_alpine(GetThreshold(TemperatureLevel::Alpine));
      pbWorld.set_temperature_boreal(GetThreshold(TemperatureLevel::Boreal));
      pbWorld.set_temperature_cool(GetThreshold(TemperatureLevel::Cool));
      pbWorld.set_temperature_warm(GetThreshold(TemperatureLevel::Warm));
      pbWorld.set_temperature_subtropical(
         GetThreshold(TemperatureLevel::Subtropical));
   }

   if (HasIcecap())
   {
      ::World::World_DoubleMatrix* pbIcecap = new ::World::World_DoubleMatrix();
      ToProtobufMatrix(icecap_, pbIcecap);
      pbWorld.set_allocated_icecap(pbIcecap);
   }

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

bool World::ProtobufDeserialize(std::istream& input)
{
   ::World::World pbWorld;
   bool           success = false;

   try
   {
      success = pbWorld.ParseFromIstream(&input);
   }
   catch (const std::exception& ex)
   {
      BOOST_LOG_TRIVIAL(error) << ex.what();
   }

   if (success)
   {
      try
      {
         name_ = pbWorld.name();
         size_ = {static_cast<uint32_t>(pbWorld.width()),
                  static_cast<uint32_t>(pbWorld.height())};
         seed_ = pbWorld.generationdata().seed();

         generationParams_.numPlates_  = pbWorld.generationdata().n_plates();
         generationParams_.oceanLevel_ = pbWorld.generationdata().ocean_level();
         generationParams_.step_ =
            Step::step(StepTypeFromString(pbWorld.generationdata().step()));

         // Elevation
         FromProtobufMatrix(pbWorld.heightmapdata(), elevation_);
         SetThreshold(ElevationThreshold::Sea, pbWorld.heightmapth_sea());
         SetThreshold(ElevationThreshold::Hill, pbWorld.heightmapth_plain());
         SetThreshold(ElevationThreshold::Mountain, pbWorld.heightmapth_hill());

         // Plates
         FromProtobufMatrix(pbWorld.plates(), plates_);

         // Ocean
         FromProtobufMatrix(pbWorld.ocean(), ocean_);
         FromProtobufMatrix(pbWorld.sea_depth(), seaDepth_);

         // Biome
         FromProtobufMatrix<::World::World_IntegerMatrix, Biome, int32_t>(
            pbWorld.biome(), biome_, [](const int32_t& value) {
               return biomeIndices_.right.at(value);
            });

         // Humidity
         if (pbWorld.has_humidity())
         {
            FromProtobufMatrix(pbWorld.humidity(), humidity_);

            for (const auto& quantile : pbWorld.humidity().quantiles())
            {
               SetThreshold(humidityQuantiles_.right.at(quantile.key()),
                            quantile.value());
            }

            SetThreshold(HumidityLevel::Superhumid,
                         std::numeric_limits<float>::max());
         }

         FromProtobufMatrix(pbWorld.irrigation(), irrigation_);

         FromProtobufMatrix(pbWorld.permeabilitydata(), permeability_);
         if (pbWorld.has_permeability_low())
         {
            SetThreshold(PermeabilityLevel::Low, pbWorld.permeability_low());
            SetThreshold(PermeabilityLevel::Medium, pbWorld.permeability_med());
            SetThreshold(PermeabilityLevel::High,
                         std::numeric_limits<float>::max());
         }

         FromProtobufMatrix(pbWorld.watermapdata(), waterMap_);
         if (pbWorld.has_watermap_creek())
         {
            SetThreshold(WaterThreshold::Creek, pbWorld.watermap_creek());
            SetThreshold(WaterThreshold::River, pbWorld.watermap_river());
            SetThreshold(WaterThreshold::MainRiver,
                         pbWorld.watermap_mainriver());
         }

         FromProtobufMatrix(pbWorld.precipitationdata(), precipitation_);
         if (pbWorld.has_precipitation_low())
         {
            SetThreshold(PrecipitationLevel::Low, pbWorld.precipitation_low());
            SetThreshold(PrecipitationLevel::Medium,
                         pbWorld.precipitation_med());
            SetThreshold(PrecipitationLevel::High, 0.0f);
         }

         FromProtobufMatrix(pbWorld.temperaturedata(), temperature_);
         if (pbWorld.has_temperature_polar())
         {
            SetThreshold(TemperatureLevel::Polar, pbWorld.temperature_polar());
            SetThreshold(TemperatureLevel::Alpine,
                         pbWorld.temperature_alpine());
            SetThreshold(TemperatureLevel::Boreal,
                         pbWorld.temperature_boreal());
            SetThreshold(TemperatureLevel::Cool, pbWorld.temperature_cool());
            SetThreshold(TemperatureLevel::Warm, pbWorld.temperature_warm());
            SetThreshold(TemperatureLevel::Subtropical,
                         pbWorld.temperature_subtropical());
            SetThreshold(TemperatureLevel::Tropical,
                         std::numeric_limits<float>::max());
         }

         FromProtobufMatrix(pbWorld.lakemap(), lakeMap_);

         FromProtobufMatrix(pbWorld.rivermap(), riverMap_);

         FromProtobufMatrix(pbWorld.icecap(), icecap_);
      }
      catch (const std::exception& ex)
      {
         success = false;
         BOOST_LOG_TRIVIAL(error) << ex.what();
      }
   }

   return success;
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

template<class T, class U, class V>
static void ToProtobufMatrix(const boost::multi_array<T, 2>&   source,
                             U*                                pbMatrix,
                             const std::function<V(const T&)>& transform)
{
   const uint32_t width  = source.shape()[1];
   const uint32_t height = source.shape()[0];

   for (uint32_t y = 0; y < height; y++)
   {
      auto* row = pbMatrix->add_rows();
      for (uint32_t x = 0; x < width; x++)
      {
         row->add_cells(transform(source[y][x]));
      }
   }
}

template<class T, class U, class V>
static void FromProtobufMatrix(const T&                          pbMatrix,
                               boost::multi_array<U, 2>&         dest,
                               const std::function<U(const V&)>& transform)
{
   // Assumes each row has the same size
   const uint32_t height = pbMatrix.rows_size();
   const uint32_t width  = (height > 0) ? pbMatrix.rows(0).cells_size() : 0u;

   dest.resize(boost::extents[height][width]);

   for (uint32_t y = 0; y < height; y++)
   {
      auto row = pbMatrix.rows(y);
      for (uint32_t x = 0; x < width && x < row.cells_size(); x++)
      {
         dest[y][x] = transform(row.cells(x));
      }
   }
}

static int32_t WorldengineTag()
{
   return ('W' << 24) | ('o' << 16) | ('e' << 8) | 'n';
}

static int32_t VersionHashcode()
{
   int hashcode = 0;

   boost::char_separator<char>                   sep(".");
   boost::tokenizer<boost::char_separator<char>> t(WORLDENGINE_VERSION, sep);
   for (const std::string& str : t)
   {
      // 3 version components
      hashcode = (hashcode << 8) | std::stoi(str);
   }

   // 4th component (0)
   hashcode <<= 8;

   return hashcode;
}

} // namespace WorldEngine
