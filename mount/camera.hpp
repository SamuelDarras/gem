#pragma once

#include "../gem/geometry.hpp"

namespace mount {
    struct Camera {
        Camera(gem::vec<3, float> _position, gem::vec<3, float> _up, gem::vec<3, float> _center, gem::vec<5> _vp) 
        : position(_position), up(_up.normalize()), center(_center), vp(_vp) {
            { // Look at
                auto z = (position-center).normalize();
                auto x = (up ^ z).normalize();
                auto y = (z ^ x).normalize();

                model = gem::mat<4, 4>::identity();
                view  = gem::mat<4, 4>::identity();
                model.setRow(0, x.proj<4>(0.0f));
                model.setRow(1, y.proj<4>(0.0f));
                model.setRow(2, z.proj<4>(0.0f));

                view.setCol(3, (position*(-1)).proj<4>(1.0f));

                model_view = model*view;
            }

            { // Projection
                projection = gem::mat<4, 4>::identity();
                projection(3, 2) = -1.0/(position - center).norm();
            }

            { // Viewport
                viewport = gem::mat<4, 4>::identity();
                viewport(0, 3) = vp(0) + vp(2)/2.0f; // Width
                viewport(1, 3) = vp(1) + vp(3)/2.0f; // Height
                viewport(2, 3) = vp(4)/2.0f;         // Depth

                viewport(0, 0) = vp(2)/2.0f;
                viewport(1, 1) = vp(3)/2.0f;
                viewport(2, 2) = vp(4)/2.0f;
            }
        }

        gem::vec<3, float> position;
        gem::vec<3, float> up;
        gem::vec<3, float> center;

        gem::vec<5, float> vp;

        gem::mat<4, 4> model;
        gem::mat<4, 4> view;
        gem::mat<4, 4> model_view;

        gem::mat<4, 4> projection;
        gem::mat<4, 4> viewport;
    };
}