//
// Created by ianpo on 15/05/2025.
//

#include "Imagine/Layers/Layer.hpp"

namespace Imagine::Core {

	Layer::Layer(std::string name) :
		m_DebugName(std::move(name)) {}

	Layer::~Layer() {}
} // namespace Imagine::Core
