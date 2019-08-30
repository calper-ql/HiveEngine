//
// Created by calpe on 6/22/2019.
//

#include <HiveEngine/Utilities.h>
#include <iostream>
#include <HiveEngine/Context.h>

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
        bool inited = false;
        glm::dvec3 last_com = physical_data()->center_of_mass;
        if(physical_data()->total_mass > 0.0) {
            last_com /= physical_data()->total_mass;
            inited = true;
        }
		if (parent == nullptr) {
			calculate_mass_data();
			glm::dmat3 moi;
			calculate_moi_data(moi);

		}

        if(parent == nullptr && physical_data()->total_mass > 0.0 && inited){
            glm::dvec3 com = physical_data()->center_of_mass / physical_data()->total_mass;
            auto diff = last_com - com;
            physical_data()->next_position = physical_data()->position + diff;
        } else {
            physical_data()->next_position = physical_data()->position;
        }

        physical_data()->next_position += (physical_data()->velocity / (double) ticks_per_second);
		

        if(parent == nullptr && physical_data()->total_mass > 0.0){
			auto tt = physical_data()->total_torque;
			auto moi = physical_data()->moment_of_inertia;
			auto L = tt / moi;
			if (L.x != L.x) L.x = 0.0;
			if (L.y != L.y) L.y = 0.0;
			if (L.z != L.z) L.z = 0.0;
            auto w = L;
            glm::dquat angular_velocity = glm::dquat(glm::dvec3(0.0, 0.0, 0.0));
            angular_velocity *= glm::angleAxis(w[0], glm::dvec3(1.0, 0.0, 0.0) * angular_velocity);
            angular_velocity *= glm::angleAxis(w[1], glm::dvec3(0.0, 1.0, 0.0) * angular_velocity);
            angular_velocity *= glm::angleAxis(w[2], glm::dvec3(0.0, 0.0, 1.0) * angular_velocity);
            physical_data()->angular_velocity = glm::mat3_cast(angular_velocity);
        }


        glm::dquat rot_quat = glm::pow(glm::quat_cast(physical_data()->angular_velocity), 1.0f / (double) ticks_per_second);
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
            physical_data()->global_rotation = parent->physical_data()->global_rotation * physical_data()->rotation;
            physical_data()->global_position = parent->physical_data()->global_rotation * physical_data()->position + parent->physical_data()->global_position;
        } else {
            physical_data()->global_position = physical_data()->position;
            physical_data()->global_rotation = physical_data()->rotation;
        }
    }

    void Node::calculate_next_global() {
        if(parent){
            physical_data()->next_global_rotation = parent->physical_data()->next_global_rotation * physical_data()->next_rotation;
            physical_data()->next_global_position = parent->physical_data()->next_global_rotation * physical_data()->next_position + parent->physical_data()->next_global_position;
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

    Node *Node::deep_copy(Context *new_context, Node* parent) {
        Node* node = new Node(new_context, parent);

        node->scene_id = scene_id;
        node->mesh_id = mesh_id;
        node->name = name;

        *node->physical_data() = *physical_data();

        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)){
                Node* child = children.get(i)->deep_copy(new_context, node);
            }
        }

        return node;
    }

    void Node::apply_force(Force force) {
        if(parent){
            Force adjusted;
            adjusted.force = force.force;
            adjusted.leverage = physical_data()->position + physical_data()->rotation * force.leverage;
            adjusted.is_relative = force.is_relative;
            parent->apply_force(adjusted);
        } else {
            Force adjusted;
            adjusted.force = force.force;
            adjusted.leverage = physical_data()->center_of_mass + force.leverage;
            adjusted.is_relative = force.is_relative;

			auto tt_add = glm::cross(adjusted.leverage, adjusted.force);
            physical_data()->total_torque += tt_add;
        }
    }

    void Node::calculate_mass_data(Node* root) {
		representation_mass_data.recalculate = true;
        if(scene_id > -1 && mesh_id > -1 && representation_mass_data.recalculate){
            if(context->asset_manager == nullptr){
                spdlog::error("Asset manager is nullptr in context");
                process_error();
            }
            representation_mass_data.position = context->asset_manager->scenes[scene_id].meshes[mesh_id].get_center_of_mass();
            representation_mass_data.moment_of_inertia = context->asset_manager->scenes[scene_id].meshes[mesh_id].get_moment_of_inertia();
            representation_mass_data.mass = physical_data()->mass;
            representation_mass_data.recalculate = true;
        }

        if(valid_representation()){
            physical_data()->total_mass = physical_data()->mass;
            physical_data()->center_of_mass = (physical_data()->rotation * representation_mass_data.position) * physical_data()->mass;
			physical_data()->moment_of_inertia = glm::dmat3(0.0);// representation_mass_data.moment_of_inertia* representation_mass_data.mass;
        } else {
            physical_data()->center_of_mass = {};
            physical_data()->total_mass = 0;
            physical_data()->moment_of_inertia = glm::dmat3(0.0);
        }

        for (int i = 0; i < children.size(); ++i) {
            if(children.get_state(i)) children.get(i)->calculate_mass_data();
        }

        if(parent){
            parent->physical_data()->total_mass += physical_data()->total_mass;
            glm::dvec3 shift = physical_data()->center_of_mass + physical_data()->position * physical_data()->mass;
            parent->physical_data()->center_of_mass += parent->physical_data()->rotation * shift;
			//auto moi = calculate_moment_of_inertia(physical_data()->rotation * (physical_data()->position + physical_data()->center_of_mass), physical_data()->mass);
            //parent->physical_data()->moment_of_inertia += (moi +  physical_data()->moment_of_inertia);
        }

    }

	void Node::calculate_moi_data(glm::dmat3* moi, glm::dvec3 position) {

	}

    bool Node::valid_representation() {
        return scene_id >= 0 && mesh_id >= 0;
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

        for (int i = 0; i < root->mNumMeshes; ++i) {
            cntx->scene_id = scene_id;
            cntx->mesh_id = root->mMeshes[i];
            cntx->physical_data()->mass = asset_manager->scenes[scene_id].meshes[cntx->mesh_id].surface_area();
        }

        for (int i = 0; i < root->mNumChildren; ++i) {
            Node* child = load_ai_node(scene, root->mChildren[i], scene_id, cntx);
        }

        return cntx;
    }

    Context::Context(System* system, AssetManager* asset_manager) {
		if (system) {
			this->system = system;
			this->id = system->contexts.add(this);
		}
        this->asset_manager = asset_manager;
    }

    Context::~Context() {
        if(system) system->contexts.remove(id);
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

    void Context::copy_from(Context *other) {
        for (int i = 0; i < other->root_nodes.size(); ++i) {
            if(other->root_nodes.get_state(i)){
                other->root_nodes.get(i)->deep_copy(this);
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