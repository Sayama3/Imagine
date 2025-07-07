//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUShader.hpp"

namespace Imagine::Core {

	Buffer CPUFileShader::GetShaderContent() {
		return FileSystem::ReadBinaryFile(path.GetFullPath());
	}
	std::string CPUFileShader::GetName() {
		return path.path.filename().string();
	}

	Buffer CPUMemoryShader::GetShaderContent() {
		return Buffer::Copy(shader);
	}
	std::string CPUMemoryShader::GetName() {
		return name;
	}

} // namespace Imagine::Core
