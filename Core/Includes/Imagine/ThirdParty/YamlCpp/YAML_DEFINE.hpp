//
// Created by ianpo on 07/07/2025.
//

#pragma once

#include <yaml-cpp/yaml.h>

#define KEY(x) YAML::Key << x
#define VAL(x) YAML::Value << x
#define KEYVAL(x, y) KEY(x) << VAL(y)


#define YAML_SIMPLE_EMITTER_FUNC(Type, FuncTo)                                 \
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Type &v) { \
		out << FuncTo;                                                         \
		return out;                                                            \
	}


#define YAML_SIMPLE_CONVERTER_FUNC(Type, FuncTo, FuncFrom)       \
	template<>                                                   \
	struct convert<Type> {                                     \
		inline static Node encode(const Type &v) {              \
			Node node;                                           \
			node = FuncTo;                                       \
			return node;                                         \
		}                                                        \
                                                                 \
		inline static bool decode(const Node &node, Type &v) { \
			return FuncFrom;                                     \
		}                                                        \
	};

#define YAML_SIMPLE_MGN_EMITTER_FUNC(Type, FuncTo) YAML_SIMPLE_EMITTER_FUNC(Imagine::Core::##Type, FuncTo)
#define YAML_SIMPLE_MGN_CONVERTER_FUNC(Type, FuncTo, FuncFrom) YAML_SIMPLE_CONVERTER_FUNC(Imagine::Core::##Type, FuncTo, FuncFrom)

#define YAML_SIMPLE_MGN_ENUM_EMITTER(EnumType) \
	YAML_SIMPLE_MGN_EMITTER_FUNC(EnumType, Imagine::Core::##EnumType##ToString(v))

#define YAML_SIMPLE_MGN_ENUM_CONVERTER(EnumType) \
	YAML_SIMPLE_MGN_CONVERTER_FUNC(EnumType, Imagine::Core::##EnumType##ToString(v), Imagine::Core::Try##EnumType##FromString(node.as<std::string>(), v))


#define YAML_TEMPLATE_EMITTER(IMAGINE_TYPE)                                                        \
	inline YAML::Emitter &operator<<(YAML::Emitter &out, const Imagine::Core::##IMAGINE_TYPE &v) { \
		out << YAML::BeginMap;                                                                     \
		{                                                                                          \
		}                                                                                          \
		out << YAML::EndMap;                                                                       \
		return out;                                                                                \
	}


#define YAML_TEMPLATE_CONVERTER(IMAGINE_TYPE)                                           \
	template<>                                                                          \
	struct convert<Imagine::Core::##IMAGINE_TYPE> {                                     \
		inline static Node encode(const Imagine::Core::##IMAGINE_TYPE &v) {             \
			Node node;                                                                  \
			return node;                                                                \
		}                                                                               \
                                                                                        \
		inline static bool decode(const Node &node, Imagine::Core::##IMAGINE_TYPE &v) { \
			return true;                                                                \
		}                                                                               \
	};
