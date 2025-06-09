//
// Created by ianpo on 03/05/2025.
//

#pragma once
#include "Macros.hpp"

namespace Imagine::Core {

	template<class Base>
	struct RTTI {
	private:
		static inline std::unordered_map<std::string, Base *(*) ()> Factory{};

	public:
		RTTI() = default;
		~RTTI() = default;

		template<typename Self>
		inline RTTI() :
			type(typeid(Self).name()), parent(nullptr), createFunc(Self::Create) {
			// MGN_TRACE(type);
			Initialize();
		}

		template<typename Self, typename ParentType>
		inline RTTI() :
			type(typeid(Self).name()), parent(ParentType::rtti), createFunc(Self::Create) {
			// MGN_TRACE(type);
			Initialize();
		}

		inline explicit RTTI(std::string name, Base *(*createFunc)(), const RTTI<Base> *parent_rtti) :
			type(std::move(name)), parent(parent_rtti), createFunc(createFunc) {
			// MGN_TRACE(type);
			Initialize();
		}

		bool IsRoot() const { return !parent || parent == this; }
		bool IsExactlyA(const RTTI<Base> &other) const;
		bool IsA(const RTTI<Base> &other) const;

	public:
		bool operator==(const RTTI<Base> &other) const {
			return IsExactlyA(other);
		}
		bool operator!=(const RTTI<Base> &other) const {
			return !(*this == other);
		}

	public:
		Base *Create() { return createFunc(); }
		static inline Base *FactoryCreate(const std::string &type) {
#ifdef MGN_DEBUG
			if (Factory.contains(type)) {
				auto &createFunc = Factory.at(type);
				auto *obj = createFunc();
				return obj;
			}
			else {
				return nullptr;
			}
#else
			return Factory.contains(type) ? Factory.at(type)() : nullptr;
#endif
		}

		static inline Base *FactoryCreate(const RTTI<Base> &rtti) {
			return FactoryCreate(rtti.type);
		}

		template<class T>
		static inline Base *FactoryCreate() {
			return FactoryCreate(T::rtti.type);
		}

	private:
		void Initialize() {
			Factory.insert({type, createFunc});
		}

	public:
		const std::string type;
		const RTTI<Base> *const parent;

	private:
		Base *(*createFunc)();
	};

	template<class Base>
	bool RTTI<Base>::IsExactlyA(const RTTI<Base> &other) const {
		return type == other.type && parent == other.parent;
	}

	template<class Base>
	bool RTTI<Base>::IsA(const RTTI<Base> &other) const {
		const RTTI<Base> *comp = this;
		do {
			if (other.IsExactlyA(*comp)) return true;
			comp = comp->parent;
		} while (comp != nullptr);
		return false;
	}
} // namespace Imagine::Core

template<class Base>
struct std::hash<::Imagine::Core::RTTI<Base>> {
	std::size_t operator()(const ::Imagine::Core::RTTI<Base> &s) const noexcept {
		const std::size_t h1 = std::hash<std::string>{}(s.type);
		const std::size_t h2 = std::hash<uint64_t>{}(s.parent);
		return h1 ^ (h2 << 1);
	}
};

#define RTTI_IMPLEMENT_CREATE(Base, Self) \
	inline static Base *Create() { return new Self(); }
#define RTTI_ROOT_ADVANCED_DECLARATION(Base, TypeNameStr) inline static ::Imagine::Core::RTTI<Base> rtti{TypeNameStr, Base::Create, nullptr};
#define RTTI_ADVANCED_DECLARATION(Base, Self, TypeNameStr, Parent) inline static ::Imagine::Core::RTTI<Base> rtti{TypeNameStr, Self::Create, &Parent::rtti};
#define RTTI_IMPLEMENT_THIS_RTTI(Base, Self) \
	virtual ::Imagine::Core::RTTI<Base> &get_rtti() { return Self::rtti; };

#define RTTI_ROOT_DECLARATION(Base) RTTI_IMPLEMENT_CREATE(Base, Base) RTTI_ROOT_ADVANCED_DECLARATION(Base, #Base) RTTI_IMPLEMENT_THIS_RTTI(Base, Base)
#define RTTI_DECLARATION(Base, Self, Parent) RTTI_IMPLEMENT_CREATE(Base, Self) RTTI_ADVANCED_DECLARATION(Base, Self, #Self, Parent) RTTI_IMPLEMENT_THIS_RTTI(Base, Self)
