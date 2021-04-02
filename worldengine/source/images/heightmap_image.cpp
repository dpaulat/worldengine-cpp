#include "heightmap_image.h"

namespace WorldEngine
{
HeightmapImage::HeightmapImage() : Image(false, true) {}
HeightmapImage::~HeightmapImage() {}

void HeightmapImage::DrawImage(const World&                       world,
                               boost::gil::gray8_image_t::view_t& target) const
{
   DrawGrayscaleFromArray(world.GetElevationData(), target);
}
} // namespace WorldEngine
