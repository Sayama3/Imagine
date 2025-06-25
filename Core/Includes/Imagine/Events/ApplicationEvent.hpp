//
// Created by ianpo on 14/07/2023.
//

#pragma once

#include "Event.hpp"

namespace Imagine::Core {

	class WindowResizeEvent : public Event {
	public:
		WindowResizeEvent(unsigned int width, unsigned int height) :
			m_Width(width), m_Height(height) {}

		unsigned int GetWidth() const { return m_Width; }
		unsigned int GetHeight() const { return m_Height; }

		std::string string() const override {
			std::stringstream ss;
			ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowResize)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	private:
		unsigned int m_Width, m_Height;
	};

	class WindowMinifyEvent : public Event {
	public:
		WindowMinifyEvent(bool minify) :
			m_Minify(minify) {}

		bool GetMinify() const { return m_Minify; }

		std::string string() const override {
			std::stringstream ss;
			ss << "WindowMinifyEvent: " << m_Minify;
			return ss.str();
		}

		EVENT_CLASS_TYPE(EventType::WindowDropFile)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	private:
		bool m_Minify;
	};

	class WindowDropFileEvent : public Event {
	public:
		explicit WindowDropFileEvent(const uint64_t fileCount, const char **files) {
			m_DropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				m_DropFilesPath[i] = files[i];
			}
		}
		explicit WindowDropFileEvent(const uint64_t fileCount, const std::string *files) {
			m_DropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				m_DropFilesPath[i] = files[i];
			}
		}
		explicit WindowDropFileEvent(const uint64_t fileCount, const std::filesystem::path *files) {
			m_DropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				m_DropFilesPath[i] = files[i];
			}
		}
		virtual ~WindowDropFileEvent() = default;

	public:
		struct Iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = std::filesystem::path;
			using difference_type = std::ptrdiff_t;
			using pointer = const value_type *;
			using reference = const value_type &;

		public:
			explicit Iterator(const WindowDropFileEvent *pEvent, const uint32_t index) :
				pEvent(pEvent), index(index) {}

			Iterator operator++(int) {
				Iterator res{*this};
				++(*this);
				return res;
			}
			Iterator &operator++() {
				++index;
				return *this;
			}

			reference operator*() const { return GetFilePath(); }
			pointer operator->() const { return &GetFilePath(); }

			bool operator==(const Iterator &o) const { return pEvent == o.pEvent && index == o.index; }
			bool operator!=(const Iterator &o) const { return !(*this == o); }
			auto operator<=>(const Iterator &o) const { return index <=> o.index; }

		public:
			[[nodiscard]] const std::filesystem::path &GetFilePath() const {
				return pEvent->GetFile(index);
			}

		private:
			const WindowDropFileEvent *pEvent{nullptr};
			uint64_t index;
		};

		Iterator begin() const {return Iterator{this, 0};}
		Iterator end() const {return Iterator{this, m_DropFilesPath.size()};}

	public:
		[[nodiscard]] inline uint64_t GetFileCount() const { return m_DropFilesPath.size(); }
		[[nodiscard]] inline const std::filesystem::path &GetFile(const uint64_t index) const { return m_DropFilesPath[index]; }

	public:
		std::string string() const override {
			std::stringstream ss;
			ss << GetName() << " has droped files :[";
			for (uint64_t i = 0; i < m_DropFilesPath.size(); ++i) {
				ss << m_DropFilesPath[i].string();
				if (i != m_DropFilesPath.size() - 1) ss << ", ";
			}
			ss << "]";
			return ss.str();
		}

	public:
		EVENT_CLASS_TYPE(EventType::WindowMinify)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	private:
		std::vector<std::filesystem::path> m_DropFilesPath;
	};

	class WindowCloseEvent : public Event {
	public:
		WindowCloseEvent() = default;

		EVENT_CLASS_TYPE(EventType::WindowClose)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	};

	class AppTickEvent : public Event {
	public:
		AppTickEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppTick)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	};

	class AppUpdateEvent : public Event {
	public:
		AppUpdateEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppUpdate)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	};

	class AppRenderEvent : public Event {
	public:
		AppRenderEvent() = default;

		EVENT_CLASS_TYPE(EventType::AppRender)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	};
} // namespace Imagine::Core
