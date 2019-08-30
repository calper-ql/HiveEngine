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
#include <HiveEngine/Renderer/MeshDrawing.h>
#include <HiveEngine/AssetManager.h>

#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <iostream>
#include <random>

class RandomEntity : public HiveEngine::Node {
public:
    RandomEntity(HiveEngine::Context *cntx, std::default_random_engine &re, Node* parent=nullptr) : Node(cntx, parent) {
        std::uniform_real_distribution<> real_d(-0.5, 0.5);
        if(parent) real_d = std::uniform_real_distribution<>(-0.1, 0.1);
        std::uniform_real_distribution<> vel_d(-0.0014, 0.0014);
        if(parent) vel_d = std::uniform_real_distribution<>(-0.0000001, 0.0000001);

        std::uniform_real_distribution<> xrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);
        std::uniform_real_distribution<> yrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);
        std::uniform_real_distribution<> zrot(-HiveEngine::PI/10.0, HiveEngine::PI/10.0);

        physical_data()->angular_velocity = HiveEngine::generate_rotation_matrix('x', xrot(re));
        physical_data()->angular_velocity *= HiveEngine::generate_rotation_matrix('y', yrot(re));
        physical_data()->angular_velocity *= HiveEngine::generate_rotation_matrix('z', zrot(re));

        physical_data()->position = {real_d(re), real_d(re), real_d(re)};
        physical_data()->velocity = {vel_d(re), vel_d(re), vel_d(re)};

        physical_data()->mass = 0.001;
        physical_data()->radius = 0.1 / (float(get_level()+1));

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

    for (int j = 0; j < 0; ++j) {
        new RandomEntity(&c1, re);
    }

    for (int j = 0; j < 0; ++j) {
        new RandomEntity(&c2, re);
    }

    HiveEngine::AssetManager am;
    am.load_file("../data/kinetic_missile/kinetic_missile.fbx");

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -1.30});
    camera.traverse_modifier = 0.001;

    HiveEngine::Renderer::Context renderer_context;
    auto test_directive = HiveEngine::Renderer::StandardDirective(&renderer_context);

    auto mesh_drawing_handler = new HiveEngine::Renderer::MeshDrawingHandler();
    for (int i = 0; i < am.scenes.size(); ++i) {
        mesh_drawing_handler->add_scene(&test_directive, am.scenes[i].meshes, &camera);
    }

    c1.representation = mesh_drawing_handler;

    auto missile = am.scenes[0].context->root_nodes.get(0)->deep_copy(&c1);

    //HiveEngine::Renderer::AABBDrawing aabb_drawing_1(&test_directive, &camera);
    //HiveEngine::Renderer::AABBDrawing aabb_drawing_2(test_directive, &camera);

    HiveEngine::Renderer::LineDrawing line_drawing(&test_directive, &camera);

    auto grid_lines = HiveEngine::generate_grid_lines_basic(1000);
    for (auto line: grid_lines) line_drawing.add_line(line);

    c1.asset_manager = &am;

    HiveEngine::Renderer::TargetMarker tm_origin(&line_drawing);
    HiveEngine::Renderer::TargetMarker tm_com(&line_drawing);

    try {
        //renderer_context.validation_layers.clear();
        renderer_context.init_window();
        renderer_context.init_vulkan();

        camera.set_as_mouse_wheel_callback(renderer_context.get_window());

        while(!glfwWindowShouldClose(renderer_context.get_window())){

			if (glfwGetKey(renderer_context.get_window(), GLFW_KEY_J) == GLFW_PRESS) {
				HiveEngine::Force force;
				force.leverage = { 1.0, 0.0, 0.0 };
				force.force = { 0.0, 1.0, 0.0 };
				force.is_relative = false;

				missile->apply_force(force);
			}

			if (glfwGetKey(renderer_context.get_window(), GLFW_KEY_L) == GLFW_PRESS) {
				HiveEngine::Force force;
				force.leverage = { -1.0, 0.0, 0.0 };
				force.force = { 0.0, 1.0, 0.0 };
				force.is_relative = false;

				missile->apply_force(force);
			}


            c1.calculate_next_step(60);
            //2.calculate_next_step(60);
            c1.calculate_bounding_boxes();
            //c2.calculate_bounding_boxes();
            c1.induce_next_step();
            //c2.induce_next_step();

            c1.update_representation();
			std::cout << HiveEngine::dvec3_to_str(missile->physical_data()->position) << std::endl;

            tm_origin.set(missile->physical_data()->position, 0.1, {1.0, 1.0, 1.0, 1.0});
            tm_com.set(
                    missile->physical_data()->position + missile->physical_data()->center_of_mass/missile->physical_data()->total_mass,
            1.1, {1.0, 0.0, 0.0, 1.0});

			//aabb_drawing_1.daabb_buffer.copy(&c1.node_bounding_box);
			//aabb_drawing_1.significance_buffer.copy(&c1.node_level);

            camera.get_user_movement(renderer_context.get_window());
            camera.get_user_input(renderer_context.get_window());

            camera.set_perspective(camera.get_fov(), HiveEngine::Renderer::get_window_ratio(renderer_context.get_window()), 0.0001, 1000.0);
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