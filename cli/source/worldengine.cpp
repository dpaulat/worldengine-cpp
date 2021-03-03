#include "worldengine.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include <common.h>
#include <world.h>

namespace po = boost::program_options;

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

enum class WorldFormat
{
   protobuf
};

void GenerateWorld(const std::string& worldName,
                   const std::string& outputDir,
                   WorldFormat        worldFormat);
void PrintWorldInfo(const World& world);

void GenerateWorld(const std::string& worldName,
                   const std::string& outputDir,
                   WorldFormat        worldFormat)
{
   BOOST_LOG_TRIVIAL(info) << "Producing output:";

   // Save data
   std::string filename = outputDir + "/" + worldName + ".world";
   if (worldFormat == WorldFormat::protobuf) {}
}

void PrintWorldInfo(const World& world)
{
   std::cout << "Name               : " << world.name() << std::endl;
   std::cout << "Width              : " << world.width() << std::endl;
   std::cout << "Height             : " << world.height() << std::endl;
   std::cout << "Seed               : " << world.seed() << std::endl;
   std::cout << "Num Plates         : " << world.numPlates() << std::endl;
   std::cout << "Ocean Level        : " << world.oceanLevel() << std::endl;
   std::cout << "Step               : " << world.step().name_ << std::endl;

   std::cout << "Has Biome          : " << world.HasBiome() << std::endl;
   std::cout << "Has Humidity       : " << world.HasHumidity() << std::endl;
   std::cout << "Has Irrigation     : " << world.HasIrrigiation() << std::endl;
   std::cout << "Has Permeability   : " << world.HasPermeability() << std::endl;
   std::cout << "Has Watermap       : " << world.HasWatermap() << std::endl;
   std::cout << "Has Precipitations : " << world.HasPrecipitations()
             << std::endl;
   std::cout << "Has Temperature    : " << world.HasTemperature() << std::endl;
}

void CliMain(int argc, const char** argv)
{
   // TODO: Parameter validation

   // Generic options
   bool version;
   bool help;
   bool verbose;

   // Configuration
   std::string outputDir;
   std::string worldName;
   std::string fileFormat;
   uint32_t    seed;
   std::string step;
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
   std::string seaColor;
   bool        notDrawBiome;
   bool        notDrawMountains;
   bool        notDrawRivers;
   bool        drawOuterBorder;

   // Export options
   std::string           exportFormat;
   std::string           exportDatatype;
   std::vector<uint32_t> exportDimensions;
   std::vector<uint32_t> exportNormalize;
   std::vector<uint32_t> exportSubset;

   po::positional_options_description p;
   p.add("operator", 1);
   p.add("file", 1);

   po::options_description genericOptions("Generic options");
   genericOptions.add_options() //
      ("version",
       po::bool_switch(&version)->default_value(false),
       "Print version string")
      //
      ("help,h",
       po::bool_switch(&help)->default_value(false),
       "Produce help message")
      //
      ("verbose,v",
       po::bool_switch(&verbose)->default_value(false),
       "Enable verbose messages");

   po::options_description configuration("Configuration");
   configuration.add_options() //
      ("output-dir,o",
       po::value<std::string>(&outputDir)
          ->value_name("dir")
          ->default_value("."),
       "Set output directory")
      //
      ("worldname,n", //
       po::value<std::string>(&worldName),
       "Set world name")
      //
      ("format,f",
       po::value<std::string>(&fileFormat)->default_value("protobuf"),
       "Set file format\n"
       "Valid formats: hdf5, protobuf")
      //
      ("seed,s",
       po::value<uint32_t>(&seed),
       "Initializes the pseudo-random generation")
      //
      ("step,t",
       po::value<std::string>(&step)->default_value("full"),
       "Specifies how far to proceed in the world generation process\n"
       "Valid steps: plates, precipitations, full")
      //
      ("width,x",
       po::value<uint32_t>(&width)->default_value(512),
       "Width of the world to be generated")
      //
      ("height,y",
       po::value<uint32_t>(&height)->default_value(512),
       "Height of the world to be generated")
      //
      ("plates,q",
       po::value<uint32_t>(&numPlates)->default_value(10),
       "Number of plates")
      //
      ("black-and-white",
       po::bool_switch(&blackAndWhite)->default_value(false),
       "Generate maps in black and white");

   po::options_description generateOptions(
      "Generate options (plate and world modes only)");
   generateOptions.add_options() //
      ("rivers,r",               //
       po::bool_switch(&rivers)->default_value(false),
       "Generate rivers map")
      //
      ("grayscale-heightmap",
       po::bool_switch(&grayscaleHeightmap)->default_value(false),
       "Produce a grayscale heightmap")
      //
      ("ocean-level",
       po::value<float>(&oceanLevel)->default_value(1.0f),
       "Elevation cutoff for sea level")
      //
      ("temps",
       po::value<std::vector<float>>(&temps)->multitoken()->default_value(
          DEFAULT_TEMPS),
       "Provide alternate ranges for temperatures")
      //
      ("humidity",
       po::value<std::vector<float>>(&humids)->multitoken()->default_value(
          DEFAULT_HUMIDS),
       "Provide alternate ranges for humidities")
      //
      ("gamma-value",
       po::value<float>(&gammaValue)->default_value(DEFAULT_GAMMA_CURVE),
       "Gamma value for temperature/precipitation gamma correction curve")
      //
      ("gamma-offset",
       po::value<float>(&curveOffset)->default_value(DEFAULT_CURVE_OFFSET),
       "Adjustment value for temperature/precipitation gamma correction curve")
      //
      ("not-fade-borders",
       po::bool_switch(&notFadeBorders)->default_value(false),
       "Don't fade borders")
      //
      ("scatter",
       po::bool_switch(&scatterPlot)->default_value(false),
       "Generate scatter plot")
      //
      ("sat",
       po::bool_switch(&satelliteMap)->default_value(false),
       "Generate satellite map")
      //
      ("ice",
       po::bool_switch(&icecapsMap)->default_value(false),
       "Generate ice caps map");

   po::options_description ancientOptions(
      "Ancient map options (ancient map mode only)");
   ancientOptions.add_options() //
      ("worldfile,w",
       po::value<std::string>(&worldFile)->value_name("filename"),
       "File to be loaded")
      //
      ("generated-file,g",
       po::value<std::string>(&generatedFile)->value_name("filename"),
       "File to be generated")
      //
      ("resize-factor,f",
       po::value<uint32_t>(&resizeFactor)->default_value(1),
       "Resize factor\n"
       "NOTE: This can only be used to increase the size of the map")
      //
      ("sea-color",
       po::value<std::string>(&seaColor)->default_value("brown"),
       "Sea color\n"
       "Valid values: blue, brown")
      //
      ("not-draw-biome",
       po::bool_switch(&notDrawBiome)->default_value(false),
       "Don't draw biome")
      //
      ("not-draw-mountains",
       po::bool_switch(&notDrawMountains)->default_value(false),
       "Don't draw mountains")
      //
      ("not-draw-rivers",
       po::bool_switch(&notDrawRivers)->default_value(false),
       "Don't draw rivers")
      //
      ("draw-outer-border",
       po::bool_switch(&drawOuterBorder)->default_value(false),
       "Draw outer land border");

   po::options_description exportOptions("Export options");
   exportOptions.add_options() //
      ("export-format",
       po::value<std::string>(&exportFormat)->default_value("PNG"),
       "Export to a specific format\n"
       "All possible formats: http://www.gdal.org/formats_list.html")
      //
      ("export-datatype",
       po::value<std::string>(&exportDatatype)->default_value("uint16"),
       "Type of stored data\n"
       "Valid values: int16, int32, uint16, uint32, float32")
      //
      ("export-dimensions",
       po::value<std::vector<uint32_t>>(&exportDimensions)->multitoken(),
       "Export to desired dimensions\n"
       "Example: 4096 4096")
      //
      ("export-normalize",
       po::value<std::vector<uint32_t>>(&exportNormalize)->multitoken(),
       "Normalize the data set between min and max\n"
       "Example: 0 255")
      //
      ("export-subset",
       po::value<std::vector<uint32_t>>(&exportSubset)->multitoken());

   po::options_description options("Allowed options");
   options.add(genericOptions);
   options.add(configuration);
   options.add(generateOptions);
   options.add(ancientOptions);
   options.add(exportOptions);

   po::variables_map vm;
   po::store(
      po::command_line_parser(argc, argv).options(options).positional(p).run(),
      vm);
   po::notify(vm);

   if (help)
   {
      std::cout << options << std::endl;
      return;
   }
}

} // namespace WorldEngine
