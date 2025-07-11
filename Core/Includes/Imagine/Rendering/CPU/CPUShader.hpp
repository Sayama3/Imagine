//
// Created by ianpo on 03/07/2025.
//

#pragma once
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/FileSystem.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"

namespace Imagine {

	class CPUShader : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::ShaderSource);
	public:
		CPUShader() = default;
		virtual ~CPUShader() = default;
	protected:
		CPUShader(ShaderStage stage);
	public:
		virtual Buffer GetShaderContent() = 0;
		virtual std::string GetName() const = 0;
	public:
		ShaderStage stage;
	};

	class CPUFileShader final : public CPUShader {
	public:
		CPUFileShader() = default;
		~CPUFileShader() = default;
		CPUFileShader(ShaderStage stage, Path path);

	public:
		virtual Buffer GetShaderContent() override;
		virtual std::string GetName() const override;
		Path path;
	};

	class CPUMemoryShader final : public CPUShader {
	public:
		virtual Buffer GetShaderContent() override;
		virtual std::string GetName() const override;
		Buffer shader;
		std::string name;
	};

} // namespace Imagine
