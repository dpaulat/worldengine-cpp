#include "worldengine/images/scatter_plot_image.h"

#include <boost/log/trivial.hpp>

namespace WorldEngine
{
ScatterPlotImage::ScatterPlotImage(const World& world, uint32_t size) :
    Image(world, Size(size, size)), size_(size)
{
}
ScatterPlotImage::~ScatterPlotImage() {}

void ScatterPlotImage::DrawImage(boost::gil::rgb8_image_t::view_t& target)
{
   const HumidityArrayType&    humidity    = world_.GetHumidityData();
   const TemperatureArrayType& temperature = world_.GetTemperatureData();

   // Find minimum and maximum values of humidity and temperature on land so we
   // can normalize temperature and humidity to the char.
   float minHumidity    = std::numeric_limits<float>::max();
   float maxHumidity    = std::numeric_limits<float>::lowest();
   float minTemperature = std::numeric_limits<float>::max();
   float maxTemperature = std::numeric_limits<float>::lowest();

   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (world_.IsLand(x, y))
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
   const std::vector<HumidityLevel>    hValues({HumidityLevel::Arid,
                                             HumidityLevel::Semiarid,
                                             HumidityLevel::Subhumid,
                                             HumidityLevel::Humid,
                                             HumidityLevel::Perhumid});
   const std::vector<TemperatureLevel> tValues({TemperatureLevel::Polar,
                                                TemperatureLevel::Alpine,
                                                TemperatureLevel::Boreal,
                                                TemperatureLevel::Cool,
                                                TemperatureLevel::Subtropical});

   for (size_t i = 0; i < 5; i++)
   {
      int32_t hMin = std::max(
         static_cast<int32_t>((size_ - 1) *
                              (world_.GetThreshold(hValues[i]) - minHumidity) /
                              humidityDelta),
         0);
      int32_t hMax;
      if (i != 4)
      {
         hMax =
            std::min(static_cast<int32_t>(
                        (size_ - 1) *
                        (world_.GetThreshold(hValues[i + 1]) - minHumidity) /
                        humidityDelta),
                     static_cast<int32_t>(size_));
      }
      else
      {
         hMax = size_;
      }
      int32_t vMax = std::clamp<int32_t>(
         static_cast<int32_t>(
            (size_ - 1) * (world_.GetThreshold(tValues[i]) - minTemperature) /
            temperatureDelta),
         0,
         size_ - 1);

      BOOST_LOG_TRIVIAL(trace) << "i, hMin, hMax, vMax: (" << i << ", " << hMin
                               << ", " << hMax << ", " << vMax << ")";

      for (int32_t y = hMin; y < hMax; y++)
      {
         for (int32_t x = 0; x < vMax; x++)
         {
            target(x, (size_ - 1) - y) =
               boost::gil::rgb8_pixel_t(128, 128, 128);
         }
      }
   }

   // Draw lines based on thresholds
   for (TemperatureLevel t : TemperatureIterator())
   {
      int32_t v = static_cast<int32_t>(
         (size_ - 1) * (world_.GetThreshold(t) - minTemperature) /
         temperatureDelta);
      if (0 < v && v < static_cast<int32_t>(size_))
      {
         for (uint32_t y = 0; y < size_; y++)
         {
            target(v, (size_ - 1) - y) = boost::gil::rgb8_pixel_t(0, 0, 0);
         }
      }
   }

   for (HumidityLevel p : HumidityIterator())
   {
      int32_t h = static_cast<int32_t>(
         (size_ - 1) * (world_.GetThreshold(p) - minHumidity) / humidityDelta);
      if (0 < h && h < static_cast<int32_t>(size_))
      {
         for (uint32_t x = 0; x < size_; x++)
         {
            target(x, (size_ - 1) - h) = boost::gil::rgb8_pixel_t(0, 0, 0);
         }
      }
   }

   // Draw gamma curve
   const float curveGamma = world_.gammaCurve();
   const float curveBonus = world_.curveOffset();

   for (uint32_t x = 0; x < size_; x++)
   {
      uint32_t y = static_cast<uint32_t>(
         (size_ - 1) *
         ((std::pow(static_cast<float>(x) / (size_ - 1), curveGamma) *
           (1 - curveBonus)) +
          curveBonus));
      target(x, (size_ - 1) - y) = boost::gil::rgb8_pixel_t(255, 0, 0);
   }

   // Examine all cells in the map and if it is land get the temperature and
   // humidity for the cell.
   for (uint32_t y = 0; y < world_.height(); y++)
   {
      for (uint32_t x = 0; x < world_.width(); x++)
      {
         if (world_.IsLand(x, y))
         {
            float t = temperature[y][x];
            float p = humidity[y][x];

            // Get red and blue values depending on temperature and humidity
            uint8_t r = 0;
            uint8_t b = 0;

            switch (world_.GetTemperatureLevel(x, y))
            {
            case TemperatureLevel::Polar:
               r = 0; //
               break;

            case TemperatureLevel::Alpine:
               r = 42; //
               break;

            case TemperatureLevel::Boreal:
               r = 85; //
               break;

            case TemperatureLevel::Cool:
               r = 128; //
               break;

            case TemperatureLevel::Warm:
               r = 170; //
               break;

            case TemperatureLevel::Subtropical:
               r = 213; //
               break;

            case TemperatureLevel::Tropical:
            default:
               r = 255; //
               break;
            }

            switch (world_.GetHumidityLevel(x, y))
            {
            case HumidityLevel::Superarid:
               b = 32; //
               break;

            case HumidityLevel::Perarid:
               b = 64; //
               break;

            case HumidityLevel::Arid:
               b = 96; //
               break;

            case HumidityLevel::Semiarid:
               b = 128; //
               break;

            case HumidityLevel::Subhumid:
               b = 160; //
               break;

            case HumidityLevel::Humid:
               b = 192; //
               break;

            case HumidityLevel::Perhumid:
               b = 224; //
               break;

            case HumidityLevel::Superhumid:
            default:
               b = 255; //
               break;
            }

            // Calculate x and y position based on normalized temperature and
            // humidity
            uint32_t nx = static_cast<uint32_t>(
               (size_ - 1) * (t - minTemperature) / temperatureDelta);
            uint32_t ny = static_cast<uint32_t>(
               (size_ - 1) * (p - minHumidity) / humidityDelta);

            target(nx, (size_ - 1) - ny) = boost::gil::rgb8_pixel_t(r, 128, b);
         }
      }
   }
}
} // namespace WorldEngine
