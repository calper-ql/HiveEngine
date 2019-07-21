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
#include <glm/gtx/quaternion.hpp>

namespace HiveEngine {
    class System;
    class Context;
    class Node;

    class NodeRepresentation {
    private:
        int scene_id = -1;
        int mesh_id = -1;
    public:
        NodeRepresentation(int scene_id, int mesh_id);
        virtual void update_global_orientation(glm::dvec3 position, glm::mat3 rotation) = 0;
        virtual void update_orientation(glm::dvec3 position, glm::mat3 rotation) = 0;
        virtual ~NodeRepresentation() = default;

        int get_scene_id();
        int get_mesh_id();
    };

    struct NodePhysicalData {
        float health = 0.0;
        double radius = 0.0;
        double mass = 0.0;
        glm::dvec3 position = {};
        glm::mat3 rotation = glm::mat3(1.0f);
        glm::dvec3 global_position = {};
        glm::mat3 global_rotation = glm::mat3(1.0f);

        glm::dvec3 velocity = {};
        glm::mat3 angular_velocity = glm::mat3(1.0f);

        glm::dvec3 next_position = {};
        glm::mat3 next_rotation = glm::mat3(1.0f);
        glm::dvec3 next_global_position = {};
        glm::mat3 next_global_rotation = glm::mat3(1.0f);
    };


    class Node {
    public:
        std::string name = "";
        Context* context = nullptr;
        int physical_id = -1;
        int bbox_id = -1;
        NodeRepresentation* representation = nullptr;

        int scene_id = -1;
        int mesh_id = -1;

        Node* parent = nullptr;
        int parent_id = -1; // becomes id on context if parent is nullptr;

        Buffer<Node*> children;

        explicit Node(Context* context, Node* parent=nullptr);
        virtual ~Node();

        void erase_children();
        void set_representation(NodeRepresentation* node_representation);

        void remove_representation();
        void update_representation();

        void calculate_next_step(unsigned ticks_per_second);
        void induce_next_step();

        void calculate_current_global();
        void calculate_next_global();
        DAABB calculate_bounding_box();

        void add_children(Node* child);
        void set_parent(Node* parent);

        NodePhysicalData* physical_data();
        unsigned get_level();

        void calculate_level();

        void print();
    };

    class ContextRepresentation {
    public:
        ContextRepresentation() = default;
        virtual ~ContextRepresentation() = default;
        virtual NodeRepresentation* create_node_representation(int scene_id, int mesh_id) = 0;
        virtual void update_position(glm::dvec3 new_position) = 0;
    };

    class Context {
    public:
        size_t id;
        glm::dvec3 position = {0.0, 0.0, 0.0};
        double radius = 0;
        System* system = nullptr;
        ContextRepresentation* representation = nullptr;

        Buffer<NodePhysicalData> node_physical_data;
        Buffer<int> node_level;
        Buffer<DAABB> node_bounding_box;

        Buffer<Node*> root_nodes;

        explicit Context(System* system);
        virtual ~Context();

        void calculate_next_step(unsigned ticks_per_second=60);
        void calculate_bounding_boxes();
        void induce_next_step();

        Node* load_ai_node(aiScene* scene, aiNode *root, int scene_id, Node* parent=nullptr);

        void update_representation();
    };

    class System {
    public:
        Buffer<Context*> contexts;

        System();
        ~System();


    };
}


#endif //HIVEENGINE_CONTEXT_H
