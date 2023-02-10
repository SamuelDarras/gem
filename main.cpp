#include <fstream>
#include <iostream>
#include <chrono>

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "gouraud.hpp"
#include "textured.hpp"
#include "textured_tan.hpp"
#include "mount/camera.hpp"

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
    // std::ifstream objs("../obj/diablo3_pose/diablo3_pose.obj");
    auto model = gem::Model::fromStream(objs);
    objs.close();
    
    constexpr int width  = 2048*4;
    constexpr int height = 2048*4;

    mount::Camera camera(
        gem::vec<>(0.7f, 0.4f, 1.0f),
        gem::vec<>(0.0f, 1.0f, 0.0f),
        gem::vec<>(0.0f, 0.0f, 0.0f),
        gem::vec<5>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 1.0f)
    );

    cut::TGAImage texture;
    texture.read_tga_file("../obj/african_head/african_head_diffuse.tga");

    cut::TGAImage normal_map;
    // normal_map.read_tga_file("../obj/african_head/african_head_nm.tga");
    normal_map.read_tga_file("../obj/african_head/african_head_nm_tangent.tga");
    
    cut::TGAImage specular_map;
    specular_map.read_tga_file("../obj/african_head/african_head_spec.tga");

    cut::TGAImage image(width, height, cut::TGAImage::RGB);

    TexturedTan shader;
    // Textured shader;
    shader.model  = &model;
    shader.camera = &camera;
    shader.uniform_texture    = &texture;
    shader.uniform_normal_map = &normal_map;
    shader.uniform_spec_map   = &specular_map;
    model.draw(image, shader);

    image.write_tga_file("./output.tga");
}