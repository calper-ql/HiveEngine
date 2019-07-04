//
// Created by calpe on 6/18/2019.
//

#include "HiveEngine/Renderer/OptixDrawing.h"

namespace HiveEngine::Renderer {


    OptixDrawing::OptixDrawing(Context *context, std::string program_space_path) : Drawing(context) {
        rtx_context = optix::Context::create();

        rtx_context->setRayTypeCount(1);
        rtx_context->setEntryPointCount(1);

        rtx_context->setStackSize(1024*8);
        rtx_context->setPrintEnabled(true);
        rtx_context->setExceptionEnabled(RT_EXCEPTION_ALL, true);

        rtx_context->setMaxCallableProgramDepth(10);

        int RTX = true; // try once with true then false to see performance difference
        if (rtGlobalSetAttribute(RT_GLOBAL_ATTRIBUTE_ENABLE_RTX, sizeof(RTX), &RTX) != RT_SUCCESS)
            spdlog::critical("Error setting RTX mode.");
        else
            spdlog::info("OptiX RTX execution mode is " + std::to_string(RTX));

        for(auto &entry: std::filesystem::directory_iterator(program_space_path)){
            if(entry.path().generic_string().find(".cu.ptx")!=std::string::npos) {
                std::string program_name = entry.path().generic_string();
                program_name.erase(program_name.find(".cu.ptx"));

                int underline_count = 0;
                for(size_t i = 0; i < program_name.size(); i++){
                    if(program_name[i] == '_'){
                        underline_count++;
                        if(underline_count >= 5){
                            program_name.erase(0, i+1);
                            break;
                        }
                    }
                }
                spdlog::info("loading RTX program: " + program_name);
                optix::Program program = rtx_context->createProgramFromPTXFile(entry.path().string(), program_name);
                program_space[program_name] = program;
            }
        }

        vert_shader = context->shaders["optix_shader.vert"];
        frag_shader = context->shaders["optix_shader.frag"];

        program = glCreateProgram();
        glAttachShader(program, vert_shader);
        glAttachShader(program, frag_shader);
        glLinkProgram(program);

        GLint program_linked = GL_FALSE;
        glGetProgramiv(program, GL_LINK_STATUS, &program_linked);
        if(program_linked != GL_TRUE){
            spdlog::critical(" ======= PROGRAM INFOLOG =======");
            GLint log_size;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_size);
            std::vector<char> info_log(log_size);
            glGetProgramInfoLog(program, log_size, nullptr, info_log.data());
            spdlog::critical("  " + std::string(info_log.data(), info_log.size()));
            spdlog::error("Shader compilation failed, aborting!");
            process_error();
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        root_node = rtx_context->createGroup();
        root_node->setAcceleration(rtx_context->createAcceleration("Trbvh"));
    }

    void rt_buffer_to_gl(optix::Buffer buffer, GLuint &gl_tex_id) {

        // Query buffer information
        RTsize buffer_width_rts, buffer_height_rts;
        buffer->getSize(buffer_width_rts, buffer_height_rts);
        uint32_t width = static_cast<int>(buffer_width_rts);
        uint32_t height = static_cast<int>(buffer_height_rts);
        RTformat buffer_format = buffer->getFormat();

        if (!gl_tex_id) {
            glGenTextures(1, &gl_tex_id);

        }

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gl_tex_id);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_LINEAR);

        // send PBO or host-mapped image data to texture
        const unsigned pboId = buffer->getGLBOId();
        GLvoid* imageData = 0;
        if (pboId)
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboId);
        else
            imageData = buffer->map(0, RT_BUFFER_MAP_READ);

        RTsize elmt_size = buffer->getElementSize();
        if (elmt_size % 8 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 8);
        else if (elmt_size % 4 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
        else if (elmt_size % 2 == 0) glPixelStorei(GL_UNPACK_ALIGNMENT, 2);
        else                          glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        GLenum pixel_format = GL_RGBA;

        if (buffer_format == RT_FORMAT_UNSIGNED_BYTE4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, pixel_format, GL_UNSIGNED_BYTE, imageData);
        else if (buffer_format == RT_FORMAT_FLOAT4)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, width, height, 0, pixel_format, GL_FLOAT, imageData);
        else if (buffer_format == RT_FORMAT_FLOAT3)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, width, height, 0, pixel_format, GL_FLOAT, imageData);

        if (pboId)
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
        else
            buffer->unmap();

    }

    void OptixDrawing::draw() {
        Drawing::draw();
        for (int j = 0; j < perspectives.size(); ++j) {
            if(perspectives.get_state(j)){
                auto perspective = perspectives.get(j);

                if(perspective.window_bound){
                    if(perspective.width != get_window_size().x || perspective.height != get_window_size().y){
                        this->resize_perspective(perspective.id, get_window_size());
                    }
                }

                if(cameras.get_state(j) && cameras.get(j) && cameras.get(j)->get_package().apply){
                    CameraPackage package = cameras.get(j)->get_package_no_perspective();
                    root_node->getAcceleration()->markDirty();

                    rtx_context->setRayGenerationProgram(0, program_space[perspective.raygen_program]);
                    rtx_context->setMissProgram(0, program_space[perspective.miss_program]);
                    rtx_context->setExceptionProgram(0, program_space[perspective.exception_program]);

                    glm::vec3 pos = glm::vec3(package.pos);
                    rtx_context["origin"]->setFloat(pos.x, pos.y, pos.z);
                    glm::mat4 view = package.view;

                    rtx_context["view"]->setMatrix4x4fv(false, glm::value_ptr(view));
                    rtx_context["fov"]->setFloat(package.fov);
                    rtx_context["result_buffer"]->set(perspective.image);

                    rtx_context["sysTopObject"]->set(root_node);

                    try {
                        rtx_context->validate();
                        rtx_context->launch(0, perspective.width, perspective.height);
                    } catch (optix::Exception& e) {
                        spdlog::error(e.getErrorString());
                    }

                    rt_buffer_to_gl(perspective.image, gl_tex_id);

                    glUseProgram(program);

                    float vertices[] = {
                            // Positions          // Colors           // Texture Coords
                            -1.0f,  -1.0f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // Top Right
                            -1.0f, 1.0f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // Bottom Right
                            1.0f, 1.0f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // Bottom Left
                            1.0f,  -1.0f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // Top Left
                    };

                    unsigned int indices[] = {
                            0, 1, 3, // first triangle
                            1, 2, 3  // second triangle
                    };

                    glBindVertexArray(VAO);

                    glBindBuffer(GL_ARRAY_BUFFER, VBO);
                    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

                    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

                    // position attribute
                    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0);
                    glEnableVertexAttribArray(0);
                    // color attribute
                    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float)));
                    glEnableVertexAttribArray(1);
                    // texture coord attribute
                    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float)));
                    glEnableVertexAttribArray(2);

                    auto texLoc = glGetUniformLocation(program, "texture1");
                    glUniform1i(texLoc, 0);
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, gl_tex_id);

                    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

                    glClear(GL_DEPTH_BUFFER_BIT);
                }


            }
        }
    }

    OptixDrawing::~OptixDrawing() {
        for (int i = 0; i < cameras.size(); ++i) {
            if(cameras.get_state(i)){
                remove_perspective(i);
            }
        }

        rtx_context->destroy();

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    }

    size_t OptixDrawing::add_perspective(Camera *camera, glm::uvec2 size, bool window_bound, std::string raygen_program, std::string miss_program, std::string exception_program) {
        OptixDrawingPerspective p;
        p.window_bound = window_bound;
        p.raygen_program = raygen_program;
        p.miss_program = miss_program;
        p.exception_program = exception_program;

        p.width = size.x;
        p.height = size.y;

        glGenBuffers(1, &p.texture_id);

        size_t idx = perspectives.add(p);
        cameras.add(camera);

        perspectives.get_data()[idx].id = idx;

        resize_perspective(idx, size);

        return idx;
    }

    void OptixDrawing::remove_perspective(size_t idx) {
        glDeleteBuffers(1, &(perspectives.get_data()[idx].texture_id));

        cameras.remove(idx);
        perspectives.remove(idx);
    }

    void OptixDrawing::resize_perspective(size_t idx, glm::uvec2 new_size) {
        if(perspectives.get_state(idx)){
            auto p = perspectives.get(idx);
            p.width = new_size.x;
            p.height = new_size.y;

            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, p.texture_id);
            glBufferData(GL_PIXEL_UNPACK_BUFFER, p.width * p.height * sizeof(float) * 4, (void*) 0, GL_STREAM_READ);
            glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

            p.image = rtx_context->createBufferFromGLBO(RT_BUFFER_OUTPUT, p.texture_id);
            p.image->setFormat(RT_FORMAT_FLOAT4);
            p.image->setSize(p.width, p.height);

            perspectives.set(idx, p);
            spdlog::info("Optix perspective " + std::to_string(idx) + " resizing -> "
            + std::to_string(new_size.x) + " " + std::to_string(new_size.x));

        }
    }

    std::vector<optix::GeometryGroup> OptixDrawing::extract_geometry_groups(aiScene *scene) {
        std::vector<optix::GeometryGroup> geom_groups;

        for (int i = 0; i < scene->mNumMeshes; ++i) {
            optix::GeometryTriangles gt = rtx_context->createGeometryTriangles();
            spdlog::info("============= MESH =============");
            spdlog::info(std::string(scene->mMeshes[i]->mName.C_Str()));

            void* dst;

            std::vector<glm::uvec3> indices;
            for (int j = 0; j < scene->mMeshes[i]->mNumFaces; ++j) {
                if(scene->mMeshes[i]->mFaces[j].mNumIndices == 3){
					auto face = scene->mMeshes[i]->mFaces[j];
					glm::uvec3 indice = { face.mIndices[0], face.mIndices[1], face.mIndices[2] };
					indices.push_back(indice);
                }

            }

            optix::Buffer indicesBuffer = rtx_context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_UNSIGNED_INT3, indices.size());
            dst = indicesBuffer->map(0, RT_BUFFER_MAP_WRITE);
            memcpy(dst, indices.data(), sizeof(glm::uvec3) * indices.size());
            indicesBuffer->unmap();

            spdlog::info("Triangle count: " + std::to_string(indices.size()));

            optix::Buffer vertexBuffer = rtx_context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, scene->mMeshes[i]->mNumVertices);
            dst = vertexBuffer->map(0, RT_BUFFER_MAP_WRITE);
            memcpy(dst, scene->mMeshes[i]->mVertices, sizeof(glm::vec3) * scene->mMeshes[i]->mNumVertices);
            vertexBuffer->unmap();

            spdlog::info("Vertex count: " + std::to_string(scene->mMeshes[i]->mNumVertices));

            optix::Buffer normalBuffer = rtx_context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, scene->mMeshes[i]->mNumVertices);
            dst = normalBuffer->map(0, RT_BUFFER_MAP_WRITE);
            memcpy(dst, scene->mMeshes[i]->mNormals, sizeof(glm::vec3) * scene->mMeshes[i]->mNumVertices);
            normalBuffer->unmap();

            optix::Buffer uvBuffer = rtx_context->createBuffer(RT_BUFFER_INPUT, RT_FORMAT_FLOAT3, scene->mMeshes[i]->mNumVertices);
            dst = uvBuffer->map(0, RT_BUFFER_MAP_WRITE);
            memcpy(dst, scene->mMeshes[i]->mTextureCoords, sizeof(glm::vec3) * scene->mMeshes[i]->mNumVertices);
            uvBuffer->unmap();

            gt->setTriangleIndices(indicesBuffer, RT_FORMAT_UNSIGNED_INT3);
            gt->setPrimitiveCount(indices.size());
            gt->setVertices(scene->mMeshes[i]->mNumVertices, vertexBuffer, RT_FORMAT_FLOAT3);
            gt->setBuildFlags(RTgeometrybuildflags(0));
            gt->setAttributeProgram(program_space["triangle_attributes"]);

            optix::GeometryGroup gg = rtx_context->createGeometryGroup();
            gg->setAcceleration(rtx_context->createAcceleration("Trbvh"));

            gg->getAcceleration()->setProperty("vertex_buffer_name", "vertexBuffer");
            gg->getAcceleration()->setProperty("vertex_buffer_stride", "12");

            gg->getAcceleration()->setProperty("index_buffer_name", "indicesBuffer");
            gg->getAcceleration()->setProperty("index_buffer_stride", "12");

            gg->getAcceleration()->markDirty();

            optix::Material mat = rtx_context->createMaterial();
            mat->setClosestHitProgram(0, program_space["chit"]);

            optix::GeometryInstance gi = rtx_context->createGeometryInstance();
            gi->setGeometryTriangles(gt);
            gi->setMaterialCount(1);
            gi->setMaterial(0, mat);

            gt["vertexBuffer"]->set(vertexBuffer);
            gt["indicesBuffer"]->set(indicesBuffer);
            gt["normalBuffer"]->set(normalBuffer);
            //gt["uvBuffer"]->set(uvBuffer);

            gg->addChild(gi);
            geom_groups.push_back(gg);
        }


        return geom_groups;
    }

    size_t OptixDrawing::add_scene(aiScene *scene) {
        auto i = scenes.size();
        scenes.push_back(extract_geometry_groups(scene));
        return i;
    }

    optix::Transform OptixDrawing::configure_context(HiveEngine::Context* physics_context) {
        optix::Transform transform = rtx_context->createTransform();
        glm::mat4 mat = glm::mat4(physics_context->get_rotation());
        glm::vec3 pos = glm::vec3(physics_context->get_position());

        if(!physics_context->parent) pos = -pos;

        mat[3] = glm::vec4(pos, 1.0);

        spdlog::info("transform: " + dvec3_to_str(physics_context->get_position()));
        optix::Matrix4x4 matrixPlane(glm::value_ptr(mat));
        transform->setMatrix(true, matrixPlane.getData(), matrixPlane.inverse().getData());

        optix::Group group = rtx_context->createGroup();

        spdlog::info("Context: " + physics_context->name + " || repr count: " + std::to_string(physics_context->representations.size()));
        for (ContextRepresentation repr: physics_context->representations) {
            group->addChild(scenes[repr.scene_id][repr.mesh_id]);
            spdlog::info("Adding repr: " + std::to_string(repr.scene_id) + " -> " + std::to_string(repr.mesh_id));
        }

        for (int i = 0; i < physics_context->entity_mass.size(); ++i) {
            if(physics_context->contexts.get_state(i) && physics_context->contexts.get(i) != nullptr){
                group->addChild(configure_context(physics_context->contexts.get(i)));
            }
        }

        group->setAcceleration(rtx_context->createAcceleration("Trbvh"));
        group->getAcceleration()->markDirty();
        transform->setChild(group);

        return transform;
    }
}