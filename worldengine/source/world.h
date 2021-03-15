#pragma once

#include "common.h"

#include <string>
#include <vector>

#include <boost/multi_array.hpp>

namespace WorldEngine
{

typedef float    ElevationDataType;
typedef bool     OceanDataType;
typedef uint16_t PlateDataType;
typedef float    TemperatureDataType;

typedef boost::multi_array<ElevationDataType, 2>   ElevationArrayType;
typedef boost::multi_array<OceanDataType, 2>       OceanArrayType;
typedef boost::multi_array<PlateDataType, 2>       PlateArrayType;
typedef boost::multi_array<TemperatureDataType, 2> TemperatureArrayType;

class World
{
private:
   explicit World();

public:
   World(const std::string&          name,
         Size                        size,
         uint32_t                    seed,
         const GenerationParameters& generationParams,
         const std::vector<float>&   temps       = DEFAULT_TEMPS,
         const std::vector<float>&   humids      = DEFAULT_HUMIDS,
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

   const ElevationArrayType&   GetElevationData() const;
   const OceanArrayType&       GetOceanData() const;
   const TemperatureArrayType& GetTemperatureData() const;

   ElevationArrayType&   GetElevationData();
   OceanArrayType&       GetOceanData();
   PlateArrayType&       GetPlateData();
   TemperatureArrayType& GetTemperatureData();

   float GetThreshold(ThresholdType type) const;

   void SetElevationData(const float* heightmap);
   void SetPlatesData(const uint32_t* platesmap);

   void SetThreshold(ThresholdType type, float value);

   bool ProtobufSerialize(std::string& output) const;

private:
   std::string          name_;
   Size                 size_;
   uint32_t             seed_;
   GenerationParameters generationParams_;
   std::vector<float>   temps_;
   std::vector<float>   humids_;
   float                gammaCurve_;
   float                curveOffset_;

   ElevationArrayType   elevation_;
   PlateArrayType       plates_;
   OceanArrayType       ocean_;
   TemperatureArrayType temperature_;

   float thresholds_[static_cast<uint32_t>(ThresholdType::Count)];

   static int32_t WorldengineTag();
   static int32_t VersionHashcode();

   template<typename T, typename U>
   void SetArrayData(const U* source, boost::multi_array<T, 2>& dest);
};

} // namespace WorldEngine
