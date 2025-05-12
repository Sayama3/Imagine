//
// Created by ianpo on 03/05/2025.
//

#pragma once
#include "Macros.hpp"
#include "RTTI.hpp"

namespace Imagine::Core {

 	template<class Base>
 	struct RTTI {
 	private:
 		static inline std::unordered_map<std::string, Base*(*)()> Factory{};
 	public:
 		RTTI() = default;
 		~RTTI() = default;

 		template<typename Self>
 		inline RTTI() : type(typeid(Self).name()), parent(nullptr), createFunc(Self::Create) {
 			MGN_TRACE(type);
 			Initialise();
 		}

 		template<typename Self, typename ParentType>
 		inline RTTI() : type(typeid(Self).name()), parent(ParentType::rtti), createFunc(Self::Create) {
 			MGN_TRACE(type);
 			Initialise();
 		}

 		inline explicit RTTI(std::string name, Base*(*createFunc)(), const RTTI<Base>* parent_rtti) : type(std::move(name)), parent(parent_rtti), createFunc(createFunc) {
 			MGN_TRACE(type);
 			Initialise();
 		}

 		bool IsRoot() const {return !parent || parent==this;}
 		bool IsExactlyA(const RTTI<Base>& other) const;
 		bool IsA(const RTTI<Base>& other) const;
 	public:
 		Base* Create() {return createFunc();}
 		static inline Base* FactoryCreate(const std::string& type) {return Factory.contains(type) ? Factory.at(type)() : nullptr;}
 		static inline Base* FactoryCreate(const RTTI<Base>& rtti) {return Factory.contains(rtti.type) ? Factory.at(rtti.type)() : nullptr;}
 	private:
 		void Initialise()
 		{
 			Factory.insert({type, createFunc});
 		}
 	public:
 		const std::string type;
 		const RTTI<Base>* const parent;
 	private:
 		Base* (*createFunc) ();
 	};

 	template<class Base>
 	bool RTTI<Base>::IsExactlyA(const RTTI<Base>&other) const {
 		return type == other.type && parent == other.parent;
 	}

 	template<class Base>
 	bool RTTI<Base>::IsA(const RTTI<Base> &other) const {
 		const RTTI<Base>* comp = this;
 		do {
 			if (other.IsExactlyA(comp)) return true;
 			comp = comp->parent;
 		} while (parent != nullptr);
 		return false;
 	}
}

template<class Base>
struct ::std::hash<::Imagine::Core::RTTI<Base>>
{
	std::size_t operator()(const ::Imagine::Core::RTTI<Base>& s) const noexcept
	{
		const std::size_t h1 = std::hash<std::string>{}(s.type);
		const std::size_t h2 = std::hash<uint64_t>{}(s.parent);
		return h1 ^ (h2 << 1);
	}
};

#define RTTI_IMPLEMENT_CREATE(Base, Self) inline static Base* Create() { return new Self(); }
#define RTTI_ADVANCED_DECLARATION(Base, Self, TypeNameStr, Parent) inline static ::Imagine::Core::RTTI<Base> rtti{TypeNameStr, Self::Create, (typeid(Self)==typeid(Parent)?nullptr:&Parent::rtti)}

#define RTTI_DECLARATION(Base, Self, Parent) RTTI_IMPLEMENT_CREATE(Base, Self) RTTI_ADVANCED_DECLARATION(Base, Self, #Self, Parent)
#define RTTI_ROOT_DECLARATION(Base) RTTI_DECLARATION(Base, Base, Base)

//
// namespace Imagine::Core {
//
// 	class Something;
// 	using RTTISomething = ::Imagine::Core::RTTI<Something>;
//
//  	#define RTTISomething_DECLARATION(Self, Parent) RTTI_DECLARATION(::Imagine::Core::Something, Self, Parent)
//
//  	class Something {
//  	public:
//  		RTTI_ROOT_DECLARATION(Something);
// 		Something() {}
// 		virtual ~Something() {};
//
// 	};
//
// 	class SomeOtherThing : public Something {
// 	public:
// 		RTTISomething_DECLARATION(SomeOtherThing, Something);
// 		SomeOtherThing() {}
// 		virtual ~SomeOtherThing() override {};
// 	};
//
// 	class AnotherThing : public Something {
// 	public:
// 		RTTISomething_DECLARATION(AnotherThing, Something);
// 		AnotherThing() {}
// 		virtual ~AnotherThing() override {};
// 	};
//
// 	class AnotherSomeOtherThing : public SomeOtherThing {
// 	public:
// 		RTTISomething_DECLARATION(AnotherSomeOtherThing, SomeOtherThing);
// 		AnotherSomeOtherThing()
// 		{
// 		}
// 		virtual ~AnotherSomeOtherThing() override {};
// 	};
// }
