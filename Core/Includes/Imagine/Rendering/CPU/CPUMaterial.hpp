//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Rendering/MaterialComponents.hpp"
#include "Imagine/Rendering/RenderTypes.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"

namespace Imagine::Core {

	enum class RenderType : uint8_t {
		Opaque,
		Transparent,
	};

	class CPUMaterial final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::Material);
	public:
		MaterialLayout layout;
		std::array<AssetHandle, 5> shaders;
		RenderType renderType = RenderType::Opaque;
		Topology topology = Topology::Triangle;
	};

} // namespace Imagine::Core
