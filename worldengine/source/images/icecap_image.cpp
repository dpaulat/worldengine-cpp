#include "icecap_image.h"

namespace WorldEngine
{
IcecapImage::IcecapImage() : Image(false, true) {}
IcecapImage::~IcecapImage() {}

void IcecapImage::DrawImage(const World&                       world,
                            boost::gil::gray8_image_t::view_t& target)
{
   DrawGrayscaleFromArray(world.GetIcecapData(), target);
}
} // namespace WorldEngine
