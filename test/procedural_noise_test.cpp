//
// Created by calpe on 6/13/2019.
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

    HiveEngine::Renderer::Context context(true);
    context.init_window({1400, 1000}, true);
    context.load_shaders("../shaders");

    HiveEngine::Renderer::FontManager font_manager;
    font_manager.load_font("ffdin", "../data/fonts/ffdin.ttf");

    HiveEngine::Renderer::TextDrawing ffdin_font(&context, &font_manager, "ffdin");
    ffdin_font.add_text("The Expanse |/'!", {0.0, 0.2, 0.0}, 0.1);


    while(context.draw()){

    }

    return EXIT_SUCCESS;
}