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
		[[nodiscard]] static constexpr std::string MaterialFillingToString(const MaterialFilling value) {
			switch (value) {

				case MaterialFilling::Fill: return "Fill";
				case MaterialFilling::Wire: return "Wire";
			}
			return "Unknown";
		}
		[[nodiscard]] static constexpr bool TryMaterialFillingFromString(const std::string& str, MaterialFilling& val) {
			if(str == "Fill") {val = MaterialFilling::Fill; return true;}
			if(str == "Wire") {val = MaterialFilling::Wire; return true;}
			return false;
		}
		[[nodiscard]] static constexpr std::string MaterialPassToString(const MaterialPass value) {
			switch (value) {

				case MaterialPass::MainColor: return "MainColor";
				case MaterialPass::Transparent: return "Transparent";
				case MaterialPass::Other: return "Other";
			}
			return "Unknown";
		}
		[[nodiscard]] static constexpr bool TryMaterialPassFromString(const std::string& str, MaterialPass& val) {
			if(str == "MainColor") {val = MaterialPass::MainColor; return true;}
			if(str == "Transparent") {val = MaterialPass::Transparent; return true;}
			if(str == "Other") {val = MaterialPass::Other; return true;}
			return false;
		}
		[[nodiscard]] static constexpr std::string MaterialTypeToString(const MaterialType type) {
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

		[[nodiscard]] static constexpr MaterialType MaterialTypeFromString(const std::string& str) {
			if(str == "None") return MaterialType::None;
			if(str == "Float1") return MaterialType::Float1;
			if(str == "Float2") return MaterialType::Float2;
			if(str == "Float3") return MaterialType::Float3;
			if(str == "Float4") return MaterialType::Float4;
			if(str == "Color3") return MaterialType::Color3;
			if(str == "Color4") return MaterialType::Color4;
			if(str == "Int1") return MaterialType::Int1;
			if(str == "Int2") return MaterialType::Int2;
			if(str == "Int3") return MaterialType::Int3;
			if(str == "Int4") return MaterialType::Int4;
			if(str == "FMat2") return MaterialType::FMat2;
			if(str == "FMat3") return MaterialType::FMat3;
			if(str == "FMat4") return MaterialType::FMat4;
			if(str == "IMat2") return MaterialType::IMat2;
			if(str == "IMat3") return MaterialType::IMat3;
			if(str == "IMat4") return MaterialType::IMat4;
			if(str == "VirtualTexture2D") return MaterialType::VirtualTexture2D;
			if(str == "VirtualTexture3D") return MaterialType::VirtualTexture3D;
			if(str == "Texture2D") return MaterialType::Texture2D;
			if(str == "Texture3D") return MaterialType::Texture3D;
			if(str == "Cubemap") return MaterialType::Cubemap;
			if(str == "Vertex") return MaterialType::Vertex;
			if(str == "ExternalBuffer") return MaterialType::ExternalBuffer;
			return MaterialType::None;
		}

		[[nodiscard]] static constexpr bool TryMaterialTypeFromString(const std::string& str, MaterialType& value) {
			if(str == "None") {value = MaterialType::None; return true;}
			if(str == "Float1") {value = MaterialType::Float1; return true;}
			if(str == "Float2") {value = MaterialType::Float2; return true;}
			if(str == "Float3") {value = MaterialType::Float3; return true;}
			if(str == "Float4") {value = MaterialType::Float4; return true;}
			if(str == "Color3") {value = MaterialType::Color3; return true;}
			if(str == "Color4") {value = MaterialType::Color4; return true;}
			if(str == "Int1") {value = MaterialType::Int1; return true;}
			if(str == "Int2") {value = MaterialType::Int2; return true;}
			if(str == "Int3") {value = MaterialType::Int3; return true;}
			if(str == "Int4") {value = MaterialType::Int4; return true;}
			if(str == "FMat2") {value = MaterialType::FMat2; return true;}
			if(str == "FMat3") {value = MaterialType::FMat3; return true;}
			if(str == "FMat4") {value = MaterialType::FMat4; return true;}
			if(str == "IMat2") {value = MaterialType::IMat2; return true;}
			if(str == "IMat3") {value = MaterialType::IMat3; return true;}
			if(str == "IMat4") {value = MaterialType::IMat4; return true;}
			if(str == "VirtualTexture2D") {value = MaterialType::VirtualTexture2D; return true;}
			if(str == "VirtualTexture3D") {value = MaterialType::VirtualTexture3D; return true;}
			if(str == "Texture2D") {value = MaterialType::Texture2D; return true;}
			if(str == "Texture3D") {value = MaterialType::Texture3D; return true;}
			if(str == "Cubemap") {value = MaterialType::Cubemap; return true;}
			if(str == "Vertex") {value = MaterialType::Vertex; return true;}
			if(str == "ExternalBuffer") {value = MaterialType::ExternalBuffer; return true;}
			return false;
		}

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
		[[nodiscard]] BufferView GetView();

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
			Fields(1, {name, type, defaultValue}) {}

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

		std::vector<MaterialField> Fields;
		std::string Name{};
		bool Read{true};
		bool Write{true};
		BufferType GPUBufferType{BufferType::Uniform};
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
		return format_to(ctx.out(), "{}", Imagine::Core::Helper::MaterialTypeToString(value));
	}
};