#include "worldengine/common.h"

#include <iomanip>
#include <istream>
#include <sstream>

#include <boost/algorithm/string.hpp>

namespace WorldEngine
{

const Step& Step::step(StepType type)
{
   switch (type)
   {
   case StepType::Plates: return STEP_PLATES;
   case StepType::Precipitations: return STEP_PRECIPITATIONS;
   case StepType::Full:
   default: return STEP_FULL;
   }
}

std::string Step::name() const
{
   return StepTypeToString(stepType_);
}

std::string GenerateTemporaryFilename(const std::string& prefix,
                                      const std::string& suffix)
{
   time_t t = std::time(nullptr);
   tm     time;

#if defined(_MSC_VER)
   localtime_s(&time, &t);
#else
   localtime_r(&t, &time);
#endif

   std::ostringstream oss;
   oss << std::put_time(&time, "%Y%m%dT%H%M%S");
   return std::string(prefix + oss.str() + suffix);
}

std::string BiomeToString(Biome type)
{
   switch (type)
   {
   case Biome::Ocean: return "Ocean";
   case Biome::Sea: return "Sea";
   case Biome::PolarDesert: return "Polar Desert";
   case Biome::Ice: return "Ice";
   case Biome::SubpolarDryTundra: return "Subpolar Dry Tundra";
   case Biome::SubpolarMoistTundra: return "Subpolar Moist Tundra";
   case Biome::SubpolarWetTundra: return "Subpolar Wet Tundra";
   case Biome::SubpolarRainTundra: return "Subpolar Rain Tundra";
   case Biome::BorealDesert: return "Boreal Desert";
   case Biome::BorealDryScrub: return "Boreal Dry Scrub";
   case Biome::BorealMoistForest: return "Boreal Moist Forest";
   case Biome::BorealWetForest: return "Boreal Wet Forest";
   case Biome::BorealRainForest: return "Boreal Rain Forest";
   case Biome::CoolTemperateDesert: return "Cool Temperate Desert";
   case Biome::CoolTemperateDesertScrub: return "Cool Temperate Desert Scrub";
   case Biome::CoolTemperateSteppe: return "Cool Temperate Steppe";
   case Biome::CoolTemperateMoistForest: return "Cool Temperate Moist Forest";
   case Biome::CoolTemperateWetForest: return "Cool Temperate Wet Forest";
   case Biome::CoolTemperateRainForest: return "Cool Temperate Rain Forest";
   case Biome::WarmTemperateDesert: return "Warm Temperate Desert";
   case Biome::WarmTemperateDesertScrub: return "Warm Temperate Desert Scrub";
   case Biome::WarmTemperateThornScrub: return "Warm Temperate Thorn Scrub";
   case Biome::WarmTemperateDryForest: return "Warm Temperate Dry Forest";
   case Biome::WarmTemperateMoistForest: return "Warm Temperate Moist Forest";
   case Biome::WarmTemperateWetForest: return "Warm Temperate Wet Forest";
   case Biome::WarmTemperateRainForest: return "Warm Temperate Rain Forest";
   case Biome::SubtropicalDesert: return "Subtropical Desert";
   case Biome::SubtropicalDesertScrub: return "Subtropical Desert Scrub";
   case Biome::SubtropicalThornWoodland: return "Subtropical Thorn Woodland";
   case Biome::SubtropicalDryForest: return "Subtropical Dry Forest";
   case Biome::SubtropicalMoistForest: return "Subtropical Moist Forest";
   case Biome::SubtropicalWetForest: return "Subtropical Wet Forest";
   case Biome::SubtropicalRainForest: return "Subtropical Rain Forest";
   case Biome::TropicalDesert: return "Tropical Desert";
   case Biome::TropicalDesertScrub: return "Tropical Desert Scrub";
   case Biome::TropicalThornWoodland: return "Tropical Thorn Woodland";
   case Biome::TropicalVeryDryForest: return "Tropical Very Dry Forest";
   case Biome::TropicalDryForest: return "Tropical Dry Forest";
   case Biome::TropicalMoistForest: return "Tropical Moist Forest";
   case Biome::TropicalWetForest: return "Tropical Wet Forest";
   case Biome::TropicalRainForest: return "Tropical Rain Forest";
   case Biome::BareRock: return "Bare Rock";
   default: return "?";
   }
}

std::ostream& operator<<(std::ostream& os, const Biome& type)
{
   os << BiomeToString(type);
   return os;
}

std::string BiomeGroupToString(BiomeGroup type)
{
   switch (type)
   {
   case BiomeGroup::BorealForest: return "Boreal Forest";
   case BiomeGroup::CoolTemperateForest: return "Cool Temperate Forest";
   case BiomeGroup::WarmTemperateForest: return "Warm Temperate Forest";
   case BiomeGroup::TropicalDryForest: return "Tropical Dry Forest";
   case BiomeGroup::Tundra: return "Tundra";
   case BiomeGroup::Iceland: return "Iceland";
   case BiomeGroup::Jungle: return "Jungle";
   case BiomeGroup::Savanna: return "Savanna";
   case BiomeGroup::HotDesert: return "Hot Desert";
   case BiomeGroup::ColdParklands: return "Cold Parklands";
   case BiomeGroup::Steppe: return "Steppe";
   case BiomeGroup::CoolDesert: return "Cool Desert";
   case BiomeGroup::Chaparral: return "Chaparral";
   case BiomeGroup::None: return "None";
   default: return "?";
   }
}

std::ostream& operator<<(std::ostream& os, const BiomeGroup& type)
{
   os << BiomeGroupToString(type);
   return os;
}

std::string ExportDataTypeToString(ExportDataType type)
{
   switch (type)
   {
   case ExportDataType::Int16: return "int16";
   case ExportDataType::Int32: return "int32";
   case ExportDataType::Uint16: return "uint16";
   case ExportDataType::Uint32: return "uint32";
   case ExportDataType::Float32: return "float32";
   default: return "?";
   }
}

ExportDataType ExportDataTypeFromString(const std::string& value)
{
   if (boost::iequals(value, "int16"))
   {
      return ExportDataType::Int16;
   }
   else if (boost::iequals(value, "int32"))
   {
      return ExportDataType::Int32;
   }
   else if (boost::iequals(value, "uint16"))
   {
      return ExportDataType::Uint16;
   }
   else if (boost::iequals(value, "uint32"))
   {
      return ExportDataType::Uint32;
   }
   else if (boost::iequals(value, "float32"))
   {
      return ExportDataType::Float32;
   }

   throw std::invalid_argument("Cannot convert " + value +
                               " to ExportDataType");
}

std::ostream& operator<<(std::ostream& os, const ExportDataType& type)
{
   os << ExportDataTypeToString(type);
   return os;
}

std::istream& operator>>(std::istream& in, ExportDataType& type)
{
   std::string token;
   in >> token;

   try
   {
      type = ExportDataTypeFromString(token);
   }
   catch (std::invalid_argument&)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

std::string PointToString(Point p)
{
   std::string value = '(' + std::to_string(p.first) + ',' + ' ' +
                       std::to_string(p.second) + ')';

   return value;
}

std::string SeaColorToString(SeaColor color)
{
   switch (color)
   {
   case SeaColor::Blue: return "blue";
   case SeaColor::Brown: return "brown";
   default: return "?";
   }
}

SeaColor SeaColorFromString(const std::string& value)
{
   if (boost::iequals(value, "blue"))
   {
      return SeaColor::Blue;
   }
   else if (boost::iequals(value, "brown"))
   {
      return SeaColor::Brown;
   }

   throw std::invalid_argument("Cannot convert " + value + " to SeaColor");
}

std::ostream& operator<<(std::ostream& os, const SeaColor& color)
{
   os << SeaColorToString(color);
   return os;
}

std::istream& operator>>(std::istream& in, SeaColor& color)
{
   std::string token;
   in >> token;

   try
   {
      color = SeaColorFromString(token);
   }
   catch (std::invalid_argument&)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

std::string StepTypeToString(StepType step)
{
   switch (step)
   {
   case StepType::Plates: return "plates";
   case StepType::Precipitations: return "precipitations";
   case StepType::Full: return "full";
   default: return "?";
   }
}

StepType StepTypeFromString(const std::string& value)
{
   if (boost::iequals(value, "plates"))
   {
      return StepType::Plates;
   }
   else if (boost::iequals(value, "precipitations"))
   {
      return StepType::Precipitations;
   }
   else if (boost::iequals(value, "full"))
   {
      return StepType::Full;
   }

   throw std::invalid_argument("Cannot convert " + value + " to StepType");
}

std::ostream& operator<<(std::ostream& os, const StepType& step)
{
   os << StepTypeToString(step);
   return os;
}

std::istream& operator>>(std::istream& in, StepType& step)
{
   std::string token;
   in >> token;

   try
   {
      step = StepTypeFromString(token);
   }
   catch (std::invalid_argument&)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

std::string WorldFormatToString(WorldFormat format)
{
   switch (format)
   {
   case WorldFormat::Protobuf: return "protobuf";
   case WorldFormat::HDF5: return "hdf5";
   default: return "?";
   }
}

WorldFormat WorldFormatFromString(const std::string& value)
{
   if (boost::iequals(value, "protobuf"))
   {
      return WorldFormat::Protobuf;
   }
   else if (boost::iequals(value, "hdf5"))
   {
      return WorldFormat::HDF5;
   }

   throw std::invalid_argument("Cannot convert " + value + " to WorldFormat");
}

std::ostream& operator<<(std::ostream& os, const WorldFormat& format)
{
   os << WorldFormatToString(format);
   return os;
}

std::istream& operator>>(std::istream& in, WorldFormat& format)
{
   std::string token;
   in >> token;

   try
   {
      format = WorldFormatFromString(token);
   }
   catch (std::invalid_argument&)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

} // namespace WorldEngine

std::ostream& operator<<(std::ostream& os, const WorldEngine::Point& p)
{
   os << WorldEngine::PointToString(p);
   return os;
}
