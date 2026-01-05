#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>


struct Image {
    // TODO: More formats
    enum class Format { I };

    uint32_t width, height;
    Format format;
    std::vector<uint8_t> data;

    static std::optional<Image> FromFile(const std::filesystem::path& path, Format format);
};
