#include "Image.h"

#include <cassert>
#include <cstring>

#include <stb/stb_image.h>

uint8_t Image::operator()(const uint32_t x, const uint32_t y) const {
    return data[y * width + x];
}

uint8_t& Image::operator()(const uint32_t x, const uint32_t y) {
    return data[y * width + x];
}

std::optional<Image> Image::FromFile(const std::filesystem::path& path, Format format) {
    int desiredChannels = 0;
    switch (format) {
    case Format::I:
        desiredChannels = 1;
        break;
    }
    assert(desiredChannels);

    stbi_set_flip_vertically_on_load(true);

    int w, h, bpp;
    stbi_uc* buf = stbi_load(path.string().c_str(), &w, &h, &bpp, desiredChannels);

    if (!buf)
        return std::nullopt;

    Image img;
    img.width = w;
    img.height = h;
    img.format = format;
    img.data.resize(w * h * desiredChannels);
    memcpy(img.data.data(), buf, w * h);

    stbi_image_free(buf);
    return img;
}
