//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_ENTITY_H
#define HIVEENGINE_ENTITY_H

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <HiveEngine/Common.h>

namespace HiveEngine {
    class Entity {
    private:
        glm::dvec3 position; // expressed in meters
        glm::dvec3 velocity; // expressed in meters per second
        double mass; // expressed in kilograms

        glm::dmat3 rotation_matrix; // expressed in radians
        glm::dvec3 total_torque_counter; // expressed inn radians
        glm::dmat3 moment_of_inertia;
        glm::dvec3 torque_resistance;

        glm::dquat angular_velocity;

        double radius; // expressed in meters

        glm::dmat3 calculated_moment_of_inertia;

        Entity* parent; // parent link
        std::vector<Entity*> children;

        std::vector<Force> applied_forces;

        /* ========= Internal Functions =========  */
        glm::dvec3 calculate_relative_position();

        glm::dvec3 calculate_relative_position(Entity* from);

    public:
        Entity(glm::dvec3 position, double radius, double mass);

        virtual ~Entity();


        /* ========= Functions =========  */
        virtual EntityStepOutput step(unsigned steps_per_second);

        double calculate_total_mass();

        void add_child(Entity* c);

        void erase_children_vector();

        glm::dmat3 calculate_rotation_matrix();

        glm::dvec3 calculate_throw_vector(glm::dvec3 relative_point, bool parent_supported);

        glm::dvec3 calculate_position();

        virtual void apply_force(glm::dvec3 leverage, glm::dvec3 force, bool is_relative);

        void get_all_children(std::vector<Entity*> *list);

        CentralMass calculate_central_mass();

        void set_torque_resistance(glm::dvec3 percentages);

        /* ========= GETTERS & SETTERS =========  */

        const glm::dvec3 &get_position() const;

        void set_position(const glm::dvec3 &position);

        const glm::dvec3 &get_velocity() const;

        void set_velocity(const glm::dvec3 &velocity);

        double get_mass() const;

        void set_mass(double mass);

        const glm::dmat3 &get_rotation_matrix() const;

        void set_rotation_matrix(const glm::dmat3 &rotation_matrix);

        const glm::dmat3 &get_moment_of_inertia() const;

        void set_moment_of_inertia(const glm::dmat3 &moment_of_inertia);

        double get_radius() const;

        void set_radius(double radius);

        Entity *get_parent() const;

        void set_parent(Entity *parent);

        const std::vector<Force> &get_applied_forces() const;

        glm::dvec3 get_total_torque();

        void set_total_torque(glm::dvec3 t);
    };


}


#endif //HIVEENGINE_ENTITY_H
