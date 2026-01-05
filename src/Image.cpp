#include "Image.h"

#include <stb/stb_image.h>

std::optional<Image> Image::FromFile(const std::filesystem::path& path) {
    stbi_set_flip_vertically_on_load(true);

    int w, h, bpp;
    stbi_uc* buf = stbi_load(path.string().c_str(), &w, &h, &bpp, STBI_grey);

    if (!buf)
        return std::nullopt;

    Image img;
    img.width = w;
    img.height = h;
    img.data.resize(w * h);
    memcpy(img.data.data(), buf, w * h);

    stbi_image_free(buf);
    return img;
}
