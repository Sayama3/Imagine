//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Rendering/MaterialComponents.hpp"
#include "Imagine/Core/InternalCore.hpp"

namespace Imagine::Core {

	MaterialField::MaterialField(std::string name, MaterialType type) :
		name(std::move(name)), type(type) {}
	MaterialField::MaterialField(const uint32_t count, std::string name, MaterialType type) :
		name(std::move(name)), type(type), count(count) {}

	uint64_t MaterialField::GetSize() const {
		return Helper::MaterialTypeSize(type);
	}

	ConstBufferView MaterialField::GetView() const {
		if (type == MaterialType::None) return {};
		return ConstBufferView{(const void *) data.data(), GetSize()};
	}
	BufferView MaterialField::GetView() {
		if (type == MaterialType::None) return {};
		return BufferView{(void *) data.data(), GetSize()};
	}

	void MaterialBlock::Push(std::string name, MaterialType type) {
		Fields.push_back({std::move(name), type});
	}

	std::vector<MaterialBlock> MaterialBlock::SplitBlock() const {
		std::vector<MaterialBlock> blocks{MaterialBlock{}};
		for (uint32_t i = 0; i < Fields.size(); ++i) {
			if (Helper::IsBufferType(Fields[i].type)) {
				blocks.back().Fields.push_back(Fields[i]);
			}
			else {
				if (!blocks.empty()) blocks.emplace_back();
				blocks.back().Fields.push_back({Fields[i]});
				blocks.emplace_back();
			}
		}
		return blocks;
	}

	bool MaterialBlock::IsABuffer() const {
		for (const MaterialField &field: Fields) {
			if (!Helper::IsBufferType(field.type)) return false;
		}
		return true;
	}
	void MaterialBlock::Add(const MaterialBlock &block) {
		Fields.insert(Fields.end(), block.Fields.begin(), block.Fields.end());
	}

	MaterialBlock::MaterialBlock(const MaterialField &field) :
		Fields(field.count, field) {
		for (uint32_t i = 0; i < Fields.size(); ++i) {
			auto &field = Fields[i];
			field.count = 1;
			field.name += "_" + std::to_string(i);
		}
	}

	MaterialBlock::MaterialBlock(const std::initializer_list<MaterialField> &fields) {
		for (const auto &field: fields) {
			for (uint32_t i = 0; i < field.count; ++i) {
				Fields.push_back(field);
				Fields.back().count = 1;
				Fields.back().name += "_" + std::to_string(i);
			}
		}
	}

	MaterialBlock::MaterialBlock(const std::initializer_list<MaterialField> &fields, const bool readOnly) :
		MaterialBlock(fields) {
		Write = !readOnly;
	}
	MaterialBlock::MaterialBlock(const std::initializer_list<MaterialField> &fields, const bool readOnly, const BufferType bufferType) : MaterialBlock(fields, readOnly) {
		GPUBufferType = bufferType;
	}

	MaterialBlock::MaterialBlock(const std::string &name, MaterialType type) :
		Fields{1, MaterialField{name, type}} {
	}

	MaterialBlock MaterialBlock::GetSceneBlock() {
		return {{
						{"View", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
						{"Proj", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
						{"ViewProj", MaterialType::FMat4, Math::Identity<glm::fmat4>()},
						{"Camera Position", MaterialType::Float4, glm::vec4(0)},
						{"Ambient Color", MaterialType::Color4, glm::vec4(1)},
						{"Sunlight Direction", MaterialType::Float4, glm::vec4(1)},
						{"Sunlight Color", MaterialType::Color4, glm::vec4(1)},
				},
				true};
	}
	MaterialBlock MaterialBlock::GetLightBlock() {
		return {{
						{32, "Light", MaterialType::Light},
						{"Light Count", MaterialType::Int1, 0},
				},
				true, SSBO};
	}
	MaterialBlock MaterialBlock::GetPBRConstantBlock() {
		return {
				{"Tint", MaterialType::Color4, glm::vec4(1)},
				{"Emissive", MaterialType::Color4, glm::vec4(0)},
				{"Metal Factor", MaterialType::Float3, glm::vec3(1,0,0)},
				{"Rough Factor", MaterialType::Float3, glm::vec3(1,0,0)},
		};
	}
	uint64_t MaterialBlock::GetSize() const {
		uint64_t total{0};
		for (const auto &field: Fields) {
			total += field.GetSize();
		}
		return total;
	}

	Buffer MaterialBlock::GetCompactBuffer() const {
		Buffer buffer{GetSize()};
		uint64_t offset{0};
		for (uint64_t i = 0; i < Fields.size(); ++i) {
			auto &field = Fields[i];
			auto view = field.GetView();

			if (!view.IsValid()) continue;

			memcpy(buffer.Get<uint8_t>() + offset, view.Get(), view.Size());
			offset += view.Size();
		}
		return buffer;
	}

	MaterialSet::MaterialSet(const std::initializer_list<MaterialBlock> &blocks) :
		Blocks(blocks) {
	}

	MaterialSet MaterialSet::GetSceneSet() {
		return {MaterialBlock::GetSceneBlock(), MaterialBlock::GetLightBlock()};
	}
	MaterialSet MaterialSet::GetPBRSet() {
		return {
				MaterialBlock::GetPBRConstantBlock(),
				{"Color Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
				{"Normal Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
				{"Emissive Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
				{"Metal Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
				{"Roughness Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
				{"Ambient Occlusion Texture", MaterialType::Texture2D, NULL_ASSET_HANDLE},
		};
	}

	uint32_t MaterialSet::CountBindinds() const {
		uint32_t count{0};
		for (const MaterialBlock &block: Blocks) {
			if (block.IsABuffer()) {
				count += 1;
			}
			else {
				count += static_cast<uint32_t>(block.Fields.size());
			}
		}
		return count;
	}

	MaterialPushConstant MaterialPushConstant::GetTransformPC() {
		return MaterialPushConstant{
				{{
						 {"WorldMatrix", MaterialType::FMat4, Math::Identity<glm::f32mat4>()},
						 {"NormalMatrix", MaterialType::FMat4, Math::Identity<glm::f32mat4>()},
						 {"VertexBuffer", MaterialType::ExternalBuffer},
				 },
				 true},
				ShaderStage::Vertex,
				0};
	}

	MaterialLayout MaterialLayout::GetDefaultLayout() {
		return {
			{MaterialSet::GetSceneSet(), MaterialSet::GetPBRSet()},
			{MaterialPushConstant::GetTransformPC()}
		};
	}
} // namespace Imagine::Core
