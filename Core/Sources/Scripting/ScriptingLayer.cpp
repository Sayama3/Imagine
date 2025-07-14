//
// Created by ianpo on 14/07/2025.
//

#include "Imagine/Scripting/ScriptingLayer.hpp"

namespace Imagine {
	void ScriptingLayer::OnAttach() {
	}

	void ScriptingLayer::OnDetach() {
	}

	void ScriptingLayer::OnEvent(Event &event) {
		EventDispatcher dispatch(event);
		dispatch.Dispatch<AppUpdateEvent>(MGN_DISPATCH_FALSE(OnUpdate));
	}
	void ScriptingLayer::Load(const Path &path) {
		m_Scripts.emplace_back(path);
	}

	void ScriptingLayer::Load(const std::filesystem::path &path) {
		m_Scripts.emplace_back(path);
	}

	void ScriptingLayer::OnUpdate(AppUpdateEvent &e) {
		for (LuaScript &script: m_Scripts) {
			script.TryReload();
			script.Update(e.GetTimeStep());
		}
	}
} // namespace Imagine
