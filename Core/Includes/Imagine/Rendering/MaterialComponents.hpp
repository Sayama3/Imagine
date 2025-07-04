//
// Created by ianpo on 03/07/2025.
//

#pragma once

#include "Imagine/Assets/AssetHandle.hpp"
#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Math/Core.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Imagine/Rendering/ShaderParameters.hpp"
#include <spdlog/spdlog.h>

#define MGN_CHECK_MAT_SIZE(TYPE, MAT_TYPE) static_assert(sizeof(TYPE) == Helper::MaterialTypeSize(MAT_TYPE));
#define MGN_IMPLEMENT_MAT_TYPE(TYPE)                                                    \
	[[nodiscard]] static inline constexpr MaterialType GetStaticType() { return TYPE; } \
	[[nodiscard]] inline bool IsProperType() const { return GetStaticType() == MaterialField::type; }

namespace Imagine::Core {

	enum class MaterialFilling : uint8_t {
		Fill,
		Wire,
	};

	enum class MaterialPass : uint8_t {
		MainColor,
		Transparent,
		Other
	};

	enum class MaterialType : uint16_t {
		None = 0,

		Float1,
		Float2,
		Float3,
		Float4,

		Color3,
		Color4,

		Int1,
		Int2,
		Int3,
		Int4,

		FMat2,
		FMat3,
		FMat4,

		IMat2,
		IMat3,
		IMat4,

		VirtualTexture2D,
		VirtualTexture3D,

		Texture2D,
		Texture3D,
		Cubemap,

		Vertex,

		ExternalBuffer,
	};


	namespace Helper {

		static inline constexpr bool IsBufferType(const MaterialType type) {
			switch (type) {
				case MaterialType::Float1:
				case MaterialType::Float2:
				case MaterialType::Float3:
				case MaterialType::Float4:
				case MaterialType::Color3:
				case MaterialType::Color4:
				case MaterialType::Int1:
				case MaterialType::Int2:
				case MaterialType::Int3:
				case MaterialType::Int4:
				case MaterialType::FMat2:
				case MaterialType::FMat3:
				case MaterialType::FMat4:
				case MaterialType::IMat2:
				case MaterialType::IMat3:
				case MaterialType::IMat4:
				case MaterialType::Vertex:
				case MaterialType::ExternalBuffer:
					return true;
				default:
					return false;
			}
		}

		[[nodiscard]] static constexpr std::string MaterialTypeString(const MaterialType type) {
			switch (type) {
				case MaterialType::None: return "None";
				case MaterialType::Float1: return "Float1";
				case MaterialType::Float2: return "Float2";
				case MaterialType::Float3: return "Float3";
				case MaterialType::Float4: return "Float4";
				case MaterialType::Color3: return "Color3";
				case MaterialType::Color4: return "Color4";
				case MaterialType::Int1: return "Int1";
				case MaterialType::Int2: return "Int2";
				case MaterialType::Int3: return "Int3";
				case MaterialType::Int4: return "Int4";
				case MaterialType::FMat2: return "FMat2";
				case MaterialType::FMat3: return "FMat3";
				case MaterialType::FMat4: return "FMat4";
				case MaterialType::IMat2: return "IMat2";
				case MaterialType::IMat3: return "IMat3";
				case MaterialType::IMat4: return "IMat4";
				case MaterialType::VirtualTexture2D: return "VirtualTexture2D";
				case MaterialType::VirtualTexture3D: return "VirtualTexture3D";
				case MaterialType::Texture2D: return "Texture2D";
				case MaterialType::Texture3D: return "Texture3D";
				case MaterialType::Cubemap: return "Cubemap";
				case MaterialType::Vertex: return "Vertex";
				case MaterialType::ExternalBuffer: return "ExternalBuffer";
			}
			return "Unknown";
		}
		[[nodiscard]] static constexpr uint64_t MaterialTypeSize(const MaterialType type) {
			switch (type) {
				case MaterialType::None:
					return 0;
				case MaterialType::Float1:
					return sizeof(float) * 1u;
				case MaterialType::Float2:
					return sizeof(float) * 2u;
				case MaterialType::Float3:
					return sizeof(float) * 3u;
				case MaterialType::Float4:
					return sizeof(float) * 4u;
				case MaterialType::Color3:
					return sizeof(float) * 3u;
				case MaterialType::Color4:
					return sizeof(float) * 4u;
				case MaterialType::Int1:
					return sizeof(int32_t) * 1u;
				case MaterialType::Int2:
					return sizeof(int32_t) * 2u;
				case MaterialType::Int3:
					return sizeof(int32_t) * 3u;
				case MaterialType::Int4:
					return sizeof(int32_t) * 4u;
				case MaterialType::FMat2:
					return sizeof(float) * 2u * 2u;
				case MaterialType::FMat3:
					return sizeof(float) * 3u * 3u;
				case MaterialType::FMat4:
					return sizeof(float) * 4u * 4u;
				case MaterialType::IMat2:
					return sizeof(int32_t) * 2u * 2u;
				case MaterialType::IMat3:
					return sizeof(int32_t) * 3u * 3u;
				case MaterialType::IMat4:
					return sizeof(int32_t) * 4u * 4u;
				case MaterialType::VirtualTexture2D:
					return sizeof(AssetHandle); // Assuming to be some sort of GPU Handle.
				case MaterialType::VirtualTexture3D:
					return sizeof(AssetHandle); // Assuming to be some sort of GPU Handle.
				case MaterialType::Texture2D:
					return sizeof(AssetHandle);
				case MaterialType::Texture3D:
					return sizeof(AssetHandle);
				case MaterialType::Cubemap:
					return sizeof(AssetHandle);
				case MaterialType::Vertex:
					return sizeof(Vertex);
				case MaterialType::ExternalBuffer:
					return sizeof(uint64_t);
			}

			return 0;
		}
	} // namespace Helper

	struct MaterialField {
		// This is the size of a Vertex as it's currently my biggest datatype
		static inline constexpr uint64_t DataSize{4 * 4 * 5};

	public:
		MaterialField() = default;
		MaterialField(std::string name, MaterialType type);
		template<typename T>
		constexpr MaterialField(std::string name, const MaterialType type, const T &defaultValue) :
			name(std::move(name)), type(type) {
			memcpy(data.data(), &defaultValue, std::min(sizeof(T), DataSize));
		}

	public:
		[[nodiscard]] uint64_t GetSize() const;
		[[nodiscard]] ConstBufferView GetView() const;

	public:
		std::string name{};
		// Preallocated data for all subsequent types.
		std::array<uint8_t, DataSize> data{0};
		MaterialType type{MaterialType::None};
	};

	template<typename T>
	struct TMaterialField : public MaterialField {
		static_assert(sizeof(T) <= MaterialField::DataSize);

		[[nodiscard]] T &value() { return *(T *) data.data(); }
		[[nodiscard]] const T &value() const { return *(const T *) data.data(); }
	};

	struct MaterialFieldFloat1 : public TMaterialField<glm::fvec1> {
		MGN_CHECK_MAT_SIZE(glm::fvec1, MaterialType::Float1);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Float1);
	};
	struct MaterialFieldFloat2 : public TMaterialField<glm::fvec2> {
		MGN_CHECK_MAT_SIZE(glm::fvec2, MaterialType::Float2);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Float2);
	};
	struct MaterialFieldFloat3 : public TMaterialField<glm::fvec3> {
		MGN_CHECK_MAT_SIZE(glm::fvec3, MaterialType::Float3);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Float3);
	};
	struct MaterialFieldFloat4 : public TMaterialField<glm::fvec4> {
		MGN_CHECK_MAT_SIZE(glm::fvec4, MaterialType::Float4);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Float4);
	};
	struct MaterialFieldColor3 : public TMaterialField<glm::fvec3> {
		MGN_CHECK_MAT_SIZE(glm::fvec3, MaterialType::Color3);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Color3);
	};
	struct MaterialFieldColor4 : public TMaterialField<glm::fvec4> {
		MGN_CHECK_MAT_SIZE(glm::fvec4, MaterialType::Color4);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Color4);
	};
	struct MaterialFieldInt1 : public TMaterialField<glm::i32vec1> {
		MGN_CHECK_MAT_SIZE(glm::i32vec1, MaterialType::Int1);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Int1);
	};
	struct MaterialFieldInt2 : public TMaterialField<glm::i32vec2> {
		MGN_CHECK_MAT_SIZE(glm::i32vec2, MaterialType::Int2);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Int2);
	};
	struct MaterialFieldInt3 : public TMaterialField<glm::i32vec3> {
		MGN_CHECK_MAT_SIZE(glm::i32vec3, MaterialType::Int3);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Int3);
	};
	struct MaterialFieldInt4 : public TMaterialField<glm::i32vec4> {
		MGN_CHECK_MAT_SIZE(glm::i32vec4, MaterialType::Int4);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Int4);
	};
	struct MaterialFieldFMat2 : public TMaterialField<glm::fmat2> {
		MGN_CHECK_MAT_SIZE(glm::fmat2, MaterialType::FMat2);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::FMat2);
	};
	struct MaterialFieldFMat3 : public TMaterialField<glm::fmat3> {
		MGN_CHECK_MAT_SIZE(glm::fmat3, MaterialType::FMat3);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::FMat3);
	};
	struct MaterialFieldFMat4 : public TMaterialField<glm::fmat4> {
		MGN_CHECK_MAT_SIZE(glm::fmat4, MaterialType::FMat4);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::FMat4);
	};
	struct MaterialFieldIMat2 : public TMaterialField<glm::i32mat2x2> {
		MGN_CHECK_MAT_SIZE(glm::i32mat2x2, MaterialType::IMat2);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::IMat2);
	};
	struct MaterialFieldIMat3 : public TMaterialField<glm::i32mat3x3> {
		MGN_CHECK_MAT_SIZE(glm::i32mat3x3, MaterialType::IMat3);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::IMat3);
	};
	struct MaterialFieldIMat4 : public TMaterialField<glm::i32mat4x4> {
		MGN_CHECK_MAT_SIZE(glm::i32mat4x4, MaterialType::IMat4);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::IMat4);
	};
	struct MaterialFieldVirtualTexture2D : public TMaterialField<AssetHandle> {
		MGN_CHECK_MAT_SIZE(AssetHandle, MaterialType::VirtualTexture2D);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::VirtualTexture2D);
	};
	struct MaterialFieldVirtualTexture3D : public TMaterialField<AssetHandle> {
		MGN_CHECK_MAT_SIZE(AssetHandle, MaterialType::VirtualTexture3D);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::VirtualTexture3D);
	};
	struct MaterialFieldTexture2D : public TMaterialField<AssetHandle> {
		MGN_CHECK_MAT_SIZE(AssetHandle, MaterialType::Texture2D);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Texture2D);
	};
	struct MaterialFieldTexture3D : public TMaterialField<AssetHandle> {
		MGN_CHECK_MAT_SIZE(AssetHandle, MaterialType::Texture3D);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Texture3D);
	};
	struct MaterialFieldCubemap : public TMaterialField<AssetHandle> {
		MGN_CHECK_MAT_SIZE(AssetHandle, MaterialType::Cubemap);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Cubemap);
	};
	struct MaterialFieldVertex : public TMaterialField<Vertex> {
		MGN_CHECK_MAT_SIZE(Vertex, MaterialType::Vertex);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::Vertex);
	};
	struct MaterialFieldExternalBuffer : public TMaterialField<uint64_t> {
		MGN_CHECK_MAT_SIZE(uint64_t, MaterialType::ExternalBuffer);
		MGN_IMPLEMENT_MAT_TYPE(MaterialType::ExternalBuffer);
	};


	class MaterialBlock {
	public:
		enum BufferType : uint8_t {
			SSBO,
			Uniform,
		};
	public:
		MaterialBlock() = default;
		~MaterialBlock() = default;
		MaterialBlock(const MaterialField &field);
		MaterialBlock(const std::initializer_list<MaterialField> &fields);
		MaterialBlock(const std::initializer_list<MaterialField> &fields, bool readOnly);

		MaterialBlock(const std::string &name, MaterialType type);
		template<typename T>
		constexpr MaterialBlock(const std::string &name, const MaterialType type, const T &defaultValue) :
			fields(1, {name, type, defaultValue}) {}

	public:
		static MaterialBlock GetSceneBlock();
		static MaterialBlock GetPBRConstantBlock();

	public:
		void Push(std::string name, MaterialType type);
		/// This function will separate the buffer from the non-buffer types and return a material block per optimal binding.
		std::vector<MaterialBlock> SplitBlock() const;
		bool IsABuffer() const;
		void Add(const MaterialBlock& block);
	public:
		[[nodiscard]] uint64_t GetSize() const;
		[[nodiscard]] Buffer GetCompactBuffer() const;

		std::vector<MaterialField> fields;
		std::string name{};
		bool read{true};
		bool write{true};
		BufferType bufferType;
	};

	class MaterialSet {
	public:
		MaterialSet() = default;
		~MaterialSet() = default;
		MaterialSet(const std::initializer_list<MaterialBlock> &blocks);

		static MaterialSet GetSceneSet();
		static MaterialSet GetPBRSet();
	public:
		uint32_t CountBindinds() const;
	public:
		std::vector<MaterialBlock> Blocks;
		ShaderStage Stages{ShaderStage::All};
	};

	class MaterialPushConstant {
	public:
		MaterialBlock Block;
		ShaderStage Stages{ShaderStage::Vertex};
		uint32_t offset{0};
	};

	class MaterialLayout {
	public:
		std::vector<MaterialSet> Sets;
		std::vector<MaterialPushConstant> PushConstants;
	};

} // namespace Imagine::Core


template<>
struct fmt::formatter<Imagine::Core::MaterialFilling> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::MaterialFilling value, format_context &ctx) const -> decltype(ctx.out())
	{
		std::string str{"Unknown."};
		switch (value) {
			case Imagine::Core::MaterialFilling::Fill:
				str = "Fill";
				break;
			case Imagine::Core::MaterialFilling::Wire:
				str = "Wire";
				break;
		}
		return format_to(ctx.out(), "{}", str);
	}
};
template<>
struct fmt::formatter<Imagine::Core::MaterialPass> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::MaterialPass value, format_context &ctx) const -> decltype(ctx.out())
	{
		std::string str{"Unknown."};
		switch (value) {
			case Imagine::Core::MaterialPass::MainColor:
				str = "MainColor";
				break;
			case Imagine::Core::MaterialPass::Transparent:
				str = "Transparent";
				break;
			case Imagine::Core::MaterialPass::Other:
				str = "Other";
				break;
		}
		return format_to(ctx.out(), "{}", str);
	}
};
template<>
struct fmt::formatter<Imagine::Core::MaterialType> : fmt::formatter<std::string>
{
	auto format(Imagine::Core::MaterialType value, format_context &ctx) const -> decltype(ctx.out())
	{
		return format_to(ctx.out(), "{}", Imagine::Core::Helper::MaterialTypeString(value));
	}
};

template<typename S>
void serialize(S &s, Imagine::Core::MaterialType &mt) {
	s.value2b(mt);
}
template<typename S>
void serialize(S &s, Imagine::Core::MaterialField &mf) {
	s.text1b(mf.name.c_str(), 32);
	s.container1b(mf.data);
	s.serialize(mf.type);
}

template<typename S>
void serialize(S &s, Imagine::Core::MaterialBlock &v) {
	s.container(v.fields, 100, serialize);
	s.text1b(v.name.c_str(), 32);
	s.value1b(v.read);
	s.value1b(v.write);
	s.value1b(v.bufferType);
}

template<typename S>
void serialize(S &s, Imagine::Core::MaterialSet &v) {
	s.container(v.Blocks, 100, serialize);
	s.value2b(v.Stages);
}

template<typename S>
void serialize(S &s, Imagine::Core::MaterialPushConstant &v) {
	s.object(v.Block);
	s.value2b(v.Stages);
	s.value4b(v.offset);
}