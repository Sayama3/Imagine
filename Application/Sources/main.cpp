//
// Created by Sayama on 29/04/2025.
//

#include <iostream>

#include "Imagine/Application/Application.hpp"
#include "Imagine/Core/Logger.hpp"
#include "Imagine/Core/Macros.hpp"

int main(int argc, char** argv)
{
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

	ApplicationParameters params {
		std::string{"Imagine"},
		WindowParameters {
			900,
			600,
			true
		},
		true,
	};

	// ApplicationParameters params {
	// 	std::string{"Imagine"},
	// 	std::nullopt,
	// 	true,
	// };

	Application* application = new Application(params);

	application->Run();

	delete application;
	application = nullptr;

	Imagine::Core::Log::Shutdown();

	return 0;
}
