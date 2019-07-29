//
// Created by calpe on 6/23/2019.
//


#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Context.h>
#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/StandardDirective.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Renderer/AABBDrawing.h>
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

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -1.30});
    camera.traverse_modifier = 0.07;

    HiveEngine::Renderer::Context renderer_context;
    auto test_directive = new HiveEngine::Renderer::StandardDirective(&renderer_context);

    auto missile = c1.load_ai_node(scene, scene->mRootNode, 0);

    //missile->physical_data()->velocity = {0.1, 0.0, 0.0};
    missile->physical_data()->angular_velocity = HiveEngine::generate_rotation_matrix('z', HiveEngine::PI/10.0);

    //HiveEngine::Renderer::AABBDrawing aabb_drawing_1(test_directive, &camera);
    //HiveEngine::Renderer::AABBDrawing aabb_drawing_2(test_directive, &camera);


    HiveEngine::Renderer::LineDrawing line_drawing(test_directive, &camera);
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

    test_directive->register_drawing(&line_drawing);

    try {
        renderer_context.validation_layers.clear();
        renderer_context.init_window();
        renderer_context.init_vulkan();

        while(!glfwWindowShouldClose(renderer_context.get_window())){
            c1.calculate_next_step(60);
            c2.calculate_next_step(60);
            c1.calculate_bounding_boxes();
            c2.calculate_bounding_boxes();
            c1.induce_next_step();
            c2.induce_next_step();

            c1.update_representation();

            camera.get_user_movement(renderer_context.get_window());
            camera.get_user_input(renderer_context.get_window());

            camera.set_perspective(90, HiveEngine::Renderer::get_window_ratio(renderer_context.get_window()), 0.0001, 1000.0);
            glfwPollEvents();
            renderer_context.main_loop();
        }

        renderer_context.wait_device();
        renderer_context.cleanup();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}