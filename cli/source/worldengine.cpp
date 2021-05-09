#include "worldengine.h"
#include "types.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

#include <boost/assign.hpp>
#include <boost/bind/bind.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/random/random_device.hpp>

#include <worldengine/common.h>
#include <worldengine/export.h>
#include <worldengine/generation.h>
#include <worldengine/plates.h>
#include <worldengine/world.h>

#include <worldengine/images/ancient_map_image.h>
#include <worldengine/images/biome_image.h>
#include <worldengine/images/elevation_image.h>
#include <worldengine/images/heightmap_image.h>
#include <worldengine/images/icecap_image.h>
#include <worldengine/images/ocean_image.h>
#include <worldengine/images/precipitation_image.h>
#include <worldengine/images/river_image.h>
#include <worldengine/images/satellite_image.h>
#include <worldengine/images/scatter_plot_image.h>
#include <worldengine/images/simple_elevation_image.h>
#include <worldengine/images/temperature_image.h>
#include <worldengine/images/world_image.h>

namespace po = boost::program_options;

namespace WorldEngine
{

int AddOptions(int                      argc,
               const char**             argv,
               ArgumentsType&           args,
               po::options_description& options,
               po::variables_map&       vm);

template<typename T>
static void CheckCount(const std::vector<T>& v,
                       const size_t&         count,
                       const std::string&    errorMessage = "");

template<typename T>
static void CheckRange(const T&           value,
                       const T&           min,
                       const T&           max,
                       const std::string& errorMessage = "");

void GeneratePlates(const std::string& worldName,
                    uint32_t           width,
                    uint32_t           height,
                    uint32_t           seed,
                    const std::string& outputDir,
                    uint32_t           numPlates = DEFAULT_NUM_PLATES);
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
              float                     gammaCurve    = DEFAULT_GAMMA_CURVE,
              float                     curveOffset   = DEFAULT_CURVE_OFFSET,
              bool                      fadeBorders   = DEFAULT_FADE_BORDERS,
              bool                      blackAndWhite = DEFAULT_BLACK_AND_WHITE,
              bool                      gsHeightmap   = DEFAULT_GS_HEIGHTMAP,
              bool                      rivers        = DEFAULT_RIVERS_MAP,
              bool                      scatterPlot   = DEFAULT_SCATTER_PLOT,
              bool                      satelliteMap  = DEFAULT_SATELLITE_MAP,
              bool                      icecapsMap    = DEFAULT_ICECAPS_MAP,
              bool                      worldMap      = DEFAULT_WORLD_MAP,
              bool                      elevationMap  = DEFAULT_ELEVATION_MAP,
              bool elevationShadows = DEFAULT_ELEVATION_SHADOWS);

std::shared_ptr<World> LoadWorld(const std::string& filename,
                                 WorldFormat        format);

void PrintUsage(const std::string&             programName,
                const po::options_description& options);
void PrintWorldInfo(const World& world);
void SetLogLevel(const ArgumentsType& args, const po::variables_map& vm);
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
      ("format",
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
       po::value<uint32_t>(&args.numPlates)
          ->default_value(DEFAULT_NUM_PLATES)
          ->notifier(boost::bind(&CheckRange<uint32_t>,
                                 boost::placeholders::_1,
                                 1,
                                 100,
                                 "Number of plates should be in [1, 100]")),
       "Number of plates\n"
       "Valid values: [1, 100]")
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
          ->default_value(DEFAULT_TEMPS)
          ->notifier(
             boost::bind(&CheckCount<float>,
                         boost::placeholders::_1,
                         6,
                         "List of temperatures must have exactly 6 values")),
       "Provide alternate ranges for temperatures")
      //
      ("humidity",
       po::value<std::vector<float>>(&args.humids)
          ->multitoken()
          ->default_value(DEFAULT_HUMIDS)
          ->notifier(
             boost::bind(&CheckCount<float>,
                         boost::placeholders::_1,
                         7,
                         "List of humidities must have exactly 7 values")),
       "Provide alternate ranges for humidities")
      //
      ("gamma-value",
       po::value<float>(&args.gammaValue)
          ->default_value(DEFAULT_GAMMA_CURVE)
          ->notifier([](const float& value) {
             if (value <= 0)
             {
                BOOST_LOG_TRIVIAL(error)
                   << "Gamma value must be greater than 0";
                throw po::validation_error(
                   po::validation_error::invalid_option_value);
             }
          }),
       "Gamma value for temperature/precipitation gamma correction curve\n"
       "Valid values: Positive floating point")
      //
      ("gamma-offset",
       po::value<float>(&args.curveOffset)
          ->default_value(DEFAULT_CURVE_OFFSET)
          ->notifier([](const float& value) {
             if (value < 0 || value >= 1)
             {
                BOOST_LOG_TRIVIAL(error)
                   << "Gamma offset must be between [0.0, 1.0)";
                throw po::validation_error(
                   po::validation_error::invalid_option_value);
             }
          }),
       "Adjustment value for temperature/precipitation gamma correction curve\n"
       "Valid values: [0.0, 1.0)")
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
       "Generate ice caps map")
      //
      ("world-map",
       po::bool_switch(&args.worldMap)->default_value(false),
       "Generate world map")
      //
      ("elevation-map",
       po::bool_switch(&args.elevationMap)->default_value(false),
       "Generate elevation map")
      //
      ("elevation-shadows",
       po::bool_switch(&args.elevationShadows)->default_value(false),
       "Draw shadows on elevation map");

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
       po::value<std::vector<int32_t>>(&args.exportNormalize)->multitoken(),
       "Normalize the data set between min and max\n"
       "Example: 0 255")
      //
      ("export-subset",
       po::value<std::vector<uint32_t>>(&args.exportSubset)->multitoken(),
       "Selects a subwindow from the data set\n"
       "Arguments: <xoff> <yoff> <xsize> <ysize>\n"
       "Example: 128 128 256 256");

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

template<typename T>
static void CheckCount(const std::vector<T>& v,
                       const size_t&         count,
                       const std::string&    errorMessage)
{
   if (v.size() != count)
   {
      BOOST_LOG_TRIVIAL(error) << errorMessage;
      throw po::validation_error(po::validation_error::invalid_option_value);
   }
}

template<typename T>
static void CheckRange(const T&           value,
                       const T&           min,
                       const T&           max,
                       const std::string& errorMessage)
{
   if (value < min || value > max)
   {
      BOOST_LOG_TRIVIAL(error) << errorMessage;
      throw po::validation_error(po::validation_error::invalid_option_value);
   }
}

void GeneratePlates(const std::string& worldName,
                    uint32_t           width,
                    uint32_t           height,
                    uint32_t           seed,
                    const std::string& outputDir,
                    uint32_t           numPlates)
{
   // Eventually this method should be invoked when generation is called and
   // asked to stop at step "plates", it should not be a different operation.
   float*    heightmap;
   uint32_t* platesmap;

   void* p = GeneratePlatesSimulation(&heightmap,
                                      &platesmap,
                                      seed,
                                      width,
                                      height,
                                      DEFAULT_SEA_LEVEL,
                                      DEFAULT_EROSION_PERIOD,
                                      DEFAULT_FOLDING_RATIO,
                                      DEFAULT_AGGR_OVERLAP_ABS,
                                      DEFAULT_AGGR_OVERLAP_REL,
                                      DEFAULT_CYCLE_COUNT,
                                      numPlates);

   std::shared_ptr<World> world = std::shared_ptr<World>(
      new World(worldName,
                Size(width, height),
                seed,
                GenerationParameters(numPlates, -1.0f, STEP_PLATES)));

   world->SetElevationData(heightmap);
   world->SetPlatesData(platesmap);

   PlatecApiDestroy(p);

   // Generate images
   const std::string platesFilename =
      outputDir + "/plates_" + worldName + ".png";
   SimpleElevationImage(*world).Draw(platesFilename);
   BOOST_LOG_TRIVIAL(info) << "Plates image generated in " << platesFilename;

   CenterLand(*world);

   const std::string centeredPlatesFilename =
      outputDir + "/centered_plates_" + worldName + ".png";
   SimpleElevationImage(*world).Draw(centeredPlatesFilename);
   BOOST_LOG_TRIVIAL(info) << "Centered plates image generated in "
                           << centeredPlatesFilename;
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
                                     bool                      blackAndWhite,
                                     bool                      gsHeightmap,
                                     bool                      rivers,
                                     bool                      scatterPlot,
                                     bool                      satelliteMap,
                                     bool                      icecapsMap,
                                     bool                      worldMap,
                                     bool                      elevationMap,
                                     bool                      elevationShadows)
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
      std::string data;
      if (world->ProtobufSerialize(data))
      {
         try
         {
            std::ofstream ofs(worldFilename,
                              std::ios_base::out | std::ios_base::binary);
            ofs << data;
            ofs.close();
         }
         catch (const std::exception& ex)
         {
            BOOST_LOG_TRIVIAL(error) << ex.what();
         }
      }
      else
      {
         BOOST_LOG_TRIVIAL(error) << "Error serializing world data";
      }
   }
   else if (worldFormat == WorldFormat::HDF5)
   {
      bool success = world->SaveHdf5(worldFilename);
      if (!success)
      {
         BOOST_LOG_TRIVIAL(error) << "Error writing world data to HDF5 file";
      }
   }
   else
   {
      BOOST_LOG_TRIVIAL(error)
         << "Unknown format " << worldFormat << ", not saving";
   }
   BOOST_LOG_TRIVIAL(info) << "World data saved in " << worldFilename;

   // Generate images
   std::string oceanFilename = outputDir + "/" + worldName + "_ocean.png";
   OceanImage(*world).Draw(oceanFilename);
   BOOST_LOG_TRIVIAL(info) << "Ocean image generated in " << oceanFilename;

   if (step.includePrecipitations_)
   {
      std::string precipitationFilename =
         outputDir + "/" + worldName + "_precipitation.png";
      PrecipitationImage(*world).Draw(precipitationFilename, blackAndWhite);
      BOOST_LOG_TRIVIAL(info)
         << "Precipitation image generated in " << precipitationFilename;

      std::string temperatureFilename =
         outputDir + "/" + worldName + "_temperature.png";
      TemperatureImage(*world).Draw(temperatureFilename, blackAndWhite);
      BOOST_LOG_TRIVIAL(info)
         << "Temperature image generated in " << temperatureFilename;
   }

   if (step.includeBiome_)
   {
      std::string biomeFilename = outputDir + "/" + worldName + "_biome.png";
      BiomeImage(*world).Draw(biomeFilename);
      BOOST_LOG_TRIVIAL(info) << "Biome image generated in " << biomeFilename;
   }

   std::string elevationFilename =
      outputDir + "/" + worldName + "_elevation.png";
   SimpleElevationImage(*world).Draw(
      elevationFilename, world->GetThreshold(ElevationThreshold::Sea));
   BOOST_LOG_TRIVIAL(info) << "Simple elevation image generated in "
                           << elevationFilename;

   if (gsHeightmap)
   {
      std::string heightmapFilename =
         outputDir + "/" + worldName + "_grayscale.png";
      HeightmapImage(*world).Draw(heightmapFilename);
      BOOST_LOG_TRIVIAL(info)
         << "Grayscale heightmap image generated in " << heightmapFilename;
   }

   if (rivers)
   {
      std::string riverFilename = outputDir + "/" + worldName + "_rivers.png";
      RiverImage(*world).Draw(riverFilename);
      BOOST_LOG_TRIVIAL(info) << "River image generated in " << riverFilename;
   }

   if (scatterPlot)
   {
      std::string scatterPlotFilename =
         outputDir + "/" + worldName + "_scatter.png";
      ScatterPlotImage(*world, DEFAULT_SCATTER_PLOT_SIZE)
         .Draw(scatterPlotFilename);
      BOOST_LOG_TRIVIAL(info)
         << "Scatter plot image generated in " << scatterPlotFilename;
   }

   if (satelliteMap)
   {
      std::string satelliteFilename =
         outputDir + "/" + worldName + "_satellite.png";
      SatelliteImage(*world, seed).Draw(satelliteFilename);
      BOOST_LOG_TRIVIAL(info)
         << "Satellite image generated in " << satelliteFilename;
   }

   if (icecapsMap)
   {
      std::string icecapFilename = outputDir + "/" + worldName + "_icecaps.png";
      IcecapImage(*world).Draw(icecapFilename);
      BOOST_LOG_TRIVIAL(info) << "Icecap image generated in " << icecapFilename;
   }

   if (worldMap)
   {
      std::string worldMapFilename = outputDir + "/" + worldName + "_world.png";
      WorldImage(*world).Draw(worldMapFilename);
      BOOST_LOG_TRIVIAL(info)
         << "World map image generated in " << worldMapFilename;
   }

   if (elevationMap)
   {
      std::string elevationMapFilename =
         outputDir + "/" + worldName + "_elevation_";
      if (elevationShadows)
      {
         elevationMapFilename += "shadow.png";
      }
      else
      {
         elevationMapFilename += "no_shadow.png";
      }
      ElevationImage(*world, elevationShadows).Draw(elevationMapFilename);
      BOOST_LOG_TRIVIAL(info)
         << "Elevation image generated in " << elevationMapFilename;
   }

   return world;
}

std::shared_ptr<World> LoadWorld(const std::string& worldFilename,
                                 WorldFormat        format)
{
   std::shared_ptr<World> world = std::make_shared<World>();

   bool success = false;

   if (format == WorldFormat::Protobuf)
   {

      std::ifstream input(worldFilename,
                          std::ios_base::in | std::ios_base::binary);
      success = world->ProtobufDeserialize(input);
   }
   else if (format == WorldFormat::HDF5)
   {
      success = world->ReadHdf5(worldFilename);
   }

   if (!success)
   {
      world = nullptr;
   }

   return world;
}

void PrintArguments(const ArgumentsType& args)
{
   std::cout << "WorldEngine C++ - A World Generator (version "
             << WORLDENGINE_VERSION << ")" << std::endl;
   std::cout << "----------------------------------------------------"
             << std::endl;
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
      std::cout << " World map            : " << args.worldMap << std::endl;
      std::cout << " Elevation map        : " << args.elevationMap << std::endl;
      std::cout << " Elevation shadows    : " << args.elevationShadows
                << std::endl;
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

   // Print warning messages
   auto minmaxTemps =
      std::minmax_element(args.temps.cbegin(), args.temps.cend());
   auto minmaxHumids =
      std::minmax_element(args.humids.cbegin(), args.humids.cend());

   if (!std::is_sorted(args.temps.cbegin(), args.temps.cend()))
   {
      BOOST_LOG_TRIVIAL(warning) << "Temperature array not in ascending order";
   }
   if (*minmaxTemps.first < 0.0f)
   {
      BOOST_LOG_TRIVIAL(warning)
         << "Minimum value in temperature array less than 0";
   }
   if (*minmaxTemps.second > 1.0f)
   {
      BOOST_LOG_TRIVIAL(warning)
         << "Maximum value in temperature array greater than 1";
   }
   if (!std::is_sorted(args.humids.cbegin(), args.humids.cend()))
   {
      BOOST_LOG_TRIVIAL(warning) << "Humidity array not in ascending order";
   }
   if (*minmaxHumids.first < 0.0f)
   {
      BOOST_LOG_TRIVIAL(warning)
         << "Minimum value in humidity array less than 0";
   }
   if (*minmaxHumids.second > 1.0f)
   {
      BOOST_LOG_TRIVIAL(warning)
         << "Maximum value in humidity array greater than 1";
   }
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
   std::cout << "Has Irrigation     : " << world.HasIrrigation() << std::endl;
   std::cout << "Has Permeability   : " << world.HasPermeability() << std::endl;
   std::cout << "Has Watermap       : " << world.HasWatermap() << std::endl;
   std::cout << "Has Precipitations : " << world.HasPrecipitations()
             << std::endl;
   std::cout << "Has Temperature    : " << world.HasTemperature() << std::endl;
}

void SetLogLevel(const ArgumentsType& args, const po::variables_map& vm)
{
   boost::log::trivial::severity_level severity = boost::log::trivial::info;

   if (args.verbose)
   {
      severity = boost::log::trivial::debug;
   }

   boost::log::core::get()->set_filter(boost::log::trivial::severity >=
                                       severity);
}

void TransformArguments(ArgumentsType& args)
{
   std::transform(args.temps.begin(),
                  args.temps.end(),
                  args.temps.begin(),
                  [](const float& t) -> float { return 1.0f - t; });

   std::transform(args.humids.begin(),
                  args.humids.end(),
                  args.humids.begin(),
                  [](const float& h) -> float { return 1.0f - h; });
}

int ValidateArguments(ArgumentsType& args, const po::variables_map& vm)
{
   int status = 0;

   if (std::filesystem::exists(args.outputDir))
   {
      if (!std::filesystem::is_directory(args.outputDir))
      {
         BOOST_LOG_TRIVIAL(error)
            << "Output directory exists, but is not a directory: "
            << args.outputDir;
         status = -1;
      }
   }
   else
   {
      BOOST_LOG_TRIVIAL(info)
         << "Creating output directory: " << args.outputDir;
      std::filesystem::create_directory(args.outputDir);
   }

   if (args.operation == OperationType::Info ||
       args.operation == OperationType::Export)
   {
      if (!vm.count("file"))
      {
         BOOST_LOG_TRIVIAL(error)
            << "For operations info and export, file parameter is required";
         status = -1;
      }
      else if (!std::filesystem::exists(args.file) ||
               std::filesystem::is_directory(args.file))
      {
         BOOST_LOG_TRIVIAL(error) << "The specified world file does not exist";
         status = -1;
      }
   }

   if (!vm.count("seed"))
   {
      // Unlike std::random_device, the boost version is guaranteed to be
      // non-deterministic
      boost::random::random_device            generator;
      std::uniform_int_distribution<uint32_t> distribution(MIN_SEED, MAX_SEED);
      args.seed = distribution(generator);
   }

   if (args.worldName.empty())
   {
      args.worldName = "seed_" + std::to_string(args.seed);
   }

   return status;
}

void CliMain(int argc, const char** argv)
{
   int                     status;
   ArgumentsType           args;
   po::options_description options("Allowed options");
   po::variables_map       vm;

   std::cout << std::boolalpha;

   status = AddOptions(argc, argv, args, options, vm);

   if (status == 0)
   {
      SetLogLevel(args, vm);
      status = ValidateArguments(args, vm);
   }

   if (args.help || status != 0)
   {
      PrintUsage((argc > 0 ? argv[0] : "worldengine.exe"), options);
      return;
   }

   PrintArguments(args);

   TransformArguments(args);

   std::shared_ptr<World> world;

   if (args.operation == OperationType::World)
   {
      BOOST_LOG_TRIVIAL(info) << "Starting world generation...";

      world = GenerateWorld(args.worldName,
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
                            args.blackAndWhite,
                            args.grayscaleHeightmap,
                            args.rivers,
                            args.scatterPlot,
                            args.satelliteMap,
                            args.icecapsMap,
                            args.worldMap,
                            args.elevationMap,
                            args.elevationShadows);
   }
   else if (args.operation == OperationType::Plates)
   {
      BOOST_LOG_TRIVIAL(info) << "Starting plates generation...";

      GeneratePlates(args.worldName,
                     args.width,
                     args.height,
                     args.seed,
                     args.outputDir,
                     args.numPlates);
   }
   else if (args.operation == OperationType::AncientMap)
   {
      world = LoadWorld(args.file, args.worldFormat);
      if (world != nullptr)
      {
         if (args.generatedFile.empty())
         {
            args.generatedFile =
               args.outputDir + "/ancient_map_" + world->name() + ".png";
         }

         BOOST_LOG_TRIVIAL(info) << "Generating ancient map...";

         AncientMapImage(*world,
                         world->seed(),
                         args.resizeFactor,
                         args.seaColor,
                         !args.notDrawBiome,
                         !args.notDrawRivers,
                         !args.notDrawMountains,
                         args.drawOuterBorder)
            .Draw(args.generatedFile);

         BOOST_LOG_TRIVIAL(info)
            << "Ancient map image generated in " << args.generatedFile;
      }
   }
   else if (args.operation == OperationType::Info)
   {
      world = LoadWorld(args.file, args.worldFormat);
      if (world != nullptr)
      {
         PrintWorldInfo(*world);
      }
   }
   else if (args.operation == OperationType::Export)
   {
      world = LoadWorld(args.file, args.worldFormat);
      if (world != nullptr)
      {
         BOOST_LOG_TRIVIAL(info) << "Exporting image...";

         const std::string path =
            args.outputDir + "/" + world->name() + "_elevation";
         ExportImage(*world,
                     args.exportFormat,
                     args.exportDatatype,
                     args.exportDimensions,
                     args.exportNormalize,
                     args.exportSubset,
                     path);
      }
   }
   else
   {
      throw std::invalid_argument("Unknown operation: " +
                                  OperationTypeToString(args.operation));
   }

   BOOST_LOG_TRIVIAL(info) << "Done";
}

} // namespace WorldEngine
