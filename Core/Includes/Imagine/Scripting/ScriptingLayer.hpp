//
// Created by ianpo on 14/07/2025.
//

#pragma once
#include "Imagine/Events/ApplicationEvent.hpp"
#include "Imagine/Layers/Layer.hpp"
#include "Imagine/Scripting/LuaScript.hpp"

namespace Imagine {

	class ScriptingLayer final : public Layer {
	public:
		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event &event) override;
	public:
	void Load(const Path &path);
	void Load(const std::filesystem::path &path);

	private:
		void OnUpdate(AppUpdateEvent &e);

	private:
		std::vector<LuaScript> m_Scripts;
	};

} // namespace Imagine
