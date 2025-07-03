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
		using Buffer = std::array<uint8_t, MaterialField::DataSize>;

		struct SetFieldPosition {
			uint32_t SetIndex;
			uint32_t BlockIndex;
			uint32_t FieldIndex;
			bool operator==(const SetFieldPosition &o ) const;
			bool operator!=(const SetFieldPosition &o ) const;
		};
		struct BufferFieldPosition {
			uint32_t BufferIndex;
			uint32_t FieldIndex;
			bool operator==(const BufferFieldPosition &o ) const;
			bool operator!=(const BufferFieldPosition &o ) const;
		};
		struct PushConstantFieldPosition {
			uint32_t BufferIndex;
			uint32_t FieldIndex;
			bool operator==(const PushConstantFieldPosition &o ) const;
			bool operator!=(const PushConstantFieldPosition &o ) const;
		};
	public:
		std::map<SetFieldPosition, Buffer> SetEditions;
		std::map<BufferFieldPosition, Buffer> BufferEditions;
		std::map<PushConstantFieldPosition, Buffer> PushConstantEditions;
		AssetField<CPUMaterial> Material;
	};

} // namespace Imagine::Core
