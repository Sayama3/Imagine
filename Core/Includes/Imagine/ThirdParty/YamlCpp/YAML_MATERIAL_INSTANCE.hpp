//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include "Imagine/Rendering/CPU/CPUMaterialInstance.hpp"
#include "YAML_DEFINE.hpp"
#include "YAML_MATERIAL.hpp"
#include "YAML_STD.hpp"


namespace YAML {
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUMaterialInstance::SetFieldPosition &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Set Index", v.SetIndex);
			out << KEYVAL("Binding Index", v.BindingIndex);
			out << KEYVAL("Field Index", v.FieldIndex);
		}
		out << YAML::EndMap;
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUMaterialInstance::PushConstantFieldPosition &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Push Constant Index", v.PCIndex);
			out << KEYVAL("Field Index", v.FieldIndex);
		}
		out << YAML::EndMap;
		return out;
	}
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::CPUMaterialInstance &v) {
		out << YAML::BeginMap;
		{
			out << KEYVAL("Handle", v.Handle);
			out << KEYVAL("Set Editions", v.SetEditions);
			out << KEYVAL("Push Constant Editions", v.PushConstantEditions);
			out << KEYVAL("Material", v.Material);
		}
		out << YAML::EndMap;
		return out;
	}
} // namespace Imagine::ThirdParty::YamlCpp

namespace YAML {
	template<> struct convert<::Imagine::CPUMaterialInstance::SetFieldPosition> {
		inline static Node encode(const Imagine::CPUMaterialInstance::SetFieldPosition& v) {
			Node node;
			node["Set Index"] =  v.SetIndex;
			node["Binding Index"] =  v.BindingIndex;
			node["Field Index"] =  v.FieldIndex;
			return node;
		}
		inline static bool decode(const Node& node, Imagine::CPUMaterialInstance::SetFieldPosition& v) {
			v.SetIndex = node["Set Index"].as<uint32_t>();
			v.BindingIndex = node["Binding Index"].as<uint32_t>();
			v.FieldIndex = node["Field Index"].as<uint32_t>();
			return true;
		}
	};
	template<> struct convert<::Imagine::CPUMaterialInstance::PushConstantFieldPosition> {
		inline static Node encode(const Imagine::CPUMaterialInstance::PushConstantFieldPosition& v) {
			Node node;
			node["Push Constant Index"] =  v.PCIndex;
			node["Field Index"] =  v.FieldIndex;
			return node;
		}
		inline static bool decode(const Node& node, Imagine::CPUMaterialInstance::PushConstantFieldPosition& v) {
			v.PCIndex = node["Push Constant Index"].as<uint32_t>();
			v.FieldIndex = node["Field Index"].as<uint32_t>();
			return true;
		}
	};
	template<>
	struct convert<::Imagine::CPUMaterialInstance> {
		inline static Node encode(const ::Imagine::CPUMaterialInstance &v) {
			Node node;
			node["Handle"] = v.Handle;
			node["Set Editions"] = v.SetEditions;
			node["Push Constant Editions"] = v.PushConstantEditions;
			node["Material"] = v.Material;
			return node;
		}
		inline static bool decode(const Node &node, ::Imagine::CPUMaterialInstance &v) {
			v.Handle = node["Handle"].as<Imagine::AssetHandle>();
			v.SetEditions = node["Set Editions"].as<std::map<Imagine::CPUMaterialInstance::SetFieldPosition, Imagine::CPUMaterialInstance::MaterialDataBuffer>>();
			v.PushConstantEditions = node["Push Constant Editions"].as<std::map<Imagine::CPUMaterialInstance::PushConstantFieldPosition, Imagine::CPUMaterialInstance::MaterialDataBuffer>>();
			v.Material = node["Material"].as<Imagine::AssetHandle>();
			return true;
		}
	};
}
