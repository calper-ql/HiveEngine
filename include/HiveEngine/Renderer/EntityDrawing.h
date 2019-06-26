//
// Created by calpe on 6/26/2019.
//

#ifndef HIVEENGINE_ENTITYDRAWING_H
#define HIVEENGINE_ENTITYDRAWING_H


#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Context.h>

#include <HiveEngine/Utilities.h>

namespace HiveEngine::Renderer {


    class EntityDrawing : public Drawing {
    public:
        std::vector<glm::dvec3> pos;
        std::vector<double> radius;
        std::vector<int> significance;
        HiveEngine::Renderer::Camera* camera = nullptr;

        GLuint texture_id;
        GLuint VBO, VBO_POS, VBO_RAD, VBO_significance, VAO;
        GLuint program;

        GLuint vert_shader;
        GLuint frag_shader;

        glm::dvec3 offset = {};

        unsigned line_width;

        EntityDrawing(Context *context, unsigned line_width=0);

        ~EntityDrawing() override;

        void draw() override;
    };
}


#endif //HIVEENGINE_ENTITYDRAWING_H
