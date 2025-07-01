//
// Created by ianpo on 01/07/2025.
//

#pragma once

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h> // Post processing flags
#include <assimp/scene.h> // Output data structure

namespace Imagine::ThirdParty::Assimp {

	// TODO: See for aiProcess_SplitLargeMeshes


	inline static constexpr aiPostProcessSteps GetSmoothPostProcess() {
		return static_cast<aiPostProcessSteps>(aiProcess_Triangulate |
											   aiProcess_JoinIdenticalVertices |
											   aiProcess_GenUVCoords |
											   aiProcess_GenSmoothNormals |
											   aiProcess_CalcTangentSpace |
											   aiProcess_ConvertToLeftHanded |
											   aiProcess_SortByPType);
	}

	inline static constexpr aiPostProcessSteps GetHardPostProcess() {
		return static_cast<aiPostProcessSteps>(aiProcess_Triangulate |
											   aiProcess_GenUVCoords |
											   aiProcess_GenNormals |
											   aiProcess_CalcTangentSpace |
											   aiProcess_ConvertToLeftHanded |
											   aiProcess_SortByPType);
	}

	template<bool OptimizeGraph, bool SplitLargeMeshes>
	inline static constexpr aiPostProcessSteps GetOptimizationPostProcess() {
		if constexpr (OptimizeGraph) {
			if constexpr (SplitLargeMeshes) {
				return static_cast<aiPostProcessSteps>(aiProcess_ImproveCacheLocality |
													   aiProcess_FindInstances |
													   aiProcess_OptimizeMeshes |
													   aiProcess_RemoveRedundantMaterials |
													   aiProcess_OptimizeGraph |
													   aiProcess_SplitLargeMeshes);
			}
			else {
				return static_cast<aiPostProcessSteps>(aiProcess_ImproveCacheLocality |
													   aiProcess_FindInstances |
													   aiProcess_OptimizeMeshes |
													   aiProcess_OptimizeGraph |
													   aiProcess_RemoveRedundantMaterials);
			}
		}
		else {
			if constexpr (SplitLargeMeshes) {
				return static_cast<aiPostProcessSteps>(aiProcess_ImproveCacheLocality |
													   aiProcess_FindInstances |
													   aiProcess_OptimizeMeshes |
													   aiProcess_RemoveRedundantMaterials |
													   aiProcess_SplitLargeMeshes);
			}
			else {
				return static_cast<aiPostProcessSteps>(aiProcess_ImproveCacheLocality |
													   aiProcess_FindInstances |
													   aiProcess_OptimizeMeshes |
													   aiProcess_RemoveRedundantMaterials);
			}
		}
	}

	template<bool OptimizeGraph, bool SplitLargeMeshes>
	inline static constexpr aiPostProcessSteps GetOptimizedSmoothPostProcess() {
		return static_cast<aiPostProcessSteps>(GetSmoothPostProcess() | GetOptimizationPostProcess<OptimizeGraph, SplitLargeMeshes>());
	}

	template<bool OptimizeGraph, bool SplitLargeMeshes>
	inline static constexpr aiPostProcessSteps GetOptimizedHardPostProcess() {
		return static_cast<aiPostProcessSteps>(GetHardPostProcess() | GetOptimizationPostProcess<OptimizeGraph, SplitLargeMeshes>());
	}

} // namespace Imagine::ThirdParty::Assimp
