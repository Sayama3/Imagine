//
// Created by ianpo on 14/07/2025.
//

#include "Imagine/Scripting/ScriptingLayer.hpp"

#include "Imagine/Scene/Entity.hpp"
#include "Imagine/ThirdParty/ImGui.hpp"

#ifdef _WIN32
#include <shellapi.h>
#endif
namespace Imagine {
	void ScriptingLayer::OnAttach() {
	}

	void ScriptingLayer::OnDetach() {
	}

	void ScriptingLayer::OnEvent(Event &event) {
		EventDispatcher dispatch(event);
		dispatch.Dispatch<AppUpdateEvent>(MGN_DISPATCH_FALSE(OnUpdate));
		dispatch.Dispatch<ImGuiEvent>(MGN_DISPATCH_FALSE(OnImGui));
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
	void ScriptingLayer::OnImGui(ImGuiEvent &e) {
#ifdef MGN_IMGUI
		for (LuaScript &script: m_Scripts) {
			auto path = script.GetPath();
			const auto pathStr = path.make_preferred().string();
			const std::string WindowName = "Lua - " + path.filename().string();

			ImGui::PushID(pathStr.c_str());
			ImGui::Begin(WindowName.c_str());
			{
				static ImGuiTableFlags flags = ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg;
#ifdef _WIN32
				const std::string buttonStr = "Path - " + pathStr;
				if (ImGui::Button(buttonStr.c_str())) ShellExecuteA(NULL, "open", pathStr.c_str(), NULL, NULL, SW_HIDE);
#else
				ImGui::Text("Path - %s", pathStr.c_str());
#endif

				static constexpr int c_IsSoft = 0;
				static constexpr int c_IsHard = 1;
				int hardReload = script.m_HardReload ? c_IsHard : c_IsSoft;
				if (ImGui::Combo("Reload Type", &hardReload, "Soft\0Hard")) {
					script.m_HardReload = hardReload == c_IsHard;
				}

				ImGui::SameLine();
				if (ImGui::Button("Hard Reload")) {
					const bool old = script.m_HardReload;
					script.m_HardReload = true;
					script.Load(script.m_Path);
					script.m_HardReload = old;
				}
				ImGui::SameLine();
				if (ImGui::Button("Soft Reload")) {
					const bool old = script.m_HardReload;
					script.m_HardReload = false;
					script.Load(script.m_Path);
					script.m_HardReload = old;
				}

				static EntityID id{EntityID::NullID};
				static uint32_t step = 1;
				static uint32_t fast_step = 100;
				ImGui::InputScalar("Entity ID", ImGuiDataType_U32, &id.id, &step, &fast_step, "%u");

				if (ImGui::Button("SetEntity")) {
					sol::protected_function func = (*script.m_State)["SetEntity"];
					if (func) {
						auto result = func(id.id);
						if (!result.valid()) {
							const sol::error err = result;
							script.m_LoggerStack.push_front({LuaScript::Log::Error, err.what()});
							MGN_CORE_ERROR("[Lua] Failed to run SetEntity {0}\n{1}", script.m_Path, err.what());
						}
					}
				}

				ImGui::BeginDisabled(true);

				if (ImGui::BeginTable("events_tables", LuaScript::Count, flags)) {

					for (uint16_t i = 0; i < LuaScript::Count; ++i) {
						ImGui::TableSetupColumn(LuaScript::EventToString((LuaScript::Event) i).c_str());
					}
					ImGui::TableHeadersRow();

					ImGui::TableNextRow();
					for (uint16_t i = 0; i < LuaScript::Count; ++i) {
						if (ImGui::TableSetColumnIndex(i)) {
							const std::string eventStr = "##" + LuaScript::EventToString((LuaScript::Event) i);
							ImGui::Checkbox(eventStr.c_str(), &script.m_EventsValidity[i]);
						}
					}
					ImGui::EndTable();
				}
				ImGui::EndDisabled();

				ImGui::Spacing();
				ImGui::SeparatorText("Logs");
				ImGui::Spacing();

				for (const LuaScript::Log& log : script.m_LoggerStack) {
					switch (log.type) {
						case LuaScript::Log::Error:
							ImGui::PushStyleColor(ImGuiCol_Text, ImVec4{0.8, 0.3, 0.2, 1.0});
							ImGui::Text("[Error] [%s] - ",log.time.c_str());
							ImGui::SameLine();
							ImGui::TextUnformatted(log.log.c_str());
							ImGui::PopStyleColor();
							break;
						case LuaScript::Log::Info:
							ImGui::Text("[Info] [%s] - ",log.time.c_str());
							ImGui::SameLine();
							ImGui::TextUnformatted(log.log.c_str());
							break;
						case LuaScript::Log::None:
							ImGui::Text(log.log.c_str(), log.time.c_str());
							break;
					}
				}

				for (int64_t i = script.m_LoggerStack.size(); i < script.m_LoggerStack.capacity(); ++i) {
					ImGui::TextUnformatted("");
				}
			}
			ImGui::End();
			ImGui::PopID();
		}
#endif
	}
} // namespace Imagine
