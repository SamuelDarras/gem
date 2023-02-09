
#pragma once

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "mount/camera.hpp"

struct Textured : cut::Shader {
    gem::Model*    model;
    mount::Camera* camera;

    cut::TGAImage* uniform_texture;
    cut::TGAImage* uniform_normal_map;
    cut::TGAImage* uniform_spec_map;

    gem::mat<3, 3> varying_normal;
    gem::mat<3, 3> varying_uv;

    gem::vec<3, float> vertex(int32_t faceId, int32_t nthVert) {
        varying_normal.setCol(nthVert, model->norm(faceId, nthVert));
        varying_uv.setCol(nthVert, model->uv(faceId, nthVert));
        
        auto v = camera->viewport * camera->projection * camera->model_view * (model->vert(faceId, nthVert));
        auto screen_v = v.proj<3>() / v(3);
        return screen_v;
    }
    bool fragment(gem::vec<3, float> bar, cut::TGAColor& color) {
        auto uv = varying_uv * bar;
        gem::vec<3, float> ligt_dir(1.0f, 0.0f, 1.0f);
        ligt_dir = ligt_dir.normalize();

        auto norm_color = uniform_normal_map->get(uv(0)*uniform_normal_map->width(), (1.0f-uv(1))*uniform_normal_map->height());
        gem::vec<3, float> norm(
            static_cast<float>(norm_color.bgra[2]),
            static_cast<float>(norm_color.bgra[1]),
            static_cast<float>(norm_color.bgra[0])
        );
        norm = norm/128.0f - 1.0f;
        
        float light = std::max(0.0f, norm * ligt_dir.normalize());

        auto diff = uniform_texture->get(uv(0)*uniform_texture->width(), (1.0f-uv(1))*uniform_texture->height());
        auto spec_color = uniform_texture->get(uv(0)*uniform_texture->width(), (1.0f-uv(1))*uniform_texture->height());
        auto model_spec = static_cast<float>(spec_color.bgra[0]+spec_color.bgra[1]+spec_color.bgra[2])/255.0f;

        auto r = (norm * (norm * ligt_dir * 2.0f) - ligt_dir).normalize();
        float spec = pow(std::max(r(2), 0.0f), model_spec*5);
        // float spec = pow(std::max(r(2), 0.0f), 20);

        color = cut::TGAColor(diff.bgra[2]*(light + 0.6f*spec), diff.bgra[1]*(light + 0.6f*spec), diff.bgra[0] * (light + 0.6f*spec));
        // color = cut::TGAColor(r(0)*255, r(1)*255, r(2)*255);
        // color = cut::TGAColor(diff.bgra[2] * spec*255, diff.bgra[1] * spec*255, diff.bgra[0] * spec*255);
        // color = cut::TGAColor(spec*255, spec*255, spec*255);
        // color = cut::TGAColor(model_spec*255, model_spec*255, model_spec*255);
        return false;
    }

};