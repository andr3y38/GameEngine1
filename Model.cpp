#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include "Model.h"

namespace GE {

	bool Model::loadFromFile(const char* filename) {
		std::vector<Vertex> loadedVertices;

		Assimp::Importer imp;

		const aiScene* pScene = imp.ReadFile(filename, aiProcessPreset_TargetRealtime_Quality | aiProcess_FlipUVs);

		if (!pScene) {
			return false;
		}
		for (int MeshIdx = 0; MeshIdx < pScene->mNumMeshes; MeshIdx++) {
			const aiMesh* mesh = pScene->mMeshes[MeshIdx];
			for (int faceIdx = 0; faceIdx < mesh->mNumFaces; faceIdx++) {
				const aiFace& face = mesh->mFaces[faceIdx];




				for (int vertIdx = 0; vertIdx < 3; vertIdx++) {
					const aiVector3D* pos = &mesh->mVertices[face.mIndices[vertIdx]];
					const aiVector3D uv = mesh->mTextureCoords[0][face.mIndices[vertIdx]];
					loadedVertices.push_back(Vertex(pos->x, pos->y, pos->z, uv.x, uv.y));
					
				}
			}
		}

		numVertices = loadedVertices.size();

		vertices = new Vertex[numVertices];
		std::copy(loadedVertices.begin(), loadedVertices.end(), vertices);

		return true;


	}


}