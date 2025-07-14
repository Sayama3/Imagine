//
// Created by ianpo on 13/07/2025.
//

#include "Imagine/Scripting/LuaScript.hpp"
#include <format>
#include "Imagine/Core/Inputs.hpp"
#include "Imagine/Layers/ImGuiLayer.hpp"
#include "Imagine/Math/Core.hpp"
#include "Imagine/Rendering/Camera.hpp"
#include "Imagine/Rendering/Renderer.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

#define BIND_RW_VAL(CLASS_TYPE, TYPE, property) [](const CLASS_TYPE* t) ->TYPE {return t->property;},[](CLASS_TYPE* t, TYPE v) {t->property = v;}
#define BIND_R_VAL(CLASS_TYPE, TYPE, property) [](const CLASS_TYPE* t) ->TYPE {return t->property;}

namespace Imagine {
	LuaScript::Log::local_time LuaScript::Log::Now() {
		return local_time{
				std::chrono::current_zone(),
				std::chrono::system_clock::now(),
		};
	}
	LuaScript::Log::Log(Type t, std::string str) :
		type(t), log(std::move(str)) {
		time = std::format("{:%T}", Now());
	}

	LuaScript::Log::Log(std::string str) :
		type(Info), log(std::move(str)) {
		time = std::format("{:%T}", Now());
	}

	LuaScript::LuaScript() {
	}

	LuaScript::LuaScript(const Path &path) :
		m_Path(path.GetFullPath()) {
		Load(path.GetFullPath());
	}

	LuaScript::LuaScript(const std::filesystem::path &path) :
		m_Path(path) {

		Load(path);
	}

	LuaScript::LuaScript(LuaScript &&o) noexcept :
		m_State(std::move(o.m_State)), m_Path(std::move(o.m_Path)), m_TimeEdited(std::move(o.m_TimeEdited)) {
	}

	LuaScript &LuaScript::operator=(LuaScript &&o) noexcept {
		swap(o);
		return *this;
	}

	LuaScript::~LuaScript() = default;

	void LuaScript::swap(LuaScript &o) noexcept {
		std::swap(m_State, o.m_State);
		std::swap(m_Path, o.m_Path);
		std::swap(m_TimeEdited, o.m_TimeEdited);
	}

	bool LuaScript::Load(const std::filesystem::path &path) {
		m_LoggerStack.push_front({Log::None, "============ Reload at %s ============"});
		m_State = CreateScope<sol::state>();
		const auto result = m_State->safe_script_file(path.string(), sol::script_pass_on_error);
		m_Path = path;
		m_TimeEdited = std::filesystem::last_write_time(m_Path);
		m_IsValid = result.valid();
		if (!m_IsValid) {
			sol::error err = result;
			m_LoggerStack.push_front({Log::Error, err.what()});
			MGN_CORE_ERROR("[Lua] Failed to load script {0}\n{1}", path, err.what());
			return false;
		}
		else {
			m_State->open_libraries(sol::lib::base, sol::lib::package, sol::lib::coroutine, sol::lib::string, sol::lib::math, sol::lib::table);
			LoadLogger();
			LoadMathType();
			LoadKeyboardTypes();
			LoadKeyboardFuncs();
			LoadMouseTypes();
			LoadMouseFuncs();
			LoadScene();
			for (uint16_t i = 0; i < Count; ++i) {
				const Event e = (Event) i;
				sol::protected_function eventFunc = (*m_State)[EventToString(e)];
				m_EventsValidity[i] = eventFunc.valid();
			}
		}
		return m_IsValid;
	}

	bool LuaScript::TryReload() {
		if (std::filesystem::last_write_time(m_Path) > m_TimeEdited) {
			return Load(m_Path);
		}
		return false;
	}

	void LuaScript::LoadLogger() {
		// m_LoggerStack.reserve(1000);
		m_State->set_function("print", [this](sol::variadic_args args) {
			std::string str;
			for (auto arg: args) {
				str += (*m_State)["tostring"](arg.get<sol::object>()).get<std::string>() + "\t";
			}
			m_LoggerStack.push_front({Log::Info, std::move(str)});
		});
	}

	void LuaScript::LoadMathType() {
		auto Vec2Type = (*m_State).new_usertype<Vec2>("Vec2", sol::constructors<Vec2()>());
		Vec2Type["x"] = &Vec2::x;
		Vec2Type["y"] = &Vec2::y;

		auto Vec3Type = (*m_State).new_usertype<Vec3>("Vec3", sol::constructors<Vec3()>());
		Vec3Type["x"] = &Vec3::x;
		Vec3Type["y"] = &Vec3::y;
		Vec3Type["y"] = &Vec3::z;

		auto RectType = m_State->new_usertype<Rect<float>>("Rect", sol::constructors<Rect<float>()>());
		RectType["min_x"] = &Rect<float>::minX;
		RectType["min_y"] = &Rect<float>::minY;

		RectType["max_x"] = &Rect<float>::maxX;
		RectType["max_y"] = &Rect<float>::maxY;

		RectType["center_x"] = sol::readonly_property(BIND_R_VAL(Rect<float>, float, GetCenter().x));
		RectType["center_y"] = sol::readonly_property(BIND_R_VAL(Rect<float>, float, GetCenter().y));

		RectType["size_x"] = sol::readonly_property(BIND_R_VAL(Rect<float>, float, GetSize().x));
		RectType["size_y"] = sol::readonly_property(BIND_R_VAL(Rect<float>, float, GetSize().y));

		RectType["IsInside"] = [](Rect<float>* rect, float x, float y) {return rect->IsInside({x,y});};
		RectType["GetRelative"] = &Rect<float>::GetRelative;
		RectType["GetGlobal"] = &Rect<float>::GetGlobal;
	}

	void LuaScript::LoadKeyboardTypes() {
		(*m_State).new_enum<Key>("Key", {{"Space", Key::Space},
										 {"Apostrophe", Key::Apostrophe},
										 {"Comma", Key::Comma},
										 {"Minus", Key::Minus},
										 {"Period", Key::Period},
										 {"Slash", Key::Slash},
										 {"Key0", Key::Key0},
										 {"Key1", Key::Key1},
										 {"Key2", Key::Key2},
										 {"Key3", Key::Key3},
										 {"Key4", Key::Key4},
										 {"Key5", Key::Key5},
										 {"Key6", Key::Key6},
										 {"Key7", Key::Key7},
										 {"Key8", Key::Key8},
										 {"Key9", Key::Key9},
										 {"Semicolon", Key::Semicolon},
										 {"Equal", Key::Equal},
										 {"A", Key::A},
										 {"B", Key::B},
										 {"C", Key::C},
										 {"D", Key::D},
										 {"E", Key::E},
										 {"F", Key::F},
										 {"G", Key::G},
										 {"H", Key::H},
										 {"I", Key::I},
										 {"J", Key::J},
										 {"K", Key::K},
										 {"L", Key::L},
										 {"M", Key::M},
										 {"N", Key::N},
										 {"O", Key::O},
										 {"P", Key::P},
										 {"Q", Key::Q},
										 {"R", Key::R},
										 {"S", Key::S},
										 {"T", Key::T},
										 {"U", Key::U},
										 {"V", Key::V},
										 {"W", Key::W},
										 {"X", Key::X},
										 {"Y", Key::Y},
										 {"Z", Key::Z},
										 {"LeftBracket", Key::LeftBracket},
										 {"Backslash", Key::Backslash},
										 {"RightBracket", Key::RightBracket},
										 {"GraveAccent", Key::GraveAccent},
										 {"World1", Key::World1},
										 {"World2", Key::World2},
										 {"Escape", Key::Escape},
										 {"Enter", Key::Enter},
										 {"Tab", Key::Tab},
										 {"Backspace", Key::Backspace},
										 {"Insert", Key::Insert},
										 {"Delete", Key::Delete},
										 {"Right", Key::Right},
										 {"Left", Key::Left},
										 {"Down", Key::Down},
										 {"Up", Key::Up},
										 {"PageUp", Key::PageUp},
										 {"PageDown", Key::PageDown},
										 {"Home", Key::Home},
										 {"End", Key::End},
										 {"CapsLock", Key::CapsLock},
										 {"ScrollLock", Key::ScrollLock},
										 {"NumLock", Key::NumLock},
										 {"PrintScreen", Key::PrintScreen},
										 {"Pause", Key::Pause},
										 {"F1", Key::F1},
										 {"F2", Key::F2},
										 {"F3", Key::F3},
										 {"F4", Key::F4},
										 {"F5", Key::F5},
										 {"F6", Key::F6},
										 {"F7", Key::F7},
										 {"F8", Key::F8},
										 {"F9", Key::F9},
										 {"F10", Key::F10},
										 {"F11", Key::F11},
										 {"F12", Key::F12},
										 {"F13", Key::F13},
										 {"F14", Key::F14},
										 {"F15", Key::F15},
										 {"F16", Key::F16},
										 {"F17", Key::F17},
										 {"F18", Key::F18},
										 {"F19", Key::F19},
										 {"F20", Key::F20},
										 {"F21", Key::F21},
										 {"F22", Key::F22},
										 {"F23", Key::F23},
										 {"F24", Key::F24},
										 {"F25", Key::F25},
										 {"Kp0", Key::Kp0},
										 {"Kp1", Key::Kp1},
										 {"Kp2", Key::Kp2},
										 {"Kp3", Key::Kp3},
										 {"Kp4", Key::Kp4},
										 {"Kp5", Key::Kp5},
										 {"Kp6", Key::Kp6},
										 {"Kp7", Key::Kp7},
										 {"Kp8", Key::Kp8},
										 {"Kp9", Key::Kp9},
										 {"KpDecimal", Key::KpDecimal},
										 {"KpDivide", Key::KpDivide},
										 {"KpMultiply", Key::KpMultiply},
										 {"KpSubtract", Key::KpSubtract},
										 {"KpAdd", Key::KpAdd},
										 {"KpEnter", Key::KpEnter},
										 {"KpEqual", Key::KpEqual},
										 {"LeftShift", Key::LeftShift},
										 {"LeftControl", Key::LeftControl},
										 {"LeftAlt", Key::LeftAlt},
										 {"LeftSuper", Key::LeftSuper},
										 {"RightShift", Key::RightShift},
										 {"RightControl", Key::RightControl},
										 {"RightAlt", Key::RightAlt},
										 {"RightSuper", Key::RightSuper},
										 {"Menu", Key::Menu},
										 {"Last", Key::Last}});
	}

	void LuaScript::LoadKeyboardFuncs() {
		(*m_State)["GetKeyPressed"] = [](Key key) -> bool { return Inputs::GetKeyboardState()[key].IsPressed(); };
		(*m_State)["GetKeyReleased"] = [](Key key) -> bool { return Inputs::GetKeyboardState()[key].IsReleased(); };
		(*m_State)["GetKeyDown"] = [](Key key) -> bool { return Inputs::GetKeyboardState()[key].IsDown(); };
		(*m_State)["GetKeyUp"] = [](Key key) -> bool { return Inputs::GetKeyboardState()[key].IsUp(); };
	}

	void LuaScript::LoadMouseTypes() {
		(*m_State).new_enum<Mouse>("Mouse", {
													{"Left", Mouse::Left},
													{"Right", Mouse::Right},
													{"Middle", Mouse::Middle},
													{"Button4", Mouse::Button4},
													{"Button5", Mouse::Button5},
													{"Button6", Mouse::Button6},
													{"Button7", Mouse::Button7},
													{"Button8", Mouse::Button8},
											});
	}

	void LuaScript::LoadMouseFuncs() {

		(*m_State)["GetMousePressed"] = [](Mouse mouse) -> bool { return Inputs::GetMouseState()[mouse].IsPressed(); };
		(*m_State)["GetMouseReleased"] = [](Mouse mouse) -> bool { return Inputs::GetMouseState()[mouse].IsReleased(); };
		(*m_State)["GetMouseDown"] = [](Mouse mouse) -> bool { return Inputs::GetMouseState()[mouse].IsDown(); };
		(*m_State)["GetMouseUp"] = [](Mouse mouse) -> bool { return Inputs::GetMouseState()[mouse].IsUp(); };

		(*m_State)["GetMousePosition"] = []() -> Vec2 {
			return Vec2{Inputs::GetMouseState().x, Inputs::GetMouseState().y};
		};

		(*m_State)["GetMouseScroll"] = []() -> Vec2 {
			return Vec2{Inputs::GetMouseState().WheelX, Inputs::GetMouseState().WheelY};
		};

		(*m_State)["GetMouseMotion"] = []() -> Vec2 {
			return Vec2{Inputs::GetMouseState().MotionX, Inputs::GetMouseState().MotionY};
		};
	}
	void LuaScript::LoadScene() {
		auto CameraType = (*m_State).new_usertype<Camera>("Camera", sol::constructors<Camera()>());
		CameraType["velocity_x"] = sol::property(BIND_RW_VAL(Camera, float, velocity.x));
		CameraType["velocity_y"] = sol::property(BIND_RW_VAL(Camera, float, velocity.y));
		CameraType["velocity_z"] = sol::property(BIND_RW_VAL(Camera, float, velocity.z));
		CameraType["position_x"] = sol::property(BIND_RW_VAL(Camera, float, position.x));
		CameraType["position_y"] = sol::property(BIND_RW_VAL(Camera, float, position.y));
		CameraType["position_z"] = sol::property(BIND_RW_VAL(Camera, float, position.z));
		CameraType["pitch"] = &Camera::pitch;
		CameraType["yaw"] = &Camera::yaw;
		CameraType["pitchVelocity"] = &Camera::pitchVelocity;
		CameraType["yawVelocity"] = &Camera::yawVelocity;
		CameraType["velocityMultiplier"] = &Camera::velocityMultiplier;
		CameraType["Main"] = sol::var(Camera::s_MainCamera);



		(*m_State)["SetCameraVelocity"] = [](const sol::table& velocity) {
			Camera::s_MainCamera->velocity.x = velocity.get_or("x", 0.0);
			Camera::s_MainCamera->velocity.y = velocity.get_or("y", 0.0);
			Camera::s_MainCamera->velocity.z = velocity.get_or("z", 0.0);
		};

		(*m_State)["SetCameraYawVelocity"] = [](float velocity) {
			Camera::s_MainCamera->yawVelocity = velocity;
		};

		(*m_State)["SetCameraPitchVelocity"] = [](float velocity) {
			Camera::s_MainCamera->pitchVelocity = velocity;
		};

	}

	const std::filesystem::path &LuaScript::GetPath() const {
		return m_Path;
	}

	void LuaScript::Update(TimeStep ts) {
		if (!m_IsValid) return;
		if (!m_EventsValidity[UpdateEvent]) return;

		(*m_State)["CanEditScene"] = !ThirdParty::ImGuiLib::EventsBlocked();
		if (auto renderer = Renderer::Get()) {
			(*m_State)["Viewport"] = renderer->GetViewport();
		}

		// LoadKeyboardFuncs();

		sol::protected_function eventFunc = (*m_State)[EventToString(UpdateEvent)];
		if (eventFunc) {
			auto result = eventFunc(ts.GetSeconds());
			if (!result.valid()) {
				m_EventsValidity[UpdateEvent] = false;
				const sol::error err = result;
				m_LoggerStack.push_front({Log::Error, err.what()});
				MGN_CORE_ERROR("[Lua] Failed to run Update {0}\n{1}", m_Path, err.what());
			}
		}
	}
} // namespace Imagine
