//
// Created by ianpo on 03/07/2025.
//

#include "Imagine/Assets/AssetHandle.hpp"

namespace Imagine {

	bool operator==(const AssetHandle &lft, const AssetHandle &rht) { return lft.m_Id == rht.m_Id; }

	bool operator!=(const AssetHandle &lft, const AssetHandle &rht) { return lft.m_Id != rht.m_Id; }

	std::strong_ordering AssetHandle::operator<=>(const AssetHandle &rht) const { return m_Id <=> rht.m_Id; }

	std::string AssetHandle::string() const {
		using namespace std::string_literals;
		return "AssetHandle{"s + std::to_string(m_Id.m_UUID1) + "_"s + std::to_string(m_Id.m_UUID2) + "}"s;
	}

	AssetHandle::operator bool() const {
		return m_Id.operator bool();
	}
	UUID AssetHandle::GetID() const {
		return m_Id;
	}
} // namespace Imagine
