//
// Created by Sayama on 29/04/2025.
//

#include "Imagine/Core/UUID.hpp"
#include <random>

namespace Imagine::Core {
	// Necessary element ro create a random 64bits number;
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_RandomEngine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

	UUID::UUID() :
		m_UUID1(s_UniformDistribution(s_RandomEngine)), m_UUID2(s_UniformDistribution(s_RandomEngine)) {
	}

	std::string UUID::string() const {
		using namespace std::string_literals;
		return "UUID{"s + std::to_string(m_UUID1) + "_"s + std::to_string(m_UUID2) + "}"s;
	}

	UUID::operator bool() const {
		return m_UUID1 != 0 && m_UUID2 != 0;
	}

	bool operator==(const UUID &lft, const UUID &rht) {
		return lft.m_UUID1 == rht.m_UUID1 &&
			   lft.m_UUID2 == rht.m_UUID2;
	}

	bool operator!=(const UUID &lft, const UUID &rht) {
		return !(lft == rht);
	}

	std::strong_ordering UUID::operator<=>(const UUID &rht) const {
		const auto &lft = *this;
		if (lft.m_UUID1 == rht.m_UUID1) {
			return lft.m_UUID2 <=> rht.m_UUID2;
		}
		return lft.m_UUID1 <=> rht.m_UUID1;
	}

	UUID::const_iterator UUID::begin() const {
		return cbegin64();
	}
	UUID::const_iterator UUID::end() const {
		return cend64();
	}
	UUID::const_iterator UUID::cbegin() const {
		return begin();
	}

	UUID::const_iterator UUID::cend() const {
		return end();
	}

	UUID::const_iterator_64 UUID::cbegin64() const {
		return m_Array64.cbegin();
	}
	UUID::const_iterator_64 UUID::cend64() const {
		return m_Array64.cend();
	}

	UUID::const_iterator_32 UUID::cbegin32() const {
		return m_Array32.cbegin();
	}
	UUID::const_iterator_32 UUID::cend32() const {
		return m_Array32.cend();
	}

	UUID::const_iterator_8 UUID::cbegin8() const {
		return m_Array8.cbegin();
	}
	UUID::const_iterator_8 UUID::cend8() const {
		return m_Array8.cend();
	}

	UUID::iterator UUID::begin() {
		return begin64();
	}

	UUID::iterator UUID::end() {
		return end64();
	}

	UUID::iterator_64 UUID::begin64() {
		return m_Array64.begin();
	}
	UUID::iterator_64 UUID::end64() {
		return m_Array64.end();
	}
	UUID::iterator_32 UUID::begin32() {
		return m_Array32.begin();
	}
	UUID::iterator_32 UUID::end32() {
		return m_Array32.end();
	}
	UUID::iterator_8 UUID::begin8() {
		return m_Array8.begin();
	}
	UUID::iterator_8 UUID::end8() {
		return m_Array8.end();
	}
} // namespace Imagine::Core
