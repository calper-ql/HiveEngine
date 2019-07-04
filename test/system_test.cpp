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

class RandomEntity {
public:
    size_t id;
    HiveEngine::Context *cntx;

    RandomEntity(HiveEngine::Context *cntx, std::default_random_engine &re) {
        this->cntx = cntx;
        std::uniform_real_distribution<> real_d(-1.4, 1.4);
        std::uniform_real_distribution<> vel_d(-0.0014, 0.0014);

        this->id = cntx->new_entity();

        cntx->entity_significance.set(id, 0);
        cntx->entity_position.set(id, {real_d(re) / 10.0, real_d(re), real_d(re)});
        cntx->entity_velocity.set(id, {vel_d(re) / 10.0, vel_d(re), vel_d(re)});
        cntx->entity_mass.set(id, 0.001);
        cntx->entity_radius.set(id, 0.001);
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

int main(int argc, char *argv[]) {
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
              << "." << HiveEngine::get_minor_version() << std::endl;

    std::default_random_engine re(41);

    HiveEngine::System sys;
    HiveEngine::Context c1(&sys);
    HiveEngine::Context c2(&sys);

    c1.set_position({-5.0, -0.4, -0.1});
    c2.set_position({0.0, 0.0, 0.0});
    c1.set_radius(30.0);
    c2.set_radius(50.0);

    for (int j = 0; j < 10000; ++j) {
        RandomEntity(&c1, re);
    }

    for (int j = 0; j < 50; ++j) {
        RandomEntity(&c2, re);
    }

    Assimp::Importer importer;
    aiScene* scene = (aiScene*)importer.ReadFile( "../data/kinetic_missile/kinetic_missile.dae", aiProcess_Triangulate);

    if( !scene){
        spdlog::error("Couldn't load scene");
        HiveEngine::process_error();
    }

    print_assimp_node(scene->mRootNode, 0);

    auto id = c1.new_entity();
    c1.entity_significance.set(id, 1);
    c1.entity_position.set(id, {0.0, 4000.0, 0.0});
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
    camera.set_position({0.0, 0.0, -10.30});

    HiveEngine::Renderer::Context renderer_context(true);
    renderer_context.init_window({1400, 1000}, true);
    renderer_context.load_shaders("../shaders");

    HiveEngine::Renderer::OptixDrawing optix_drawing(&renderer_context, "../");
    optix_drawing.add_perspective(&camera, {100, 100}, true, "color_raygen", "miss", "exception");

    size_t scene_id = optix_drawing.add_scene(scene);
    c1.load_ai_node(scene, scene->mRootNode, scene_id);
    optix_drawing.root_node->addChild(optix_drawing.configure_context(&c1));

    HiveEngine::Renderer::AABBDrawing aabb_drawing_1(&renderer_context);
    HiveEngine::Renderer::AABBDrawing aabb_drawing_2(&renderer_context);

    camera.traverse_modifier = 0.02;

    aabb_drawing_1.camera = &camera;
    aabb_drawing_2.camera = &camera;

    do {
        sys.step(60);
        sys.check_colliding_contexts(10);
        sys.process_collision_map(60);

        aabb_drawing_1.bbox = c1.bounding_boxes;
        aabb_drawing_1.significance = c1.entity_significance.get_data();
        aabb_drawing_1.offset = c1.get_position();

        aabb_drawing_2.bbox = c2.bounding_boxes;
        aabb_drawing_2.significance = c2.entity_significance.get_data();
        aabb_drawing_2.offset = c2.get_position();

        camera.get_user_movement(renderer_context.get_window());
        camera.get_user_input(renderer_context.get_window());
    } while (renderer_context.draw());

    return EXIT_SUCCESS;
}