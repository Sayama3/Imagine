//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Core.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Rendering/Vertex.hpp"
#include "Types.hpp"

namespace Imagine::Math {

	template<typename T = Real, glm::qualifier Q = glm::defaultp>
	class MeshGraph3D {
	public:
		// type definitions
		using vec = glm::vec<3, T, Q>;
		using IdType = uint32_t;

	public:
		// struct definition
		struct VertexID {
			VertexID() = default;
			~VertexID() = default;
			VertexID(const IdType _id) :
				id(_id) {}
			IdType id;

			bool operator==(const VertexID &o) const { return id == o.id; }
			bool operator!=(const VertexID &o) const { return !(*this == o); }
			auto operator<=>(const VertexID &o) const { return id <=> o.id; }
		};
		struct EdgeID {
			EdgeID() = default;
			~EdgeID() = default;
			EdgeID(const IdType _id) :
				id(_id) {}
			IdType id;

			bool operator==(const EdgeID &o) const { return id == o.id; }
			bool operator!=(const EdgeID &o) const { return !(*this == o); }
			auto operator<=>(const EdgeID &o) const { return id <=> o.id; }
		};
		struct TriangleID {
			TriangleID() = default;
			~TriangleID() = default;
			TriangleID(const IdType _id) :
				id(_id) {}
			IdType id;

			bool operator==(const TriangleID &o) const { return id == o.id; }
			bool operator!=(const TriangleID &o) const { return !(*this == o); }
			auto operator<=>(const TriangleID &o) const { return id <=> o.id; }
		};

		struct Vertex {
			vec position;
		};
		struct Edge {
			VertexID begin;
			VertexID end;
		};
		struct Triangle {
			VertexID vertices[3];
			EdgeID edges[3];
		};

		void AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices);

	private:
		Core::AutoIdSparseSet<Vertex, IdType> m_Vertices{50};
		Core::AutoIdSparseSet<Edge, IdType> m_Edges{50};
		Core::AutoIdSparseSet<Triangle, IdType> m_Triangle{50};
	};

	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices) {
		std::vector<VertexID> m_IDMap;

		{
			std::unordered_map<vec, VertexID> m_IDs;
			m_IDMap.resize(vertices.Count<Imagine::Vertex>());
			const Imagine::Vertex* array = vertices.Get<Imagine::Vertex>();
			for (uint64_t i = 0; i < vertices.Count<Imagine::Vertex>(); ++i) {
				const auto& p = array[i].position;
				if (m_IDs.contains(p)) {
					const auto id = m_IDs.at(p);
					m_IDMap[i] = {id};
				} else {
					const auto id = m_Vertices.Create({p});
					m_IDs[p] = id;
					m_IDMap[i] = {id};
				}
			}
		}

		{
			std::unordered_map<ReversiblePair<VertexID, VertexID>, EdgeID> edges;

			const auto count = indices.Count<uint32_t>();
			const auto* array = indices.Get<uint32_t>();
			for (uint64_t i = 0; i < count; i += 3) {
				const auto vertId1 = m_IDMap[array[i + 0]];
				const auto vertId2 = m_IDMap[array[i + 1]];
				const auto vertId3 = m_IDMap[array[i + 2]];

				const Edge edge1 = {vertId1, vertId2};
				const Edge edge2 = {vertId2, vertId3};
				const Edge edge3 = {vertId3, vertId1};

				EdgeID edge1Id;
				EdgeID edge2Id;
				EdgeID edge3Id;

				if (edges.contains({edge1.begin, edge1.end})) {
					edge1Id = edges.at({edge1.begin, edge1.end});
					edge1 = m_Edges.Get(edge1Id);
				} else {
					edge1Id = m_Edges.Create(edge1);
					edges[{edge1.begin, edge1.end}] = edge1Id;
				}

				if (edges.contains({edge2.begin, edge2.end})) {
					edge2Id = edges.at({edge2.begin, edge2.end});
					edge2 = m_Edges.Get(edge2Id);
				} else {
					edge2Id = m_Edges.Create(edge2);
					edges[{edge2.begin, edge2.end}] = edge2Id;
				}

				if (edges.contains({edge3.begin, edge3.end})) {
					edge3Id = edges.at({edge3.begin, edge3.end});
					edge3 = m_Edges.Get(edge3Id);
				} else {
					edge3Id = m_Edges.Create(edge3);
					edges[{edge3.begin, edge3.end}] = edge3Id;
				}

				const Triangle triangle {
					{vertId1, vertId2, vertId3},
					{edge1Id, edge2Id, edge3Id},
				};

				m_Triangle.Create(triangle);
			}
		}
	}
} // namespace Imagine::Math
