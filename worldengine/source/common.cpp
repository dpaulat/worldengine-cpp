#include "common.h"

#include <istream>

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
   catch (std::invalid_argument& e)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
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
   catch (std::invalid_argument& e)
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
   catch (std::invalid_argument& e)
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
   catch (std::invalid_argument& e)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

} // namespace WorldEngine
