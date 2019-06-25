//
// Created by calpe on 6/22/2019.
//

#ifndef HIVEENGINE_CONTEXT_H
#define HIVEENGINE_CONTEXT_H

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/Buffer.hpp>

#include <glm/glm.hpp>
#include <map>
#include <string>
#include <utility>

namespace HiveEngine {
    class Context;
    class System;

    class Context {
    public:
        System* system;
        size_t id = 0;
        glm::dvec3 position = {0.0, 0.0, 0.0};
        double radius = 0;

        Buffer<double> entity_radius;
        Buffer<double> entity_mass;
        Buffer<glm::dvec3> entity_position;
        Buffer<glm::dvec3> entity_velocity;
        Buffer<int> entity_significance;

        std::vector<HiveEngine::DAABB> bounding_boxes;

        Buffer<size_t> significant_entities;

        explicit Context(System* system);
        ~Context();

        void calculate_positions(unsigned ticks_per_second);
        void calculate_bounding_boxes(unsigned ticks_per_second);

        size_t new_entity();
        void remove_entity(size_t id);

        void step(unsigned ticks_per_second);

        std::vector<HiveEngine::DAABB_LINES> generate_bbox_lines();
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
