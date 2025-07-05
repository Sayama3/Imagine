//
// Created by ianpo on 23/05/2025.
//

#include "Imagine/Core/FileSystem.hpp"

#include "Imagine/Core/Profiling.hpp"
#include "Imagine/Project/Project.hpp"

namespace fs = std::filesystem;

namespace Imagine::Core {
	Path::Path(Path &&other) noexcept {
		swap(other);
	}

	Path &Path::operator=(Path &&other) noexcept {
		swap(other);
		return *this;
	}

	std::filesystem::path Path::GetFullPath() const {
		MGN_PROFILE_FUNCTION();
		std::filesystem::path p = FileSystem::GetRootPath(source) / path;
		p.make_preferred();
		return p;
	}

	bool Path::operator==(const Path &rhs) const {
		return this->source == rhs.source && this->path == rhs.path;
	}

	bool Path::equivalent(const Path &rt) const {
		return source == rt.source && std::filesystem::equivalent(GetFullPath(), rt.GetFullPath());
	}

	bool Path::operator!=(const Path &rhs) const {
		return !(*this == rhs);
	}

	std::string Path::id() const {
		std::filesystem::path p = GetFileSourceName(source) / path;
		return p.string();
	}

	void Path::swap(Path &other) noexcept {
		std::swap(path, other.path);
		std::swap(source, other.source);
	}

	Path Path::GetCachePath() const {
		return {FileSource::Cache, id()};
	}

	Path Path::GetPath(std::filesystem::path path) {
		const std::string pathStr = path.make_preferred().string();

		for (int i = 0; i < FileSourceCount; ++i) {
			auto source = static_cast<FileSource>(i);
			auto sourcePath = FileSystem::GetRootPath(source);
			std::string rootSourceStr = sourcePath.make_preferred().string();
			if (pathStr.starts_with(rootSourceStr)) {
				std::filesystem::path localPath = std::filesystem::relative(path, sourcePath);
				return {source, localPath};
			}
		}
		return {};
	}
	Buffer FileSystem::readBinaryFile(const std::filesystem::path &filePath) {
		const std::string fileStr = filePath.string();
		return std::move(readBinaryFile(fileStr.c_str()));
	}
	Buffer FileSystem::readBinaryFile(const char *filePath) {
		Buffer fileContent;

		// Don't use the CFile because fseek(file.filePtr, 0, SEEK_END) for a binary file is undefined behavior...
		//  Counting on C++ to correct this.
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			return fileContent;
		}
		const size_t fileSize = file.tellg();

		fileContent.Reallocate(fileSize);

		file.seekg(0);
		file.read(fileContent.Get<char>(), fileSize);
		file.close();

		return std::move(fileContent);
	}
	std::vector<char> FileSystem::readTextFile(const std::filesystem::path &filePath) {
		const std::string fileStr = filePath.string();
		return std::move(readTextFile(fileStr.c_str()));
	}
	std::vector<char> FileSystem::readTextFile(const char *filePath) {
		std::vector<char> fileContent;

		CFile file(filePath, "r");

		fseek(file.filePtr, 0, SEEK_END);
		const auto size = ftell(file.filePtr);
		fseek(file.filePtr, 0, SEEK_SET);

		fileContent.resize(size);
		fgets(fileContent.data(), static_cast<int>(size), file.filePtr);

		return std::move(fileContent);
	}

	bool FileSystem::Exist(Path path) {
		return path.source != FileSource::None && fs::exists(path.GetFullPath());
	}

	std::filesystem::path FileSystem::GetRootPath(FileSource source) {
		switch (source) {
			case FileSource::None:		{ return Project::GetProjectFilePath().parent_path(); }
			case FileSource::Engine:	{ return s_EditorPath / "EngineAssets/"; }
			case FileSource::Assets:	{ return Project::GetAssetsDirectory(); }
			case FileSource::Scripts:	{ return Project::GetScriptsDirectory(); }
			case FileSource::Cache:		{ return Project::GetCacheDirectory(); }
			default: break;
		}
		return Project::GetProjectFilePath().parent_path();
	}

	std::string Path::string() const {
		return GetFullPath().string();
	}
} // namespace Imagine::Core
