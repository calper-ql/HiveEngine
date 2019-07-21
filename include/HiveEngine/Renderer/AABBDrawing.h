//
// Created by calpe on 6/24/2019.
//

#ifndef HIVEENGINE_AABBDRAWING_H
#define HIVEENGINE_AABBDRAWING_H

#include <HiveEngine/Renderer/Drawing.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Context.h>

namespace HiveEngine::Renderer {


    class AABBDrawing : public Drawing {
    public:
        std::vector<DAABB> bbox;
        std::vector<int> significance;
        HiveEngine::Renderer::Camera* camera = nullptr;

        GLuint texture_id;
        GLuint VBO, VBO_DAABB, VBO_significance, VAO;
        GLuint program;

        GLuint vert_shader;
        GLuint frag_shader;

        glm::dvec3 offset = {};

        unsigned line_width;

        AABBDrawing(Context *context, unsigned line_width=1);

        ~AABBDrawing() override;

        void draw() override;
    };
}


#endif //HIVEENGINE_AABBDRAWING_H
