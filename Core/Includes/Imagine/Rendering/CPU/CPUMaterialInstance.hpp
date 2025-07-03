//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "CPUMaterial.hpp"
#include "Imagine/Assets/AssetField.hpp"

namespace Imagine::Core {

	class CPUMaterialInstance final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::MaterialInstance);
	public:
		AssetField<CPUMaterial> material;
	};

} // namespace Imagine::Core
