#pragma once

#include "worldengine/world.h"

namespace WorldEngine
{

bool ExportImage(const World&                 world,
                 const std::string&           exportFiletype,
                 ExportDataType               exportDatatype,
                 const std::vector<uint32_t>& exportDimensions,
                 const std::vector<int32_t>&  exportNormalize,
                 const std::vector<uint32_t>& exportSubset,
                 const std::string&           path = "seed_output");

}
