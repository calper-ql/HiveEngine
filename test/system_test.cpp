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
#include <HiveEngine/Utilities.h>

#include <iostream>
#include <random>

class RandomEntity {
public:
    size_t id;
    HiveEngine::Context *cntx;

    RandomEntity(HiveEngine::Context *cntx, std::default_random_engine &re) {
        this->cntx = cntx;
        std::uniform_real_distribution<> real_d(-1.4, 1.4);

        this->id = cntx->new_entity();

        cntx->entity_significance.set(id, 0);
        cntx->entity_position.set(id, {real_d(re) / 10.0, real_d(re), real_d(re)});
        cntx->entity_velocity.set(id, {0, 0, 0});
        cntx->entity_mass.set(id, 0.01);
        cntx->entity_radius.set(id, 0.01);
    }
};

int main(int argc, char *argv[]) {
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
              << "." << HiveEngine::get_minor_version() << std::endl;

    std::default_random_engine re(41);

    HiveEngine::System sys;
    HiveEngine::Context c1(&sys);
    HiveEngine::Context c2(&sys);

    c1.position = {-5.0, 0.0, 0.0};
    c2.position = {10.0, 0.0, 0.0};
    c1.radius = 30.0;
    c2.radius = 5.0;

    for (int j = 0; j < 5000; ++j) {
        RandomEntity(&c1, re);
    }

    for (int j = 0; j < 5000; ++j) {
        RandomEntity(&c2, re);
    }

    auto id = c1.new_entity();
    c1.entity_significance.set(id, 1);
    c1.entity_position.set(id, {0.0, 4000.0, 0.0});
    c1.entity_velocity.set(id, {0.0, -1100.00, 0.0});
    c1.entity_mass.set(id, 0.1);
    c1.entity_radius.set(id, 0.1);
    c1.significant_entities.add(id);

    id = c1.new_entity();
    c1.entity_significance.set(id, 1);
    c1.entity_position.set(id, {0.0, 4000.0, 1.0});
    c1.entity_velocity.set(id, {0.0, -1100.00, 0.0});
    c1.entity_mass.set(id, 0.1);
    c1.entity_radius.set(id, 0.1);
    c1.significant_entities.add(id);

    id = c1.new_entity();
    c1.entity_significance.set(id, 1);
    c1.entity_position.set(id, {0.0, 4000.0, -1.0});
    c1.entity_velocity.set(id, {0.0, -100.00, 0.0});
    c1.entity_mass.set(id, 0.1);
    c1.entity_radius.set(id, 0.1);
    c1.significant_entities.add(id);

    id = c2.new_entity();
    c2.entity_significance.set(id, 1);
    c2.entity_position.set(id, {2.0, -1.0, 0.0});
    c2.entity_velocity.set(id, {-0.5, 0.0, 0.0});
    c2.entity_mass.set(id, 0.1);
    c2.entity_radius.set(id, 0.3);
    c2.significant_entities.add(id);

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -3.30});

    HiveEngine::Renderer::Context renderer_context(true);
    renderer_context.init_window({1400, 1000}, true);
    renderer_context.load_shaders("../shaders");

    HiveEngine::Renderer::AABBDrawing aabb_drawing_1(&renderer_context);
    HiveEngine::Renderer::AABBDrawing aabb_drawing_2(&renderer_context);

    HiveEngine::Renderer::EntityDrawing entity_drawing_1(&renderer_context);
    HiveEngine::Renderer::EntityDrawing entity_drawing_2(&renderer_context);

    camera.traverse_modifier = 0.02;

    aabb_drawing_1.camera = &camera;
    aabb_drawing_2.camera = &camera;

    entity_drawing_1.camera = &camera;
    entity_drawing_2.camera = &camera;


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

        entity_drawing_1.pos = c1.entity_position.get_data();
        entity_drawing_1.radius = c1.entity_radius.get_data();
        entity_drawing_1.significance = c1.entity_significance.get_data();
        entity_drawing_1.offset = c1.position;

        entity_drawing_2.pos = c2.entity_position.get_data();
        entity_drawing_2.radius = c2.entity_radius.get_data();
        entity_drawing_2.significance = c2.entity_significance.get_data();
        entity_drawing_2.offset = c2.position;

        camera.get_user_movement(renderer_context.get_window());
        camera.get_user_input(renderer_context.get_window());
    } while (renderer_context.draw());

    return EXIT_SUCCESS;
}