//
// Created by ianpo on 03/05/2025.
//

#pragma once

namespace Imagine::Core {

	template<class Base>
	struct RTTI {
		RTTI() = default;
		~RTTI() = default;
		explicit RTTI(std::string name, const RTTI<Base>* parent_rtti) : type(std::move(name)), parent(parent_rtti) {}
		explicit RTTI(const char* cname, const RTTI<Base>* parent_rtti) : type(cname), parent(parent_rtti) {}

		bool IsExactlyA(const RTTI<Base>& other) const;
		bool IsA(const RTTI<Base>& other) const;

		const std::string type;
		const RTTI<Base>* const parent;
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

#define RTTI_DECLARATION(base) static ::Imagine::Core::RTTI<base> rtti;
#define RTTI_DEFINITION_BASE(base) ::Imagine::Core::RTTI<base> base::rtti{#base , nullptr};
#define RTTI_DEFINITION(type, base, parent) ::Imagine::Core::RTTI<base> type::rtti{#type, &parent::rtti};
