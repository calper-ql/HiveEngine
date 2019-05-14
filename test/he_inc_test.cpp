//
// Created by calper on 4/18/19.
//

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/DynamicSphere.h>
#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/StandardDirective.h>
#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Renderer/ImageDrawing.h>

#include <random>

int main(int argc, char* argv[]){
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
    << "." << HiveEngine::get_minor_version() << std::endl;

    HiveEngineRenderer::Camera camera;
    camera.set_position({0.0, 0.0, -3.30});

    HiveEngineRenderer::Context context;
    auto test_directive = new HiveEngineRenderer::StandardDirective(&context);
    auto line_drawing = new HiveEngineRenderer::LineDrawing(test_directive, &camera);
    auto image_drawing = new HiveEngineRenderer::ImageDrawing(test_directive);

    test_directive->register_drawing(line_drawing);
    test_directive->register_drawing(image_drawing);

    std::default_random_engine g;
    std::uniform_real_distribution<float> d(-0.3f, 0.3f);
    std::uniform_real_distribution<float> c(0.0f, 1.0f);

    glm::vec3 last_point = {-0.9, d(g), d(g)};
    glm::vec4 last_color = {c(g), c(g), c(g), 1.0f};
    int size = 1000;
    for (int i = 0; i < size; ++i) {
        glm::vec3 next_point = last_point + glm::vec3(1.8f/(float)size, d(g), d(g));
        glm::vec4 next_color = {c(g), c(g), c(g), 1.0f};

        float clamp = 0.9;
        if(next_point.x < -clamp) next_point.x = -clamp;
        if(next_point.y < -clamp) next_point.y = -clamp;
        if(next_point.z < -clamp) next_point.z = -clamp;
        if(next_point.x > clamp) next_point.x = clamp;
        if(next_point.y > clamp) next_point.y = clamp;
        if(next_point.z > clamp) next_point.z = clamp;

        HiveEngine::Line line;

        line.a.position = last_point;
        line.a.color = last_color;

        line.b.position = next_point;
        line.b.color = next_color;

        line_drawing->line_buffer.add(line);

        last_point = next_point;
        last_color = next_color;

    }

    for (int i = 250; i < 500; ++i) {
        line_drawing->line_buffer.remove(i);
    }

    image_drawing->add_image(-1, {-0.5, 0.0, 0.0}, 0.4, 0.4);
    image_drawing->add_image(0, {0.5, 0.5, 0.0}, 0.4, 0.4);

    for(int i = 0; i < 15; ++i){
        image_drawing->add_image(i%2==0?-1:-2, {0.0+i/(float)20, 0.0, 0.0}, 0.2, 0.2);
    }



    try {
        context.init_window();
        context.init_vulkan();

        while(!glfwWindowShouldClose(context.get_window())){
            camera.set_perspective(90, HiveEngineRenderer::get_window_ratio(context.get_window()), 0.0001, 1000.0);
            camera.get_user_input(context.get_window());
            camera.get_user_movement(context.get_window());
            glfwPollEvents();
            context.main_loop();
        }

        context.wait_device();
        context.cleanup();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}