//
// Created by Sayama on 29/04/2025.
//

#pragma once

namespace Imagine::Core {
	class Renderer {
	public:
		static Renderer* CreateRenderer();
	public:
		virtual ~Renderer() {}
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;
	};
}