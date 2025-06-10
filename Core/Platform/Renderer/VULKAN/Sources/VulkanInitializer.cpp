//
// Created by Iannis on 10/06/2025.
//

#include "Imagine/Vulkan/VulkanInitializer.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include "Imagine/Vulkan/VulkanRenderer.hpp"


namespace Imagine::Vulkan {
	namespace Initializer {
		std::optional<std::vector<std::shared_ptr<MeshAsset>>> LoadMeshes(VulkanRenderer *engine, const std::filesystem::path &filePath) {
			// Create an instance of the Importer class
			Assimp::Importer importer;

			// And have it read the given file with some example postprocessing
			// Usually - if speed is not the most important aspect for you - you'll
			// probably to request more postprocessing than we do in this example.
			const aiScene* scene = importer.ReadFile( filePath.string(),
			  aiProcess_CalcTangentSpace       |
			  aiProcess_Triangulate            |
			  aiProcess_JoinIdenticalVertices  |
			  aiProcess_SortByPType);

			// If the import failed, report it
			if (nullptr == scene) {
				MGN_CORE_ERROR(importer.GetErrorString());
				return std::nullopt;
			}

			//TODO: Import the scene for real.

			// We're done. Everything will be cleaned up by the importer destructor
			return std::nullopt;
		}
	} // namespace Initializer
} // namespace Imagine::Vulkan
