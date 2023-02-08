#pragma once

#include <cstdint>
#include "../gem/geometry.hpp"
#include "../gem/facet.hpp"
#include "tgaimage.h"

namespace cut {
    struct Shader {
        virtual gem::vec<3, float> vertex(int32_t faceId, int32_t nthVert) = 0;
        virtual bool fragment(gem::vec<3, float> bar, TGAColor& color)                 = 0;
    };
}