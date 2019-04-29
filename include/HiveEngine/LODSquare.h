//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_LODSQUARE_H
#define HIVEENGINE_LODSQUARE_H
#include <glm/glm.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include <algorithm>

namespace HiveEngine {
    class LODSquare;

    class __LODSquareState {
    public:
        LODSquare* un;
        LODSquare* dn;
        LODSquare* ln;
        LODSquare* rn;

        LODSquare* luc;
        LODSquare* ruc;
        LODSquare* ldc;
        LODSquare* rdc;

        LODSquare* parent;

        __LODSquareState();
        bool operator==(const __LODSquareState &rhs);

    };

    class LODSquareCreateDelegate {
    public:
        LODSquareCreateDelegate* secondary = nullptr;
        LODSquareCreateDelegate();
        virtual void created(LODSquare* n);
        virtual void deleted(LODSquare* n);
        virtual void changed(LODSquare* n);
    };

    enum class LODSquareSide {
        UP, RIGHT, DOWN, LEFT, NONE
    };

    class DynamicSphere;

    class LODSquare {
    public:
        unsigned size;
        unsigned depth;
        glm::vec3 lu, ru, rd, ld;

        std::vector<glm::vec3> vertices;
        std::vector<glm::vec3> normals;
        std::vector<glm::vec2> uvs;
        std::vector<glm::uvec3> indices;

        size_t payload_idx;
        LODSquareCreateDelegate* lscd;

        LODSquare(unsigned size, unsigned depth, glm::vec3 lu, glm::vec3 ru, glm::vec3 rd, glm::vec3 ld);
        ~LODSquare();
        void populate_vertices();
        void populate_indices();

        LODSquare* create_sub_lod(bool left, bool up);
        unsigned erase_sub_load(bool left, bool up, unsigned max_allowed);

        LODSquare* get_sub_lod(bool left, bool up);
        void set_sub_lod(bool left, bool up, LODSquare* inp);
        LODSquare* get_neighbor(LODSquareSide side);
        void set_neighbor(LODSquareSide side, LODSquare* inp);

        glm::vec4 get_sub_center(bool left, bool up);

        bool is_changed();
        void reset_corners();

    private:

        void* payload_ptr;
        __LODSquareState last_state;
        bool changed;

        LODSquare* un;
        LODSquare* dn;
        LODSquare* ln;
        LODSquare* rn;

        LODSquare* luc;
        LODSquare* ruc;
        LODSquare* ldc;
        LODSquare* rdc;

        LODSquare* parent;

        size_t __indice_iter = 0;

        void process_tile(std::vector<glm::uvec3> desc, bool ying);

        glm::vec3 get_center();

        bool p_left();
        bool p_up();

        void equalize_out_corners(bool left, bool up);

        LODSquare* request_equalize(LODSquare*, LODSquareSide request_side);

        LODSquareSide get_parent_direction_side(LODSquareSide which);

        unsigned get_dependency_count();

        LODSquareSide side_of(LODSquare* other);

        void erase_reference(LODSquare* requester);

        __LODSquareState get_state();

    };
}

#endif //HIVEENGINE_LODSQUARE_H
