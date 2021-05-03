#pragma once

#include <worldengine/common.h>

#include <string>
#include <vector>

namespace std
{
std::ostream& operator<<(std::ostream& os, const std::vector<float>& v);
} // namespace std

namespace WorldEngine
{
enum class OperationType
{
   World,
   Plates,
   AncientMap,
   Info,
   Export
};

struct ArgumentsType
{
   // Positional options
   OperationType operation;
   std::string   file;

   // Generic options
   bool version;
   bool help;
   bool verbose;

   // Configuration
   std::string outputDir;
   std::string worldName;
   WorldFormat worldFormat;
   uint32_t    seed;
   StepType    step;
   uint32_t    width;
   uint32_t    height;
   uint32_t    numPlates;
   bool        blackAndWhite;

   // Generate options
   bool               rivers;
   bool               grayscaleHeightmap;
   float              oceanLevel;
   std::vector<float> temps;
   std::vector<float> humids;
   float              gammaValue;
   float              curveOffset;
   bool               notFadeBorders;
   bool               scatterPlot;
   bool               satelliteMap;
   bool               icecapsMap;

   // Ancient map options
   std::string worldFile;
   std::string generatedFile;
   uint32_t    resizeFactor;
   SeaColor    seaColor;
   bool        notDrawBiome;
   bool        notDrawMountains;
   bool        notDrawRivers;
   bool        drawOuterBorder;

   // Export options
   std::string           exportFormat;
   ExportDataType        exportDatatype;
   std::vector<uint32_t> exportDimensions;
   std::vector<int32_t>  exportNormalize;
   std::vector<uint32_t> exportSubset;
};

bool IsGenerationOption(OperationType args);

std::string   OperationTypeToString(OperationType operation);
OperationType OperationTypeFromString(const std::string& value);
std::ostream& operator<<(std::ostream& os, const OperationType& operation);
std::istream& operator>>(std::istream& in, OperationType& operation);
} // namespace WorldEngine
