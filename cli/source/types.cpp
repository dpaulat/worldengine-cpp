#include "types.h"

#include <istream>

#include <boost/algorithm/string.hpp>

namespace std
{

std::ostream& operator<<(std::ostream& os, const std::vector<float>& v)
{
   for (float item : v)
   {
      os << item << " ";
   }
   return os;
}

} // namespace std

namespace WorldEngine
{

bool IsGenerationOption(OperationType operation)
{
   return (operation == OperationType::World ||
           operation == OperationType::Plates);
}

std::string OperationTypeToString(OperationType operation)
{
   switch (operation)
   {
   case OperationType::World: return "world";
   case OperationType::Plates: return "plates";
   case OperationType::AncientMap: return "ancient_map";
   case OperationType::Info: return "info";
   case OperationType::Export: return "export";
   default: return "?";
   }
}

OperationType OperationTypeFromString(const std::string& value)
{
   if (boost::iequals(value, "world"))
   {
      return OperationType::World;
   }
   else if (boost::iequals(value, "plates"))
   {
      return OperationType::Plates;
   }
   else if (boost::iequals(value, "ancient_map"))
   {
      return OperationType::AncientMap;
   }
   else if (boost::iequals(value, "info"))
   {
      return OperationType::Info;
   }
   else if (boost::iequals(value, "export"))
   {
      return OperationType::Export;
   }

   throw std::invalid_argument("Cannot convert " + value + " to OperationType");
}

std::ostream& operator<<(std::ostream& os, const OperationType& operation)
{
   os << OperationTypeToString(operation);
   return os;
}

std::istream& operator>>(std::istream& in, OperationType& operation)
{
   std::string token;
   in >> token;

   try
   {
      operation = OperationTypeFromString(token);
   }
   catch (std::invalid_argument&)
   {
      in.setstate(std::ios_base::failbit);
   }

   return in;
}

} // namespace WorldEngine
