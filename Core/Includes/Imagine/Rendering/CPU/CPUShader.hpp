//
// Created by ianpo on 03/07/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"

namespace Imagine::Core {

	class CPUShader : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::ShaderSource);
	public:
		virtual Buffer GetShaderContent() = 0;
	public:
		ShaderStage stage;
	};

	class CPUFileShader final : public CPUShader {
	public:
		virtual Buffer GetShaderContent() override;
	public:
		Path path;
	};

	class CPUMemoryShader final : public CPUShader {
	public:
		virtual Buffer GetShaderContent() override;
	public:
		Buffer shader;
	};

} // namespace Imagine::Core
