#pragma once

#include <cstdint>
#include <istream>
#include <vector>

#include "geometry.hpp"

namespace gem {
    struct Facet {
        static Facet fromStream(std::istream& is);

        gem::vec<3, int32_t> vert(int32_t nthVert) {
            vec<3, int32_t> r;
            r(0) =  vertexIndexs[nthVert];
            r(1) = textureIndexs[nthVert];
            r(2) =  normalIndexs[nthVert];
            return r;
        }

        std::vector<gem::vec<3, int32_t>> verts() {
            std::vector<vec<3, int32_t>> r;
            for (int32_t i = 0; i < dimension(); i++) {
                r.push_back(vert(i));
            }
            return r;
        }

        int32_t dimension() {
            return vertexIndexs.size();
        }

    private:
        void  addVertexIndex(int32_t idx) {
            vertexIndexs.push_back(idx);
        };
        void addTextureIndex(int32_t idx) {
            textureIndexs.push_back(idx);
        };
        void  addNormalIndex(int32_t idx) {
            normalIndexs.push_back(idx);
        };

        std::vector<int32_t> vertexIndexs, textureIndexs, normalIndexs;
    };
}
