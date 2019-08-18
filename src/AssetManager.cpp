//
// Created by calpe on 8/4/2019.
//

#include <HiveEngine/AssetManager.h>
#include <HiveEngine/Context.h>

namespace HiveEngine {

	AssetManager::AssetManager() {

	}

    size_t AssetManager::load_file(std::string path) {
        auto size = scenes.size();

        Assimp::Importer importer;
        aiScene *scene = (aiScene *) importer.ReadFile(path.c_str(),
                                                       aiProcess_CalcTangentSpace | aiProcess_Triangulate |
                                                       aiProcess_JoinIdenticalVertices |
                                                       aiProcess_SortByPType);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            spdlog::error("Couldn't load scene: " + path);
            HiveEngine::process_error();
        }

        auto meshes = HiveEngine::ai_scene_to_meshes(scene);

        Scene sscene;
		sscene.context = new Context(nullptr, this);
        sscene.meshes = meshes;
        sscene.context->load_ai_node(scene, scene->mRootNode, size);
        scenes.push_back(sscene);

        return size;
    }

    AssetManager::~AssetManager() {
		for (size_t i = 0; i < scenes.size(); i++) {
			delete scenes[i].context;
		}
    }
}