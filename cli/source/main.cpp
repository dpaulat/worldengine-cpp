#include "worldengine.h"

#include <boost/python/numpy.hpp>

namespace numpy = boost::python::numpy;

int main(int argc, const char** argv)
{
   // Initialize Python runtime
   Py_Initialize();
   numpy::initialize();

   WorldEngine::CliMain(argc, argv);

   return 0;
}
