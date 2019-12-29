#pragma once

#include <vector>

#include <assimp/scene.h>

#include "Model.h"

class AssimpModelLoader
{

public:

	static Model loadModel(const std::string& filename);

	static std::vector<std::string> getImportExtensions();

private:

	static std::vector<Mesh> processNode(aiNode* node, const aiScene* scene);

	static Mesh processMesh(aiMesh* mesh, const aiScene* scene);

};

