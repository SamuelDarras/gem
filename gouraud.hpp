
#pragma once

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/model.hpp"

struct Gouraud : cut::Shader {
    gem::Model& m;

    Gouraud(gem::Model& _m) : m(_m) {};

    gem::mat<4, 3> varying_normal;

    gem::vec<4, float> vertex(int32_t faceId, int32_t nthVert) {
        auto n = m.norm(faceId, nthVert).proj<4>();
        varying_normal.setCol(nthVert, n);
        return (m.vert(faceId, nthVert)+1)*255;
    }
    bool fragment(gem::vec<3, float> bar, cut::TGAColor& color) {
        gem::vec<4, float> ligt_dir(1.0f, 0.0f, 0.0f, 0.0f);
        ligt_dir = ligt_dir.normalize();
        // auto norm = (varying_normal*bar).normalize();
        auto norm = ((varying_normal*bar).normalize()+1)/2;
        auto light = std::max(0.0f, norm*ligt_dir);
        color = cut::TGAColor(norm(0)*255, norm(1)*255, norm(2)*255);
        // color = cut::TGAColor(light*255, light*255, light*255);
        return false;
    }

};