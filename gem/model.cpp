#include "model.hpp"

#include "geometry.hpp"
#include "facet.hpp"
#include "../cut/draw.hpp"

#include <cstdint>
#include <limits>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

gem::Model gem::Model::fromStream(std::istream &is) {
    gem::Model r;
    
    std::string line;
    while(!is.eof()) {
        std::getline(is, line);
        std::istringstream ls(line.c_str());
        
        std::string t;
        ls >> t;

        if(!line.compare(0, 2, "f ")) {
            gem::Facet f = gem::Facet::fromStream(ls);
            r.facetsVec.push_back(f);
        } else if(!line.compare(0, 2, "v ")) {
            gem::vec<4, float> vertex;
            int8_t idx = 3;
            
            while(!ls.eof()) {
                ls >> vertex(3-idx);
                idx--;
            }
            idx++;
            if (idx) vertex(3) = 1.0;
            r.vertices.push_back(vertex);
        } else if(!line.compare(0, 3, "vn ")) {
            gem::vec<3, float> normal;
            int8_t idx = 2;
            
            while(!ls.eof()) {
                ls >> normal(2-idx);
                idx--;
            }
            r.normals.push_back(normal);
        }
    }
    return r;
}

inline void initZBuffer(int size, float* zbuffer) {
    for (int i = 0;  i < size; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }
}

void gem::Model::draw(cut::TGAImage& image, cut::Shader &shader) {
    int zbufferSize = image.width() * image.height();
    float* zbuffer = (float*) malloc(zbufferSize * sizeof(float));
    initZBuffer(zbufferSize, zbuffer);

    for (int32_t faceId = 0; faceId < (int32_t) faces().size(); faceId++) {
        std::vector<vec<3, float>> screen_coords;
        auto face = facetsVec[faceId];
        for (int32_t i = 0; i < face.dimension(); i++) {
            screen_coords.push_back(
                shader.vertex(faceId, i));
        }

        cut::triangle(
            screen_coords,
            zbuffer,
            image,
            shader
        );
    }
}