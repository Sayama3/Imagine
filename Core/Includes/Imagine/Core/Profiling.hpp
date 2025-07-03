#pragma once

#include "Imagine/Core/Macros.hpp"

#define TracyFunction MGN_FUNC

#include <tracy/Tracy.hpp>

#if MGN_PROFILING

#define MGN_PROFILE_BEGIN_SESSION(name, filepath) //::Imagine::Core::Instrumentor::Get().BeginSession(name, filepath)
#define MGN_PROFILE_END_SESSION() //::Imagine::Core::Instrumentor::Get().EndSession()
#define MGN_FRAME_START() FrameMark//FrameMarkStart(s_MainFrame)
#define MGN_FRAME_END() //FrameMarkEnd(s_MainFrame)
#define MGN_PROFILE_SCOPE(name) ZoneScopedN(name)
#define MGN_PROFILE_FUNCTION() ZoneScoped

#else

#define MGN_PROFILE_BEGIN_SESSION(name, filepath)
#define MGN_PROFILE_END_SESSION()
#define MGN_FRAME_START()
#define MGN_FRAME_END()
#define MGN_PROFILE_SCOPE(name)
#define MGN_PROFILE_FUNCTION()

#endif