//
// Created by Iannis on 23/06/2025.
//

#pragma once
#include "Imagine/Core/Inputs.hpp"

namespace Imagine {
namespace SDL3 {

	class SDL3Mouse final : public Core::MouseInput {
	public:
	public:
		SDL3Mouse() = default;
		~SDL3Mouse() = default;
	public:
		virtual Vec2 GetPosition() override;
		virtual Vec2 GetMovement() override;
		virtual ButtonState GetButtonState(Imagine::Mouse button) override;
		virtual bool IsButtonPressed(Imagine::Mouse button) override;
		virtual bool IsButtonReleased(Imagine::Mouse button) override;
		virtual bool IsButtonDown(Imagine::Mouse button) override;
		virtual bool IsButtonUp(Imagine::Mouse button) override;
	public:
		Vec2 m_MousePosition{0};
		Vec2 m_MouseMovement{0};
		std::array<uint8_t, 12> m_ButtonStates;
	};

class SDL3Inputs {

};

} // SDL3
} // Imagine
