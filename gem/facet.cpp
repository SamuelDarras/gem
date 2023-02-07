#include "facet.hpp"

#include <cstdint>

gem::Facet gem::Facet::fromStream(std::istream& is) {
    Facet r;

    char _;
    int32_t vIdx, tIdx, nIdx;
    while (is >> vIdx >> _ >> tIdx >> _ >> nIdx) {
        r.addVertexIndex( vIdx);
        r.addTextureIndex(tIdx);
        r.addNormalIndex( nIdx);
    }

    return r;
}