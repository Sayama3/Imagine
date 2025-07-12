//
// Created by ianpo on 23/05/2025.
//

#pragma once

#include <utility>

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/Macros.hpp"

namespace Imagine {

	struct CFile {
		CFile() = default;
		CFile(const char *filePath, const char *mode = "w") :
			filePtr(fopen(filePath, mode)) {}
		CFile(FILE *filePtr) :
			filePtr(filePtr) {}
		~CFile() {
			Close();
		}

		inline void Open(const char *filePath, const char *mode = "w") {
			Close();
			filePtr = fopen(filePath, mode);
		}

		inline void Close() {
			if (filePtr) {
				fclose(filePtr);
				filePtr = nullptr;
			}
		}

		[[nodiscard]] inline bool IsValid() const { return filePtr; }

		FILE *filePtr{nullptr};
	};


	/// This variable contain the number of filesources excluding "None".
	/// So if you iterate using this value, add 1 to your value.
	static inline constexpr uint8_t FileSourceCount{4};
	enum class FileSource : uint8_t {
		None = 0,
		Engine,
		Assets,
		Scripts,
		Cache,
		External,
	};

	inline static constexpr std::string GetFileSourceName(const FileSource source) {
		switch (source) {
			case FileSource::None:
				return "None";
			case FileSource::Engine:
				return "Engine";
			case FileSource::Assets:
				return "Assets";
			case FileSource::Scripts:
				return "Scripts";
			case FileSource::Cache:
				return "Cache";
			case FileSource::External:
				return "External";
		}
		return "Unknown";
	}

	inline static constexpr FileSource GetFileSourceFromName(const std::string& sourceStr) {
		if(sourceStr == "Engine") return FileSource::Engine;
		if(sourceStr == "Assets") return FileSource::Assets;
		if(sourceStr == "Scripts") return FileSource::Scripts;
		if(sourceStr == "Cache") return FileSource::Cache;
		if(sourceStr == "External") return FileSource::External;
		return FileSource::None;
	}

	inline static constexpr bool TryGetFileSourceFromName(const std::string& sourceStr, FileSource& source) {
		if(sourceStr == "Engine") {source = FileSource::Engine; return true;}
		if(sourceStr == "Assets") {source = FileSource::Assets; return true;}
		if(sourceStr == "Scripts") {source = FileSource::Scripts; return true;}
		if(sourceStr == "Cache") {source = FileSource::Cache; return true;}
		if(sourceStr == "External") {source = FileSource::External; return true;}
		if(sourceStr == "None") {source = FileSource::None; return true;}
		return false;
	}

	struct Path {
	public:
		Path() = default;
		inline Path(const FileSource source, std::filesystem::path path) :
			source(source), path(std::move(path)) {}

		Path(const Path &) = default;
		Path &operator=(const Path &) = default;

		Path(Path &&other) noexcept;
		Path &operator=(Path &&other) noexcept;

		void swap(Path &other) noexcept;

	public:
		[[nodiscard]] std::filesystem::path GetFullPath() const;
		[[nodiscard]] Path GetCachePath() const;

		[[nodiscard]] inline bool empty() const { return path.empty() || source == FileSource::None; }
		[[nodiscard]] std::string string() const;
		[[nodiscard]] std::string id() const;
		[[nodiscard]] bool equivalent(const Path &rhs) const;

	public:
		bool operator==(const Path &rhs) const;
		bool operator!=(const Path &rhs) const;

	public:
		inline static Path GetPath(std::filesystem::path path);
		inline static bool Equivalent(const Path &lhs, const Path &rhs) { return lhs.equivalent(rhs); }

	public:
		std::filesystem::path path;
		FileSource source = FileSource::None;
	};

	class FileSystem {
	private:
		friend class Application;
		friend struct Path;
		inline static std::filesystem::path s_EditorPath{"./"};

	public:
		static std::optional<uint32_t> ReadFourCC(const std::filesystem::path &path);
		static std::optional<uint32_t> ReadFourCC(const char *cpath);

	private:
		static std::optional<uint32_t> InternalReadFourCC(const char *cpath);

	public:
		static Buffer ReadBinaryFile(const std::filesystem::path &filePath);
		static Buffer ReadBinaryFile(const char *filePath);
		static std::vector<uint8_t> ReadBinaryFileInVector(const std::filesystem::path &filePath);
		static std::vector<uint8_t> ReadBinaryFileInVector(const char *filePath);

		static std::string ReadTextFile(const std::filesystem::path &filePath);
		static std::string ReadTextFile(const char *filePath);

	public:
		static bool WriteTextFile(const std::filesystem::path &filePath, const std::string& view);
		static bool WriteBinaryFile(const std::filesystem::path &filePath, ConstBufferView view);
		static bool WriteBinaryFile(const char *filePath, ConstBufferView view);

		template<typename T>
		static bool WriteBinaryFile(const std::filesystem::path &filePath, ConstBufferView view);
		template<typename T>
		static bool WriteBinaryFile(const char *filePath, ConstBufferView view);

		[[nodiscard]] static bool Exist(Path path);

	public:
		static std::filesystem::path GetRootPath(FileSource source);
	};

	template<typename T>
	bool FileSystem::WriteBinaryFile(const std::filesystem::path &filePath, ConstBufferView view) {
		std::string path = filePath.string();
		return WriteBinaryFile<T>(path.c_str(), view);
	}

	template<typename T>
	bool FileSystem::WriteBinaryFile(const char *filePath, ConstBufferView view) {
		CFile cfile(filePath, "bw");

		if (!cfile.IsValid()) return false;

		const size_t count = view.Count<T>();
		const size_t writtenSize = fwrite(view.Get(), sizeof(T), count, cfile.filePtr);

		return writtenSize == count;
	}

} // namespace Imagine


inline std::ostream &operator<<(std::ostream &os, const Imagine::Path &value) {
	return os << "Path{" << GetFileSourceName(value.source) << ": " << value.path.string() << "}";
}

template<>
struct fmt::formatter<Imagine::Path> : fmt::formatter<std::string> {
	auto format(const Imagine::Path &value, format_context &ctx) const -> decltype(ctx.out()) {
		const std::string pathStr = value.path.string();
		return format_to(ctx.out(), "Path{{ {}: {} }}", GetFileSourceName(value.source), pathStr);
	}
};
