//
// Created by Iannis on 16/06/2025.
//

#pragma once

#include "Entity.hpp"

namespace Imagine::Core {
	struct Root {};

	struct Parent {
		EntityID parent{EntityID::NullID};
	};

	struct Sibling {
		EntityID previous{EntityID::NullID};
		EntityID next{EntityID::NullID};
	};

	struct Child {
		EntityID firstChild{EntityID::NullID};
	};
}