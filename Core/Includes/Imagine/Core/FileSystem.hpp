//
// Created by ianpo on 23/05/2025.
//

#pragma once

#include <utility>

#include "Imagine/Core/Buffer.hpp"
#include "Imagine/Core/Macros.hpp"

namespace Imagine::Core {

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
		}
		return "Unknown";
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
		static Buffer readBinaryFile(const std::filesystem::path &filePath);
		static Buffer readBinaryFile(const char *filePath);
		static std::vector<char> readTextFile(const std::filesystem::path &filePath);
		static std::vector<char> readTextFile(const char *filePath);

		[[nodiscard]] static bool Exist(Path path);
	public:
		static std::filesystem::path GetRootPath(FileSource source);
	};

} // namespace Imagine::Core


inline std::ostream &operator<<(std::ostream &os, const Imagine::Core::Path &value) {
	return os << "Path{" << GetFileSourceName(value.source) << ": " << value.path.string() << "}";
}

template<>
struct fmt::formatter<Imagine::Core::Path> : fmt::formatter<std::string> {
	auto format(const Imagine::Core::Path& value, format_context &ctx) const -> decltype(ctx.out()) {
		const std::string pathStr = value.path.string();
		return format_to(ctx.out(), "Path{{ {}: {} }}", GetFileSourceName(value.source), pathStr);
	}
};
