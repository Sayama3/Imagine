//
// Created by ianpo on 03/05/2025.
//

#pragma once

// namespace Imagine::Core {
//
// 	template<class Base>
// 	struct RTTI {
// 		RTTI() = default;
// 		~RTTI() = default;
// 		explicit RTTI(std::string name, const RTTI<Base>* parent_rtti) : type(std::move(name)), parent(parent_rtti) {}
// 		explicit RTTI(const char* cname, const RTTI<Base>* parent_rtti) : type(cname), parent(parent_rtti) {}
//
// 		bool IsExactlyA(const RTTI<Base>& other) const;
// 		bool IsA(const RTTI<Base>& other) const;
//
// 		const std::string type;
// 		const RTTI<Base>* const parent;
// 	};
//
// 	template<class Base>
// 	bool RTTI<Base>::IsExactlyA(const RTTI<Base>&other) const {
// 		return type == other.type && parent == other.parent;
// 	}
//
// 	template<class Base>
// 	bool RTTI<Base>::IsA(const RTTI<Base> &other) const {
// 		const RTTI<Base>* comp = this;
// 		do {
// 			if (other.IsExactlyA(comp)) return true;
// 			comp = comp->parent;
// 		} while (parent != nullptr);
// 		return false;
// 	}
// }
//
// #define RTTI_DECLARATION(base) static ::Imagine::Core::RTTI<base> rtti;
// #define RTTI_DEFINITION_BASE(base) ::Imagine::Core::RTTI<base> base::rtti{#base , nullptr};
// #define RTTI_DEFINITION(type, base, parent) ::Imagine::Core::RTTI<base> type::rtti{#type, &parent::rtti};


namespace Imagine::Core {

	template<class Base>
	class RTTIBase {
	public:
		virtual ~RTTIBase() = default;
		RTTIBase() : parent(nullptr) {}
		explicit RTTIBase(const RTTIBase* parent) : parent(parent) {}

		[[nodiscard]] virtual bool IsRoot() const {
			return parent == nullptr;
		}
		[[nodiscard]] virtual bool IsExactlyA(const RTTIBase& other) const {
			return this == &other;
		}
		[[nodiscard]] virtual bool IsA(const RTTIBase& other) const {
			const RTTIBase* comp = this;
			do {
				if (other.IsExactlyA(*comp)) return true;
				comp = comp->parent;
			} while (comp != nullptr);
			return false;
		}

		const RTTIBase* const parent;
	};

	template<class Self, class Parent, class Base>
	class RTTI {
	public:
		// inline static RTTIBase<Base> rtti {typeid(Self)==typeid(Parent) ? nullptr : &typename Parent::rtti};
	public:
		[[nodiscard]] inline static bool StaticIsRoot() {
			return typeid(Self) == typeid(Parent);
		}
		[[nodiscard]] inline bool IsRoot() const {
			return StaticIsRoot();
		}

		template<class OtherSelf, class OtherParent>
		[[nodiscard]] inline static bool StaticIsExactlyA(const RTTI<OtherSelf, OtherParent, Base>& other) {
			return typeid(Self) == typeid(OtherSelf) && typeid(Parent) == typeid(OtherParent);
		}
		template<class OtherSelf, class OtherParent>
		[[nodiscard]] inline bool IsExactlyA(const RTTI<OtherSelf, OtherParent, Base>& other) const {
			return StaticIsExactlyA(other);
		}

		template<class OtherSelf, class OtherParent>
		[[nodiscard]] inline static bool StaticIsA(const RTTI<OtherSelf, OtherParent, Base>& other) {
			return StaticIsExactlyA(other) || (!StaticIsRoot() && Parent::StaticIsA(other));
		}
		template<class OtherSelf, class OtherParent>
		[[nodiscard]] inline bool IsA(const RTTI<OtherSelf, OtherParent, Base>& other) const {
			return StaticIsA(other);
		}
	};


	class Something : public RTTI<Something, Something, Something> {

		Something() {}
		virtual ~Something() {};

	};

	class SomeOtherThing : public virtual RTTI<SomeOtherThing, Something, Something>, public virtual Something {
		SomeOtherThing(): Something() {}

		virtual ~SomeOtherThing() override {};
	};

	class AnotherThing : public RTTI<AnotherThing, Something, Something>, public virtual Something {
		AnotherThing(): Something() {}

		virtual ~AnotherThing() override {};
	};

	class AnotherSomeOtherThing : public RTTI<AnotherSomeOtherThing, SomeOtherThing, Something>, public virtual SomeOtherThing {
		AnotherSomeOtherThing(): SomeOtherThing() {}

		virtual ~AnotherSomeOtherThing() override {};
	};
}