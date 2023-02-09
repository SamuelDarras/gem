
#pragma once

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "mount/camera.hpp"

struct Gouraud : cut::Shader {
    gem::Model*    model;
    mount::Camera* camera;

    cut::TGAImage* uniform_texture;
    cut::TGAImage* uniform_normal_map;
    cut::TGAImage* uniform_spec_map;

    gem::mat<3, 3> varying_normal;
    gem::mat<3, 3> varying_uv;

    gem::vec<3, float> vertex(int32_t faceId, int32_t nthVert) {
        varying_normal.setCol(nthVert, model->norm(faceId, nthVert));
        
        auto v = camera->viewport * camera->projection * camera->model_view * (model->vert(faceId, nthVert));
        auto screen_v = v.proj<3>() / v(3);
        return screen_v;
    }
    bool fragment(gem::vec<3, float> bar, cut::TGAColor& color) {
        gem::vec<3, float> ligt_dir(1.0f, 1.0f, 1.0f);
        ligt_dir = ligt_dir.normalize();

        auto norm = (varying_normal * bar).normalize();
        float light = std::max(0.0f, norm * ligt_dir.normalize());
        color = cut::TGAColor(light*255, light*255, light*255);
        return false;
    }

};