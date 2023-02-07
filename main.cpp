#include <fstream>
#include <iostream>
#include <chrono>

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "gouraud.hpp"

int main() {
    // gem::vec<3, float> v1(4.0f, 4.0f, 0.0f);
    // gem::vec<3, float> v2(0.0f, 0.0f, 1.0f);
    // gem::vec<3, float> v3(1.0f, 0.0f, 1.0f);

    // std::cout << v1 << "\n";
    // std::cout << v2 << "\n";

    // std::cout << (v1 + v2) << "\n";

    // std::cout << v1.normalize() << "\n";

    // constexpr unsigned int size = 100;
    // gem::mat<size, size> m;
    // for (unsigned int i = 0; i < m.n_cols; i++) {
    //     for (unsigned int j = 0; j < m.n_row; j++) {
    //         m(j, i) = (float)(rand()%10 - 5);
    //     }
    // }
    // gem::vec<size> b;
    // for (unsigned int i = 0; i < b.n_dim; i++) {
    //     b(i) = (float)(rand()%10 - 5);
    // }
    // auto start = std::chrono::high_resolution_clock::now();
    // auto solution = m.solve(b);
    // auto stop = std::chrono::high_resolution_clock::now();
    // auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
    // std::cout << duration.count() << " ms\n";
    // std::cout << m << "\n";
    // std::cout << solution << "\n";

    // auto m1 = mat<4, 4>::ones();
    // auto m2 = mat<4, 4>::identity();
    // m2(0, 2) = 2.0;
    // std::cout << m1 << "\n";
    // std::cout << m2 << "\n";
    // std::cout << m1-m2 << "\n";
    // std::cout << m1*m2 << "\n";
    // std::cout << m2 << "\n";
    // std::cout << v3.proj<4>() << "\n";
    // std::cout << m2 * v3.proj<4>() << "\n";
    // std::cout << m2.col(0) << "\n";
    // std::cout << m2.row(1) << "\n";

    // auto big_m1 = mat<10, 50, float>::identity();
    // auto big_m2 = mat<50, 20>::identity();
    // std::cout << ((big_m1*2.0f)+1.0f) * big_m2 << "\n";

    // auto m3 = mat<4, 2>::identity();
    // std::cout << m3 << "\n";
    // std::cout << m3.T() << "\n";

    std::ifstream objs("../obj/african_head/african_head.obj");
    auto model = gem::Model::fromStream(objs);
    objs.close();

    cut::TGAImage image(512, 512, cut::TGAImage::RGB);

    // TODO: camera !!
    Gouraud shader(model);
    model.draw(image, shader);

    // std::cout << sizeof(cut::TGAHeader) << "\n";

    image.write_tga_file("./output.tga");
}