#include "worldengine.h"
#include "types.h"

#include <iostream>
#include <random>

#include <boost/assign.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>

#include <common.h>
#include <plates.h>
#include <world.h>

namespace po = boost::program_options;

namespace WorldEngine
{

int AddOptions(int                      argc,
               const char**             argv,
               ArgumentsType&           args,
               po::options_description& options,
               po::variables_map&       vm);
std::shared_ptr<World>
     GenerateWorld(const std::string&        worldName,
                   uint32_t                  width,
                   uint32_t                  height,
                   uint32_t                  seed,
                   uint32_t                  numPlates,
                   const std::string&        outputDir,
                   const Step&               step,
                   float                     oceanLevel,
                   const std::vector<float>& temps,
                   const std::vector<float>& humids,
                   WorldFormat               worldFormat,
                   float                     gammaCurve  = DEFAULT_GAMMA_CURVE,
                   float                     curveOffset = DEFAULT_CURVE_OFFSET,
                   bool                      fadeBorders = DEFAULT_FADE_BORDERS,
                   bool blackAndWhite = DEFAULT_BLACK_AND_WHITE);
void PrintUsage(const std::string&             programName,
                const po::options_description& options);
void PrintWorldInfo(const World& world);
int  ValidateArguments(ArgumentsType& args, const po::variables_map& vm);

int AddOptions(int                      argc,
               const char**             argv,
               ArgumentsType&           args,
               po::options_description& options,
               po::variables_map&       vm)
{
   po::positional_options_description p;
   p.add("operation", 1);
   p.add("file", 1);

   po::options_description hidden("Hidden options");
   hidden.add_options() //
      ("operation",
       po::value<OperationType>(&args.operation)
          ->default_value(OperationType::World))
      //
      ("file", po::value<std::string>(&args.file));

   po::options_description genericOptions("Generic options");
   genericOptions.add_options() //
      ("version",
       po::bool_switch(&args.version)->default_value(false),
       "Print version string")
      //
      ("help,h",
       po::bool_switch(&args.help)->default_value(false),
       "Produce help message")
      //
      ("verbose,v",
       po::bool_switch(&args.verbose)->default_value(false),
       "Enable verbose messages");

   po::options_description configuration("Configuration");
   configuration.add_options() //
      ("output-dir,o",
       po::value<std::string>(&args.outputDir)
          ->value_name("dir")
          ->default_value("."),
       "Set output directory")
      //
      ("worldname,n", //
       po::value<std::string>(&args.worldName),
       "Set world name")
      //
      ("format,f",
       po::value<WorldFormat>(&args.worldFormat)
          ->default_value(WorldFormat::Protobuf),
       "Set file format\n"
       "Valid formats: hdf5, protobuf")
      //
      ("seed,s",
       po::value<uint32_t>(&args.seed),
       "Initializes the pseudo-random generation")
      //
      ("step,t",
       po::value<StepType>(&args.step)->default_value(StepType::Full),
       "Specifies how far to proceed in the world generation process\n"
       "Valid steps: plates, precipitations, full")
      //
      ("width,x",
       po::value<uint32_t>(&args.width)->default_value(512),
       "Width of the world to be generated")
      //
      ("height,y",
       po::value<uint32_t>(&args.height)->default_value(512),
       "Height of the world to be generated")
      //
      ("plates,q",
       po::value<uint32_t>(&args.numPlates)->default_value(DEFAULT_NUM_PLATES),
       "Number of plates")
      //
      ("black-and-white",
       po::bool_switch(&args.blackAndWhite)->default_value(false),
       "Generate maps in black and white");

   po::options_description generateOptions(
      "Generate options (plate and world modes only)");
   generateOptions.add_options() //
      ("rivers,r",               //
       po::bool_switch(&args.rivers)->default_value(false),
       "Generate rivers map")
      //
      ("grayscale-heightmap",
       po::bool_switch(&args.grayscaleHeightmap)->default_value(false),
       "Produce a grayscale heightmap")
      //
      ("ocean-level",
       po::value<float>(&args.oceanLevel)->default_value(DEFAULT_OCEAN_LEVEL),
       "Elevation cutoff for sea level")
      //
      ("temps",
       po::value<std::vector<float>>(&args.temps)
          ->multitoken()
          ->default_value(DEFAULT_TEMPS),
       "Provide alternate ranges for temperatures")
      //
      ("humidity",
       po::value<std::vector<float>>(&args.humids)
          ->multitoken()
          ->default_value(DEFAULT_HUMIDS),
       "Provide alternate ranges for humidities")
      //
      ("gamma-value",
       po::value<float>(&args.gammaValue)->default_value(DEFAULT_GAMMA_CURVE),
       "Gamma value for temperature/precipitation gamma correction curve")
      //
      ("gamma-offset",
       po::value<float>(&args.curveOffset)->default_value(DEFAULT_CURVE_OFFSET),
       "Adjustment value for temperature/precipitation gamma correction curve")
      //
      ("not-fade-borders",
       po::bool_switch(&args.notFadeBorders)->default_value(false),
       "Don't fade borders")
      //
      ("scatter",
       po::bool_switch(&args.scatterPlot)->default_value(false),
       "Generate scatter plot")
      //
      ("sat",
       po::bool_switch(&args.satelliteMap)->default_value(false),
       "Generate satellite map")
      //
      ("ice",
       po::bool_switch(&args.icecapsMap)->default_value(false),
       "Generate ice caps map");

   po::options_description ancientOptions(
      "Ancient map options (ancient map mode only)");
   ancientOptions.add_options() //
      ("worldfile,w",
       po::value<std::string>(&args.worldFile)->value_name("filename"),
       "File to be loaded")
      //
      ("generated-file,g",
       po::value<std::string>(&args.generatedFile)->value_name("filename"),
       "File to be generated")
      //
      ("resize-factor,f",
       po::value<uint32_t>(&args.resizeFactor)->default_value(1),
       "Resize factor\n"
       "NOTE: This can only be used to increase the size of the map")
      //
      ("sea-color",
       po::value<SeaColor>(&args.seaColor)->default_value(SeaColor::Brown),
       "Sea color\n"
       "Valid values: blue, brown")
      //
      ("not-draw-biome",
       po::bool_switch(&args.notDrawBiome)->default_value(false),
       "Don't draw biome")
      //
      ("not-draw-mountains",
       po::bool_switch(&args.notDrawMountains)->default_value(false),
       "Don't draw mountains")
      //
      ("not-draw-rivers",
       po::bool_switch(&args.notDrawRivers)->default_value(false),
       "Don't draw rivers")
      //
      ("draw-outer-border",
       po::bool_switch(&args.drawOuterBorder)->default_value(false),
       "Draw outer land border");

   po::options_description exportOptions("Export options");
   exportOptions.add_options() //
      ("export-format",
       po::value<std::string>(&args.exportFormat)->default_value("PNG"),
       "Export to a specific format\n"
       "All possible formats: http://www.gdal.org/formats_list.html")
      //
      ("export-datatype",
       po::value<ExportDataType>(&args.exportDatatype)
          ->default_value(ExportDataType::Uint16),
       "Type of stored data\n"
       "Valid values: int16, int32, uint16, uint32, float32")
      //
      ("export-dimensions",
       po::value<std::vector<uint32_t>>(&args.exportDimensions)->multitoken(),
       "Export to desired dimensions\n"
       "Example: 4096 4096")
      //
      ("export-normalize",
       po::value<std::vector<uint32_t>>(&args.exportNormalize)->multitoken(),
       "Normalize the data set between min and max\n"
       "Example: 0 255")
      //
      ("export-subset",
       po::value<std::vector<uint32_t>>(&args.exportSubset)->multitoken());

   options.add(genericOptions);
   options.add(configuration);
   options.add(generateOptions);
   options.add(ancientOptions);
   options.add(exportOptions);

   po::options_description cmdline_options;
   cmdline_options.add(options);
   cmdline_options.add(hidden);

   try
   {
      po::store(po::command_line_parser(argc, argv)
                   .options(cmdline_options)
                   .positional(p)
                   .run(),
                vm);
      po::notify(vm);
   }
   catch (po::too_many_positional_options_error& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }
   catch (po::error_with_option_name& e)
   {
      std::cerr << e.what() << std::endl;
      return -1;
   }

   return 0;
}

std::shared_ptr<World> GenerateWorld(const std::string&        worldName,
                                     uint32_t                  width,
                                     uint32_t                  height,
                                     uint32_t                  seed,
                                     uint32_t                  numPlates,
                                     const std::string&        outputDir,
                                     const Step&               step,
                                     float                     oceanLevel,
                                     const std::vector<float>& temps,
                                     const std::vector<float>& humids,
                                     WorldFormat               worldFormat,
                                     float                     gammaCurve,
                                     float                     curveOffset,
                                     bool                      fadeBorders,
                                     bool                      blackAndWhite)
{
   std::shared_ptr<World> world = WorldGen(worldName,
                                           width,
                                           height,
                                           seed,
                                           temps,
                                           humids,
                                           gammaCurve,
                                           curveOffset,
                                           numPlates,
                                           oceanLevel,
                                           step,
                                           fadeBorders);

   BOOST_LOG_TRIVIAL(info) << "Producing output";

   // Save data
   std::string worldFilename = outputDir + "/" + worldName + ".world";
   if (worldFormat == WorldFormat::Protobuf)
   {
      // TODO: Write
   }
   else if (worldFormat == WorldFormat::HDF5)
   {
      // TODO: SaveWorldToHdf5()
   }
   else
   {
      BOOST_LOG_TRIVIAL(error)
         << "Unknown format " << worldFormat << ", not saving";
   }
   BOOST_LOG_TRIVIAL(info) << "World data saved in " << worldFilename;

   // Generate images
   std::string oceanFilename = outputDir + "/" + worldName + "_ocean.png";
   // TODO: DrawOceanOnFile();
   BOOST_LOG_TRIVIAL(info) << "Ocean image generated in " << oceanFilename;

   if (step.includePrecipitations_)
   {
      std::string precipitationFilename =
         outputDir + "/" + worldName + "_precipitation.png";
      // TODO: DrawPrecipitationOnFile();
      BOOST_LOG_TRIVIAL(info)
         << "Precipitation image generated in " << precipitationFilename;

      std::string temperatureFilename =
         outputDir + "/" + worldName + "_temperature.png";
      // TODO: DrawTemperatureLevelsOnFile();
      BOOST_LOG_TRIVIAL(info)
         << "Temperature image generated in " << temperatureFilename;
   }

   if (step.includeBiome_)
   {
      std::string biomeFilename = outputDir + "/" + worldName + "_biome.png";
      // TODO: DrawBiomeOnFile();
      BOOST_LOG_TRIVIAL(info) << "Biome image generated in " << biomeFilename;
   }

   std::string elevationFilename = outputDir + "/" + worldName + "_biome.png";
   // TODO: DrawSimpleElevationOnFile();
   BOOST_LOG_TRIVIAL(info) << "Elevation image generated in "
                           << elevationFilename;

   return world;
}

void PrintArguments(const ArgumentsType& args)
{
   std::cout << "WorldEngine - A World Generator (version 0.19.1)" << std::endl;
   std::cout << "------------------------------------------------" << std::endl;
   if (IsGenerationOption(args.operation))
   {
      std::cout << " Operation            : " << args.operation << " generation"
                << std::endl;
      std::cout << " Seed                 : " << args.seed << std::endl;
      std::cout << " Name                 : " << args.worldName << std::endl;
      std::cout << " Width                : " << args.width << std::endl;
      std::cout << " Height               : " << args.height << std::endl;
      std::cout << " Number of plates     : " << args.numPlates << std::endl;
      std::cout << " World format         : " << args.worldFormat << std::endl;
      std::cout << " Black and white maps : " << args.blackAndWhite
                << std::endl;
      std::cout << " Step                 : " << args.step << std::endl;
      std::cout << " Grayscale heightmap  : " << args.grayscaleHeightmap
                << std::endl;
      std::cout << " Icecaps heightmap    : " << args.icecapsMap << std::endl;
      std::cout << " Rivers map           : " << args.rivers << std::endl;
      std::cout << " Scatter plot         : " << args.scatterPlot << std::endl;
      std::cout << " Satellite map        : " << args.satelliteMap << std::endl;
      std::cout << " Fade borders         : " << !args.notFadeBorders
                << std::endl;
      std::cout << " Temperature ranges   : " << args.temps << std::endl;
      std::cout << " Humidity ranges      : " << args.humids << std::endl;
      std::cout << " Gamma value          : " << args.gammaValue << std::endl;
      std::cout << " Gamma offset         : " << args.curveOffset << std::endl;
   }
   if (args.operation == OperationType::AncientMap)
   {
      std::cout << " Operation              : " << args.operation
                << " generation" << std::endl;
      std::cout << " Resize factor          : " << args.resizeFactor
                << std::endl;
      std::cout << " World file             : " << args.worldFile << std::endl;
      std::cout << " Sea color              : " << args.seaColor << std::endl;
      std::cout << " Draw biome             : " << !args.notDrawBiome
                << std::endl;
      std::cout << " Draw rivers            : " << !args.notDrawRivers
                << std::endl;
      std::cout << " Draw mountains         : " << !args.notDrawMountains
                << std::endl;
      std::cout << " Draw outer land border : " << args.drawOuterBorder
                << std::endl;
   }

   // TODO: Print warnings
}

void PrintUsage(const std::string&             programName,
                const po::options_description& options)
{
   std::cout << "Usage: " << programName
             << " [<operation> [<file>]] [<options>] " << std::endl;
   std::cout << std::endl;
   std::cout << "Arguments:" << std::endl;
   std::cout << "  operation (=world)                    "
             << "Valid operations: world, plates, ancient_map," << std::endl;
   std::cout << "                                        "
             << "info, export" << std::endl;
   std::cout << "  file                                  "
             << "Input filename for info and export" << std::endl;
   std::cout << "                                        "
             << "operations " << std::endl;
   std::cout << std::endl;
   std::cout << options << std::endl;
}

void PrintWorldInfo(const World& world)
{
   std::cout << "Name               : " << world.name() << std::endl;
   std::cout << "Width              : " << world.width() << std::endl;
   std::cout << "Height             : " << world.height() << std::endl;
   std::cout << "Seed               : " << world.seed() << std::endl;
   std::cout << "Num Plates         : " << world.numPlates() << std::endl;
   std::cout << "Ocean Level        : " << world.oceanLevel() << std::endl;
   std::cout << "Step               : " << world.step().name() << std::endl;

   std::cout << "Has Biome          : " << world.HasBiome() << std::endl;
   std::cout << "Has Humidity       : " << world.HasHumidity() << std::endl;
   std::cout << "Has Irrigation     : " << world.HasIrrigiation() << std::endl;
   std::cout << "Has Permeability   : " << world.HasPermeability() << std::endl;
   std::cout << "Has Watermap       : " << world.HasWatermap() << std::endl;
   std::cout << "Has Precipitations : " << world.HasPrecipitations()
             << std::endl;
   std::cout << "Has Temperature    : " << world.HasTemperature() << std::endl;
}

int ValidateArguments(ArgumentsType& args, const po::variables_map& vm)
{
   // TODO: Validate file positional parameter

   if (!vm.count("seed"))
   {
      std::default_random_engine              generator;
      std::uniform_int_distribution<uint32_t> distribution(MIN_SEED, MAX_SEED);
      args.seed = distribution(generator);
   }

   if (args.worldName.empty())
   {
      args.worldName = "seed_" + std::to_string(args.seed);
   }

   return 0;
}

void CliMain(int argc, const char** argv)
{
   int                     status;
   ArgumentsType           args;
   po::options_description options("Allowed options");
   po::variables_map       vm;

   status = AddOptions(argc, argv, args, options, vm);

   if (args.help || status != 0)
   {
      PrintUsage((argc > 0 ? argv[0] : "worldengine.exe"), options);
      return;
   }

   status = ValidateArguments(args, vm);

   PrintArguments(args);

   if (args.operation == OperationType::World)
   {
      BOOST_LOG_TRIVIAL(info) << "Starting world generation...";

      std::shared_ptr<World> world = GenerateWorld(args.worldName,
                                                   args.width,
                                                   args.height,
                                                   args.seed,
                                                   args.numPlates,
                                                   args.outputDir,
                                                   Step::step(args.step),
                                                   args.oceanLevel,
                                                   args.temps,
                                                   args.humids,
                                                   args.worldFormat,
                                                   args.gammaValue,
                                                   args.curveOffset,
                                                   !args.notFadeBorders,
                                                   args.blackAndWhite);

      if (args.grayscaleHeightmap)
      {
         // TODO: GenerateGrayscaleHeightmap();
      }

      if (args.rivers)
      {
         // TODO: GenerateRiversMap();
      }

      if (args.scatterPlot)
      {
         // TODO: DrawScatterPlot
      }

      if (args.satelliteMap)
      {
         // TODO: DrawSatelliteMap();
      }

      if (args.icecapsMap)
      {
         // TODO: DrawIcecapsMap();
      }
   }
   else if (args.operation == OperationType::Plates)
   {
      // TODO
   }
   else if (args.operation == OperationType::AncientMap)
   {
      // TODO
   }
   else if (args.operation == OperationType::Info)
   {
      // TODO
   }
   else if (args.operation == OperationType::Export)
   {
      // TODO
   }
   else
   {
      throw std::invalid_argument("Unknown operation: " +
                                  OperationTypeToString(args.operation));
   }

   BOOST_LOG_TRIVIAL(info) << "Done";
}

} // namespace WorldEngine
