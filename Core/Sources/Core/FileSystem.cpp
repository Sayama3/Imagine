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
	std::optional<uint32_t> FileSystem::ReadFourCC(const std::filesystem::path &path) {
		MGN_PROFILE_FUNCTION();
		if (!fs::exists(path)) return std::nullopt;
		std::error_code ec;
		const uintmax_t size = fs::file_size(path, ec);
		if (size < 4 || size == static_cast<std::uintmax_t>(-1)) return std::nullopt;

		const auto str = path.string();
		return InternalReadFourCC(str.c_str());
	}

	std::optional<uint32_t> FileSystem::ReadFourCC(const char *cpath) {
		MGN_PROFILE_FUNCTION();
		fs::path stdpath = cpath;
		if (!fs::exists(stdpath)) return std::nullopt;
		std::error_code ec;
		const uintmax_t size = fs::file_size(stdpath, ec);
		if (size < 4 || size == static_cast<std::uintmax_t>(-1)) return std::nullopt;

		return InternalReadFourCC(cpath);
	}

	std::optional<uint32_t> FileSystem::InternalReadFourCC(const char *cpath) {
		CFile file(cpath, "rb");
		if (!file.IsValid()) return std::nullopt;

		union {
			char fourcc_char[4];
			uint32_t fourcc_uint = 0;
		};

		const char *buff = fgets(fourcc_char, 4, file.filePtr);

		if (buff == nullptr) return std::nullopt;

		return fourcc_uint;
	}

	Buffer FileSystem::ReadBinaryFile(const std::filesystem::path &filePath) {
		const std::string fileStr = filePath.string();
		return std::move(ReadBinaryFile(fileStr.c_str()));
	}

	Buffer FileSystem::ReadBinaryFile(const char *filePath) {
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

	std::vector<uint8_t> FileSystem::ReadBinaryFileInVector(const std::filesystem::path &filePath) {
		const std::string fileStr = filePath.string();
		return std::move(ReadBinaryFileInVector(fileStr.c_str()));
	}
	std::vector<uint8_t> FileSystem::ReadBinaryFileInVector(const char *filePath) {
		std::vector<uint8_t> fileContent;

		// Don't use the CFile because fseek(file.filePtr, 0, SEEK_END) for a binary file is undefined behavior...
		//  Counting on C++ to correct this.
		std::ifstream file(filePath, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			return fileContent;
		}
		const size_t fileSize = file.tellg();

		fileContent.resize(fileSize);

		file.seekg(0);
		file.read(reinterpret_cast<char *>(fileContent.data()), fileSize);
		file.close();

		return std::move(fileContent);
	}

	std::vector<char> FileSystem::ReadTextFile(const std::filesystem::path &filePath) {
		const std::string fileStr = filePath.string();
		return std::move(ReadTextFile(fileStr.c_str()));
	}

	std::vector<char> FileSystem::ReadTextFile(const char *filePath) {
		std::vector<char> fileContent;

		CFile file(filePath, "r");

		fseek(file.filePtr, 0, SEEK_END);
		const auto size = ftell(file.filePtr);
		fseek(file.filePtr, 0, SEEK_SET);

		fileContent.resize(size);
		fgets(fileContent.data(), static_cast<int>(size), file.filePtr);

		return std::move(fileContent);
	}

	bool FileSystem::WriteBinaryFile(const std::filesystem::path &filePath, ConstBufferView view) {
		std::string path = filePath.string();
		return WriteBinaryFile(path.c_str(), view);
	}

	bool FileSystem::WriteBinaryFile(const char *filePath, ConstBufferView view) {
		CFile cfile(filePath, "wb");

		if (!cfile.IsValid()) return false;

		const size_t writtenSize = fwrite(view.Get(), 1, view.Size(), cfile.filePtr);

		return writtenSize == view.Size();
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
