//
// Created by Iannis on 02/05/2025.
//

#pragma once
#include "Imagine/Core/Math.hpp"
#include "Imagine/Rendering/DrawContext.hpp"
#include "Imagine/Scene/Entity.hpp"

namespace Imagine::Core {

	//TODO: Move the rendering to ECS like

	struct Renderable {
		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) = 0;
	};

	struct Node : public Renderable {
		// parent pointer must be a weak pointer to avoid circular dependencies
		std::weak_ptr<Node> parent;
		std::vector<std::shared_ptr<Node>> children;

		glm::mat4 localTransform;
		glm::mat4 worldTransform;

		void refreshTransform(const glm::mat4& parentMatrix)
		{
			worldTransform = parentMatrix * localTransform;
			for (auto& c : children) {
				c->refreshTransform(worldTransform);
			}
		}

		virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx)
		{
			// draw children
			for (auto& c : children) {
				c->Draw(topMatrix, ctx);
			}
		}

		struct MeshNode : public Node {

			std::shared_ptr<MeshAsset> mesh;

			virtual void Draw(const glm::mat4& topMatrix, DrawContext& ctx) override;
		};
	};

} // namespace Imagine::Core
