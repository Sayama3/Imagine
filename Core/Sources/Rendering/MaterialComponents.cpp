//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/MaterialComponents.hpp"

namespace Imagine::Core {

	MaterialField::MaterialField(std::string name, MaterialType type) :
		name(std::move(name)), type(type) {}
	uint64_t MaterialField::GetSize() const {
		return Helper::MaterialTypeSize(type);
	}

	ConstBufferView MaterialField::GetView() const {
		if (type == MaterialType::None) return {};
		return ConstBufferView{(const void *) data.data(), GetSize()};
	}

	void MaterialBlock::Push(std::string name, MaterialType type) {
		fields.push_back({std::move(name), type});
	}

	MaterialBlock::MaterialBlock(const MaterialField &field) :
		fields(1, field) {
	}

	MaterialBlock::MaterialBlock(const std::initializer_list<MaterialField> &fields) :
		fields(fields) {
	}

	MaterialBlock::MaterialBlock(const std::initializer_list<MaterialField> &fields, const bool readOnly) :
		fields(fields), write(!readOnly) {
	}
	MaterialBlock::MaterialBlock(const std::string &name, MaterialType type) :
		fields(1, {name, type}) {
	}

	MaterialBlock MaterialBlock::GetSceneBlock() {
		return {
				{"View", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
				{"Proj", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
				{"ViewProj", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
				{"Ambient Color", MaterialType::Color4, glm::vec4(1)},
				{"Sunlight Direction", MaterialType::Float4, glm::vec4(1)},
				{"Sunlight Color", MaterialType::Color4, glm::vec4(1)},
		};
	}
	MaterialBlock MaterialBlock::GetPBRConstantBlock() {
		return {
				{"Tint", MaterialType::Color4, glm::vec4(1)},
				{"Emissive", MaterialType::Color4, glm::vec4(0)},
				{"Metal Factor", MaterialType::Float1, glm::vec1(1)},
				{"Rough Factor", MaterialType::Float1, glm::vec1(1)},
		};
	}
	uint64_t MaterialBlock::GetSize() const {
		uint64_t total{0};
		for (const auto &field: fields) {
			total += field.GetSize();
		}
		return total;
	}

	Buffer MaterialBlock::GetCompactBuffer() const {
		Buffer buffer{GetSize()};
		uint64_t offset{0};
		for (uint64_t i = 0; i < fields.size(); ++i) {
			auto &field = fields[i];
			auto view = field.GetView();

			if (!view.IsValid()) continue;

			memcpy(buffer.Get<uint8_t>() + offset, view.Get(), view.Size());
			offset += view.Size();
		}
		return buffer;
	}

	MaterialSet::MaterialSet(const std::initializer_list<MaterialBlock> &blocks) : Blocks(blocks) {
	}

	MaterialSet MaterialSet::GetSceneSet() {
		return {MaterialBlock::GetSceneBlock()};
	}
	MaterialSet MaterialSet::GetPBRSet() {
		return {
				MaterialBlock::GetPBRConstantBlock(),
				{"Color Texture", MaterialType::Texture2D},
				{"Metal Texture", MaterialType::Texture2D},
				{"Roughness Texture", MaterialType::Texture2D},
				{"Normal Texture", MaterialType::Texture2D},
				{"Ambien Occlusion Texture", MaterialType::Texture2D},
		};
	}
} // namespace Imagine::Core
