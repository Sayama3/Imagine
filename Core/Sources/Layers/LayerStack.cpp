//
// Created by ianpo on 18/07/2023.
//

#include "Imagine/Layers/LayerStack.hpp"

namespace Imagine {
	LayerStack::LayerStack() = default;

	LayerStack::~LayerStack() {
		Clear();
	}

	void LayerStack::Clear() {
		for (Layer* layer : m_Layers) {
			delete layer;
		}
		m_Layers.clear();
	}

	void LayerStack::PushLayer(Layer *layer) {
		MGN_PROFILE_FUNCTION();
		m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
		m_LayerInsertIndex++;
	}

	void LayerStack::PushOverlay(Layer *overlay) {
		MGN_PROFILE_FUNCTION();
		m_Layers.emplace_back(overlay);
	}

	void LayerStack::PopLayer(Layer *layer) {
		MGN_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin(), m_Layers.begin()+m_LayerInsertIndex+1, layer);
		if(it != m_Layers.begin()+m_LayerInsertIndex+1){
			m_Layers.erase(it);
			m_LayerInsertIndex--;
		}
	}

	void LayerStack::PopOverlay(Layer *overlay) {
		MGN_PROFILE_FUNCTION();
		auto it = std::find(m_Layers.begin()+m_LayerInsertIndex+1, m_Layers.end(), overlay);
		if(it != m_Layers.end()){
			m_Layers.erase(it);
		}
	}
} // PhysicalLayers