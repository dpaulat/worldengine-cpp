#include <gtest/gtest.h>

#include <worldengine/plates.h>

namespace WorldEngine
{

void CheckEqual(const World& w1, const World& w2)
{
   EXPECT_EQ(w1.name(), w2.name());
   EXPECT_EQ(w1.width(), w2.width());
   EXPECT_EQ(w1.height(), w2.height());
   EXPECT_EQ(w1.oceanLevel(), w2.oceanLevel());
   EXPECT_EQ(w1.seed(), w2.seed());
   EXPECT_EQ(w1.numPlates(), w2.numPlates());
   EXPECT_EQ(w1.step().stepType_, w2.step().stepType_);
   EXPECT_EQ(w1.GetElevationData(), w2.GetElevationData());
   EXPECT_EQ(w1.GetPlateData(), w2.GetPlateData());
   EXPECT_EQ(w1.GetOceanData(), w2.GetOceanData());
   EXPECT_EQ(w1.GetSeaDepthData(), w2.GetSeaDepthData());
   EXPECT_EQ(w1.GetBiomeData(), w2.GetBiomeData());
   EXPECT_EQ(w1.GetHumidityData(), w2.GetHumidityData());
   EXPECT_EQ(w1.GetIrrigationData(), w2.GetIrrigationData());
   EXPECT_EQ(w1.GetPermeabilityData(), w2.GetPermeabilityData());
   EXPECT_EQ(w1.GetWaterMapData(), w2.GetWaterMapData());
   EXPECT_EQ(w1.GetLakeMapData(), w2.GetLakeMapData());
   EXPECT_EQ(w1.GetRiverMapData(), w2.GetRiverMapData());
   EXPECT_EQ(w1.GetPrecipitationData(), w2.GetPrecipitationData());
   EXPECT_EQ(w1.GetTemperatureData(), w2.GetTemperatureData());
   EXPECT_EQ(w1.GetIcecapData(), w2.GetIcecapData());

   for (ElevationThreshold t : ElevationIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
   for (HumidityLevel t : HumidityIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
   for (PermeabilityLevel t : PermeabilityIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
   for (WaterThreshold t : WaterIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
   for (PrecipitationLevel t : PrecipitationIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
   for (TemperatureLevel t : TemperatureIterator())
   {
      EXPECT_EQ(w1.GetThreshold(t), w2.GetThreshold(t));
   }
}

TEST(SerializationTest, ProtobufTest)
{
   std::shared_ptr<World> world = WorldGen("Dummy", 32, 16, 1);

   std::string serialized;
   world->ProtobufSerialize(serialized);

   std::shared_ptr<World> deserialized = std::make_shared<World>();
   std::stringstream      input(serialized);
   deserialized->ProtobufDeserialize(input);

   CheckEqual(*world, *deserialized);
}

TEST(SerializationTest, HDF5Test)
{
   const std::string filename = GenerateTemporaryFilename("hdf5-test-");

   std::shared_ptr<World> w1 = WorldGen("Dummy", 32, 16, 1);
   w1->SaveHdf5(filename);

   std::shared_ptr<World> w2 = std::make_shared<World>();
   w2->ReadHdf5(filename);

   std::remove(filename.c_str());

   CheckEqual(*w1, *w2);
}

} // namespace WorldEngine
