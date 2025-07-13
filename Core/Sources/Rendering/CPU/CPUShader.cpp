//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUShader.hpp"

#include "Imagine/Assets/AssetManager.hpp"

namespace Imagine {

	void CPUShader::Initialize(Ref<CPUShader> shader) {
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::Vertex)) s_Shaders[0] = shader;
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::TessellationControl)) s_Shaders[1] = shader;
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::TessellationEvaluation)) s_Shaders[2] = shader;
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::Geometry)) s_Shaders[3] = shader;
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::Fragment)) s_Shaders[4] = shader;
		if (CHECK_SHADER_STAGE_BIT(shader->stage, ShaderStage::Compute)) s_Shaders[5] = shader;
	}

	void CPUShader::Shutdown() {
		for (auto &shader: s_Shaders) {
			shader.reset();
		}
	}
	void CPUShader::TryRegister() {
		if (const auto active = Project::GetActive()) {
			if (const auto assets = active->GetAssetManager()) {
				for (Ref<CPUShader> &shader: s_Shaders) {
					assets->AddAsset(shader);
				}
			}
		}
	}
	CPUShader::CPUShader(const ShaderStage stage) :
		stage(stage) {
	}

	Ref<CPUFileShader> CPUFileShader::Initialize(ShaderStage stage, Path path) {
		Ref<CPUFileShader> shader = CreateRef<CPUFileShader>(stage, std::move(path));
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::Vertex)) CPUFileShader::s_Shaders[0] = shader;
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::TessellationControl)) CPUFileShader::s_Shaders[1] = shader;
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::TessellationEvaluation)) CPUFileShader::s_Shaders[2] = shader;
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::Geometry)) CPUFileShader::s_Shaders[3] = shader;
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::Fragment)) CPUFileShader::s_Shaders[4] = shader;
		if (CHECK_SHADER_STAGE_BIT(stage, ShaderStage::Compute)) CPUFileShader::s_Shaders[5] = shader;
		return shader;
	}

	CPUFileShader::CPUFileShader(const ShaderStage stage, Path path) :
		CPUShader(stage), path(std::move(path)) {
	}

	Buffer CPUFileShader::GetShaderContent() {
		return FileSystem::ReadBinaryFile(path.GetFullPath());
	}
	std::string CPUFileShader::GetName() const {
		return path.path.filename().string();
	}

	Buffer CPUMemoryShader::GetShaderContent() {
		return Buffer::Copy(shader);
	}
	std::string CPUMemoryShader::GetName() const {
		return name;
	}

} // namespace Imagine
