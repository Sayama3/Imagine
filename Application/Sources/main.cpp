//
// Created by Sayama on 29/04/2025.
//

#include <iostream>

#include "Imagine/Application/Application.hpp"
#include "Imagine/ApplicationLayer.hpp"
#include "Imagine/Core/Logger.hpp"
#include "Imagine/Core/Macros.hpp"

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

	Core::Application *application = Core::Application::Initialize(params);

	application->PushLayer<Imagine::Application::ApplicationLayer>();

	application->Run();

	Core::Application::Shutdown();
	Imagine::Core::Log::Shutdown();

	return 0;
}
