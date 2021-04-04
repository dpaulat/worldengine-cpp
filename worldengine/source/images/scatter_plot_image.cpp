#include "scatter_plot_image.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{
ScatterPlotImage::ScatterPlotImage(size_t size) : Image(false), size_(size) {}
ScatterPlotImage::~ScatterPlotImage() {}

void ScatterPlotImage::DrawImage(const World&                      world,
                                 boost::gil::rgb8_image_t::view_t& target)
{
   const HumidityArrayType&    humidity    = world.GetHumidityData();
   const TemperatureArrayType& temperature = world.GetTemperatureData();

   // Find minimum and maximum values of humidity and temperature on land so we
   // can normalize temperature and humidity to the char.
   float minHumidity    = std::numeric_limits<float>::max();
   float maxHumidity    = std::numeric_limits<float>::lowest();
   float minTemperature = std::numeric_limits<float>::max();
   float maxTemperature = std::numeric_limits<float>::lowest();

   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         if (world.IsLand(x, y))
         {
            if (humidity[y][x] < minHumidity)
            {
               minHumidity = humidity[y][x];
            }
            if (humidity[y][x] > maxHumidity)
            {
               maxHumidity = humidity[y][x];
            }
            if (temperature[y][x] < minTemperature)
            {
               minTemperature = temperature[y][x];
            }
            if (temperature[y][x] > maxTemperature)
            {
               maxTemperature = temperature[y][x];
            }
         }
      }
   }

   const float temperatureDelta = maxTemperature - minTemperature;
   const float humidityDelta    = maxHumidity - minHumidity;

   // Set all pixels white
   for (uint32_t y = 0; y < size_; y++)
   {
      for (uint32_t x = 0; x < size_; x++)
      {
         target(x, y) = boost::gil::rgb8_pixel_t(255, 255, 255);
      }
   }

   // Fill in "bad" boxes with gray
   const std::vector<HumidityLevels>  hValues({HumidityLevels::Arid,
                                              HumidityLevels::Semiarid,
                                              HumidityLevels::Subhumid,
                                              HumidityLevels::Humid,
                                              HumidityLevels::Perhumid});
   const std::vector<TemperatureType> tValues({TemperatureType::Polar,
                                               TemperatureType::Alpine,
                                               TemperatureType::Boreal,
                                               TemperatureType::Cool,
                                               TemperatureType::Subtropical});

   for (size_t i = 0; i < 5; i++)
   {
      int32_t hMin = std::max(
         static_cast<int32_t>((size_ - 1) *
                              (world.GetThreshold(hValues[i]) - minHumidity) /
                              humidityDelta),
         0);
      int32_t hMax;
      if (i != 4)
      {
         hMax = std::min(static_cast<int32_t>(
                            (size_ - 1) *
                            (world.GetThreshold(hValues[i + 1]) - minHumidity) /
                            humidityDelta),
                         static_cast<int32_t>(size_));
      }
      else
      {
         hMax = size_;
      }
      int32_t vMax = std::clamp<int32_t>(
         static_cast<int32_t>(
            (size_ - 1) * (world.GetThreshold(tValues[i]) - minTemperature) /
            temperatureDelta),
         0,
         size_ - 1);

      BOOST_LOG_TRIVIAL(trace) << "i, hMin, hMax, vMax: (" << i << ", " << hMin
                               << ", " << hMax << ", " << vMax << ")";

      for (uint32_t y = hMin; y < hMax; y++)
      {
         for (uint32_t x = 0; x < vMax; x++)
         {
            target(x, (size_ - 1) - y) =
               boost::gil::rgb8_pixel_t(128, 128, 128);
         }
      }
   }

   // Draw lines based on thresholds
   for (TemperatureType t : TemperatureIterator())
   {
      int32_t v = static_cast<int32_t>(
         (size_ - 1) * (world.GetThreshold(t) - minTemperature) /
         temperatureDelta);
      if (0 < v && v < size_)
      {
         for (uint32_t y = 0; y < size_; y++)
         {
            target(v, (size_ - 1) - y) = boost::gil::rgb8_pixel_t(0, 0, 0);
         }
      }
   }

   for (HumidityLevels p : HumidityIterator())
   {
      int32_t h = static_cast<int32_t>(
         (size_ - 1) * (world.GetThreshold(p) - minHumidity) / humidityDelta);
      if (0 < h && h < size_)
      {
         for (uint32_t x = 0; x < size_; x++)
         {
            target(x, (size_ - 1) - h) = boost::gil::rgb8_pixel_t(0, 0, 0);
         }
      }
   }

   // Draw gamma curve
   const float curveGamma = world.gammaCurve();
   const float curveBonus = world.curveOffset();

   for (uint32_t x = 0; x < size_; x++)
   {
      uint32_t y = static_cast<uint32_t>(
         (size_ - 1) *
         ((std::powf(static_cast<float>(x) / (size_ - 1), curveGamma) *
           (1 - curveBonus)) +
          curveBonus));
      target(x, (size_ - 1) - y) = boost::gil::rgb8_pixel_t(255, 0, 0);
   }

   // Examine all cells in the map and if it is land get the temperature and
   // humidity for the cell.
   for (uint32_t y = 0; y < world.height(); y++)
   {
      for (uint32_t x = 0; x < world.width(); x++)
      {
         if (world.IsLand(x, y))
         {
            float t = temperature[y][x];
            float p = humidity[y][x];

            // Get red and blue values depending on temperature and humidity
            uint8_t r = 0;
            uint8_t b = 0;

            switch (world.GetTemperatureType(x, y))
            {
            case TemperatureType::Polar:
               r = 0; //
               break;

            case TemperatureType::Alpine:
               r = 42; //
               break;

            case TemperatureType::Boreal:
               r = 85; //
               break;

            case TemperatureType::Cool:
               r = 128; //
               break;

            case TemperatureType::Warm:
               r = 170; //
               break;

            case TemperatureType::Subtropical:
               r = 213; //
               break;

            case TemperatureType::Tropical:
            default:
               r = 255; //
               break;
            }

            switch (world.GetHumidityLevel(x, y))
            {
            case HumidityLevels::Superarid:
               b = 32; //
               break;

            case HumidityLevels::Perarid:
               b = 64; //
               break;

            case HumidityLevels::Arid:
               b = 96; //
               break;

            case HumidityLevels::Semiarid:
               b = 128; //
               break;

            case HumidityLevels::Subhumid:
               b = 160; //
               break;

            case HumidityLevels::Humid:
               b = 192; //
               break;

            case HumidityLevels::Perhumid:
               b = 224; //
               break;

            case HumidityLevels::Superhumid:
            default:
               b = 255; //
               break;
            }

            // Calculate x and y position based on normalized temperature and
            // humidity
            uint32_t nx = (size_ - 1) * (t - minTemperature) / temperatureDelta;
            uint32_t ny = (size_ - 1) * (p - minHumidity) / humidityDelta;

            target(nx, (size_ - 1) - ny) = boost::gil::rgb8_pixel_t(r, 128, b);
         }
      }
   }
}
} // namespace WorldEngine
