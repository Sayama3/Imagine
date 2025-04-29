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

	UUID::UUID() : m_UUID(s_UniformDistribution(s_RandomEngine)) {
	}

	std::string UUID::string() const
	{
		using namespace std::string_literals;
		return "UUID{"s + std::to_string(m_UUID) + "}"s;
	}

	bool operator==(const UUID &lft, const UUID &rht)
	{
		return lft.m_UUID == rht.m_UUID;
	}

	bool operator!=(const UUID &lft, const UUID &rht)
	{
		return !(lft == rht);
	}
}
