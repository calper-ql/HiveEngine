//
// Created by calpe on 6/18/2019.
//

#include "HiveEngine/Renderer/OptixDrawing.h"

namespace HiveEngine::Renderer {


    OptixDrawing::OptixDrawing(Context *context, std::string program_space_path) : Drawing(context) {
        rtx_context = optix::Context::create();

        rtx_context->setStackSize(1024*8);
        rtx_context->setPrintEnabled(true);
        rtx_context->setExceptionEnabled(RT_EXCEPTION_ALL, true);

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
    }

    void OptixDrawing::draw() {
        Drawing::draw();

    }

    OptixDrawing::~OptixDrawing() {

    }
}