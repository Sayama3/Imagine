//
// Created by ianpo on 13/07/2025.
//

#include "portable-file-dialogs.h"

#include "Imagine/ThirdParty/FileDialogs.hpp"


namespace Imagine::ThirdParty {

	namespace FileDialogs {

		std::vector<std::string> OpenFile(const std::string &title, const std::string &initial_path, const std::vector<std::string>& filters, const Options option) {
			pfd::open_file of(title, initial_path, filters, option);
			return of.result();
		}

		std::string SaveFile(const std::string &title, const std::string &default_path, const std::vector<std::string> &filters, const Options options) {
			pfd::save_file sf(title, default_path, filters, options);
			return sf.result();
		}

		std::string SelectFolder(const std::string &title, const std::string &default_path, Options options) {
			pfd::select_folder sf(title, default_path, static_cast<pfd::opt>(options));
			return sf.result();
		}
	} // namespace FileDialogs

} // namespace Imagine::ThirdParty
