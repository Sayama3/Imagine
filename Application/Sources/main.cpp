//
// Created by Sayama on 29/04/2025.
//

#include "../../Core/Includes/Imagine/Application/Application.hpp"
#include <iostream>
#include "Imagine/Core/Logger.hpp"

int main(int argc, char** argv)
{
	using namespace Imagine;
	using namespace Imagine::Core;
	Imagine::Core::Log::Init();

	ApplicationParameters params {
		"Imagine",
		WindowParameters {
			900,
			600,
			true
		},
		true,
	};
	Application* application = new Application(params);

	application->Run();

	delete application;
	application = nullptr;

	Imagine::Core::Log::Shutdown();

	return 0;
}
