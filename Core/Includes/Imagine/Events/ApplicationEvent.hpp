//
// Created by ianpo on 14/07/2023.
//

#pragma once

#include "Event.hpp"

namespace Imagine::Core {

    class WindowResizeEvent : public Event
    {
    public:
        WindowResizeEvent(unsigned int width, unsigned int height)
                : m_Width(width), m_Height(height) {}

        unsigned int GetWidth() const { return m_Width; }
        unsigned int GetHeight() const { return m_Height; }

        std::string string() const override
        {
            std::stringstream ss;
            ss << "WindowResizeEvent: " << m_Width << ", " << m_Height;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::WindowResize)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    private:
        unsigned int m_Width, m_Height;
    };

    class WindowMinifyEvent : public Event
    {
    public:
        WindowMinifyEvent(bool minify)
                : m_Minify(minify) {}

        bool GetMinify() const { return m_Minify; }

        std::string string() const override
        {
            std::stringstream ss;
            ss << "WindowMinifyEvent: " << m_Minify ;
            return ss.str();
        }

        EVENT_CLASS_TYPE(EventType::WindowDropFile)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    private:
        bool m_Minify;
    };

	class WindowDropFileEvent : public Event
	{
	public:
		explicit WindowDropEvent(const uint64_t fileCount, const char** files) {
			dropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				dropFilesPath[i] = files[i];
			}
		}
		explicit WindowDropEvent(const uint64_t fileCount, const std::string* files) {
			dropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				dropFilesPath[i] = files[i];
			}
		}
		explicit WindowDropEvent(const uint64_t fileCount, const std::filesystem::path* files) {
			dropFilesPath.resize(fileCount);
			for (int i = 0; i < fileCount; ++i) {
				dropFilesPath[i] = files[i];
			}
		}
		virtual ~WindowDropFileEvent() = default;
	public:
		std::string string() const override
		{
			std::stringstream ss;
			ss << GetName() << " has droped files :[";
			for (uint64_t i = 0; i < dropFilesPath.size(); ++i) {
				ss << dropFilesPath[i].string();
				if (i != dropFilesPath.size() - 1) ss << ", ";
			}
			ss << "]";
			return ss.str();
		}
	public:
		EVENT_CLASS_TYPE(EventType::WindowMinify)
		EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
	private:
		std::vector<std::filesystem::path> dropFilesPath;
	};

    class WindowCloseEvent : public Event
    {
    public:
        WindowCloseEvent() = default;

        EVENT_CLASS_TYPE(EventType::WindowClose)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    };

    class AppTickEvent : public Event
    {
    public:
        AppTickEvent() = default;

        EVENT_CLASS_TYPE(EventType::AppTick)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    };

    class AppUpdateEvent : public Event
    {
    public:
        AppUpdateEvent() = default;

        EVENT_CLASS_TYPE(EventType::AppUpdate)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    };

    class AppRenderEvent : public Event
    {
    public:
        AppRenderEvent() = default;

        EVENT_CLASS_TYPE(EventType::AppRender)
        EVENT_CLASS_CATEGORY(EventCategory::EventCategoryApplication)
    };
}
