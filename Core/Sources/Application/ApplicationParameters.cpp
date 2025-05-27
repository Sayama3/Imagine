//
// Created by ianpo on 27/05/2025.
//

#include "Imagine/Application/ApplicationParameters.hpp"
#include <spdlog/fmt/fmt.h>

namespace Imagine::Core {

	uint32_t ApplicationParameters::GetMajor() const { return MGN_VERSION_MAJOR(AppVersion); }
	uint32_t ApplicationParameters::GetMinor() const { return MGN_VERSION_MINOR(AppVersion); }
	uint32_t ApplicationParameters::GetPatch() const { return MGN_VERSION_PATCH(AppVersion); }
	std::string ApplicationParameters::GetVersionStr() const { return fmt::format("{}.{}.{}", GetMajor(), GetMinor(), GetPatch()); }
	void ApplicationParameters::SetVersion(const uint32_t major, const uint32_t minor, const uint32_t patch) { AppVersion = MGN_MAKE_VERSION(major, minor, patch); }
} // namespace Imagine::Core
