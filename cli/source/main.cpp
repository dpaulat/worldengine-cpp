#include <iostream>

#include <boost/program_options.hpp>
#include <boost/python/numpy.hpp>

#include <world.h>

namespace numpy = boost::python::numpy;

void PrintWorldInfo(const WorldEngine::World& world);

int main(int argc, const char** argv)
{
   // Initialize Python runtime
   Py_Initialize();
   numpy::initialize();

   return 0;
}

void PrintWorldInfo(const WorldEngine::World& world)
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
