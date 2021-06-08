#include "worldengine/images/icecap_image.h"

namespace WorldEngine
{
IcecapImage::IcecapImage(const World& world) : Image(world, false, true) {}
IcecapImage::~IcecapImage() {}

void IcecapImage::DrawImage(boost::gil::gray8_image_t::view_t& target)
{
   DrawGrayscaleFromArray(world_.GetIcecapData(), target);
}
} // namespace WorldEngine
