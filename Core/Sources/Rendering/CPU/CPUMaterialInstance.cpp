//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"

namespace Imagine {
	CPUMaterialInstance::CPUMaterialInstance(const AssetHandle material) : Material(material) {
	}
	bool CPUMaterialInstance::SetFieldPosition::operator==(const SetFieldPosition &o) const {
		return SetIndex == o.SetIndex && BindingIndex == o.BindingIndex && FieldIndex == o.FieldIndex;
	}
	bool CPUMaterialInstance::SetFieldPosition::operator!=(const SetFieldPosition &o) const {
		return !(*this == o);
	}
	bool CPUMaterialInstance::PushConstantFieldPosition::operator==(const PushConstantFieldPosition &o) const {
		return PCIndex == o.PCIndex && FieldIndex == o.FieldIndex;
	}
	bool CPUMaterialInstance::PushConstantFieldPosition::operator!=(const PushConstantFieldPosition &o) const {
		return !(*this == o);
	}
} // namespace Imagine
