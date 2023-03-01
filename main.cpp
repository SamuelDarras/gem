#include <fstream>
#include <iostream>
#include <chrono>

#include "cut/shader.hpp"
#include "cut/tgaimage.h"
#include "gem/geometry.hpp"
#include "gem/model.hpp"
#include "mount/camera.hpp"

#include "textured.hpp"

int main(int argc, char* argv[]) {
    auto obj_path     = std::string("../obj/diablo3_pose/diablo3_pose.obj");
    auto texture_path = std::string("../obj/diablo3_pose/diablo3_pose_diffuse.tga");
    auto nm_path      = std::string("../obj/diablo3_pose/diablo3_pose_nm.tga");
    auto spec_path    = std::string("../obj/diablo3_pose/diablo3_pose_spec.tga");

    if (argc == 2) {
        obj_path = argv[1];
        auto name_pos = obj_path.find(".obj");
        texture_path = std::string(obj_path).replace(name_pos, 12, "_diffuse.tga");
        nm_path      = std::string(obj_path).replace(name_pos, 7,  "_nm.tga");
        spec_path    = std::string(obj_path).replace(name_pos, 9,  "_spec.tga");
    }

    std::ifstream objs(obj_path);
    auto model = gem::Model::fromStream(objs);
    objs.close();
    
    constexpr int width  = 4096;
    constexpr int height = 4096;

    gem::vec<3, float> light_dir(0.7f, 0.6f, 0.5f);

    mount::Camera camera(
        gem::vec<>(0.4f, 0.3f, 1.0f),
        gem::vec<>(0.0f, 1.0f, 0.0f),
        gem::vec<>(0.0f, 0.0f, 0.0f),
        gem::vec<5>(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height), 255.0f)
    );

    cut::TGAImage texture;
    texture.read_tga_file(texture_path);

    cut::TGAImage normal_map;
    normal_map.read_tga_file(nm_path);
    
    cut::TGAImage specular_map;
    specular_map.read_tga_file(spec_path);

    cut::TGAImage image(width, height, cut::TGAImage::RGB);

    Textured shader;
    shader.model  = &model;
    shader.camera = &camera;
    shader.light_dir = light_dir.normalize();
    shader.uniform_texture    = &texture;
    shader.uniform_normal_map = &normal_map;
    shader.uniform_spec_map   = &specular_map;
    model.draw(image, shader);
    image.write_tga_file("./output.tga");
}