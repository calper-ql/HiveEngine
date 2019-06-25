//
// Created by calpe on 6/21/2019.
//

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Texture.h>

#include <iostream>

#include <HiveEngine/Renderer/Context.h>
#include <HiveEngine/Renderer/GlyphDrawing.h>
#include <HiveEngine/Renderer/FontManager.h>
#include <HiveEngine/Renderer/TextDrawing.h>
#include <HiveEngine/Renderer/Camera.h>
#include <HiveEngine/Renderer/LineDrawing.h>

#include <random>

int main(int argc, char* argv[]){
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
              << "." << HiveEngine::get_minor_version() << std::endl;

    HiveEngine::Renderer::Camera camera;
    camera.set_position({0.0, 0.0, -3.30});

    auto statue_texture = HiveEngine::load_texture("../data/statue.jpg");
    std::cout << "Statue texture:" << std::endl;
    std::cout << "width:   " << statue_texture.width << std::endl;
    std::cout << "height:  " << statue_texture.height << std::endl;
    std::cout << "channel: " << statue_texture.channel << std::endl;

    HiveEngine::Renderer::Context context(true);
    context.init_window({1400, 1000}, true);
    context.load_shaders("../shaders");

    HiveEngine::Renderer::FontManager font_manager;
    font_manager.load_font("TheExpanse", "../data/fonts/TheExpanse.ttf");
    font_manager.load_font("ffdin", "../data/fonts/ffdin.ttf");

    HiveEngine::Renderer::TextDrawing ffdin_font(&context, &font_manager, "ffdin");
    ffdin_font.add_text("The Expanse |/'!", {0.0, 0.2, 0.0}, 0.1,
                        HiveEngine::Renderer::TextDescriptionState::CENTER, {0.0, 1.0, 0.0, 1.0});

    HiveEngine::Renderer::TextDrawing expanse_font(&context, &font_manager, "TheExpanse");
    expanse_font.add_text("The Expanse |/'!", {0.0, -0.2, 0.0}, 0.1, HiveEngine::Renderer::TextDescriptionState::CENTER);

    HiveEngine::Renderer::LineDrawing line_drawing(&context);
    line_drawing.add_line({1.0, 0.0, 0.0}, {1.0, 0.0, 0.0, 1.0}, {-1.0, 0.0, 0.0}, {0.0, 0.0, 1.0, 1.0});
    line_drawing.add_line({1.0, -1.0, 0.0}, {1.0, 0.0, 1.0, 1.0}, {-1.0, 1.0, 0.0}, {0.0, 1.0, 0.0, 1.0});

    line_drawing.add_line({1.0, 0.3, 0.0}, {1.0, 0.0, 0.0, 1.0}, {-1.0, 0.3, 0.0}, {0.0, 0.0, 1.0, 1.0});
    line_drawing.add_line({1.0, 0.2, 0.0}, {1.0, 0.0, 0.0, 1.0}, {-1.0, 0.2, 0.0}, {0.0, 0.0, 1.0, 1.0});



    while(context.draw()){

    }

    return EXIT_SUCCESS;
}