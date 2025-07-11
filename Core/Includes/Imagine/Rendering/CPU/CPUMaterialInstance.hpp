//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "CPUMaterial.hpp"
#include "Imagine/Assets/Asset.hpp"
#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Rendering/GPU/GPUMaterialInstance.hpp"

namespace Imagine {

	class CPUMaterialInstance final : public Asset {
	public:
		MGN_IMPLEMENT_ASSET(AssetType::MaterialInstance);
	public:
		CPUMaterialInstance() = default;
		~CPUMaterialInstance() = default;
		CPUMaterialInstance(AssetHandle material);
	public:
		using MaterialDataBuffer = std::array<uint8_t, MaterialField::DataSize>;
		struct SetFieldPosition {
			uint32_t SetIndex;
			uint32_t BindingIndex;
			uint32_t FieldIndex;
			bool operator==(const SetFieldPosition &o ) const;
			bool operator!=(const SetFieldPosition &o ) const;
			auto operator<=>(const SetFieldPosition &o) const {
				if (SetIndex == o.SetIndex) {
					if (BindingIndex == o.BindingIndex) {
						return FieldIndex <=> o.FieldIndex;
					}
					return BindingIndex <=> o.BindingIndex;
				}
				return SetIndex <=> o.SetIndex;
			}
		};
		struct PushConstantFieldPosition {
			uint32_t PCIndex;
			uint32_t FieldIndex;
			bool operator==(const PushConstantFieldPosition &o ) const;
			bool operator!=(const PushConstantFieldPosition &o ) const;
			auto operator<=>(const PushConstantFieldPosition &o) const {
				if (PCIndex == o.PCIndex) {
					return FieldIndex <=> o.FieldIndex;
				}
				return PCIndex <=> o.PCIndex;
			}
		};

		template<typename T>
		void PushSet(const SetFieldPosition pos, const T& data) {
			MaterialDataBuffer buffer{0};
			memcpy(buffer.data(), &data, std::min(sizeof(T), buffer.size()));
			SetEditions[pos] = buffer;
		}

		template<typename T>
		void PushConst(const PushConstantFieldPosition pos, const T& data) {
			MaterialDataBuffer buffer{0};
			memcpy(buffer.data(), &data, std::min(sizeof(T), buffer.size()));
			PushConstantEditions[pos] = buffer;
		}
	public:
		std::map<SetFieldPosition, MaterialDataBuffer> SetEditions;
		std::map<PushConstantFieldPosition, MaterialDataBuffer> PushConstantEditions;
		AssetHandle Material;
		Ref<GPUMaterialInstance> gpu{nullptr};
	};

} // namespace Imagine
