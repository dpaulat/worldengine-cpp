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
typedef float    PrecipitationDataType;
typedef float    HumidityDataType;

typedef boost::multi_array<ElevationDataType, 2>     ElevationArrayType;
typedef boost::multi_array<OceanDataType, 2>         OceanArrayType;
typedef boost::multi_array<PlateDataType, 2>         PlateArrayType;
typedef boost::multi_array<TemperatureDataType, 2>   TemperatureArrayType;
typedef boost::multi_array<PrecipitationDataType, 2> PrecipitationArrayType;
typedef boost::multi_array<HumidityDataType, 2>      HumidityArrayType;

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

   const std::string&        name() const;
   uint32_t                  width() const;
   uint32_t                  height() const;
   uint32_t                  seed() const;
   uint32_t                  numPlates() const;
   float                     oceanLevel() const;
   const Step&               step() const;
   const std::vector<float>& temps() const;
   const std::vector<float>& humids() const;
   float                     gammaCurve() const;
   float                     curveOffset() const;

   bool HasBiome() const;
   bool HasHumidity() const;
   bool HasIrrigiation() const;
   bool HasPermeability() const;
   bool HasWatermap() const;
   bool HasPrecipitations() const;
   bool HasTemperature() const;

   const ElevationArrayType&     GetElevationData() const;
   const OceanArrayType&         GetOceanData() const;
   const TemperatureArrayType&   GetTemperatureData() const;
   const PrecipitationArrayType& GetPrecipitationData() const;
   const HumidityArrayType&      GetHumidityData() const;

   ElevationArrayType&     GetElevationData();
   OceanArrayType&         GetOceanData();
   PlateArrayType&         GetPlateData();
   TemperatureArrayType&   GetTemperatureData();
   PrecipitationArrayType& GetPrecipitationData();
   HumidityArrayType&      GetHumidityData();

   float GetThreshold(ElevationThresholdType type) const;
   float GetThreshold(HumidityLevels type) const;
   float GetThreshold(PrecipitationLevelType type) const;
   float GetThreshold(TemperatureType type) const;

   TemperatureType GetTemperatureType(uint32_t x, uint32_t y) const;
   HumidityLevels  GetHumidityLevel(uint32_t x, uint32_t y) const;

   void SetElevationData(const float* heightmap);
   void SetPlatesData(const uint32_t* platesmap);

   void SetThreshold(ElevationThresholdType type, float value);
   void SetThreshold(HumidityLevels type, float value);
   void SetThreshold(PrecipitationLevelType type, float value);
   void SetThreshold(TemperatureType type, float value);

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

   ElevationArrayType     elevation_;
   PlateArrayType         plates_;
   OceanArrayType         ocean_;
   TemperatureArrayType   temperature_;
   PrecipitationArrayType precipitation_;
   HumidityArrayType      humidity_;

   float elevationThresholds_[static_cast<uint32_t>(
      ElevationThresholdType::Count)];
   float humidityThresholds_[static_cast<uint32_t>(HumidityLevels::Count)];
   float precipitationThresholds_[static_cast<uint32_t>(
      PrecipitationLevelType::Count)];
   float temperatureThresholds_[static_cast<uint32_t>(TemperatureType::Count)];

   static int32_t WorldengineTag();
   static int32_t VersionHashcode();

   template<typename T, typename U>
   void SetArrayData(const U* source, boost::multi_array<T, 2>& dest);
};

} // namespace WorldEngine
