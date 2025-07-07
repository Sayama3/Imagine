//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Assets/AssetType.hpp"

namespace Imagine::Core {
	AssetType AssetTypeFromString(const std::string &assetType) {
		if(assetType == "Scene") return AssetType::Scene;
		if(assetType == "Texture2D") return AssetType::Texture2D;
		if(assetType == "Texture3D") return AssetType::Texture3D;
		if(assetType == "CubeMap") return AssetType::CubeMap;
		if(assetType == "ShaderSource") return AssetType::ShaderSource;
		if(assetType == "Material") return AssetType::Material;
		if(assetType == "MaterialInstance") return AssetType::MaterialInstance;
		if(assetType == "Model") return AssetType::Model;
		if(assetType == "Mesh") return AssetType::Mesh;
		if(assetType == "Script") return AssetType::Script;
		return AssetType::None;
	}

	bool TryAssetTypeFromString(const std::string &assetType, AssetType &type) {
		if(assetType == "Scene") {type = AssetType::Scene; return true;}
		if(assetType == "Texture2D") {type = AssetType::Texture2D; return true;}
		if(assetType == "Texture3D") {type = AssetType::Texture3D; return true;}
		if(assetType == "CubeMap") {type = AssetType::CubeMap; return true;}
		if(assetType == "ShaderSource") {type = AssetType::ShaderSource; return true;}
		if(assetType == "Material") {type = AssetType::Material; return true;}
		if(assetType == "MaterialInstance") {type = AssetType::MaterialInstance; return true;}
		if(assetType == "Model") {type = AssetType::Model; return true;}
		if(assetType == "Mesh") {type = AssetType::Mesh; return true;}
		if(assetType == "Script") {type = AssetType::Script; return true;}
		return false;
	}
}