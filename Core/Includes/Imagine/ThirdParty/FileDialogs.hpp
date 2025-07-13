//
// Created by ianpo on 13/07/2025.
//

#pragma once

namespace Imagine::ThirdParty {

	namespace FileDialogs {
		// Reused from pfd::opt. Used to not have to include the dependency here.

		/// Additional option flags for various dialog constructors
		enum Options : uint8_t {
			none = 0,
			/// For file open, allow multiselect.
			multiselect = 0x1,
			/// For file save, force overwrite and disable the confirmation dialog.
			force_overwrite = 0x2,
			/// For folder select, force path to be the provided argument instead
			/// of the last opened directory, which is the Microsoft-recommended,
			/// user-friendly behaviour.
			force_path = 0x4,
		};

		std::vector<std::string> OpenFile(const std::string &title, const std::string &initial_path, const std::vector<std::string> &filters = {"All Files", "*"}, Options option = Options::none);
		std::string SaveFile(const std::string &title, const std::string &default_path = "", const std::vector<std::string> &filters = {"All Files", "*"}, Options options = Options::none);
		std::string SelectFolder(const std::string &title, const std::string &default_path = "", Options options = Options::none);
	} // namespace FileDialogs

} // namespace Imagine::ThirdParty
