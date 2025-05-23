//
// Created by ianpo on 23/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"

namespace Imagine::Core {

	struct CFile {
		CFile() = default;
		CFile(const char* filePath, const char* mode = "w") : filePtr(fopen(filePath, mode)) {}
		CFile(FILE* filePtr) : filePtr(filePtr) {}
		~CFile() {
			Close();
		}

		inline void Open(const char* filePath, const char* mode = "w") {
			Close();
			filePtr = fopen(filePath, mode);
		}

		inline void Close() {
			if (filePtr) {
				fclose(filePtr);
				filePtr = nullptr;
			}
		}

		[[nodiscard]] inline bool IsValid() const {return filePtr;}

		FILE* filePtr{nullptr};
	};

	class FileSystem {
	public:
		inline static std::vector<char> readFile(const std::filesystem::path& filePath) {
			std::string fileStr = filename.string();
			return readFile(fileStr.c_str());
		}

		inline static std::vector<char> readFile(const char* filePath) {
			std::vector<char> fileContent;

			CFile file(filePath, "r");

			fseek(file.filePtr, 0, SEEK_END);
			const uint64_t size = ftell(file.filePtr);
			fseek(file.filePtr, 0, SEEK_SET);

			fileContent.resize(size);
			fgets(fileContent.data(), size, file.filePtr);

			return fileContent;
		}
	};

}
