#pragma once

#include "common.h"

#include <string>
#include <vector>

#include <boost/python/numpy.hpp>

namespace WorldEngine
{

class World
{
private:
   explicit World();

public:
   World(const std::string&          name,
         Size                        size,
         uint32_t                    seed,
         const GenerationParameters& generationParams,
         const std::vector<float>&   temps,
         const std::vector<float>&   humids,
         float                       gammaCurve  = DEFAULT_GAMMA_CURVE,
         float                       curveOffset = DEFAULT_CURVE_OFFSET);
   ~World();

   const std::string& name() const;
   uint32_t           width() const;
   uint32_t           height() const;
   uint32_t           seed() const;
   uint32_t           numPlates() const;
   float              oceanLevel() const;
   const Step&        step() const;

   bool HasBiome() const;
   bool HasHumidity() const;
   bool HasIrrigiation() const;
   bool HasPermeability() const;
   bool HasWatermap() const;
   bool HasPrecipitations() const;
   bool HasTemperature() const;

   void SetElevationData(const boost::python::numpy::ndarray& data);
   void SetPlatesData(const boost::python::numpy::ndarray& data);

private:
   std::string          name_;
   Size                 size_;
   uint32_t             seed_;
   GenerationParameters generationParams_;
   std::vector<float>   temps_;
   std::vector<float>   humids_;
   float                gammaCurve_;
   float                curveOffset_;
};

} // namespace WorldEngine
