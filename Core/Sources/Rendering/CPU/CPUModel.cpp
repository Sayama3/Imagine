//
// Created by ianpo on 08/07/2025.
//

#include "Imagine/Rendering/CPU/CPUModel.hpp"

#include "Imagine/Assets/AssetManager.hpp"
#include "Imagine/Components/Physicalisable.hpp"
#include "Imagine/Components/Renderable.hpp"
#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"
#include "Imagine/Scene/Scene.hpp"

#include "Imagine/ThirdParty/Assimp.hpp"
#include "Imagine/ThirdParty/Stb.hpp"

#define LOAD_ASSET(ASSET) Imagine::Project::GetActive()->GetAssetManager()->AddAsset(ASSET);

namespace Imagine {

	Ref<CPUModel> CPUModel::LoadModel(const Path &filePath, Scene *coreScene, EntityID parent) {
		auto model = LoadModel(filePath.GetFullPath(), coreScene, parent);
		if (model) {
			model->modelPath = filePath;
		}
		return model;
	}

	Ref<CPUModel> CPUModel::LoadModel(const std::filesystem::path &filePath, Scene *coreScene, EntityID parent) {
		MGN_PROFILE_FUNCTION();
		using namespace Imagine;
		Assimp::Importer importer;

		if (!coreScene) return nullptr;

		const aiScene *scene = importer.ReadFile(filePath.string(), ThirdParty::Assimp::GetSmoothPostProcess());

		// If the import failed, report it
		if (nullptr == scene) {
			MGN_CORE_ERROR(importer.GetErrorString());
			return nullptr;
		}


		Ref<CPUModel> model = CreateRef<CPUModel>();

		// TODO: Find shaders in another way
		auto vertex = CreateRef<CPUFileShader>(ShaderStage::Vertex, Path{FileSource::Assets, "pbr.vert.spv"});
		auto fragment = CreateRef<CPUFileShader>(ShaderStage::Fragment, Path{FileSource::Assets, "pbr.frag.spv"});
		model->Shaders.push_back(vertex);
		model->Shaders.push_back(fragment);
		LOAD_ASSET(vertex);
		LOAD_ASSET(fragment);


		model->modelPath = {FileSource::External, filePath};
		model->Textures.resize(scene->mNumTextures + 1);

		Ref<CPUTexture2D> ErrorCheckerboard;
		for (uint32_t i = 0; i < scene->mNumTextures; ++i) {
			aiTexture *pTexture = scene->mTextures[i];
			if (pTexture->pcData == nullptr) {
				if (!ErrorCheckerboard) {
					ErrorCheckerboard = CreateRef<CPUTexture2D>();
					ErrorCheckerboard->image.Allocate(16, 16, 4);
					const uint32_t black = glm::packUnorm4x8(glm::vec4(0, 0, 0, 1));
					const uint32_t magenta = glm::packUnorm4x8(glm::vec4(1, 0, 1, 1));
					for (int x = 0; x < 16; x++) {
						for (int y = 0; y < 16; y++) {
							ErrorCheckerboard->image(x, y).As<uint32_t>() = ((x % 2) ^ (y % 2)) ? magenta : black;
						}
					}
					LOAD_ASSET(ErrorCheckerboard);
				}

				model->Textures[i] = ErrorCheckerboard;
			}
			Image<> image;
			if (pTexture->mHeight == 0) {
				image = std::move(ThirdParty::Stb::Image::LoadFromMemory(ConstBufferView{pTexture->pcData, pTexture->mWidth}, 4));
			}
			else {
				image.Allocate(pTexture->mWidth, (uint32_t) pTexture->mHeight, 4u);
				uint8_t a{255};
				uint8_t r{255};
				uint8_t g{255};
				uint8_t b{255};
				if (pTexture->achFormatHint[0] == 0) {
					a = 0u;
					r = 1u;
					g = 2u;
					b = 3u;
				}
				else {
					for (uint32_t i = 0; i < 4; ++i) {
						switch (pTexture->achFormatHint[i]) {
							case 'r':
								r = pTexture->achFormatHint[4 + i] == '8' ? i : 255;
								break;
							case 'g':
								g = pTexture->achFormatHint[4 + i] == '8' ? i : 255;
								break;
							case 'b':
								b = pTexture->achFormatHint[4 + i] == '8' ? i : 255;
								break;
							case 'a':
								a = pTexture->achFormatHint[4 + i] == '8' ? i : 255;
								break;
							default:
								break;
						}
					}
				}

				for (uint32_t x = 0; x < pTexture->mWidth; ++x) {
					for (uint32_t y = 0; y < pTexture->mHeight; ++y) {
						uint8_t *aiPixel = (uint8_t *) &pTexture->pcData[x * pTexture->mHeight + y];
						BufferView pixel = image(x, y);
						pixel.At<uint8_t>(0u) = r != 255 ? aiPixel[r] : 0;
						pixel.At<uint8_t>(1u) = g != 255 ? aiPixel[g] : 0;
						pixel.At<uint8_t>(2u) = b != 255 ? aiPixel[b] : 0;
						pixel.At<uint8_t>(3u) = a != 255 ? aiPixel[a] : 255;
					}
				}
			}

			MGN_CORE_CASSERT(image.channels == 4, "[Vulkan] image.channels == 4");
			model->Textures[i] = CreateRef<CPUTexture2D>(std::move(image));
			LOAD_ASSET(model->Textures[i]);
		}

		Ref<CPUTexture2D> WhiteImage = CreateRef<CPUTexture2D>();
		WhiteImage->image.Allocate(1, 1, 4);
		WhiteImage->image(0, 0).As<uint32_t>() = glm::packUnorm4x8(glm::vec4(1, 1, 1, 1));
		model->Textures.back() = WhiteImage;
		LOAD_ASSET(WhiteImage);

		Ref<CPUMaterial> opaqueMaterial = CPUMaterial::GetDefaultOpaque();
		Ref<CPUMaterial> transparentMaterial = CPUMaterial::GetDefaultTransparent();

		LOAD_ASSET(opaqueMaterial);
		LOAD_ASSET(transparentMaterial);

		std::unordered_map<std::string, Ref<CPUTexture2D>> loadedTextures;

		for (int i = 0; i < scene->mNumMaterials; ++i) {
			auto instance = CreateRef<CPUMaterialInstance>();
			model->Instances.push_back(instance);


			aiMaterial *material = scene->mMaterials[i]; // for some formats (like glTF) metallic and roughness may be the same file

			ai_real opacity;
			aiReturn opacityReturn = material->Get(AI_MATKEY_OPACITY, opacity);

			if (opacityReturn == aiReturn_SUCCESS && opacity != 1)
				instance->Material = transparentMaterial->Handle;
			else
				instance->Material = opaqueMaterial->Handle;

			std::array<aiTextureType, 6> texTypes{aiTextureType_BASE_COLOR, aiTextureType_NORMALS, aiTextureType_EMISSIVE, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION};
			std::array<aiString, 6> imageFiles{}; // fileBaseColor, fileNormal, fileEmissive, fileMetallic, fileRoughness, fileAo;
			std::array<bool, 6> hasImage{}; // fileBaseColor, fileNormal, fileEmissive, fileMetallic, fileRoughness, fileAo;

			for (uint64_t texId = 0; texId < texTypes.size(); ++texId) {
				hasImage[texId] = material->GetTextureCount(texTypes[texId]) > 0;
				if (hasImage[texId]) {
					const CPUMaterialInstance::SetFieldPosition pos{1, (uint32_t) texId + 1, 0};
					material->GetTexture(texTypes[texId], 0, &imageFiles[texId]);
					std::pair<const aiTexture *, int> result = scene->GetEmbeddedTextureAndIndex(imageFiles[texId].C_Str());
					if (result.first) {
						instance->PushSet(pos, model->Textures[result.second]->Handle);
					}
					else {
						const std::filesystem::path fullPath = filePath.parent_path() / imageFiles[texId].C_Str();
						const std::string pathStr = fullPath.string();
						if (!loadedTextures.contains(pathStr)) {
							Image<> img = ThirdParty::Stb::Image::Load(pathStr.c_str(), 4);
							loadedTextures[pathStr] = CreateRef<CPUTexture2D>(std::move(img));
							model->Textures.push_back(loadedTextures[pathStr]);
							LOAD_ASSET(loadedTextures[pathStr]);
						}
						Ref<CPUTexture2D> tex = loadedTextures.at(pathStr);
						instance->PushSet(pos, tex->Handle);
					}
				}
			}

			ai_real metallic = 1, roughness = 1, emIntens = 1;
			aiColor3D tint = aiColor3D{1};
			aiColor3D emColor = aiColor3D{0};

			material->Get(AI_MATKEY_BASE_COLOR, tint);
			material->Get(AI_MATKEY_EMISSIVE_INTENSITY, emIntens);
			material->Get(AI_MATKEY_COLOR_EMISSIVE, emColor);
			material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
			material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);

			instance->PushSet({1, 0, 0}, glm::fvec4(tint.r, tint.g, tint.b, opacity));
			instance->PushSet({1, 0, 1}, glm::fvec4(emColor.r * emIntens, emColor.g * emIntens, emColor.b * emIntens, 1));
			if (imageFiles[3] == imageFiles[4]) {
				// GLTF Specific MetalRough Texture
				instance->PushSet({1, 0, 2}, glm::fvec3(0, 0, metallic));
				instance->PushSet({1, 0, 3}, glm::fvec3(0, roughness,0));
			} else {
				// Other where R is each of the data.
				instance->PushSet({1, 0, 2}, glm::fvec3(metallic,0,0));
				instance->PushSet({1, 0, 3}, glm::fvec3(roughness,0,0));
			}

			LOAD_ASSET(instance);
		}

		model->Meshes.reserve(scene->mNumMeshes);

		// Load the meshes
		{
			// use the same vectors for all meshes so that the memory doesnt reallocate as
			// often
			std::vector<uint32_t> indices;
			std::vector<Vertex> vertices;

			for (uint64_t i = 0; i < scene->mNumMeshes; ++i) {
				const aiMesh *aiMesh = scene->mMeshes[i];
				if (!aiMesh->HasPositions()) {
					continue;
				}

				Ref<CPUMesh> mesh = CreateRef<CPUMesh>();
				model->Meshes.push_back(mesh);
				mesh->Name = aiMesh->mName.C_Str();

				indices.clear();
				vertices.clear();

				vertices.reserve(aiMesh->mNumVertices);
				for (uint64_t i = 0; i < aiMesh->mNumVertices; ++i) {
					vertices.emplace_back();
					const auto &pos = aiMesh->mVertices[i];
					vertices[i].position = {pos.x, pos.y, pos.z};

					if (aiMesh->HasNormals()) {
						const auto &normal = aiMesh->mNormals[i];
						vertices[i].normal = {normal.x, normal.y, normal.z};
					}

					if (aiMesh->HasTextureCoords(0)) {
						const auto &texCoords = aiMesh->mTextureCoords[0][i];
						vertices[i].uv_x = texCoords.x;
						vertices[i].uv_y = texCoords.y;
					}

					if (aiMesh->HasTangentsAndBitangents()) {
						vertices[i].tangent = {aiMesh->mTangents[i].x, aiMesh->mTangents[i].y, aiMesh->mTangents[i].z, 0};
						vertices[i].bitangent = {aiMesh->mBitangents[i].x, aiMesh->mBitangents[i].y, aiMesh->mBitangents[i].z, 0};
					}

					if (aiMesh->HasVertexColors(0)) {
						const auto &color = aiMesh->mColors[0][i];
						vertices[i].color = {color.r, color.g, color.b, color.a};
					}
				}

				indices.reserve(aiMesh->mNumFaces * 3);
				for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
					MGN_CORE_CASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace &face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				mesh->Vertices = vertices;
				mesh->Indices = indices;

				LOD surface;
				surface.index = 0;
				surface.count = indices.size();
				surface.materialInstance = model->Instances[aiMesh->mMaterialIndex]->Handle;

				mesh->Lods.push_back(surface);
				LOAD_ASSET(mesh);
			}
		}

		// Load the nodes as entity in the scene.
		{
			// Unwrapping everything in a vector to avoid recursion.
			std::vector<std::tuple<aiNode *, EntityID>> nodes{{scene->mRootNode, parent}};
			nodes.reserve(scene->mNumMeshes);
			while (!nodes.empty()) {
				// Pop the last node available
				auto [node, parentEntityID] = nodes.back();
				nodes.pop_back();

				const char *cName = node->mName.C_Str();

				const aiMatrix4x4 &localMatrix = node->mTransformation; // * parentMatrix;

				// Going from ASSIMP to GLM is going from row major to column major.
				//  Therefore, I create the glm matrix by rotating the matrix and
				//  setting the data properly using the constructor with 4 column
				//  (or vec4 basically) to be explicit and not have any issue later on.
				const glm::mat4 glmLocalMatrix{
						{localMatrix.a1, localMatrix.b1, localMatrix.c1, localMatrix.d1}, // Col 1
						{localMatrix.a2, localMatrix.b2, localMatrix.c2, localMatrix.d2}, // col 2
						{localMatrix.a3, localMatrix.b3, localMatrix.c3, localMatrix.d3}, // col 3
						{localMatrix.a4, localMatrix.b4, localMatrix.c4, localMatrix.d4}, // col 4
				};
				glm::vec3 pos;
				glm::quat rot;
				glm::vec3 scale;
				glm::vec3 skew;
				glm::vec4 perspective;
				glm::decompose(glmLocalMatrix, scale, rot, pos, skew, perspective);

				MGN_CORE_CHECK(Math::Approx(skew, glm::vec3(0)), "skew transformation not supported.");
				MGN_CORE_CHECK(Math::Approx(perspective, glm::vec4(0, 0, 0, 1)), "perspective transformation not supported.");

				if (scale.x != scale.y || scale.y != scale.z) {
					MGN_CORE_WARN("The scale of the node {} in the model {} is non-uniform. It's preferred to have a uniform scale of 1.", cName, filePath.string());
				}
				else if (Math::Magnitude2(scale) != 1) {
					// MGN_CORE_WARN("The scale of the node {} in the model {} is not 1. It's preferred to have a uniform scale of 1.");
				}

				EntityID entityId = coreScene->CreateEntity(parentEntityID);
				coreScene->SetName(entityId, node->mName.C_Str());
				coreScene->GetEntity(entityId).LocalPosition = pos;
				coreScene->GetEntity(entityId).LocalRotation = rot;
				coreScene->GetEntity(entityId).LocalScale = scale;

				if (node->mNumMeshes == 1) {
					AssetHandle meshHandle = model->Meshes[*node->mMeshes]->Handle;
					coreScene->AddComponent<Renderable>(entityId)->cpuMesh = meshHandle;
					// Physicalisable* p = coreScene->AddComponent<Physicalisable>(entityId);
					// p->Shape =  ColliderShapes::Mesh{AssetField<CPUMesh>{meshHandle}};
					// p->RBType = RB_Static;
				}
				else if (node->mNumMeshes > 1) {
					for (int i = 0; i < node->mNumMeshes; ++i) {
						const auto &pMesh = model->Meshes[node->mMeshes[i]];
						const AssetHandle meshHandle = pMesh->Handle;
						EntityID meshChild = coreScene->CreateEntity(entityId);
						coreScene->SetName(meshChild, pMesh->Name);
						coreScene->AddComponent<Renderable>(meshChild)->cpuMesh = meshHandle;
						// Physicalisable* p = coreScene->AddComponent<Physicalisable>(meshChild);
						// p->Shape =  ColliderShapes::Mesh{AssetField<CPUMesh>{meshHandle}};
						// p->RBType = RB_Static;
					}
				}


				for (int i = 0; i < node->mNumChildren; ++i) {
					aiNode *child = node->mChildren[i];
					nodes.push_back({child, entityId});
				}
			}
		}

		for (uint32_t i = 0; i < scene->mNumLights; ++i) {
			aiLight* pLight = scene->mLights[i];
			switch (pLight->mType) {
				case aiLightSource_DIRECTIONAL: {
					EntityID entityId = coreScene->CreateEntity(parent);
					coreScene->SetName(entityId, std::string{"Directional Light - "} + pLight->mName.C_Str());
					Entity& entity = coreScene->GetEntity(entityId);
					Light* light = coreScene->AddComponent<Light>(entityId);
					if(!light) continue;
					light->type = LIGHT_DIRECTIONAL;
					entity.LocalPosition = Vec3(pLight->mPosition.x,pLight->mPosition.y,pLight->mPosition.z);
					entity.LocalRotation = glm::rotation(Vec3{0,-1,0},Vec3{pLight->mDirection.x,pLight->mDirection.y,pLight->mDirection.z});
					light->color = Vec4{pLight->mColorDiffuse.r,pLight->mColorDiffuse.g,pLight->mColorDiffuse.b,1};
				}
					break;
				case aiLightSource_POINT: {
					EntityID entityId = coreScene->CreateEntity(parent);
					coreScene->SetName(entityId, std::string{"Point Light - "} + pLight->mName.C_Str());
					Entity& entity = coreScene->GetEntity(entityId);
					Light* light = coreScene->AddComponent<Light>(entityId);
					if(!light) continue;
					light->type = LIGHT_POINT;
					entity.LocalPosition = Vec3(pLight->mPosition.x,pLight->mPosition.y,pLight->mPosition.z);
					entity.LocalRotation = glm::rotation(Vec3{0,-1,0},Vec3{pLight->mDirection.x,pLight->mDirection.y,pLight->mDirection.z});
					light->direction.x = 50;
					light->color = Vec4{pLight->mColorDiffuse.r,pLight->mColorDiffuse.g,pLight->mColorDiffuse.b,1};
				}
					break;
				case aiLightSource_SPOT: {
					EntityID entityId = coreScene->CreateEntity(parent);
					coreScene->SetName(entityId, std::string{"Spot Light - "} + pLight->mName.C_Str());
					Entity& entity = coreScene->GetEntity(entityId);
					Light* light = coreScene->AddComponent<Light>(entityId);
					if(!light) continue;
					light->type = LIGHT_SPOT;
					light->direction.w = pLight->mAngleOuterCone;
					light->direction.x = 50;
					entity.LocalPosition = Vec3(pLight->mPosition.x,pLight->mPosition.y,pLight->mPosition.z);
					entity.LocalRotation = glm::rotation(Vec3{0,-1,0},Vec3{pLight->mDirection.x,pLight->mDirection.y,pLight->mDirection.z});
					light->color = Vec4{pLight->mColorDiffuse.r,pLight->mColorDiffuse.g,pLight->mColorDiffuse.b,1};
				}
					break;
				default:
					continue;
			}
		}

		LOAD_ASSET(model);
		return model;
	}
} // namespace Imagine
