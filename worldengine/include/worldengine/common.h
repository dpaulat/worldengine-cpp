#pragma once

#include <string>
#include <vector>

/**
 * @brief WorldEngine C++ Namespace
 */
namespace WorldEngine
{

template<typename T, T beginValue, T endValue>
class Iterator
{
   typedef typename std::underlying_type<T>::type value_t;
   int                                            value_;

public:
   Iterator(const T& v) : value_(static_cast<value_t>(v)) {}
   Iterator() : value_(static_cast<value_t>(beginValue)) {}
   Iterator operator++()
   {
      ++value_;
      return *this;
   }
   T        operator*() { return static_cast<T>(value_); }
   Iterator begin() { return *this; } // Default constructor
   Iterator end()
   {
      static const Iterator endIterator = ++Iterator(endValue);
      return endIterator;
   }
   bool operator!=(const Iterator& i) { return value_ != i.value_; }
};

typedef std::pair<int32_t, int32_t> Point;

enum class BiomeGroup
{
   BorealForest,
   CoolTemperateForest,
   WarmTemperateForest,
   TropicalDryForest,
   Tundra,
   Iceland,
   Jungle,
   Savanna,
   HotDesert,
   ColdParklands,
   Steppe,
   CoolDesert,
   Chaparral,
   None
};
typedef Iterator<BiomeGroup, BiomeGroup::BorealForest, BiomeGroup::Chaparral>
   BiomeGroupIterator;

enum class Biome
{
   Ocean,
   Sea,
   PolarDesert,
   Ice,
   SubpolarDryTundra,
   SubpolarMoistTundra,
   SubpolarWetTundra,
   SubpolarRainTundra,
   BorealDesert,
   BorealDryScrub,
   BorealMoistForest,
   BorealWetForest,
   BorealRainForest,
   CoolTemperateDesert,
   CoolTemperateDesertScrub,
   CoolTemperateSteppe,
   CoolTemperateMoistForest,
   CoolTemperateWetForest,
   CoolTemperateRainForest,
   WarmTemperateDesert,
   WarmTemperateDesertScrub,
   WarmTemperateThornScrub,
   WarmTemperateDryForest,
   WarmTemperateMoistForest,
   WarmTemperateWetForest,
   WarmTemperateRainForest,
   SubtropicalDesert,
   SubtropicalDesertScrub,
   SubtropicalThornWoodland,
   SubtropicalDryForest,
   SubtropicalMoistForest,
   SubtropicalWetForest,
   SubtropicalRainForest,
   TropicalDesert,
   TropicalDesertScrub,
   TropicalThornWoodland,
   TropicalVeryDryForest,
   TropicalDryForest,
   TropicalMoistForest,
   TropicalWetForest,
   TropicalRainForest,
   BareRock
};

enum class ElevationThreshold
{
   Sea,
   Hill,
   Mountain
};
typedef Iterator<ElevationThreshold,
                 ElevationThreshold::Sea,
                 ElevationThreshold::Mountain>
   ElevationIterator;

enum class ExportDataType
{
   Int16,
   Int32,
   Uint8,
   Uint16,
   Uint32,
   Float32,
   Float64
};

enum class HumidityLevel
{
   Superarid,
   Perarid,
   Arid,
   Semiarid,
   Subhumid,
   Humid,
   Perhumid,
   Superhumid,
   Last = Superhumid
};
typedef Iterator<HumidityLevel,
                 HumidityLevel::Superarid,
                 HumidityLevel::Superhumid>
   HumidityIterator;

enum class PermeabilityLevel
{
   Low,
   Medium,
   High
};
typedef Iterator<PermeabilityLevel,
                 PermeabilityLevel::Low,
                 PermeabilityLevel::High>
   PermeabilityIterator;

enum class PrecipitationLevel
{
   Low,
   Medium,
   High
};
typedef Iterator<PrecipitationLevel,
                 PrecipitationLevel::Low,
                 PrecipitationLevel::High>
   PrecipitationIterator;

enum class SeaColor
{
   Blue,
   Brown
};

enum class Simulation
{
   Precipitation,
   Erosion,
   Watermap,
   Irrigation,
   Temperature,
   Humidity,
   Permeability,
   Biome,
   Icecap
};

enum class StepType
{
   Plates,
   Precipitations,
   Full
};

enum class TemperatureLevel
{
   Polar,
   Alpine,
   Boreal,
   Cool,
   Warm,
   Subtropical,
   Tropical,
   Count,
   Last = Tropical
};
typedef Iterator<TemperatureLevel,
                 TemperatureLevel::Polar,
                 TemperatureLevel::Tropical>
   TemperatureIterator;

enum class WaterThreshold
{
   Creek,
   River,
   MainRiver
};
typedef Iterator<WaterThreshold,
                 WaterThreshold::Creek,
                 WaterThreshold::MainRiver>
   WaterIterator;

enum class WorldFormat
{
   Protobuf,
   HDF5
};

struct Step
{
   StepType stepType_;
   bool     includePlates_;
   bool     includePrecipitations_;
   bool     includeErosion_;
   bool     includeBiome_;

   Step(StepType stepType,
        bool     includePlates,
        bool     includePrecipitations,
        bool     includeErosion,
        bool     includeBiome) :
       stepType_(stepType),
       includePlates_(includePlates),
       includePrecipitations_(includePrecipitations),
       includeErosion_(includeErosion),
       includeBiome_(includeBiome)
   {
   }

   static const Step& step(StepType type);

   std::string name() const;
};

struct Size
{
   uint32_t width_;
   uint32_t height_;

   Size(uint32_t width, uint32_t height) : width_(width), height_(height) {}
   Size() : Size(0u, 0u) {}
};

struct GenerationParameters
{
   uint32_t numPlates_;
   float    oceanLevel_;
   Step     step_;

   GenerationParameters(uint32_t    numPlates,
                        float       oceanLevel,
                        const Step& step) :
       numPlates_(numPlates), oceanLevel_(oceanLevel), step_(step)
   {
   }

   GenerationParameters(uint32_t numPlates, float oceanLevel, StepType type) :
       GenerationParameters(numPlates, oceanLevel, Step::step(type))
   {
   }

   GenerationParameters() : GenerationParameters(0u, 0.0f, StepType::Full) {}
};

const std::string WORLDENGINE_VERSION("0.19.0");

const Step STEP_PLATES(StepType::Plates, true, false, false, false);
const Step
   STEP_PRECIPITATIONS(StepType::Precipitations, true, true, false, false);
const Step STEP_FULL(StepType::Full, true, true, true, true);

const uint32_t MIN_SEED = 0u;
const uint32_t MAX_SEED = UINT16_MAX;

const float    DEFAULT_SEA_LEVEL         = 0.65f;
const uint32_t DEFAULT_EROSION_PERIOD    = 60u;
const float    DEFAULT_FOLDING_RATIO     = 0.02f;
const uint32_t DEFAULT_AGGR_OVERLAP_ABS  = 1000000u;
const float    DEFAULT_AGGR_OVERLAP_REL  = 0.33f;
const uint32_t DEFAULT_CYCLE_COUNT       = 2u;
const uint32_t DEFAULT_NUM_PLATES        = 10u;
const float    DEFAULT_GAMMA_CURVE       = 1.25f;
const float    DEFAULT_CURVE_OFFSET      = 0.2f;
const float    DEFAULT_OCEAN_LEVEL       = 1.0f;
const Step     DEFAULT_STEP              = STEP_FULL;
const uint32_t DEFAULT_SCATTER_PLOT_SIZE = 512u;
const bool     DEFAULT_FADE_BORDERS      = true;
const bool     DEFAULT_BLACK_AND_WHITE   = false;
const bool     DEFAULT_GS_HEIGHTMAP      = false;
const bool     DEFAULT_RIVERS_MAP        = false;
const bool     DEFAULT_SCATTER_PLOT      = false;
const bool     DEFAULT_SATELLITE_MAP     = false;
const bool     DEFAULT_ICECAPS_MAP       = false;

const std::vector<float> DEFAULT_TEMPS {
   0.126f, 0.235f, 0.406f, 0.561f, 0.634f, 0.876f};

const std::vector<float> DEFAULT_HUMIDS {
   0.059, 0.222, 0.493, 0.764, 0.927, 0.986, 0.998};

/**
 * @brief Convert from a biomes enumeration to a string value
 * @param type Biome enumeration
 * @return String value
 */
std::string BiomeToString(Biome type);

std::ostream& operator<<(std::ostream& os, const Biome& type);

/**
 * @brief Convert from a biome group enumeration to a string value
 * @param type Biome group enumeration
 * @return String value
 */
std::string BiomeGroupToString(BiomeGroup type);

std::ostream& operator<<(std::ostream& os, const BiomeGroup& type);

/**
 * @brief Convert from an export data type enumeration to a string value
 * @param type Export data type enumeration
 * @return String value
 */
std::string ExportDataTypeToString(ExportDataType type);

/**
 * @brief Convert from a string value to an export data type enumeration
 * @param value String value
 * @return Export data type enumeration
 */
ExportDataType ExportDataTypeFromString(const std::string& value);

std::ostream& operator<<(std::ostream& os, const ExportDataType& type);
std::istream& operator>>(std::istream& in, ExportDataType& type);

/**
 * @brief Convert from a sea color enumeration to a string value
 * @param color Sea color enumeration
 * @return String value
 */
std::string SeaColorToString(SeaColor color);

/**
 * @brief Convert from a string value to a sea color enumeration
 * @param value String value
 * @return Sea color enumeration
 */
SeaColor      SeaColorFromString(const std::string& value);
std::ostream& operator<<(std::ostream& os, const SeaColor& color);
std::istream& operator>>(std::istream& in, SeaColor& color);

/**
 * @brief Convert from a step type enumeration to a string value
 * @param step Step type enumeration
 * @return String value
 */
std::string StepTypeToString(StepType step);

/**
 * @brief Convert from a string value to a step type enumeration
 * @param value String value
 * @return Step type enumeration
 */
StepType      StepTypeFromString(const std::string& value);
std::ostream& operator<<(std::ostream& os, const StepType& step);
std::istream& operator>>(std::istream& in, StepType& step);

/**
 * @brief Convert from a world format enumeration to a string value
 * @param format World format enumeration
 * @return String value
 */
std::string WorldFormatToString(WorldFormat format);

/**
 * @brief Convert from a string value to a world format enumeration
 * @param value String value
 * @return World format enumeration
 */
WorldFormat   WorldFormatFromString(const std::string& value);
std::ostream& operator<<(std::ostream& os, const WorldFormat& format);
std::istream& operator>>(std::istream& in, WorldFormat& format);

} // namespace WorldEngine
