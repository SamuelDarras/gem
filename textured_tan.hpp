
#pragma once

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "mount/camera.hpp"

struct TexturedTan : cut::Shader {
    gem::Model*    model;
    mount::Camera* camera;

    cut::TGAImage* uniform_texture;
    cut::TGAImage* uniform_normal_map;
    cut::TGAImage* uniform_spec_map;

    gem::mat<3, 3> varying_normal;
    gem::mat<3, 3> varying_uv;
    gem::mat<3, 3> ndc_tri;

    gem::vec<3, float> vertex(int32_t faceId, int32_t nthVert) {
        varying_normal.setCol(nthVert, model->norm(faceId, nthVert));
        varying_uv.setCol(nthVert, model->uv(faceId, nthVert));
        

        auto v = camera->projection * camera->model_view * (model->vert(faceId, nthVert));
        
        ndc_tri.setCol(nthVert, (v / v(3)).proj<3>());

        auto screen_v = camera->viewport * v;
        return screen_v.proj<3>() / screen_v(3);
    }
    bool fragment(gem::vec<3, float> bar, cut::TGAColor& color) {
        auto uv = varying_uv * bar;
        auto b_norm = (varying_normal * bar).normalize();
        
        gem::vec<3, float> ligt_dir(1.0f, 1.0f, 1.0f);
        ligt_dir = ligt_dir.normalize();
        
        float spec;
        {
            auto spec_color = uniform_texture->get(uv(0)*uniform_texture->width(), (1.0f-uv(1))*uniform_texture->height());
            auto model_spec = static_cast<float>(spec_color.bgra[0]+spec_color.bgra[1]+spec_color.bgra[2])/255.0f;

            auto r = (b_norm * (b_norm * ligt_dir * 2.0f) - ligt_dir).normalize();
            spec = pow(std::max(r(2), 0.0f), model_spec);
        }

        gem::vec<> norm;
        {
            gem::vec<3, float> norm_perturbation;
            auto norm_color = uniform_normal_map->get(uv(0)*uniform_normal_map->width(), (1.0f-uv(1))*uniform_normal_map->height());
            norm_perturbation = gem::vec<3, float>(
                static_cast<float>(norm_color.bgra[2]),
                static_cast<float>(norm_color.bgra[1]),
                static_cast<float>(norm_color.bgra[0])
            );
            norm_perturbation = norm_perturbation/127.5f - 1.0f;

            gem::mat<3, 3>A;
            A(0) = ndc_tri.col(1) - ndc_tri.col(0);
            A(1) = ndc_tri.col(2) - ndc_tri.col(0);
            A(2) = b_norm;

            auto i = A.solve(gem::vec<>(
                    varying_uv(0, 1) - varying_normal(0, 0),
                    varying_uv(0, 2) - varying_normal(0, 0),
                    0.0f
                )
            ).normalize();
            auto j = A.solve(gem::vec<>(
                    varying_uv(1, 1) - varying_normal(1, 0),
                    varying_uv(1, 2) - varying_normal(1, 0),
                    0.0f
                )
            ).normalize();

            gem::mat<3, 3> B;
            B.setCol(0, i);
            B.setCol(1, j);
            B.setCol(2, b_norm);

            norm = (B*norm_perturbation).normalize();
        }

        // float light = std::max(0.0f, norm * ligt_dir.normalize());
        // auto diff = uniform_texture->get(uv(0)*uniform_texture->width(), (1.0f-uv(1))*uniform_texture->height());

        float light = std::max(0.0f, norm*ligt_dir);

        // color = cut::TGAColor(diff.bgra[2]*(light + 0.4f*spec), diff.bgra[1]*(light + 0.4f*spec), diff.bgra[0] * (light + 0.4f*spec));
        // color = cut::TGAColor(r(0)*255, r(1)*255, r(2)*255);
        norm = (norm + 1.0f) / 2.0f;
        color = cut::TGAColor(norm(0)*255, norm(1)*255, norm(2)*255);
        if (std::fmod(uv(0), 0.03f) <= 0.001f) color = cut::TGAColor(light*255, 0, 0);
        else if (std::fmod(uv(1), 0.03f) <= 0.001f) color = cut::TGAColor(0, 0, light*255);
        // else color = cut::TGAColor(light*255, light*255, light*255);
        // color = cut::TGAColor(spec*255, spec*255, spec*255);
        // color = cut::TGAColor(model_spec*255, model_spec*255, model_spec*255);
        return false;
    }

};