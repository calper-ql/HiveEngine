//
// Created by calper on 4/19/19.
//

#include <HiveEngine/DynamicSphere.h>

namespace HiveEngine {
    void DynamicSphere::__dslod_div(LODSquare *temp, unsigned depth) {
        if (temp) {
            if (temp->depth < depth) {
                temp->create_sub_lod(true, true);
                temp->create_sub_lod(true, false);
                temp->create_sub_lod(false, true);
                temp->create_sub_lod(false, false);

                __dslod_div(temp->get_sub_lod(true, true), depth);
                __dslod_div(temp->get_sub_lod(true, false), depth);
                __dslod_div(temp->get_sub_lod(false, true), depth);
                __dslod_div(temp->get_sub_lod(false, false), depth);
            }
        }
    }

    DynamicSphere::DynamicSphere(RadialGenerator *pgn, unsigned resolution) {
        this->pgn = pgn;
        this->resolution = resolution;

        glm::vec3 luc = {-pgn->radius, pgn->radius, pgn->radius};
        glm::vec3 luf = {-pgn->radius, pgn->radius, -pgn->radius};
        glm::vec3 ldc = {-pgn->radius, -pgn->radius, pgn->radius};
        glm::vec3 ldf = {-pgn->radius, -pgn->radius, -pgn->radius};
        glm::vec3 ruc = {pgn->radius, pgn->radius, pgn->radius};
        glm::vec3 ruf = {pgn->radius, pgn->radius, -pgn->radius};
        glm::vec3 rdc = {pgn->radius, -pgn->radius, pgn->radius};
        glm::vec3 rdf = {pgn->radius, -pgn->radius, -pgn->radius};

        auto front = new LODSquare(resolution, 0, luc, ruc, rdc, ldc);
        auto left = new LODSquare(resolution, 0, luf, luc, ldc, ldf);
        auto right = new LODSquare(resolution, 0, ruc, ruf, rdf, rdc);
        auto back = new LODSquare(resolution, 0, ruf, luf, ldf, rdf);
        auto up = new LODSquare(resolution, 0, luf, ruf, ruc, luc);
        auto down = new LODSquare(resolution, 0, ldc, rdc, rdf, ldf);

        front->set_neighbor(LODSquareSide::LEFT, left);
        front->set_neighbor(LODSquareSide::RIGHT, right);
        front->set_neighbor(LODSquareSide::UP, up);
        front->set_neighbor(LODSquareSide::DOWN, down);

        left->set_neighbor(LODSquareSide::RIGHT, front);
        left->set_neighbor(LODSquareSide::LEFT, back);
        left->set_neighbor(LODSquareSide::UP, up);
        left->set_neighbor(LODSquareSide::DOWN, down);

        right->set_neighbor(LODSquareSide::LEFT, front);
        right->set_neighbor(LODSquareSide::RIGHT, back);
        right->set_neighbor(LODSquareSide::UP, up);
        right->set_neighbor(LODSquareSide::DOWN, down);

        back->set_neighbor(LODSquareSide::LEFT, right);
        back->set_neighbor(LODSquareSide::RIGHT, left);
        back->set_neighbor(LODSquareSide::UP, up);
        back->set_neighbor(LODSquareSide::DOWN, down);

        up->set_neighbor(LODSquareSide::LEFT, left);
        up->set_neighbor(LODSquareSide::RIGHT, right);
        up->set_neighbor(LODSquareSide::UP, back);
        up->set_neighbor(LODSquareSide::DOWN, front);

        down->set_neighbor(LODSquareSide::LEFT, left);
        down->set_neighbor(LODSquareSide::RIGHT, right);
        down->set_neighbor(LODSquareSide::UP, front);
        down->set_neighbor(LODSquareSide::DOWN, back);

        roots.push_back(front);
        roots.push_back(left);
        roots.push_back(right);
        roots.push_back(back);
        roots.push_back(up);
        roots.push_back(down);

        for (auto n: roots) {
            n->lscd = this;
            n->populate_vertices();
            n->reset_corners();
            n->payload_idx = squares.size();
            this->created(n);
        }

        alternate_state = true;
        state = true;

        max_depth = 0;
        min_depth = 0;

        limiter = 60;
    }

    int DynamicSphere::absolute_check(unsigned depth) {
        creation_count = 0;
        deletion_count = 0;
        for (auto n : roots) {
            __dslod_div(n, depth);
        }
        for (auto t: squares) {
            if (t) {
                t->populate_indices();
            }
        }
        return creation_count + deletion_count;
    }

    void DynamicSphere::__dslod_dyna_chk(LODSquare *temp, glm::dvec3 relative_point, float fork_min, float fork_max) {
        if (temp == nullptr) return;
        if (creation_count > limiter) return;
        if (deletion_count > limiter) return;

        auto p1 = temp->get_sub_center(true, true);
        auto p2 = temp->get_sub_center(true, false);
        auto p3 = temp->get_sub_center(false, true);
        auto p4 = temp->get_sub_center(false, false);

        auto k1 = glm::distance(glm::vec3(p1), glm::vec3(relative_point));
        auto k2 = glm::distance(glm::vec3(p2), glm::vec3(relative_point));
        auto k3 = glm::distance(glm::vec3(p3), glm::vec3(relative_point));
        auto k4 = glm::distance(glm::vec3(p4), glm::vec3(relative_point));

        if (state) {
            if (k1 * fork_max > p1.a) { if (temp->depth > min_depth) temp->erase_sub_load(true, true, 1); }
            if (k2 * fork_max > p2.a) { if (temp->depth > min_depth) temp->erase_sub_load(true, false, 1); }
            if (k3 * fork_max > p3.a) { if (temp->depth > min_depth) temp->erase_sub_load(false, true, 1); }
            if (k4 * fork_max > p4.a) { if (temp->depth > min_depth) temp->erase_sub_load(false, false, 1); }
        } else {
            if (k1 * fork_min < p1.a) { if (temp->depth < max_depth) temp->create_sub_lod(true, true); }
            if (k2 * fork_min < p2.a) { if (temp->depth < max_depth) temp->create_sub_lod(true, false); }
            if (k3 * fork_min < p3.a) { if (temp->depth < max_depth) temp->create_sub_lod(false, true); }
            if (k4 * fork_min < p4.a) { if (temp->depth < max_depth) temp->create_sub_lod(false, false); }
        }

        __dslod_dyna_chk(temp->get_sub_lod(true, true), relative_point, fork_min, fork_max);
        __dslod_dyna_chk(temp->get_sub_lod(true, false), relative_point, fork_min, fork_max);
        __dslod_dyna_chk(temp->get_sub_lod(false, true), relative_point, fork_min, fork_max);
        __dslod_dyna_chk(temp->get_sub_lod(false, false), relative_point, fork_min, fork_max);

    }

    int DynamicSphere::dynamic_check(glm::dvec3 relative_point, float fork_min, float fork_max) {
        creation_count = 0;
        deletion_count = 0;
        for (auto t: roots) {
            __dslod_dyna_chk(t, relative_point, fork_min, fork_max);
        }
        for (auto t: squares) {
            if (t) {
                t->populate_indices();
            }
        }
        state = !state;
        return creation_count + deletion_count;
    }

    void
    __dslod_calculate_kernel(std::vector<glm::vec3> &vertices, unsigned size, unsigned lod, unsigned x, unsigned y) {
        unsigned lc = x;
        unsigned rc = x + lod + 1;

        unsigned uc = y;
        unsigned dc = y + lod + 1;

        auto lu = vertices[(uc) * size + lc];
        auto ru = vertices[(uc) * size + rc];
        auto ld = vertices[(dc) * size + lc];
        auto rd = vertices[(dc) * size + rc];

        float itr1 = glm::distance(lu, ld) / (lod + 1);
        float itr2 = glm::distance(ru, rd) / (lod + 1);

        glm::vec3 dir1 = glm::normalize(ld - lu);
        glm::vec3 dir2 = glm::normalize(rd - ru);

        glm::vec3 lside = lu;
        glm::vec3 rside = ru;

        for (unsigned i = uc; i < dc + 1; i++) {

            float itr3 = glm::distance(lside, rside) / (lod + 2);
            glm::vec3 dir3 = glm::normalize(rside - lside);
            glm::vec3 tside = lside;

            for (unsigned j = lc; j < rc + 1; j++) {
                vertices[i * size + j] = tside;
                tside += dir3 * itr3;
            }

            lside += dir1 * itr1;
            rside += dir2 * itr2;
        }
    }

    Texture *DynamicSphere::__generate_texture(std::vector<glm::vec3> vertices, unsigned size, unsigned desired_lod,
                                               LODSquare *lods) {
        Texture *t = new Texture();

        auto lod = desired_lod > 0 ? desired_lod : 1;
        t->width = (size - 1) * lod + size;
        t->height = (size - 1) * lod + size;
        t->channel = 4;
        t->data.resize(t->width * t->height * t->channel);

        std::vector<glm::vec3> generated_vertices;
        generated_vertices.resize(t->width * t->height);

        for (unsigned i = 0; i < size; ++i) {
            for (unsigned j = 0; j < size; ++j) {
                glm::vec3 pos = vertices[i * size + j];
                generated_vertices[(i * (lod + 1)) * t->width + (j * (lod + 1))] = pos;
            }
        }

        for (unsigned i = 0; i < size - 1; i += 1) {
            for (unsigned j = 0; j < size - 1; j += 1) {
                __dslod_calculate_kernel(generated_vertices, t->width, lod, j * (lod + 1), i * (lod + 1));
            }
        }

        auto dat = t->data.data();
        for (unsigned i = 0; i < t->height; ++i) {
            for (unsigned j = 0; j < t->width; ++j) {
                generated_vertices[i * t->width + j] = glm::vec3(
                        pgn->generate_vertex(generated_vertices[i * t->width + j]));
                auto c = pgn->generate_color(generated_vertices[i * t->width + j]);
                dat[4 * (j * t->width + i) + 0] = static_cast<uint8_t>((c.r) * 0xFF);
                dat[4 * (j * t->width + i) + 1] = static_cast<uint8_t>((c.g) * 0xFF);
                dat[4 * (j * t->width + i) + 2] = static_cast<uint8_t>((c.b) * 0xFF);
                dat[4 * (j * t->width + i) + 3] = static_cast<uint8_t>((c.a) * 0xFF);
            }
        }

        return t;
    }

    void DynamicSphere::created(LODSquare *n) {
        auto lod = 2;
        lod += resolution / (n->depth + 1);
        n->reset_corners();
        if (available.empty()) {
            n->payload_idx = squares.size();
            squares.push_back(n);
            textures.push_back(__generate_texture(n->vertices, n->size, lod, n));
        } else {
            auto i = available.front();
            available.pop_front();
            squares[i] = n;
            n->payload_idx = i;
            textures[i] = __generate_texture(n->vertices, n->size, lod, n);
        }

        for (unsigned i = 0; i < n->vertices.size(); i++) {
            n->vertices[i] = glm::vec3(pgn->generate_vertex(n->vertices[i]));
            n->normals[i] = glm::vec3(pgn->generate_normal(n->vertices[i]));
        }

        creation_count++;

        if (secondary) secondary->created(n);
    }

    void DynamicSphere::deleted(LODSquare *n) {
        squares[n->payload_idx] = nullptr;
        available.push_back(n->payload_idx);
        delete textures[n->payload_idx];
        textures[n->payload_idx] = nullptr;
        deletion_count++;
        if (secondary) secondary->deleted(n);
    }

    void DynamicSphere::changed(LODSquare *n) {
        if (secondary) secondary->changed(n);
    }

}
