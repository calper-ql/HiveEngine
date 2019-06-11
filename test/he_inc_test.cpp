//
// Created by calper on 4/18/19.
//

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/DynamicSphere.h>
#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/StandardDirective.h>
#include <HiveEngine/Renderer/LineDrawing.h>
#include <HiveEngine/Renderer/GlyphDrawing.h>
#include <HiveEngine/Renderer/FontManager.h>
#include <HiveEngine/Renderer/TextDrawing.h>

#include <random>

int main(int argc, char* argv[]){
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
    << "." << HiveEngine::get_minor_version() << std::endl;

    HiveEngineRenderer::Camera camera;
    camera.set_position({0.0, 0.0, -3.30});

    auto statue_texture = HiveEngine::load_texture("../data/statue.jpg");
    std::cout << "Statue texture:" << std::endl;
    std::cout << "width:   " << statue_texture.width << std::endl;
    std::cout << "height:  " << statue_texture.height << std::endl;
    std::cout << "channel: " << statue_texture.channel << std::endl;

    HiveEngineRenderer::Context context;
    auto test_directive = new HiveEngineRenderer::StandardDirective(&context);
    auto line_drawing = new HiveEngineRenderer::LineDrawing(test_directive, &camera);

    HiveEngineRenderer::FontManager font_manager;
    font_manager.load_font("expanse", "../data/TheExpanse.ttf");
    font_manager.load_font("ffdin", "../data/ffdin.ttf");

    auto glyph = font_manager.get_glyph("ffdin", 'g');
    auto font_drawing = new HiveEngineRenderer::GlyphDrawing(test_directive, glyph.texture);

    auto text_drawing = new HiveEngineRenderer::TextDrawing(test_directive, &font_manager, "ffdin");

    test_directive->register_drawing(line_drawing);
    test_directive->register_drawing(font_drawing);
    test_directive->register_drawing(text_drawing);

    text_drawing->add_text_center("The Expanse 131 ,.,^%$", {0.0, -0.9, 0.0}, 0.1);
    text_drawing->add_text_center("The Expanse", {0.0, 0.0, 0.0}, 0.1);

    //font_drawing->add_image({-0.6, 0.0, 0.0}, glyph.texture.width/1000.0, glyph.texture.height/1000.0, {0.0, 1.0, 0.0, 1.0});

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


    try {
        context.init_window();
        context.init_vulkan();

        while(!glfwWindowShouldClose(context.get_window())){
            camera.set_perspective(90, HiveEngineRenderer::get_window_ratio(context.get_window()), 0.0001, 1000.0);
            camera.get_user_input(context.get_window());
            camera.get_user_movement(context.get_window());
            glfwPollEvents();
            context.main_loop();

            for (int i = 0; i < 2; ++i) {
                auto size = line_drawing->line_buffer.size();
                line_drawing->line_buffer.remove(rand()%size);
            }

            for (int i = 0; i < 1; ++i) {
                HiveEngine::Line line;
                glm::vec4 next_color = {c(g), c(g), c(g), 1.0f};
                line.a.position = glm::vec3(d(g), d(g), d(g));
                line.b.position = glm::vec3(d(g), d(g), d(g));
                line.a.color = next_color;
                line.b.color = next_color;
                line_drawing->line_buffer.add(line);
            }

        }

        context.wait_device();
        context.cleanup();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }



    return EXIT_SUCCESS;
}