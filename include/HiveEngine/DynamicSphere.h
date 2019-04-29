//
// Created by calper on 4/19/19.
//

#ifndef HIVEENGINE_DYNAMICSPHERE_H
#define HIVEENGINE_DYNAMICSPHERE_H

#include <HiveEngine/LODSquare.h>
#include <HiveEngine/RadialGenerator.h>
#include <HiveEngine/Texture.h>
#include <queue>

namespace HiveEngine {
    class DynamicSphere : public LODSquareCreateDelegate {
    public:
        bool alternate_state;
        bool state;

        std::vector<LODSquare*> squares;
        std::vector<Texture*> textures;
        std::deque<size_t> available;

        std::vector<LODSquare*> roots;
        unsigned max_depth;
        unsigned min_depth;
        RadialGenerator *pgn;
        unsigned resolution;

        DynamicSphere(RadialGenerator *pgn, unsigned resolution);

        int dynamic_check(glm::dvec3 relative_point, float fork_min, float fork_max);
        int absolute_check(unsigned depth);

        virtual void created(LODSquare* n);
        virtual void deleted(LODSquare* n);
        virtual void changed(LODSquare* n);

        unsigned creation_count;
        unsigned deletion_count;
        unsigned limiter;

        Texture* __generate_texture(std::vector<glm::vec3> vertices, unsigned size, unsigned desired_lod,
                                    LODSquare *lods);
        void __dslod_dyna_chk(LODSquare *temp, glm::dvec3 relative_point, float fork_min, float fork_max);
        void __dslod_div(LODSquare *temp, unsigned depth);
    };
}




#endif //HIVEENGINE_DYNAMICSPHERE_H
