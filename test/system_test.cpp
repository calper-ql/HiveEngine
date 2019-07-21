//
// Created by calpe on 6/23/2019.
//


#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Context.h>
#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Renderer/AABBDrawing.h>
#include <HiveEngine/Renderer/EntityDrawing.h>
#include <HiveEngine/Renderer/OptixDrawing.h>
#include <HiveEngine/Utilities.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <random>

class RandomEntity : public HiveEngine::Node {
public:
    RandomEntity(HiveEngine::Context *cntx, std::default_random_engine &re, Node* parent=nullptr) : Node(cntx, parent) {
        std::uniform_real_distribution<> real_d(-0.5, 0.5);
        if(parent) real_d = std::uniform_real_distribution<>(-0.01, 0.01);
        std::uniform_real_distribution<> vel_d(-0.0014, 0.0014);
        if(parent) vel_d = std::uniform_real_distribution<>(-0.0000001, 0.0000001);

        std::uniform_real_distribution<> xrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);
        std::uniform_real_distribution<> yrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);
        std::uniform_real_distribution<> zrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);

        physical_data()->angular_velocity = HiveEngine::generate_rotation_matrix('x', xrot(re));
        physical_data()->angular_velocity *= HiveEngine::generate_rotation_matrix('y', yrot(re));
        physical_data()->angular_velocity *= HiveEngine::generate_rotation_matrix('z', zrot(re));

        physical_data()->position = {real_d(re) / 10.0, real_d(re), real_d(re)};
        physical_data()->velocity = {vel_d(re) / 10.0, vel_d(re), vel_d(re)};

        physical_data()->mass = 0.001;
        physical_data()->radius = 0.01 / (float(get_level()+1));

        std::uniform_real_distribution<> child_possibility(0.0, 1.0);
        while(child_possibility(re) < 0.1 && get_level() < 4) {
            new RandomEntity(cntx, re, this);
        }
    }
};

void print_assimp_node(aiNode* node, int depth) {
    std::string dstr = "";
    for (int i = 0; i < depth; ++i) dstr += "-";
    std::cout << dstr << node->mName.C_Str() << " " << node->mNumMeshes << std::endl;
    for (int j = 0; j < node->mNumChildren; ++j) {
        print_assimp_node(node->mChildren[j], depth+1);
    }
}

void node_to_lines(HiveEngine::Node* node, std::vector<HiveEngine::Line> &lines){
    HiveEngine::Line line;
    line.a.position = node->physical_data()->global_position;
    if(node->get_level() == 0) {
        line.a.color = {1.0, 1.0, 1.0, 1.0};
    } else if (node->get_level() == 1) {
        line.a.color = {1.0, 0.0, 0.0, 1.0};
    } else if (node->get_level() == 2) {
        line.a.color = {0.0, 1.0, 0.0, 1.0};
    } else if (node->get_level() == 3) {
        line.a.color = {0.0, 0.0, 1.0, 1.0};
    } else {
        line.a.color = {1.0, 0.0, 1.0, 1.0};
    }
    line.b.position = node->physical_data()->global_position + glm::dvec3(node->physical_data()->global_rotation * glm::vec3(0.0, 0.0, node->physical_data()->radius));
    line.b.color = {1.0, 1.0, 1.0, 1.0};
    lines.push_back(line);

    for (int i = 0; i < node->children.size(); ++i) {
        if(node->children.get_state(i)) node_to_lines(node->children.get(i), lines);
    }
}

int main(int argc, char *argv[]) {
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
              << "." << HiveEngine::get_minor_version() << std::endl;

    std::default_random_engine re(41);

    HiveEngine::System sys;
    HiveEngine::Context c1(&sys);
    HiveEngine::Context c2(&sys);

    c1.position = {-5.0, -0.4, -0.1};
    c2.position = {0.0, 0.0, 0.0};
    c1.radius = 30.0;
    c2.radius = 50.0;

    for (int j = 0; j < 10000; ++j) {
        //new RandomEntity(&c1, re);
    }

    for (int j = 0; j < 50; ++j) {
        new RandomEntity(&c2, re);
    }

    Assimp::Importer importer;
    aiScene* scene = (aiScene*)importer.ReadFile( "../data/kinetic_missile/kinetic_missile.fbx", aiProcess_CalcTangentSpace       |
                                                                                                 aiProcess_Triangulate            |
                                                                                                 aiProcess_JoinIdenticalVertices  |
                                                                                                 aiProcess_SortByPType);

    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode){
        spdlog::error("Couldn't load scene");
        HiveEngine::process_error();
    }

    print_assimp_node(scene->mRootNode, 0);

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -1.30});

    HiveEngine::Renderer::Context renderer_context(true);
    renderer_context.init_window({1400, 1000}, true);
    renderer_context.load_shaders("../shaders");

    HiveEngine::Renderer::OptixDrawing optix_drawing(&renderer_context, "../");
    optix_drawing.add_perspective(&camera, {100, 100}, true, "color_raygen", "miss", "exception");

    size_t scene_id = optix_drawing.add_scene(scene);
    auto missile = c1.load_ai_node(scene, scene->mRootNode, 0);

    //missile->physical_data()->velocity = {0.1, 0.0, 0.0};
    missile->physical_data()->angular_velocity = HiveEngine::generate_rotation_matrix('z', HiveEngine::PI/10.0);

    HiveEngine::Renderer::AABBDrawing aabb_drawing_1(&renderer_context);
    HiveEngine::Renderer::AABBDrawing aabb_drawing_2(&renderer_context);

    HiveEngine::Renderer::LineDrawing line_drawing(&renderer_context, 1);
    HiveEngine::Line x_axis;
    x_axis.a.position = {0.0, 0.0, 0.0};
    x_axis.a.color = {1.0, 0.0, 0.0, 1.0};
    x_axis.b.position = {0.1, 0.0, 0.0};
    x_axis.b.color = x_axis.a.color;

    HiveEngine::Line y_axis;
    y_axis.a.position = {0.0, 0.0, 0.0};
    y_axis.a.color = {0.0, 1.0, 0.0, 1.0};
    y_axis.b.position = {0.0, 0.1, 0.0};
    y_axis.b.color = y_axis.a.color;

    HiveEngine::Line z_axis;
    z_axis.a.position = {0.0, 0.0, 0.0};
    z_axis.a.color = {0.0, 0.0, 1.0, 1.0};
    z_axis.b.position = {0.0, 0.0, 0.1};
    z_axis.b.color = z_axis.a.color;

    line_drawing.add_line(x_axis);
    line_drawing.add_line(y_axis);
    line_drawing.add_line(z_axis);
    line_drawing.camera = &camera;

    camera.traverse_modifier = 0.07;

    aabb_drawing_1.camera = &camera;
    aabb_drawing_2.camera = &camera;

    c1.representation = optix_drawing.create_context_representation();

    HiveEngine::Renderer::LineDrawing node_lines(&renderer_context, 1);
    node_lines.camera = &camera;

    std::vector<HiveEngine::Renderer::LineDescription> to_be_removed;

    missile->print();

    do {
        node_lines.offset = c1.position;
        for (int i = 0; i < to_be_removed.size(); ++i) {
            node_lines.remove_line(to_be_removed[i]);
        }
        to_be_removed.clear();

        c1.calculate_next_step(60);
        c2.calculate_next_step(60);
        c1.calculate_bounding_boxes();
        c2.calculate_bounding_boxes();
        c1.induce_next_step();
        c2.induce_next_step();

        c1.update_representation();

        std::vector<HiveEngine::Line> lines;
        node_to_lines(missile, lines);

        for (int j = 0; j < lines.size(); ++j) {
            to_be_removed.push_back(node_lines.add_line(lines[j]));
        }

        //aabb_drawing_1.bbox = c1.node_bounding_box.get_data();
        //aabb_drawing_1.significance = c1.node_level.get_data();
        //aabb_drawing_1.offset = c1.position;

        aabb_drawing_2.bbox = c2.node_bounding_box.get_data();
        aabb_drawing_2.significance = c2.node_level.get_data();
        aabb_drawing_2.offset = c2.position;

        camera.get_user_movement(renderer_context.get_window());
        camera.get_user_input(renderer_context.get_window());

        auto camera_package = camera.get_package_no_perspective();
        line_drawing.offset = -camera_package.pos + glm::vec3(0.0, 0.0, -1.0) * camera_package.view_rot;
    } while (renderer_context.draw());

    return EXIT_SUCCESS;
}