//
// Created by ianpo on 18/07/2023.
//

#pragma once

#include "Imagine/Core/TimeStep.hpp"
#include "Imagine/Events/Event.hpp"
#include "Imagine/Rendering/DrawContext.hpp"

// We shouldn't search layer by name!!

namespace Imagine::Core {
	class Layer {
	public:
		explicit Layer(std::string name = "Layer");
		virtual ~Layer();

		virtual void OnAttach() {}
		virtual void OnDetach() {}

		virtual void OnEvent(Event &event) {}

		inline const std::string &GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};

} // namespace Imagine::Core
