//
// Created by calpe on 6/23/2019.
//


#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Context.h>
#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Renderer/AABBDrawing.h>
#include <HiveEngine/Utilities.h>

#include <iostream>
#include <random>

size_t sig_count = 0;

class RandomEntity {
public:
    size_t id;
    HiveEngine::Context* cntx;
    RandomEntity(HiveEngine::Context* cntx, std::default_random_engine& re){
        this->cntx = cntx;
        std::uniform_real_distribution<> real_d(-5.5, 5.5);
        std::uniform_int_distribution<> int_d(0, 3);

        this->id = cntx->new_entity();

        double vel_div = 100.0;
        cntx->entity_position.set(id, {real_d(re), real_d(re), real_d(re)});
        cntx->entity_velocity.set(id, {real_d(re) / vel_div, real_d(re) / vel_div, real_d(re) / vel_div});

        int significance = int_d(re);
        if(significance > 0){
            sig_count++;
            if(sig_count > 10) significance = 0;
        };
        cntx->entity_significance.set(id, significance);
        if(significance > 0) cntx->significant_entities.add(this->id);

        std::uniform_real_distribution<> mass_rad_real_d(0.001, 0.04);
        double mass_radius = mass_rad_real_d(re);
        cntx->entity_mass.set(id, mass_radius);
        cntx->entity_radius.set(id, mass_radius);
    }
};

int main(int argc, char* argv[]){
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
              << "." << HiveEngine::get_minor_version() << std::endl;

    std::default_random_engine re(41);

    HiveEngine::System sys;
    HiveEngine::Context c1(&sys);
    HiveEngine::Context c2(&sys);
    //spdlog::set_level(spdlog::level::debug);

    c1.position = {0.0, 0.0, -10.0};
    c2.position = {0.0, 0.0, 10.0};
    c1.radius = 30.0;
    c2.radius = 5.0;

    for (int j = 0; j < 1000000; ++j) {
        RandomEntity(&c1, re);
    }

    sig_count = 0;

    for (int j = 0; j < 30; ++j) {
        RandomEntity(&c2, re);
    }

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -3.30});

    HiveEngine::Renderer::Context renderer_context(true);
    renderer_context.init_window({1400, 1000}, true);
    renderer_context.load_shaders("../shaders");

    HiveEngine::Renderer::AABBDrawing aabb_drawing_1(&renderer_context);
    HiveEngine::Renderer::AABBDrawing aabb_drawing_2(&renderer_context);

    camera.traverse_modifier = 0.1;

    aabb_drawing_1.camera = &camera;
    aabb_drawing_2.camera = &camera;

    do {
        sys.step(60);
        sys.check_colliding_contexts(10);
        sys.process_collision_map(60);

        aabb_drawing_1.bbox = c1.bounding_boxes;
        aabb_drawing_1.significance = c1.entity_significance.get_data();
        aabb_drawing_1.offset = c1.position;

        aabb_drawing_2.bbox = c2.bounding_boxes;
        aabb_drawing_2.significance = c2.entity_significance.get_data();
        aabb_drawing_2.offset = c2.position;

        camera.get_user_movement(renderer_context.get_window());
        camera.get_user_input(renderer_context.get_window());
    } while(renderer_context.draw());

    return EXIT_SUCCESS;
}