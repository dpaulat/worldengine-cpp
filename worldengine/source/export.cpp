#include "worldengine/export.h"

#include <unordered_map>

#pragma warning(push)
#pragma warning(disable : 26812)
#include <boost/log/trivial.hpp>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable : 26812)
#include <gdal_priv.h>
#include <cpl_conv.h>
#include <gdal_utils.h>
#pragma warning(pop)

namespace WorldEngine
{

static const std::unordered_map<ExportDataType, size_t> bppMap = {
   {ExportDataType::Int16, 16},
   {ExportDataType::Int32, 32},
   {ExportDataType::Uint8, 8},
   {ExportDataType::Uint16, 16},
   {ExportDataType::Uint32, 32},
   {ExportDataType::Float32, 32},
   {ExportDataType::Float64, 64}};

static const std::unordered_map<ExportDataType, GDALDataType> gdalTypeMap = {
   {ExportDataType::Int16, GDALDataType::GDT_Int16},
   {ExportDataType::Int32, GDALDataType::GDT_Int32},
   {ExportDataType::Uint8, GDALDataType::GDT_Byte},
   {ExportDataType::Uint16, GDALDataType::GDT_UInt16},
   {ExportDataType::Uint32, GDALDataType::GDT_UInt32},
   {ExportDataType::Float32, GDALDataType::GDT_Float32},
   {ExportDataType::Float64, GDALDataType::GDT_Float64}};

static const char** ArgListCreate(const std::vector<std::string>& argVector);
static void         ArgListDelete(const char** argList);
static void         Translate(GDALDataset**             dataset,
                              std::vector<std::string>& translateArgs);

bool ExportImage(const World&                 world,
                 const std::string&           exportFiletype,
                 ExportDataType               exportDatatype,
                 const std::vector<uint32_t>& exportDimensions,
                 const std::vector<int32_t>&  exportNormalize,
                 const std::vector<uint32_t>& exportSubset,
                 const std::string&           path)
{
   bool success = true;

   // Generate temporary filename
   const std::string intFilename = GenerateTemporaryFilename(path + "-");
   const std::string hdrFilename = intFilename + ".hdr";

   // Register all known drivers
   GDALAllRegister();

   // Get export driver
   GDALDriverManager* manager = GetGDALDriverManager();
   GDALDriver* finalDriver = manager->GetDriverByName(exportFiletype.c_str());
   if (finalDriver == nullptr)
   {
      BOOST_LOG_TRIVIAL(error) << "Driver not registered: " << exportFiletype;
      success = false;
   }

   if (success)
   {
      const char* driverName = finalDriver->GetMetadataItem(GDAL_DMD_LONGNAME);

      // Determine file suffix
      const char* ext = finalDriver->GetMetadataItem(GDAL_DMD_EXTENSION);
      std::string fileExtension = (ext != nullptr) ? ext : exportFiletype;

      // Translate export data type
      size_t       bpp   = bppMap.at(exportDatatype);
      GDALDataType eType = gdalTypeMap.at(exportDatatype);

      // Elevation data isn't actually modified, but GDAL doesn't take a const
      // void* parameter
      const boost::multi_array<float, 2>& constElevation =
         world.GetElevationData();

      // Normalize the data-set to the minimum/maximum allowed by the data type,
      // typical for 8bpp
      boost::multi_array<float, 2> elevation = constElevation;
      if (exportNormalize.size() == 2)
      {
         std::pair<float*, float*> minmax = std::minmax_element(
            elevation.data(), elevation.data() + elevation.num_elements());

         const float min = *minmax.first;
         const float max = *minmax.second;
         const float scale =
            (exportNormalize[1] - exportNormalize[0]) / (max - min);

         std::transform(
            elevation.data(),
            elevation.data() + elevation.num_elements(),
            elevation.data(),
            [&min, &scale, &newMin = std::as_const(exportNormalize[0])](
               const float& e) { return (e - min) * scale + newMin; });
      }

      // Take elevation data and push it into an intermediate ENVI format, some
      // formats don't support being written by Create()
      GDALDriver*  intDriver  = manager->GetDriverByName("ENVI");
      GDALDataset* intDataset = intDriver->Create(
         intFilename.c_str(), world.width(), world.height(), 1, eType, NULL);
      GDALRasterBand* band = intDataset->GetRasterBand(1);
      band->RasterIO(GF_Write,                  // eRWFlag
                     0,                         // nXOff
                     0,                         // nYOff
                     world.width(),             // nXSize
                     world.height(),            // nYSize
                     elevation.data(),          // pData
                     world.width(),             // nBufXSize
                     world.height(),            // nBufYSize
                     GDALDataType::GDT_Float32, // eBufType
                     0,                         // nPixelSpace
                     0,                         // nLineSpace
                     NULL);                     // psExtraArg
      GDALClose(intDataset);

      // Take the intermediate ENVI format and convert to final format
      intDataset =
         static_cast<GDALDataset*>(GDALOpen(intFilename.c_str(), GA_Update));

      std::vector<std::string> translateArgs;

      // Resize and blend if necessary
      if (exportDimensions.size() == 2)
      {
         translateArgs.push_back("-outsize");
         translateArgs.push_back(std::to_string(exportDimensions[0])); // Width
         translateArgs.push_back(std::to_string(exportDimensions[1])); // Height
      }

      // Apply changes to the dataset
      if (translateArgs.size() > 0)
      {
         // Output format = memory
         translateArgs.push_back("-of");
         translateArgs.push_back("mem");

         // Resample algorithm
         translateArgs.push_back("-r");
         translateArgs.push_back("cubicspline");

         Translate(&intDataset, translateArgs);
      }

      // Only use a specific subset of dataset
      if (exportSubset.size() == 4) // export_subset is not None
      {
         // Output format = memory
         translateArgs.push_back("-of");
         translateArgs.push_back("mem");

         // Export subset
         translateArgs.push_back("-srcwin");
         translateArgs.push_back(std::to_string(exportSubset[0])); // xoff
         translateArgs.push_back(std::to_string(exportSubset[1])); // yoff
         translateArgs.push_back(std::to_string(exportSubset[2])); // xsize
         translateArgs.push_back(std::to_string(exportSubset[3])); // ysize

         Translate(&intDataset, translateArgs);
      }

      const std::string exportFilename =
         path + "-" + std::to_string(bpp) + "." + fileExtension;
      finalDriver->CreateCopy(
         exportFilename.c_str(), intDataset, FALSE, NULL, NULL, NULL);

      BOOST_LOG_TRIVIAL(info)
         << "Exported " << driverName << " image to " << exportFilename;

      GDALClose(intDataset);

      // Remove temporary files
      std::remove(intFilename.c_str());
      std::remove(hdrFilename.c_str());
   }

   return success;
}

static const char** ArgListCreate(const std::vector<std::string>& argVector)
{
   const size_t argc    = argVector.size();
   const char** argList = new const char*[argc + 1];

   for (size_t i = 0; i < argc; i++)
   {
      argList[i] = argVector[i].c_str();
   }

   argList[argc] = NULL;

   return argList;
}

static void ArgListDelete(const char** argList)
{
   delete[] argList;
}

static void Translate(GDALDataset**             dataset,
                      std::vector<std::string>& translateArgs)
{
   // Arguments are not actually modified, but GDAL doesn't take a const
   // char** parameter
   char** args = const_cast<char**>(ArgListCreate(translateArgs));

   GDALTranslateOptions* translateOptions = GDALTranslateOptionsNew(args, NULL);
   GDALDataset*          newDataset       = static_cast<GDALDataset*>(
      GDALTranslate("", *dataset, translateOptions, NULL));
   GDALTranslateOptionsFree(translateOptions);

   GDALClose(*dataset);
   *dataset = newDataset;

   ArgListDelete(const_cast<const char**>(args));
   translateArgs.clear();
}

} // namespace WorldEngine
