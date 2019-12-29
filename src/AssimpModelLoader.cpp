
#include <assimp/Importer.hpp>
#include <assimp/Exporter.hpp>
#include <assimp/postprocess.h>

#include "AssimpModelLoader.h"

Model AssimpModelLoader::loadModel(const std::string& filename)
{
	std::vector<Mesh> meshes;

	Assimp::Importer import;
	const aiScene* scene = import.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

	if (scene == nullptr || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || scene->mRootNode == nullptr)
	{
		return Model();
	}

	meshes = processNode(scene->mRootNode, scene);

	return Model(meshes, nullptr, nullptr, true);
}

std::vector<std::string> AssimpModelLoader::getImportExtensions()
{
	Assimp::Importer importer;
	std::string szOut;
	importer.GetExtensionList(szOut);

	std::vector<std::string> extensions;

	size_t offset = 0;
	while (offset < szOut.size())
	{
		size_t end = szOut.find(';', offset);

		if (end == std::string::npos)
		{
			std::string extension = szOut.substr(offset);
			extensions.push_back(extension);

			offset = szOut.size();
		}
		else
		{
			std::string extension = szOut.substr(offset, end - offset);
			extensions.push_back(extension);

			offset = end + 1;
		}
	}

	return extensions;
}

std::vector<Mesh> AssimpModelLoader::processNode(aiNode* node, const aiScene* scene)
{
	std::vector<Mesh> meshes;
	meshes.reserve(node->mNumMeshes);

	for (unsigned int i = 0; i < node->mNumMeshes; ++i)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		meshes.push_back(processMesh(mesh, scene));
	}

	for (unsigned int i = 0; i < node->mNumChildren; ++i)
	{
		std::vector<Mesh> meshes2 = processNode(node->mChildren[i], scene);
		meshes.insert(meshes.end(), meshes2.begin(), meshes2.end());
	}

	return meshes;
}

Mesh AssimpModelLoader::processMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<Vector> vertices;
	vertices.reserve(mesh->mNumVertices);

	std::vector<Vector> normals;
	normals.reserve(mesh->mNumVertices);

	for (unsigned int i = 0; i < mesh->mNumVertices; ++i)
	{
		Vector vertex;
		vertex.m_x = mesh->mVertices[i].x;
		vertex.m_y = mesh->mVertices[i].y;
		vertex.m_z = mesh->mVertices[i].z;
		vertices.push_back(vertex);

		Vector normal;
		normal.m_x = mesh->mNormals[i].x;
		normal.m_y = mesh->mNormals[i].y;
		normal.m_z = mesh->mNormals[i].z;
		normals.push_back(normal);
	}

	std::vector<Triangle> triangles;
	triangles.reserve(mesh->mNumFaces);

	for (unsigned int i = 0; i < mesh->mNumFaces; ++i)
	{
		aiFace face = mesh->mFaces[i];
		Triangle triangle = Triangle(vertices[face.mIndices[0]], vertices[face.mIndices[1]], vertices[face.mIndices[2]],
			normals[face.mIndices[0]], normals[face.mIndices[1]], normals[face.mIndices[2]], nullptr, nullptr);

		triangles.push_back(triangle);
	}

	return Mesh(triangles, nullptr, nullptr, true);
}

