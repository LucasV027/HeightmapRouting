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

    uint8_t operator()(uint32_t x, uint32_t y) const;
    uint8_t& operator()(uint32_t x, uint32_t y);

    static std::optional<Image> FromFile(const std::filesystem::path& path, Format format);
};
