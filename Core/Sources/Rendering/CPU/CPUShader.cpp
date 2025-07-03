//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUShader.hpp"

namespace Imagine::Core {

	Buffer CPUFileShader::GetShaderContent() {
		return FileSystem::readBinaryFile(path.GetFullPath());
	}

	Buffer CPUMemoryShader::GetShaderContent() {
		return Buffer::Copy(shader);
	}

} // namespace Imagine::Core
