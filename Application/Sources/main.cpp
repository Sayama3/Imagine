//
// Created by Sayama on 29/04/2025.
//

#include <iostream>

#include "Imagine/Application/Application.hpp"
#include "Imagine/ApplicationLayer.hpp"
#include "Imagine/Core.hpp"
#include "Imagine/Layers/ImGuiLayer.hpp"

int main(int argc, char **argv) {
	using namespace Imagine;
	using namespace Imagine::Core;

	Imagine::Core::Log::Init({
			LogFileParameter{
					"Imagine.log",
					"Imagine.old.log",
			},
			c_DefaultLogPattern,
			true,
	});

	ApplicationParameters params{
			std::string{"Imagine"},
			MGN_MAKE_VERSION(0, 0, 1),
			WindowParameters{
					1200,
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

	Core::Application *application = Core::Application::Initialize(params);
	application->PushLayer<Imagine::Application::ApplicationLayer>();
	application->PushOverlay<Imagine::Core::ImGuiLayer>();

	Project::New();

	MGN_FRAME_END();
	MGN_PROFILE_END_SESSION();


	MGN_PROFILE_BEGIN_SESSION("runtime", "ImagineProfile-Runtime.json");
	application->Run();
	MGN_PROFILE_END_SESSION();

	MGN_PROFILE_BEGIN_SESSION("shutdown", "ImagineProfile-Shutdown.json");
	MGN_FRAME_START();
	Core::Application::Shutdown();
	MGN_FRAME_END();
	MGN_PROFILE_END_SESSION();
	Imagine::Core::Log::Shutdown();

	return 0;
}
