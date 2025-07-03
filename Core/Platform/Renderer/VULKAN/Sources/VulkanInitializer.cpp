//
// Created by Iannis on 10/06/2025.
//


#include "Imagine/ThirdParty/Assimp.hpp"
#include "Imagine/ThirdParty/Stb.hpp"

#include "Imagine/Components/Renderable.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/Math.hpp"
#include "Imagine/Scene/Scene.hpp"

#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include "Imagine/Vulkan/VulkanRenderer.hpp"
#include "Imagine/Vulkan/VulkanImage.hpp"


namespace Imagine::Vulkan {
	namespace Initializer {
		constexpr bool c_OverrideColorVertex = true;

		void LoadModelAsDynamic(VulkanRenderer *renderer, Core::Scene *coreScene, Core::EntityID parent, const std::filesystem::path &filePath) {
			using namespace Imagine::Core;
			Assimp::Importer importer;

			const aiScene *scene = importer.ReadFile(filePath.string(), ThirdParty::Assimp::GetSmoothPostProcess());


			// std::vector<AllocatedImage> images;
			// std::vector<AutoDeleter::Shared> deleters;
			// images.resize(scene->mNumTextures);
			// deleters.resize(scene->mNumTextures);
			//
			// for (uint32_t i = 0; i < scene->mNumTextures; ++i) {
			// 	aiTexture* pTexture = scene->mTextures[i];
			// 	if (pTexture->pcData == nullptr) {
			// 		images[i] = renderer->GetWhiteImage();
			// 	}
			// 	Core::Image<> image;
			// 	if (pTexture->mHeight == 0) {
			// 		image = std::move(ThirdParty::Stb::Image::LoadFromMemory(ConstBufferView{pTexture->pcData, pTexture->mWidth}, 4));
			// 	}
			// 	else {
			// 		image.Allocate(pTexture->mWidth, (uint32_t)pTexture->mHeight, 4u);
			// 		uint8_t a{255};
			// 		uint8_t r{255};
			// 		uint8_t g{255};
			// 		uint8_t b{255};
			// 		if (pTexture->achFormatHint[0] == 0) {
			// 			a = 0u;
			// 			r = 1u;
			// 			g = 2u;
			// 			b = 3u;
			// 		} else {
			// 			for (uint32_t i = 0; i < 4; ++i) {
			// 				switch (pTexture->achFormatHint[i]) {
			// 					case 'r': r = pTexture->achFormatHint[4+i] == '8' ? i : 255; break;
			// 					case 'g': g = pTexture->achFormatHint[4+i] == '8' ? i : 255; break;
			// 					case 'b': b = pTexture->achFormatHint[4+i] == '8' ? i : 255; break;
			// 					case 'a': a = pTexture->achFormatHint[4+i] == '8' ? i : 255; break;
			// 					default: break;
			// 				}
			// 			}
			// 		}
			//
			// 		for (uint32_t x = 0; x < pTexture->mWidth; ++x) {
			// 			for (uint32_t y = 0; y < pTexture->mHeight; ++y) {
			// 				uint8_t* aiPixel = (uint8_t*)&pTexture->pcData[x * pTexture->mHeight + y];
			// 				BufferView pixel = image(x,y);
			// 				pixel.At<uint8_t>(0u) = r != 255 ? aiPixel[r]: 0;
			// 				pixel.At<uint8_t>(1u) = g != 255 ? aiPixel[g]: 0;
			// 				pixel.At<uint8_t>(2u) = b != 255 ? aiPixel[b]: 0;
			// 				pixel.At<uint8_t>(3u) = a != 255 ? aiPixel[a]: 255;
			// 			}
			// 		}
			// 	}
			//
			//
			// 	MGN_CORE_ASSERT(image.channels == 4, "[Vulkan] image.channels == 4");
			//
			// 	images[i] = renderer->CreateImage(image.source.Get(), {image.width, image.height, 1}, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_USAGE_SAMPLED_BIT, true);
			// 	deleters[i] = AutoDeleter::MakeShared();
			// 	deleters[i]->push(renderer->GetAllocator(), images[i].allocation, images[i].image);
			// 	deleters[i]->push(images[i].imageView);
			// }
			//
			// for (int i = 0; i < scene->mNumMaterials; ++i) {
			// 	MaterialPass materialPass = MaterialPass::MainColor;
			// 	GLTFMetallicRoughness::MaterialResources materialResources{};
			//
			// 	aiMaterial *material = scene->mMaterials[i]; // for some formats (like glTF) metallic and roughness may be the same file
			//
			// 	std::array<aiTextureType, 6> texTypes{aiTextureType_BASE_COLOR, aiTextureType_NORMALS, aiTextureType_EMISSIVE, aiTextureType_METALNESS, aiTextureType_DIFFUSE_ROUGHNESS, aiTextureType_AMBIENT_OCCLUSION};
			// 	std::array<aiString, 6> imageFiles{};// fileBaseColor, fileNormal, fileEmissive, fileMetallic, fileRoughness, fileAo;
			// 	std::array<bool, 6> hasImage{};// fileBaseColor, fileNormal, fileEmissive, fileMetallic, fileRoughness, fileAo;
			// 	ai_real metallic, roughness, anisotropy;
			// 	aiColor3D color;
			//
			// 	bool useColorMap = material->GetTextureCount(aiTextureType_BASE_COLOR) > 0;
			// 	bool useNormalMap = material->GetTextureCount(aiTextureType_NORMALS) > 0;
			// 	bool useEmissiveMap = material->GetTextureCount(aiTextureType_EMISSIVE) > 0;
			// 	bool useMetalMap = material->GetTextureCount(aiTextureType_METALNESS) > 0;
			// 	bool useRoughMap = material->GetTextureCount(aiTextureType_DIFFUSE_ROUGHNESS) > 0;
			// 	bool useAo = material->GetTextureCount(aiTextureType_AMBIENT_OCCLUSION) > 0;
			//
			// 	for (uint64_t texId = 0; texId < texTypes.size(); ++texId) {
			// 		hasImage[texId] = material->GetTextureCount(texTypes[texId]) > 0;
			// 		if (hasImage[texId]) {
			// 			material->GetTexture(texTypes[texId], 0, &imageFiles[texId]);
			// 			std::pair<const aiTexture *, int> result = scene->GetEmbeddedTextureAndIndex(imageFiles[texId].C_Str());
			// 			if (result.first) {
			//
			// 			}
			// 		}
			// 	}
			//
			// 	material->Get(AI_MATKEY_BASE_COLOR, color);
			// 	material->Get(AI_MATKEY_METALLIC_FACTOR, metallic);
			// 	material->Get(AI_MATKEY_ROUGHNESS_FACTOR, roughness);
			// 	material->Get(AI_MATKEY_ANISOTROPY_FACTOR, anisotropy);
			//
			// 	/***/
			//
			// 	GLTFMetallicRoughness &metallicRoughness = renderer->GetGLTFMaterial();
			// 	metallicRoughness.WriteMaterial(renderer->GetDevice(), materialPass, materialResources, renderer->GetDescriptorAllocatorGrowable());
			//
			// 	// TODO: Load the texture and material following this part of the documentation.
			//
			// 	/*
			// 	 * Normally textures used by assets are stored in separate files, however, there are file formats
			// 	 * embedding their textures directly into the model file. Such textures are loaded into an aiTexture structure.
			// 	 * In previous versions, the path from the query for AI_MATKEY_TEXTURE(textureType, index) would be *<index>
			// 	 * where <index> is the index of the texture in aiScene::mTextures. Now this call will return a file path for
			// 	 * embedded textures in FBX files. To test if it is an embedded texture use aiScene::GetEmbeddedTexture.
			// 	 * If the returned pointer is not null, it is embedded and can be loaded from the data structure.
			// 	 * If it is null, search for a separate file. Other file types still use the old behavior.
			// 	 *
			// 	 * If you rely on the old behavior, you can use Assimp::Importer::SetPropertyBool with the key #AI_CONFIG_IMPORT_FBX_EMBEDDED_TEXTURES_LEGACY_NAMING to force the old behavior.
			// 	 *
			// 	 * There are two cases:
			// 	 * 1. The texture is NOT compressed. Its color data is directly stored in the aiTexture structure as an array of
			// 	 * aiTexture::mWidth * aiTexture::mHeight aiTexel structures. Each aiTexel represents a pixel (or “texel”) of
			// 	 * the texture image. The color data is stored in an unsigned RGBA8888 format, which can be easily used for
			// 	 * both Direct3D and OpenGL (swizzling the order of the color components might be necessary). RGBA8888 has been
			// 	 * chosen because it is well-known, easy to use , and natively supported by nearly all graphics APIs.
			// 	 * 2. This applies if aiTexture::mHeight == 0 is fulfilled. Then, the texture is stored in a compressed format such
			// 	 * as DDS or PNG. The term “compressed” does not mean that the texture data must actually be compressed, however,
			// 	 * the texture was found in the model file as if it was stored in a separate file on the hard disk.
			// 	 * Appropriate decoders (such as libjpeg, libpng, D3DX, DevIL) are required to load these textures.
			// 	 * aiTexture::mWidth specifies the size of the texture data in bytes, aiTexture::pcData is a pointer to the raw image
			// 	 * data and aiTexture::achFormatHint is either zeroed or contains the most common file extension of the embedded texture’s format.
			// 	 * This value is only set if Assimp is able to determine the file format.
			// 	 */
			// }

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return;
			}

			// TODO: Load the textures first.

			// TODO: Load the material seconds.

			std::vector<std::shared_ptr<MeshAsset>> meshes;
			meshes.reserve(scene->mNumMeshes);

			// Load the meshes
			// TODO: Reuuse the materials for the meshes.
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

					std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();
					mesh->name = aiMesh->mName.C_Str();

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

						if constexpr (c_OverrideColorVertex) {
							vertices[i].color = {vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z, 1.0};
						}
						else if (aiMesh->HasVertexColors(0)) {
							const auto &color = aiMesh->mColors[0][i];
							vertices[i].color = {color.r, color.g, color.b, color.a};
						}
					}

					indices.reserve(aiMesh->mNumFaces * 3);
					for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
						MGN_CORE_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
						const aiFace &face = aiMesh->mFaces[i];
						indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
					}

					Mesh::LOD surface;
					surface.index = 0;
					surface.count = indices.size();
					surface.material = renderer->GetDefaultMeshMaterial();

					mesh->lods.push_back(surface);

					mesh->meshBuffers = renderer->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

					meshes.emplace_back(std::move(mesh));
				}
			}

			// Load the nodes as entity in the scene.
			{
				// Unwrapping everything in a vector to avoid recursion.
				std::vector<std::tuple<aiNode *, Core::EntityID>> nodes{{scene->mRootNode, parent}};
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
						coreScene->AddComponent<Renderable>(entityId)->gpuMesh =  std::static_pointer_cast<GPUMesh>(meshes[*node->mMeshes]);
					}
					else if (node->mNumMeshes > 1) {
						for (int i = 0; i < node->mNumMeshes; ++i) {
							std::shared_ptr<MeshAsset> mesh = meshes[node->mMeshes[i]];
							EntityID meshChild = coreScene->CreateEntity(entityId);
							coreScene->SetName(meshChild, mesh->name);
							coreScene->AddComponent<Renderable>(meshChild)->gpuMesh = std::static_pointer_cast<GPUMesh>(mesh);
						}
					}


					for (int i = 0; i < node->mNumChildren; ++i) {
						aiNode *child = node->mChildren[i];
						nodes.push_back({child, entityId});
					}
				}
			}
		}

		std::shared_ptr<MeshAsset> LoadLines(VulkanRenderer *renderer, std::span<Core::LineObject> lines) {
			std::vector<uint32_t> indices;
			std::vector<Vertex> vertices;

			std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();

			for (const Core::LineObject &line: lines) {
				if (line.points.size() < 2) continue;
				const uint32_t offset = vertices.size();
				vertices.push_back(line.points.at(0));
				for (uint32_t i = 1; i < line.points.size(); ++i) {
					vertices.push_back(line.points.at(i));
					indices.push_back(offset + (i - 1));
					indices.push_back(offset + i);
				}
			}

			Core::Mesh::LOD surface;
			surface.index = 0;
			surface.count = indices.size();
			surface.material = renderer->GetDefaultLineMaterial();

			mesh->lods.push_back(surface);
			mesh->meshBuffers = renderer->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

			return mesh;
		}

		ManualDeleteMeshAsset LoadManualLines(VulkanRenderer *renderer, std::span<Core::LineObject> lines) {
			ManualDeleteMeshAsset mesh;

			std::vector<uint32_t> indices;
			std::vector<Vertex> vertices;


			for (const Core::LineObject &line: lines) {
				if (line.points.size() < 2) continue;
				const uint32_t offset = vertices.size();
				vertices.push_back(line.points.at(0));
				for (uint32_t i = 1; i < line.points.size(); ++i) {
					vertices.push_back(line.points.at(i));
					indices.push_back(offset + (i - 1));
					indices.push_back(offset + i);
				}
			}

			Core::Mesh::LOD surface;
			surface.index = 0;
			surface.count = indices.size();
			surface.material = renderer->GetDefaultLineMaterial();

			mesh.lods.push_back(surface);
			mesh.meshBuffers = renderer->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

			return std::move(mesh);
		}

		std::shared_ptr<MeshAsset> LoadPoints(VulkanRenderer *renderer, std::span<Vertex> points) {
			MGN_CORE_ASSERT(false, "The load points function is not implemented yet.");
			return nullptr;
			/*
						std::vector<uint32_t> indices;
						indices.reserve(points.size());

						std::vector<Vertex> vertices;
						vertices.reserve(points.size());

						std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();

						for (const auto & point: points) {
							vertices.push_back(point);
							indices.push_back(indices.size());
						}

						Core::Mesh::LOD surface;
						surface.index = 0;
						surface.count = indices.size();
						surface.material = renderer->GetDefaultPointMaterial();

						mesh->lods.push_back(surface);
						mesh->meshBuffers = renderer->UploadMesh(indices, vertices);

						return mesh;
			*/
		}

		std::optional<std::shared_ptr<MeshAsset>> LoadCPUMesh(VulkanRenderer *engine, const Core::CPUMesh &cpuMesh) {

			std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();

			Core::Mesh::LOD surface;
			surface.index = 0;
			surface.count = cpuMesh.Indices.size();
			surface.material = engine->GetDefaultMeshMaterial();

			mesh->lods.push_back(surface);

			mesh->meshBuffers = engine->UploadMesh(Core::ConstBufferView::Make(cpuMesh.Indices), Core::ConstBufferView::Make(cpuMesh.Vertices));

			return mesh;
		}

		std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadMeshes(VulkanRenderer *engine, const std::filesystem::path &filePath) {
			// Create an instance of the Importer class
			Assimp::Importer importer;

			// And have it read the given file with some example postprocessing
			// Usually - if speed is not the most important aspect for you - you'll
			// probably to request more postprocessing than we do in this example.
			const aiScene *scene = importer.ReadFile(filePath.string(),
													 aiProcess_Triangulate |
															 aiProcess_JoinIdenticalVertices |
															 aiProcess_GenUVCoords |
															 aiProcess_GenSmoothNormals |
															 aiProcess_CalcTangentSpace |
															 aiProcess_ConvertToLeftHanded |
															 aiProcess_SortByPType);

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return std::nullopt;
			}

			if (!scene->HasMeshes()) {
				return std::nullopt;
			}

			std::vector<std::shared_ptr<MeshAsset>> meshes;

			// use the same vectors for all meshes so that the memory doesnt reallocate as
			// often
			std::vector<uint32_t> indices;
			std::vector<Vertex> vertices;

			for (uint64_t i = 0; i < scene->mNumMeshes; ++i) {
				const aiMesh *aiMesh = scene->mMeshes[i];
				if (!aiMesh->HasPositions()) {
					continue;
				}

				std::shared_ptr<MeshAsset> mesh = std::make_shared<MeshAsset>();
				mesh->name = aiMesh->mName.C_Str();

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

					if constexpr (c_OverrideColorVertex) {
						vertices[i].color = {vertices[i].normal.x, vertices[i].normal.y, vertices[i].normal.z, 1.0};
					}
					else if (aiMesh->HasVertexColors(0)) {
						const auto &color = aiMesh->mColors[0][i];
						vertices[i].color = {color.r, color.g, color.b, color.a};
					}
				}

				indices.reserve(aiMesh->mNumFaces * 3);
				for (uint64_t i = 0; i < aiMesh->mNumFaces; ++i) {
					MGN_CORE_ASSERT(aiMesh->mFaces[i].mNumIndices == 3, "The face wasn't properly triangulated.");
					const aiFace &face = aiMesh->mFaces[i];
					indices.insert(indices.end(), face.mIndices, face.mIndices + face.mNumIndices);
				}

				Core::Mesh::LOD surface;
				surface.index = 0;
				surface.count = indices.size();

				mesh->lods.push_back(surface);

				mesh->meshBuffers = engine->UploadMesh(Core::ConstBufferView::Make(indices), Core::ConstBufferView::Make(vertices));

				meshes.emplace_back(std::move(mesh));
			}

			// We're done. Everything will be cleaned up by the importer destructor
			return meshes;
		}
	} // namespace Initializer
} // namespace Imagine::Vulkan
