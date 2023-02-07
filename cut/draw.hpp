#pragma once

#include <vector>

#include "../gem/geometry.hpp"
#include "shader.hpp"
#include "tgaimage.h"

namespace cut {
    void triangle(std::vector<gem::vec<4, float>> pts, float* zbuffer, cut::TGAImage& image, Shader& shader);
}