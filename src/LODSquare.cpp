//
// Created by calper on 4/19/19.
//

#include <HiveEngine/LODSquare.h>

namespace HiveEngine {

    LODSquare::LODSquare(unsigned size, unsigned depth, glm::vec3 lu, glm::vec3 ru, glm::vec3 rd, glm::vec3 ld) {
        this->size = size;
        this->depth = depth;
        this->lu = lu;
        this->ld = ld;
        this->ru = ru;
        this->rd = rd;

        un = nullptr;
        rn = nullptr;
        dn = nullptr;
        ln = nullptr;

        ldc = nullptr;
        rdc = nullptr;
        ruc = nullptr;
        luc = nullptr;

        parent = nullptr;

        last_state = get_state();

        if (size % 2 == 0) this->size -= 1;
        if (size < 3) this->size = 3;
        if (((size - 1) / 2) % 2 != 0) this->size += 2;

        changed = true;

        payload_idx = 0;
        payload_ptr = nullptr;
        lscd = nullptr;
    }

    LODSquare::~LODSquare() {
        if (un) un->erase_reference(this);
        if (dn) dn->erase_reference(this);
        if (ln) ln->erase_reference(this);
        if (rn) rn->erase_reference(this);
        if (parent) {
            parent->changed = true;
        }
        if (lscd) lscd->deleted(this);
    }

    void LODSquare::populate_vertices() {
        vertices.resize(size*size);
        normals.resize(size*size, {0, 0, 0});
        uvs.resize(size*size);

        if (!parent) {
            float itr1 = glm::distance(lu, ld) / (size - 1);
            float itr2 = glm::distance(ru, rd) / (size - 1);

            glm::vec3 dir1 = glm::normalize(ld - lu);
            glm::vec3 dir2 = glm::normalize(rd - ru);

            glm::vec3 lside = lu;
            glm::vec3 rside = ru;

            for (unsigned i = 0; i < size; i++) {

                float itr3 = glm::distance(lside, rside) / (size - 1);
                glm::vec3 dir3 = glm::normalize(rside - lside);
                glm::vec3 tside = lside;

                for (unsigned j = 0; j < size; j++) {
                    vertices[i*size + j] = tside;
                    uvs[i*size + j] = { i / (float)(size - 1), j / (float)(size - 1) };
                    tside += dir3 * itr3;
                }

                lside += dir1 * itr1;
                rside += dir2 * itr2;
            }
        }
        else {
            unsigned m = size / 2;
            unsigned xstart = p_left() ? 0 : m;
            unsigned ystart = p_up() ? 0 : m;

            for (unsigned i = 0; i < m + 1; i++) {
                for (unsigned j = 0; j < m + 1; j++) {
                    auto idx1 = (ystart + i)*size + (xstart)+j;
                    auto idx2 = ((i * 2)*size) + (j * 2);
                    auto vec = parent->vertices[idx1];
                    vertices[idx2] = vec;
                }
            }

            for (unsigned i = 1; i < size; i += 2) {
                for (unsigned j = 1; j < size; j += 2) {
                    auto luside = vertices[(i - 1)*size + j - 1];
                    auto ruside = vertices[(i - 1)*size + j + 1];
                    auto ldside = vertices[(i + 1)*size + j - 1];
                    auto rdside = vertices[(i + 1)*size + j + 1];
                    auto vec = (luside + ruside + ldside + rdside) / 4.0f;
                    vertices[(i)*size + j] = vec;
                    vertices[(i - 1)*size + j] = (luside + ruside) / 2.0f;
                    vertices[(i + 1)*size + j] = (ldside + rdside) / 2.0f;
                    vertices[(i)*size + j - 1] = (luside + ldside) / 2.0f;
                    vertices[(i)*size + j + 1] = (ruside + rdside) / 2.0f;
                }
            }
            uvs = parent->uvs;

        }


    }

    std::vector<glm::uvec3> gen_t(unsigned x, unsigned y, unsigned xstride, unsigned ystride, unsigned size, bool ying) {
        std::vector<glm::uvec3> v;
        unsigned _lu = y * size + x;
        unsigned _ru = y * size + x + xstride;
        unsigned _ld = (y + ystride)*size + x;
        unsigned _rd = (y + ystride)*size + x + xstride;
        if (ying) { //    |/|
            v.emplace_back(_ld, _lu, _ru);
            v.emplace_back(_ld, _rd, _ru);
        }
        else { // yang |\|
            v.emplace_back(_lu, _ru, _rd);
            v.emplace_back(_lu, _ld, _rd);
        }
        return v;
    }

    unsigned __lod_get_x(unsigned index, unsigned size) { return index % size; }
    unsigned __lod_get_y(unsigned index, unsigned size) { return index / size; }

    void LODSquare::process_tile(std::vector<glm::uvec3> d, bool ying) {
        bool pass_zero = true;
        bool pass_one = true;
        if (ying) {
            if (__lod_get_y(d[0].z, size) == 0) {
                //pass_zero = false;
                if (!un) pass_zero = false;
                else if (un->depth < depth) pass_zero = false;
            }

            if (__lod_get_y(d[1].x, size) == size - 1) {
                //d[1].y += 1;
                if (!dn) d[1].y += 1;
                else if (dn->depth < depth) d[1].y += 1;
            }

            if (__lod_get_x(d[0].x, size) == 0) {
                //d[0].y -= size;
                if (!ln) d[0].y -= size;
                else if (ln->depth < depth) d[0].y -= size;
            }

            if (__lod_get_x(d[0].z, size) == size - 1) {
                //pass_one = false;
                if (!rn) pass_one = false;
                else if (rn->depth < depth) pass_one = false;
            }

        }
        else {
            if (__lod_get_y(d[0].x, size) == 0) {
                //d[0].y += 1;
                if (!un) d[0].y += 1;
                else if (un->depth < depth) d[0].y += 1;
            }

            if (__lod_get_y(d[1].z, size) == size - 1) {
                //pass_one = false;
                if (!dn)  pass_one = false;
                else if (dn->depth < depth) pass_one = false;
            }

            if (__lod_get_x(d[0].x, size) == 0) {
                //pass_one = false;
                if (!ln) pass_one = false;
                else if (ln->depth < depth) pass_one = false;
            }

            if (__lod_get_x(d[0].z, size) == size - 1) {
                //d[0].y -= size;
                if (!rn) d[0].y -= size;
                else if (rn->depth < depth) d[0].y -= size;
            }

        }
        if (pass_zero) {
            indices[__indice_iter] = d[0];
            __indice_iter++;
        }
        if (pass_one) {
            indices[__indice_iter] = d[1];
            __indice_iter++;
        }
    }

    void LODSquare::populate_indices(){
        if (!changed) {
            if (last_state == get_state()) return;
            last_state = get_state();
            changed = true;
        }

        indices.clear();
        __indice_iter = 0;
        indices.resize((size-1)*(size-1) * 2, { 0, 0, 0 });

        if (parent) {
            set_neighbor(p_up() ? LODSquareSide::DOWN : LODSquareSide::UP, parent->get_sub_lod(p_left(), !p_up()));
            set_neighbor(p_left() ? LODSquareSide::RIGHT : LODSquareSide::LEFT, parent->get_sub_lod(!p_left(), p_up()));
        }

        unsigned mid_idx = size / 2;

        if (luc == nullptr) {
            bool side = false;
            for (unsigned i = 0; i < mid_idx; i++) {
                for (unsigned j = 0; j < mid_idx; j++) {
                    process_tile(gen_t(j, i, 1, 1, size, side), side);
                    side = !side;
                }
                if (size / 2 % 2 == 0)side = !side;
            }

        }

        if (ldc == nullptr) {
            bool side = ((size - 1) / 2) % 2 != 0;
            for (unsigned i = mid_idx; i < size - 1; i++) {
                for (unsigned j = 0; j < mid_idx; j++) {
                    process_tile(gen_t(j, i, 1, 1, size, side), side);
                    side = !side;
                }
                if (size / 2 % 2 == 0)side = !side;
            }

        }

        if (ruc == nullptr) {
            bool side = ((size - 1) / 2) % 2 != 0;
            for (unsigned i = 0; i < mid_idx; i++) {
                for (unsigned j = mid_idx; j < size - 1; j++) {
                    process_tile(gen_t(j, i, 1, 1, size, side), side);
                    side = !side;
                }
                if (size / 2 % 2 == 0)side = !side;
            }
        }

        if (rdc == nullptr) {
            bool side = false;
            for (unsigned i = mid_idx; i < size - 1; i++) {
                for (unsigned j = mid_idx; j < size - 1; j++) {
                    process_tile(gen_t(j, i, 1, 1, size, side), side);
                    side = !side;
                }
                if (size / 2 % 2 == 0)side = !side;
            }

        }

        if (lscd) lscd->changed(this);
    }

    glm::vec3 LODSquare::get_center() {
        return vertices[(size / 2)*size + (size / 2)];
    }

    LODSquare* LODSquare::create_sub_lod(bool left, bool up) {
        if (get_sub_lod(left, up)) return get_sub_lod(left, up);

        unsigned m = size / 2;
        unsigned xstart = left ? 0 : m;
        unsigned ystart = up ? 0 : m;

        auto temp = new LODSquare(size, depth + 1,
                                  vertices[ystart*size + xstart],
                                  vertices[ystart*size + (xstart + m)],
                                  vertices[(ystart + m)*size + (xstart + m)],
                                  vertices[(ystart + m)*size + xstart]);

        set_sub_lod(left, up, temp);
        temp->parent = this;

        temp->populate_vertices();

        if (parent) {
            if (p_left()) {
                if (!left) parent->create_sub_lod(false, p_up());
            }
            else {
                if (left) parent->create_sub_lod(true, p_up());
            }

            if (p_up()) {
                if (!up) parent->create_sub_lod(p_left(), false);
            }
            else {
                if (up) parent->create_sub_lod(p_left(), true);
            }
        }

        if (luc) {
            luc->dn = ldc;
            luc->rn = ruc;
        }

        if (ruc) {
            ruc->ln = luc;
            ruc->dn = rdc;
        }

        if (ldc) {
            ldc->un = luc;
            ldc->rn = rdc;
        }

        if (rdc) {
            rdc->un = ruc;
            rdc->ln = ldc;
        }

        equalize_out_corners(left, up);

        temp->set_neighbor(up ? LODSquareSide::UP : LODSquareSide::DOWN, up ? this->un : this->dn);
        temp->set_neighbor(left ? LODSquareSide::LEFT : LODSquareSide::RIGHT, left ? this->ln : this->rn);

        temp->equalize_out_corners(left, up);

        temp->lscd = lscd;
        if (lscd) lscd->created(temp);

        changed = true;

        return  temp;
    }

    unsigned LODSquare::erase_sub_load(bool left, bool up, unsigned max_allowed) {
        LODSquare* temp = get_sub_lod(left, up);

        if (temp == nullptr) return 0;

        unsigned deleted_count = 0;

        deleted_count += temp->erase_sub_load(true, true, max_allowed);
        if (deleted_count > max_allowed) return deleted_count;

        deleted_count += temp->erase_sub_load(true, false, max_allowed);
        if (deleted_count > max_allowed) return deleted_count;

        deleted_count += temp->erase_sub_load(false, true, max_allowed);
        if (deleted_count > max_allowed) return deleted_count;

        deleted_count += temp->erase_sub_load(false, false, max_allowed);
        if (deleted_count > max_allowed) return deleted_count;

        if (temp->get_dependency_count() == 0) {
            delete temp;
            set_sub_lod(left, up, nullptr);
            changed = true;
            deleted_count++;
            return deleted_count;
        }

        return deleted_count;
    }

    void LODSquare::set_sub_lod(bool left, bool up, LODSquare *inp) {
        if (left && up) luc = inp;
        if (left && !up) ldc = inp;
        if (!left && up) ruc = inp;
        if (!left && !up) rdc = inp;
    }

    LODSquare *LODSquare::get_sub_lod(bool left, bool up) {
        if (left && up) return luc;
        if (left && !up) return ldc;
        if (!left && up) return ruc;
        if (!left && !up) return rdc;
        return nullptr;
    }

    LODSquare *LODSquare::get_neighbor(LODSquareSide side) {
        if (side == LODSquareSide::UP) return un;
        if (side == LODSquareSide::RIGHT) return rn;
        if (side == LODSquareSide::LEFT) return ln;
        if (side == LODSquareSide::DOWN) return dn;
        return nullptr;
    }

    void LODSquare::set_neighbor(LODSquareSide side, LODSquare *inp) {
        changed = true;
        if (side == LODSquareSide::UP) un = inp;
        if (side == LODSquareSide::RIGHT) rn = inp;
        if (side == LODSquareSide::LEFT) ln = inp;
        if (side == LODSquareSide::DOWN) dn = inp;
    }

    bool LODSquare::p_left() {
        if (!parent) return true;
        return this == parent->luc || this == parent->ldc;
    }

    bool LODSquare::p_up() {
        if (!parent) return true;
        return this == parent->luc || this == parent->ruc;
    }

    bool LODSquare::is_changed() {
        if (changed) {
            changed = false;
            return true;
        }
        else {
            return false;
        }
    }

    glm::vec4 LODSquare::get_sub_center(bool left, bool up) {
        unsigned m = size / 2;
        unsigned xstart = left ? 0 : m;
        unsigned ystart = up ? 0 : m;

        auto __lu = vertices[ystart*size + xstart];
        auto __ru = vertices[ystart*size + (xstart + m)];
        auto __rd = vertices[(ystart + m)*size + (xstart + m)];
        auto __ld = vertices[(ystart + m)*size + xstart];

        auto t3 = (__lu + __ru + __rd + __ld) / 4.0f;
        auto ma = std::max(std::max(glm::distance(__lu, t3), glm::distance(__ld, t3)),
                           std::max(glm::distance(__ru, t3), glm::distance(__rd, t3)));
        return{ t3.x, t3.y, t3.z, ma };

    }

    void LODSquare::reset_corners() {
        lu = vertices[0 * size + 0];
        ru = vertices[0 * size + (size - 1)];
        ld = vertices[(size - 1)*size + 0];
        rd = vertices[(size - 1)*size + (size - 1)];
    }

    void LODSquare::equalize_out_corners(bool left, bool up) {
        if (left) {
            if (ln) {
                ln = ln->request_equalize(this, get_parent_direction_side(LODSquareSide::LEFT));
            }
        }
        else {
            if (rn) {
                rn = rn->request_equalize(this, get_parent_direction_side(LODSquareSide::RIGHT));
            }
        }

        if (up) {
            if (un) {
                un = un->request_equalize(this, get_parent_direction_side(LODSquareSide::UP));
            }
        }
        else {
            if (dn) {
                dn = dn->request_equalize(this, get_parent_direction_side(LODSquareSide::DOWN));
            }
        }

    }

    unsigned LODSquare::get_dependency_count() {
        unsigned dependency = 0;
        if (ln)if (ln->depth == depth) {
                if (ln->ruc) dependency++;
                if (ln->rdc) dependency++;
            }
        if (rn)if (rn->depth == depth) {
                if (rn->luc) dependency++;
                if (rn->ldc) dependency++;
            }
        if (un)if (un->depth == depth) {
                if (un->ldc) dependency++;
                if (un->rdc) dependency++;
            }
        if (dn)if (dn->depth == depth) {
                if (dn->ruc) dependency++;
                if (dn->luc) dependency++;
            }
        return dependency;
    }

    LODSquareSide LODSquare::side_of(LODSquare* other) {
        if (other == ln) return LODSquareSide::LEFT;
        if (other == rn) return LODSquareSide::RIGHT;
        if (other == un) return LODSquareSide::UP;
        if (other == dn) return LODSquareSide::DOWN;
        return LODSquareSide::NONE;
    }

    LODSquare *LODSquare::request_equalize(LODSquare *requester, LODSquareSide request_side) {
        changed = true;

        if (requester->depth <= depth) return this;
        if (request_side == LODSquareSide::NONE) return this;

        if (requester->parent == ln) {
            if (request_side == LODSquareSide::LEFT) {
                auto val = create_sub_lod(true, true);
                val->set_neighbor(LODSquareSide::LEFT, requester);
                return val;
            }
            else if (request_side == LODSquareSide::RIGHT) {
                auto val = create_sub_lod(true, false);
                val->set_neighbor(LODSquareSide::LEFT, requester);
                return val;
            }
        }
        else if (requester->parent == un) {
            if (request_side == LODSquareSide::LEFT) {
                auto val = create_sub_lod(false, true);
                val->set_neighbor(LODSquareSide::UP, requester);
                return val;
            }
            else if (request_side == LODSquareSide::RIGHT) {
                auto val = create_sub_lod(true, true);
                val->set_neighbor(LODSquareSide::UP, requester);
                return val;
            }
        }
        else if (requester->parent == rn) {
            if (request_side == LODSquareSide::LEFT) {
                auto val = create_sub_lod(false, false);
                val->set_neighbor(LODSquareSide::RIGHT, requester);
                return val;
            }
            else if (request_side == LODSquareSide::RIGHT) {
                auto val = create_sub_lod(false, true);
                val->set_neighbor(LODSquareSide::RIGHT, requester);
                return val;
            }
        }
        else if (requester->parent == dn) {
            if (request_side == LODSquareSide::LEFT) {
                auto val = create_sub_lod(true, false);
                val->set_neighbor(LODSquareSide::DOWN, requester);
                return val;
            }
            else if (request_side == LODSquareSide::RIGHT) {
                auto val = create_sub_lod(false, false);
                val->set_neighbor(LODSquareSide::DOWN, requester);
                return val;
            }
        }

        return nullptr;
    }

    LODSquareSide LODSquare::get_parent_direction_side(LODSquareSide which) {
        if (parent == nullptr) return LODSquareSide::NONE;
        if (which == LODSquareSide::LEFT) {
            if (p_up() && p_left()) return LODSquareSide::RIGHT;
            else if (!p_up() && p_left()) return LODSquareSide::LEFT;
            else return LODSquareSide::NONE;
        }
        else if (which == LODSquareSide::RIGHT) {
            if (p_up() && !p_left()) return LODSquareSide::LEFT;
            else if (!p_up() && !p_left()) return LODSquareSide::RIGHT;
            else return LODSquareSide::NONE;
        }
        else if (which == LODSquareSide::UP) {
            if (p_up() && p_left()) return LODSquareSide::LEFT;
            else if (p_up() && !p_left()) return LODSquareSide::RIGHT;
            else return LODSquareSide::NONE;
        }
        else if (which == LODSquareSide::DOWN) {
            if (!p_up() && !p_left()) return LODSquareSide::LEFT;
            else if (!p_up() && p_left()) return LODSquareSide::RIGHT;
            else return LODSquareSide::NONE;
        }
        else {
            return LODSquareSide::NONE;
        }

    }

    void LODSquare::erase_reference(LODSquare *req) {
        changed = true;
        if (req == ln) ln = nullptr;
        else if (req == rn) rn = nullptr;
        else if (req == un) un = nullptr;
        else if (req == dn) dn = nullptr;
    }

    __LODSquareState LODSquare::get_state() {
        __LODSquareState __state;
        __state.dn = dn;
        __state.un = un;
        __state.ln = ln;
        __state.rn = rn;
        __state.ldc = ldc;
        __state.luc = luc;
        __state.rdc = rdc;
        __state.ruc = ruc;
        __state.parent = parent;
        return __state;
    }

    __LODSquareState::__LODSquareState() {
        un = nullptr;
        rn = nullptr;
        dn = nullptr;
        ln = nullptr;

        ldc = nullptr;
        rdc = nullptr;
        ruc = nullptr;
        luc = nullptr;

        parent = nullptr;
    }

    bool __LODSquareState::operator==(const __LODSquareState & rhs)
    {
        return
                un==rhs.un &&
                dn == rhs.dn &&
                ln == rhs.ln &&
                rn == rhs.rn &&
                ruc == rhs.ruc &&
                ldc == rhs.ldc &&
                luc == rhs.luc &&
                rdc == rhs.rdc &&
                parent == rhs.parent;
    }

    LODSquareCreateDelegate::LODSquareCreateDelegate() {

    }

    void LODSquareCreateDelegate::created(LODSquare *n) {

    }

    void LODSquareCreateDelegate::deleted(LODSquare *n) {

    }

    void LODSquareCreateDelegate::changed(LODSquare *n) {

    }

}