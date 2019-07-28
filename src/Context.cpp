//
// Created by calpe on 6/22/2019.
//

#include <HiveEngine/Utilities.h>
#include <iostream>
#include "HiveEngine/Context.h"


namespace HiveEngine {

    Node::Node(Context *context, Node* parent) {
        this->context = context;
        this->physical_id = context->node_physical_data.add(NodePhysicalData());

        context->node_level.add(0);

        this->bbox_id = context->node_bounding_box.add(DAABB());
        this->parent = parent;
        if(parent){
            parent_id = parent->children.add(this);
        } else {
            parent_id = context->root_nodes.add(this);
        }

        calculate_current_global();
        calculate_level();
    }

    Node::~Node() {
        erase_children();
        if(parent) parent->children.remove(parent_id);
        else context->root_nodes.remove(parent_id);
        context->node_physical_data.remove(physical_id);
        context->node_level.remove(physical_id);
        context->node_bounding_box.set(bbox_id, DAABB());
        context->node_bounding_box.remove(bbox_id);
        remove_representation();
    }

    void Node::erase_children() {
        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)) delete children.get(i);
        }
    }

    void Node::set_representation(NodeRepresentation* node_representation){
        if(representation) delete representation;
        this->representation = node_representation;
    }

    void Node::remove_representation() {
        delete representation;
        representation = nullptr;
    }

    void Node::update_representation() {
        if(!representation) {
            if(context->representation){
                set_representation(context->representation->create_node_representation(scene_id, mesh_id));
            }
        }

        if(representation){
            representation->update_global_orientation(physical_data()->global_position,
                                                      physical_data()->global_rotation);
            representation->update_orientation(physical_data()->position,
                                                      physical_data()->rotation);
            for (int i = 0; i < children.size(); ++i) {
                if(children.get_state(i)) children.get(i)->update_representation();
            }
        }
    }

    void Node::calculate_next_step(unsigned ticks_per_second) {
        physical_data()->next_position = physical_data()->position + (physical_data()->velocity / (double) ticks_per_second);
        glm::quat rot_quat = glm::pow(glm::quat_cast(physical_data()->angular_velocity), 1.0f / (float) ticks_per_second);
        physical_data()->next_rotation = physical_data()->rotation * glm::mat3_cast(rot_quat);
        calculate_next_global();

        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)) children.get(i)->calculate_next_step(ticks_per_second);
        }
    }

    void Node::induce_next_step() {
        physical_data()->position = physical_data()->next_position;
        physical_data()->rotation = physical_data()->next_rotation;
        physical_data()->global_position = physical_data()->next_global_position;
        physical_data()->global_rotation = physical_data()->next_global_rotation;
        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)) children.get(i)->induce_next_step();
        }
    }

    void Node::calculate_current_global() {
        if(parent){
            physical_data()->global_rotation = glm::dmat3(parent->physical_data()->global_rotation) * glm::dmat3(physical_data()->rotation);
            physical_data()->global_position = (glm::dmat3(parent->physical_data()->global_rotation) * physical_data()->position) + parent->physical_data()->global_position;
        } else {
            physical_data()->global_position = physical_data()->position;
            physical_data()->global_rotation = physical_data()->rotation;
        }
    }

    void Node::calculate_next_global() {
        if(parent){
            physical_data()->next_global_rotation = parent->physical_data()->next_global_rotation * physical_data()->next_rotation;
            physical_data()->next_global_position = (glm::dmat3(parent->physical_data()->next_global_rotation) * physical_data()->next_position) + parent->physical_data()->next_global_position;
        } else {
            physical_data()->next_global_position = physical_data()->next_position;
            physical_data()->next_global_rotation = physical_data()->next_rotation;
        }
    }

    DAABB Node::calculate_bounding_box() {
        DAABB bbox;
        auto er = physical_data()->radius;
        auto ep = physical_data()->global_position;
        auto ev = physical_data()->next_global_position - physical_data()->global_position;
        bbox.min.x = (ep.x - er) + (ev.x < 0 ? ev.x : 0);
        bbox.min.y = (ep.y - er) + (ev.y < 0 ? ev.y : 0);
        bbox.min.z = (ep.z - er) + (ev.z < 0 ? ev.z : 0);
        bbox.max.x = (ep.x + er) + (ev.x > 0 ? ev.x : 0);
        bbox.max.y = (ep.y + er) + (ev.y > 0 ? ev.y : 0);
        bbox.max.z = (ep.z + er) + (ev.z > 0 ? ev.z : 0);

        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)) {
                bbox.combine(children.get(i)->calculate_bounding_box());
            }
        }

        context->node_bounding_box.set(bbox_id, bbox);
        return bbox;
    }

    NodePhysicalData *Node::physical_data() {
        return &(context->node_physical_data.get_data()[physical_id]);
    }

    unsigned Node::get_level() {
        return context->node_level.get(physical_id);
    }

    void Node::calculate_level() {
        if(parent) context->node_level.set(physical_id, parent->get_level() + 1);
        else context->node_level.set(physical_id, 0);
    }

    void Node::add_children(Node *child) {
        child->set_parent(this);
    }

    void Node::set_parent(Node *parent) {
        if(this->parent){
            this->parent->children.remove(parent_id);
        } else {
            context->root_nodes.remove(parent_id);
        }

        this->parent = parent;
        if(this->parent){
            parent_id = this->parent->children.add(this);
        } else {
            parent_id = context->root_nodes.add(this);
        }

        calculate_level();
    }

    void Node::print() {
        std::string  tk = "+";
        for (int i = 0; i < get_level(); ++i) {
            tk += "-";
        }
        std::cout << tk << name << std::endl;
        for (int j = 0; j < children.size(); ++j) {
            if(children.get_state(j)){
                children.get(j)->print();
            }
        }
    }

    Node* Context::load_ai_node(aiScene *scene, aiNode *root, int scene_id, Node* parent) {
        glm::mat4 transform = ai_matrix_to_glm(root->mTransformation);

        glm::mat3 rot = glm::mat3(transform);
        glm::dvec3 pos = glm::dvec3(transform[3]);

        Node* cntx = new Node(this, parent);

        cntx->physical_data()->position = pos;
        cntx->physical_data()->rotation = rot;
        cntx->calculate_current_global();

        double r = ai_get_node_radius(scene, root);
        cntx->physical_data()->radius = r;

        cntx->name = std::string(root->mName.C_Str());

        std::cout << cntx->name << std::endl;
        std::cout << mat3_to_str(cntx->physical_data()->rotation) << std::endl;
        std::cout << "---" << std::endl;
        std::cout << mat3_to_str(cntx->physical_data()->global_rotation) << std::endl;

        for (int i = 0; i < root->mNumMeshes; ++i) {
            cntx->scene_id = scene_id;
            cntx->mesh_id = root->mMeshes[i];
        }

        for (int i = 0; i < root->mNumChildren; ++i) {
            Node* child = load_ai_node(scene, root->mChildren[i], scene_id, cntx);
        }

        return cntx;
    }

    Context::Context(System *system) {
        this->system = system;
        this->id = system->contexts.add(this);
    }

    Context::~Context() {
        this->system->contexts.remove(id);
        for (int i = 0; i < root_nodes.size(); ++i) {
            if(root_nodes.get_state(i)) delete root_nodes.get(i);
        }
        if(representation) delete representation;
    }

    void Context::calculate_next_step(unsigned ticks_per_second) {
        for (int i = 0; i < root_nodes.size(); ++i) {
            if(root_nodes.get_state(i)) root_nodes.get(i)->calculate_next_step(ticks_per_second);
        }
    }

    void Context::calculate_bounding_boxes() {
        for (int i = 0; i < root_nodes.size(); ++i) {
            if(root_nodes.get_state(i)) root_nodes.get(i)->calculate_bounding_box();
        }
    }

    void Context::induce_next_step() {
        for (int i = 0; i < root_nodes.size(); ++i) {
            if(root_nodes.get_state(i)) root_nodes.get(i)->induce_next_step();
        }
    }

    void Context::update_representation() {
        if(representation){
            representation->update_position(position);
            for (int i = 0; i < root_nodes.size(); ++i) {
                if(root_nodes.get_state(i)) root_nodes.get(i)->update_representation();
            }
        }
    }


    System::System() {

    }

    System::~System() {
        for (int i = 0; i < contexts.size(); ++i) {
            auto item = contexts.get_all(i);
            if (item.second) delete item.first;
        }
    }

    NodeRepresentation::NodeRepresentation(int scene_id, int mesh_id) {
        this->scene_id = scene_id;
        this->mesh_id = mesh_id;
    }

    int NodeRepresentation::get_scene_id() {
        return scene_id;
    }

    int NodeRepresentation::get_mesh_id() {
        return mesh_id;
    }
}