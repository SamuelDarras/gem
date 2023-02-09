#pragma once

#include <istream>
#include <vector>

#include "geometry.hpp"
#include "facet.hpp"
#include "../cut/shader.hpp"

namespace gem {
    struct Model {
        static Model fromStream(std::istream& is);
        std::vector<gem::Facet>& faces() {
            return facetsVec;
        }
        gem::Facet& face(int32_t faceIdx) {
            return facetsVec[faceIdx];
        }

        gem::vec<4, float> vert(int32_t faceId, int32_t nthVert) {
            return vertices[facetsVec[faceId].vert(nthVert)(0)-1];
        }
        gem::vec<3, float> uv(int32_t faceId, int32_t nthVert) {
            return uvs[facetsVec[faceId].vert(nthVert)(1)-1];
        }
        gem::vec<3, float> norm(int32_t faceId, int32_t nthVert) {
            return normals[facetsVec[faceId].vert(nthVert)(2)-1];
        }

        void draw(cut::TGAImage& image, cut::Shader& shader);

    private:
        std::vector<gem::Facet> facetsVec;
        std::vector<gem::vec<4, float>> vertices;
        std::vector<gem::vec<3, float>> uvs;
        std::vector<gem::vec<3, float>> normals;
    };
}