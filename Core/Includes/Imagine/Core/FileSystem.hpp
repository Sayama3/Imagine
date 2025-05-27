//
// Created by ianpo on 23/05/2025.
//

#pragma once

#include "Imagine/Core/Macros.hpp"
#include "Imagine/Core/Buffer.hpp"

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
		inline static Buffer readBinaryFile(const std::filesystem::path& filePath) {
			std::string fileStr = filePath.string();
			return std::move(readBinaryFile(fileStr.c_str()));
		}

		inline static Buffer readBinaryFile(const char* filePath) {
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
		inline static std::vector<char> readTextFile(const std::filesystem::path& filePath) {
			std::string fileStr = filePath.string();
			return std::move(readTextFile(fileStr.c_str()));
		}

		inline static std::vector<char> readTextFile(const char* filePath) {
			std::vector<char> fileContent;

			CFile file(filePath, "r");

			fseek(file.filePtr, 0, SEEK_END);
			const uint64_t size = ftell(file.filePtr);
			fseek(file.filePtr, 0, SEEK_SET);

			fileContent.resize(size);
			fgets(fileContent.data(), size, file.filePtr);

			return std::move(fileContent);
		}
	};

}
