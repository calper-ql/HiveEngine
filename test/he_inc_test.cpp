//
// Created by calper on 4/18/19.
//

#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/DynamicSphere.h>
#include <HiveEngine/Renderer/Context.h>

int main(int argc, char* argv[]){
    std::cout << "HiveEngine version: " << HiveEngine::get_major_version()
    << "." << HiveEngine::get_minor_version() << std::endl;

    HiveEngineRenderer::Context context;

    try {
        context.run();
    } catch (const std::exception& e){
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}