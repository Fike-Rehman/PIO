#pragma once

// Match these values to the wiring and panel before uploading.
namespace DisplayConfig {
constexpr int sdaPin = 21;
constexpr int sclPin = 22;
constexpr int width = 128;
constexpr int height = 64; // Change to 32 for a 128x32 panel.
static_assert(height == 32 || height == 64, "Supported OLED heights: 32 or 64");
}
