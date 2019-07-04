//
// Created by calpe on 6/22/2019.
//

#ifndef HIVEENGINE_CONTEXT_H
#define HIVEENGINE_CONTEXT_H

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/Buffer.hpp>
#include <HiveEngine/Utilities.h>

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <utility>
#include <assimp/scene.h>

namespace HiveEngine {
    class Context;
    class System;

    struct ContextRepresentation {
        int scene_id = -1;
        unsigned int mesh_id = 0;
    };

    class Context {
    private:
        glm::dvec3 position = {0.0, 0.0, 0.0};
        glm::mat3 rotation = glm::mat3(1.0f);
        double radius = 0;

    public:
        System* system = nullptr;
        Context* parent = nullptr;
        size_t id = 0;

        std::string name = "";
        std::vector<ContextRepresentation> representations;

        Buffer<double> entity_radius;
        Buffer<double> entity_mass;
        Buffer<glm::dvec3> entity_position;
        Buffer<glm::dvec3> entity_velocity;
        Buffer<glm::mat3> entity_rotation;
        Buffer<int> entity_significance;
        Buffer<Context*> contexts;

        std::vector<DAABB> bounding_boxes;

        Buffer<size_t> significant_entities;

        explicit Context(System* system);
        explicit Context(Context* parent);
        ~Context();

        void calculate_positions(unsigned ticks_per_second);
        void calculate_bounding_boxes(unsigned ticks_per_second);

        size_t new_entity();
        void remove_entity(size_t id);

        void step(unsigned ticks_per_second);

        double load_ai_node(aiScene* scene, aiNode* node, int scene_id);

        void set_position(glm::dvec3 new_pos);
        void set_rotation(glm::mat3 new_rot);
        void set_radius(double radius);
        void set_significance(int sig);

        glm::dvec3 get_position();
        glm::mat3 get_rotation();
        double get_radius();

    };

    class System {
    public:
        Buffer<Context*> contexts;

        size_t context_collision_check_iterator = 0;

        System();
        ~System();

        std::vector<int> collision_map;
        size_t a_id = 0;
        size_t b_id = 0;
        size_t map_iter = 0;
        void check_colliding_contexts(size_t check_range);

        void process_collision_map(unsigned ticks_per_second);
        void process_collision(size_t a, size_t b, unsigned ticks_per_second);

        void step(unsigned ticks_per_second);


    };
}


#endif //HIVEENGINE_CONTEXT_H
