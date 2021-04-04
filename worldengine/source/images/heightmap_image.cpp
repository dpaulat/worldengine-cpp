#include "heightmap_image.h"

namespace WorldEngine
{
HeightmapImage::HeightmapImage(const World& world) : Image(world, false, true)
{
}
HeightmapImage::~HeightmapImage() {}

void HeightmapImage::DrawImage(boost::gil::gray8_image_t::view_t& target)
{
   DrawGrayscaleFromArray(world_.GetElevationData(), target);
}
} // namespace WorldEngine
