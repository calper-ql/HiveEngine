//
// Created by calper on 4/19/19.
//

#include <iostream>
#include <HiveEngine/Entity.h>


namespace HiveEngine {

    Entity::Entity(glm::dvec3 position, double radius, double mass) {
        this->position = position;
        this->radius = radius;
        this->mass = mass;
        this->parent = nullptr;
        this->velocity = glm::dvec3(0.0, 0.0, 0.0);
        this->rotation_matrix = glm::dmat3(1.0);
        this->total_torque_counter = glm::dvec3(0.0, 0.0, 0.0);
        this->moment_of_inertia = glm::dmat3(2.0f / 5.0f * mass * radius * radius);
        this->calculated_moment_of_inertia = this->moment_of_inertia;
        this->torque_resistance = glm::dvec3(1.0f, 1.0f, 1.0f);
        angular_velocity = glm::quat(glm::dvec3(0.0, 0.0, 0.0));
    }

    Entity::~Entity() {
        for (auto item : children) {
            delete item;
        }
    }

    /* ========= GETTERS & SETTERS =========  */

    const glm::dvec3 &Entity::get_position() const {
        return position;
    }

    void Entity::set_position(const glm::dvec3 &position) {
        Entity::position = position;
    }

    const glm::dvec3 &Entity::get_velocity() const {
        if (parent) return parent->get_velocity();
        return velocity;
    }

    void Entity::set_velocity(const glm::dvec3 &velocity) {
        Entity::velocity = velocity;
    }

    double Entity::get_mass() const {
        return mass;
    }

    void Entity::set_mass(double mass) {
        Entity::mass = mass;
    }

    const glm::dmat3 &Entity::get_rotation_matrix() const {
        return rotation_matrix;
    }

    void Entity::set_rotation_matrix(const glm::dmat3 &rotation_matrix) {
        Entity::rotation_matrix = rotation_matrix;
    }

    const glm::dmat3 &Entity::get_moment_of_inertia() const {
        return moment_of_inertia;
    }

    void Entity::set_moment_of_inertia(const glm::dmat3 &moment_of_inertia) {
        Entity::moment_of_inertia = moment_of_inertia;
    }

    double Entity::get_radius() const {
        return radius;
    }

    void Entity::set_radius(double radius) {
        Entity::radius = radius;
    }

    Entity *Entity::get_parent() const {
        return parent;
    }

    void Entity::set_parent(Entity *parent) {
        Entity::parent = parent;
    }

    const std::vector<Force> &Entity::get_applied_forces() const {
        return applied_forces;
    }

    /* ========= FUNCTIONS =========  */

    EntityStepOutput Entity::step(unsigned steps_per_second) {
        double deamplify_ratio = 1.0f / (double) steps_per_second;
        EntityStepOutput eso;
        eso.moment_of_inertia = moment_of_inertia;
        eso.mass = mass;
        CentralMass mc;
        if (parent == nullptr) {
            mc = calculate_central_mass();
        }

        for (auto child: children) {
            child->step(steps_per_second);
        }

        if (parent == nullptr) {
            eso.moment_of_inertia = mc.moment_of_inertia;
        }

        for (auto f: this->applied_forces) {
            glm::dvec3 force_vector = f.force;
            glm::dvec3 leverage = f.leverage;
            if (parent == nullptr) {
                if (f.is_relative) leverage += -mc.position * rotation_matrix;
                else leverage += -mc.position;
            }

            if (!f.is_relative) leverage = leverage / rotation_matrix;
            glm::dvec3 torque_vector = glm::cross(leverage, force_vector);
            if (f.is_relative) force_vector = rotation_matrix * force_vector;

            if (f.is_relative) {
                if (parent) parent->apply_force(position + leverage, force_vector, true);
            } else {
                if (parent) parent->apply_force(parent->rotation_matrix * position, force_vector, false);
            }

            eso.force += force_vector;

            if (parent) eso.torque += torque_vector * glm::abs(torque_resistance - 1.0) * deamplify_ratio;
            else eso.torque += torque_vector * deamplify_ratio;
        }

        total_torque_counter += eso.torque;
        auto total_w = total_torque_counter / eso.moment_of_inertia;
        calculated_moment_of_inertia = eso.moment_of_inertia;

        angular_velocity = glm::dquat(glm::dvec3(0.0, 0.0, 0.0));
        angular_velocity *= glm::angleAxis(total_w[0], glm::dvec3(1.0, 0.0, 0.0) * angular_velocity);
        angular_velocity *= glm::angleAxis(total_w[1], glm::dvec3(0.0, 1.0, 0.0) * angular_velocity);
        angular_velocity *= glm::angleAxis(total_w[2], glm::dvec3(0.0, 0.0, 1.0) * angular_velocity);

        rotation_matrix = rotation_matrix * glm::mat3_cast(angular_velocity);
        if (parent == nullptr) {
            velocity += (eso.force / mc.mass);
            auto mcp = mc.position;
            auto shift = (angular_velocity * (-mcp)) + mcp;
            position += shift + deamplify_ratio * velocity;
            eso.central_mass = shift;
        }

        applied_forces.clear();

        return eso;
    }

    double Entity::calculate_total_mass() {
        double m = this->mass;
        for (const auto &item : this->children) {
            m += item->calculate_total_mass();
        }
        return m;
    }

    void Entity::add_child(Entity *c) {
        children.push_back(c);
        c->parent = this;
    }

    glm::dmat3 Entity::calculate_rotation_matrix() {
        if (parent == nullptr) return rotation_matrix;
        return parent->calculate_rotation_matrix() * rotation_matrix;
    }

    glm::dvec3 Entity::calculate_throw_vector(glm::dvec3 relative_point, bool parent_supported) {
        if (parent && parent_supported) {
            return parent->calculate_throw_vector(position, true)
                   + calculate_rotation_matrix()
                     * glm::cross(total_torque_counter / calculated_moment_of_inertia, relative_point);
        }
        return glm::cross(total_torque_counter / calculated_moment_of_inertia, relative_point);
    }

    glm::dvec3 Entity::calculate_relative_position() {
        if (parent == nullptr) return glm::dvec3(0.0f, 0.0f, 0.0f);
        return parent->calculate_relative_position() + parent->calculate_rotation_matrix() * position;
    }


    glm::dvec3 Entity::calculate_position() {
        if (parent == nullptr) {
            return position;
        }
        return parent->calculate_position() + parent->calculate_rotation_matrix() * position;
    }

    void Entity::apply_force(glm::dvec3 leverage, glm::dvec3 force, bool is_relative) {
        if (glm::length(force) > 0.0) applied_forces.emplace_back(leverage, force, is_relative);
    }

    void Entity::get_all_children(std::vector<Entity *> *list) {
        for (Entity *item : children) {
            list->push_back(item);
            item->get_all_children(list);
        }
    }

    CentralMass Entity::calculate_central_mass() {
        CentralMass cm;
        std::vector<Entity *> list;
        get_all_children(&list);
        cm.mass = mass;
        cm.moment_of_inertia = moment_of_inertia;

        for (auto c: list) {
            if (c) {
                auto pos = c->calculate_relative_position(this);
                cm.position += pos * c->mass;
                cm.mass += c->mass;
            }
        }

        for (auto c: list) {
            if (c) {
                auto pos = c->calculate_relative_position(this) - cm.position;
                pos = pos;
                glm::dmat3 tens = glm::dmat3(0.0);
                auto x2 = pos.x * pos.x;
                auto y2 = pos.y * pos.y;
                auto z2 = pos.z * pos.z;

                tens[0][0] = c->mass * (y2 + z2);
                tens[1][1] = c->mass * (x2 + z2);
                tens[2][2] = c->mass * (x2 + y2);

                tens[0][1] = -c->mass * (pos.x * pos.y);
                tens[1][0] = tens[0][1];

                tens[0][2] = -c->mass * (pos.x * pos.z);
                tens[2][0] = tens[0][2];

                tens[1][2] = -c->mass * (pos.y * pos.z);
                tens[2][1] = tens[1][2];

                cm.moment_of_inertia += (tens + c->moment_of_inertia);
            }
        }

        //cm.moment_of_inertia = glm::dmat3(2.0f/5.0f * cm.mass * radius * radius);
        cm.position = rotation_matrix * cm.position;
        cm.position /= cm.mass;
        return cm;
    }

    void Entity::set_torque_resistance(glm::dvec3 percentages) {
        this->torque_resistance = percentages;
    }

    void Entity::erase_children_vector() {
        children.clear();
    }

    glm::dvec3 Entity::get_total_torque() {
        return total_torque_counter;
    }

    void Entity::set_total_torque(glm::dvec3 t) {
        total_torque_counter = t;
    }

    glm::dvec3 Entity::calculate_relative_position(Entity *from) {
        if (parent == nullptr) return glm::dvec3(0.0f, 0.0f, 0.0f);
        if (this == from) return glm::dvec3(0.0f, 0.0f, 0.0f);
        else {
            auto rmat = glm::dmat3(1.0f);
            if (parent->parent) rmat = parent->rotation_matrix;
            auto val = parent->calculate_relative_position(from) + rmat * position;
            return val;
        }

    }


}