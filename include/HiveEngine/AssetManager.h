//
// Created by calpe on 8/4/2019.
//

#ifndef HIVEENGINE_ASSETMANAGER_H
#define HIVEENGINE_ASSETMANAGER_H

#include <HiveEngine/Mesh.h>
#include <HiveEngine/Common.h>
#include <HiveEngine/HiveEngine.h>
#include <HiveEngine/Utilities.h>

namespace HiveEngine {
    class Context;

    struct Scene {
		std::vector<Mesh> meshes;
		Context* context;
	};

    class AssetManager {
    public:
        std::vector<Scene> scenes;

		AssetManager();

        size_t load_file(std::string path);

        ~AssetManager();
    };
}

#endif //HIVEENGINE_ASSETMANAGER_H
