//
// Created by calpe on 6/22/2019.
//

#include <HiveEngine/Utilities.h>
#include <iostream>
#include "HiveEngine/Context.h"


namespace HiveEngine {
    Context::Context(System* system) {
        if(system){
            this->system = system;
            this->id = system->contexts.add(this);
        }

        new_entity();
    }

    Context::~Context() {
        if(system){
            system->contexts.remove(id);
        }
    }

    void Context::calculate_bounding_boxes(unsigned ticks_per_second) {
        bounding_boxes.resize(entity_mass.size());
        for (size_t i = 0; i < entity_mass.size(); ++i) {
            DAABB bbox;
            if(entity_mass.get_state(i)){
                auto er = entity_radius.get(i);
                auto ep = entity_position.get(i);
                auto ev = entity_velocity.get(i) / (double) ticks_per_second;
                bbox.min.x = (ep.x - er) + (ev.x < 0 ? ev.x: 0);
                bbox.min.y = (ep.y - er) + (ev.y < 0 ? ev.y: 0);
                bbox.min.z = (ep.z - er) + (ev.z < 0 ? ev.z: 0);
                bbox.max.x = (ep.x + er) + (ev.x > 0 ? ev.x: 0);
                bbox.max.y = (ep.y + er) + (ev.y > 0 ? ev.y: 0);
                bbox.max.z = (ep.z + er) + (ev.z > 0 ? ev.z: 0);
            }
            bounding_boxes[i] = bbox;
        }
    }

    size_t Context::new_entity() {
        double mass = 0;
        double radius = 0;
        glm::dvec3 position = {0.0, 0.0, 0.0};
        glm::dvec3 velocity = {0.0, 0.0, 0.0};
        auto _id = entity_mass.add(mass);
        entity_radius.add(radius);
        entity_position.add(position);
        entity_velocity.add(velocity);
        entity_significance.add(0);
        return _id;
    }

    void Context::remove_entity(size_t _id) {
        entity_mass.set(id, 0.0);
        entity_radius.set(id, 0.0);
        entity_position.set(id, {0.0, 0.0, 0.0});
        entity_velocity.set(id, {0.0, 0.0, 0.0});
        entity_significance.set(_id, 0);

        entity_mass.remove(_id);
        entity_radius.remove(_id);
        entity_position.remove(_id);
        entity_velocity.remove(_id);
        entity_significance.remove(_id);
    }


    void Context::calculate_positions(unsigned ticks_per_second) {
        auto pos = entity_position.get_ptr();
        auto vel = entity_velocity.get_ptr();
        for (size_t i = 0; i < entity_position.size(); ++i) {
            pos[i] += vel[i] / (double)ticks_per_second;
        }
    }

    void Context::step(unsigned ticks_per_second) {
        calculate_positions(ticks_per_second);
        calculate_bounding_boxes(ticks_per_second);
    }

    std::vector<HiveEngine::DAABB_LINES> Context::generate_bbox_lines() {
        std::vector<HiveEngine::DAABB_LINES> lines;
        for (int i = 0; i < bounding_boxes.size(); ++i) {
            if(entity_mass.get_state(i)){
                if(entity_significance.get_all(i).first == 0){
                    lines.push_back(bounding_boxes[i].to_lines(this->position, {1.0, 1.0, 1.0, 1.0}));
                } else if(entity_significance.get_all(i).first == 1){
                    lines.push_back(bounding_boxes[i].to_lines(this->position, {1.0, 0.0, 0.0, 1.0}));
                } else if(entity_significance.get_all(i).first == 2){
                    lines.push_back(bounding_boxes[i].to_lines(this->position, {0.0, 1.0, 0.0, 1.0}));
                } else {
                    lines.push_back(bounding_boxes[i].to_lines(this->position, {0.0, 0.0, 1.0, 1.0}));
                }

            }
        }
        return lines;
    }

    void System::check_colliding_contexts(size_t check_range) {
        size_t expected_map_size = (contexts.size()*(contexts.size()+1))/2;
        if(collision_map.size() != expected_map_size){
            collision_map = std::vector<int>(expected_map_size, 0);
            spdlog::debug("System collision resizing: " + std::to_string(contexts.size()) + " --> "
            + std::to_string(expected_map_size));
            a_id = 0;
            b_id = 0;
            map_iter = 0;
        }

        for (size_t i = 0; i < check_range; ++i) {
            if(b_id >= contexts.size()){
                a_id++;
                b_id = a_id;
                if(map_iter >= collision_map.size()) {
                    a_id = 0;
                    b_id = 0;
                    map_iter = 0;
                    spdlog::debug("System collision reset!");
                }
            }

            if(contexts.get_all(a_id).second && contexts.get_all(b_id).second){
                Context* A = contexts.get_all(a_id).first;
                Context* B = contexts.get_all(b_id).first;
                if(A != nullptr && B != nullptr){
                    if(glm::distance(A->position, B->position) < A->radius + B->radius){
                        collision_map[map_iter]++;
                    } else collision_map[map_iter] = 0;
                } else collision_map[map_iter] = 0;
            } else collision_map[map_iter] = 0;


            spdlog::debug("System collision checked "
            + std::to_string(map_iter)
            + ": " + std::to_string(a_id)
            + " <=> " + std::to_string(b_id)
            + " / " + std::to_string(contexts.size()));

            b_id++;
            map_iter++;
        }
    }

    System::System() {

    }

    System::~System() {
        for (int i = 0; i < contexts.size(); ++i) {
            auto item = contexts.get_all(i);
            if(item.second) delete item.first;
        }
    }

    void System::process_collision_map(unsigned ticks_per_second) {
        size_t _a_id = 0;
        size_t _b_id = 0;
        for (size_t i = 0; i < collision_map.size(); ++i) {
            if(_b_id >= contexts.size()){
                _a_id++;
                _b_id = _a_id;
            }
            if(collision_map[i]) process_collision(_a_id, _b_id, ticks_per_second);
            _b_id++;
        }
    }

    inline bool check_deep_coll(glm::dvec3 p1, glm::dvec3 p2, glm::dvec3 v1, glm::dvec3 v2, double r1, double r2, unsigned ticks_per_second) {
        v1 /= (double) ticks_per_second;
        v2 /= (double) ticks_per_second;
        glm::dvec3 no = glm::normalize(v1 - v2);
        double dt = glm::dot(p2 - p1, no);
        glm::dvec3 diff = (p1 + (no * dt) - p2);
        dt = r1 + r2;
        return ((diff.x * diff.x) + (diff.y * diff.y) + (diff.z * diff.z)) <= ((dt) * (dt));
    }

    bool __context_item_collides(Context* ac, size_t a, Context* bc, size_t b, unsigned ticks_per_second){
        if(ac->entity_mass.get_state(a) && bc->entity_mass.get_state(b)){
            if(ac == bc) if(a == b)
                    return false;

            DAABB abox = ac->bounding_boxes[a];
            DAABB bbox = bc->bounding_boxes[b];

            abox.min += ac->position;
            abox.max += ac->position;

            bbox.min += bc->position;
            bbox.max += bc->position;

            if(abox.collides(bbox)){
                if(
                        check_deep_coll(
                        ac->entity_position.get_all(a).first+ac->position, bc->entity_position.get_all(b).first+bc->position,
                        ac->entity_velocity.get_all(a).first, bc->entity_velocity.get_all(b).first,
                        ac->entity_radius.get_all(a).first, bc->entity_radius.get_all(b).first,
                        ticks_per_second)
                        )
                {
                    return true;
                }
            }
        }
        return false;
    }

    void System::process_collision(size_t a, size_t b, unsigned ticks_per_second) {
        Context* A = contexts.get_all(a).first;
        Context* B = contexts.get_all(b).first;
        if(A != nullptr && B != nullptr) return;

        for (size_t i = 0; i < A->significant_entities.size(); ++i) {
            for (int j = 0; j < B->entity_mass.size(); ++j) {
                __context_item_collides(A, i, B, j, ticks_per_second);
            }
        }

        if(a != b){
            for (size_t i = 0; i < B->significant_entities.size(); ++i) {
                for (int j = 0; j < A->entity_mass.size(); ++j) {
                    __context_item_collides(B, i, A, j, ticks_per_second);
                }
            }
        }

    }

    void System::step(unsigned ticks_per_second) {
        for (size_t i = 0; i < contexts.size(); ++i) {
            if(contexts.get_state(i)) {
                contexts.get_all(i).first->step(ticks_per_second);
            }
        }
    }


}