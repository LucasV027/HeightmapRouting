#pragma once

#include <cstdint>
#include <filesystem>
#include <vector>


struct Image {
    uint32_t width, height;
    std::vector<uint8_t> data;

    static std::optional<Image> FromFile(const std::filesystem::path& path);
};
