//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"

namespace Imagine::Core {
	bool CPUMaterialInstance::SetFieldPosition::operator==(const SetFieldPosition &o) const {
		return SetIndex == o.SetIndex && BlockIndex == o.BlockIndex && FieldIndex == o.FieldIndex;
	}
	bool CPUMaterialInstance::SetFieldPosition::operator!=(const SetFieldPosition &o) const {
		return !(*this == o);
	}
	bool CPUMaterialInstance::BufferFieldPosition::operator==(const BufferFieldPosition &o) const {
		return BufferIndex == o.BufferIndex && FieldIndex == o.FieldIndex;
	}
	bool CPUMaterialInstance::BufferFieldPosition::operator!=(const BufferFieldPosition &o) const {
		return !(*this == o);
	}
	bool CPUMaterialInstance::PushConstantFieldPosition::operator==(const PushConstantFieldPosition &o) const {
		return BufferIndex == o.BufferIndex && FieldIndex == o.FieldIndex;
	}
	bool CPUMaterialInstance::PushConstantFieldPosition::operator!=(const PushConstantFieldPosition &o) const {
		return !(*this == o);
	}
} // namespace Imagine::Core
