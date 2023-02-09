#include <string>
#include <vector>

#include "../gem/geometry.hpp"
#include "draw.hpp"
#include "shader.hpp"
#include "tgaimage.h"

inline float triangleArea(gem::vec<3, float> a, gem::vec<3, float> b, gem::vec<3, float> c) {
    return (b(0)-a(0)) * (c(1)-b(1)) - (c(0)-b(0)) * (b(1)-a(1));
}

inline gem::vec<3, float> barycenter(gem::vec<3, float> A, gem::vec<3, float> B, gem::vec<3, float> C, gem::vec<3, float> P) {
    float area = triangleArea(A, B, C);
    area = area > 1e-2 ? area : 1;

    float abpArea = triangleArea(B, C, P);
    float bcpArea = triangleArea(C, A, P);
    float capArea = triangleArea(A, B, P);

    return gem::vec<3, float>{abpArea/area, bcpArea/area, capArea/area};
}

namespace cut {
    void triangle(std::vector<gem::vec<3, float>> pts, float* zbuffer, cut::TGAImage& image, Shader& shader) {
        auto v0 = pts[0];
        auto v1 = pts[1];
        auto v2 = pts[2];

        // std::cout << v0 << "\n";
        // std::cout << v1 << "\n";
        // std::cout << v2 << "\n";
        // std::cout << "================\n";

        gem::vec<2, int> boundingBL{ (int) std::min(std::min(v0(0), v1(0)), v2(0)), (int) std::min(std::min(v0(1), v1(1)), v2(1))  };
        gem::vec<2, int> boundingTR{ (int) std::max(std::max(v0(0), v1(0)), v2(0)), (int) std::max(std::max(v0(1), v1(1)), v2(1)) };

        #pragma omp parallel for collapse(2)
        for (int x = boundingBL(0); x <= boundingTR(0); x++) {
            for (int y = boundingBL(1); y <= boundingTR(1); y++) {
                auto b = barycenter(v0.proj<3>(), v1.proj<3>(), v2.proj<3>(), gem::vec<3, float>(static_cast<float>(x), static_cast<float>(y), 0.0f));
                if (b(0) < 0 || b(1) < 0 || b(2) < 0) continue;

                float z = v0(2)*b(0) + v1(2)*b(1) + v2(2)*b(2);
                int idx = x + y*image.width();

                TGAColor color;
                bool d = shader.fragment(b, color);

                if (!d && idx >= 0 && image.width() * image.height() > idx && zbuffer[idx] < z) {
                    zbuffer[idx] = z;

                    image.set(x, y, color);
                }

            }

        }
    }
}