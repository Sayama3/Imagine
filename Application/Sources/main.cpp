//
// Created by Sayama on 29/04/2025.
//

#include <iostream>

#include "Imagine/Application/Application.hpp"
#include "Imagine/ApplicationLayer.hpp"
#include "Imagine/Core.hpp"
#include "Imagine/Layers/ImGuiLayer.hpp"
#include "Imagine/Layers/PhysicsLayer.hpp"
#include "Imagine/Physics/ObjectLayerPairFilter.hpp"

int main(int argc, char **argv) {
	using namespace Imagine;

	Imagine::Log::Init({
			LogFileParameter{
					"Imagine.log",
					"Imagine.old.log",
			},
			c_DefaultLogPattern,
			true,
	});

	// Initialize Jolt. It's need to be done before the application started.
	// Register allocation hook. In this example we'll just let Jolt use malloc / free but you can override these if you want (see Memory.h).
	// This needs to be done before any other Jolt function is called.
	JPH::RegisterDefaultAllocator();

	ApplicationParameters params{
			std::string{"Imagine"},
			MGN_MAKE_VERSION(0, 0, 1),
			WindowParameters{
					1600,
					900,
					true,
			},
			RendererParameters{
					2,
					c_DefaultDebugRendering,
			},
	};

	// ApplicationParameters params {
	// 	std::string{"Imagine"},
	// MGN_MAKE_VERSION(0,0,1),
	// 	std::nullopt,
	// 	std::nullopt,
	// };

	MGN_PROFILE_BEGIN_SESSION("startup", "ImagineProfile-Startup.json");
	MGN_FRAME_START();
	Application *application = Application::Initialize(params);
	application->PushLayer<Imagine::Runtime::ApplicationLayer>();
	application->PushLayer<Imagine::PhysicsLayer>();
	application->PushOverlay<Imagine::ImGuiLayer>();


	MGN_FRAME_END();
	MGN_PROFILE_END_SESSION();


	MGN_PROFILE_BEGIN_SESSION("runtime", "ImagineProfile-Runtime.json");
	application->Run();
	MGN_PROFILE_END_SESSION();

	MGN_PROFILE_BEGIN_SESSION("shutdown", "ImagineProfile-Shutdown.json");
	MGN_FRAME_START();
	Application::Shutdown();
	MGN_FRAME_END();
	MGN_PROFILE_END_SESSION();
	Imagine::Log::Shutdown();

	return 0;
}
