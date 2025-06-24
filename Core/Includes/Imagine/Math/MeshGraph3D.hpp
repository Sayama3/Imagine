//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Core.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Rendering/CPUMesh.hpp"
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
			std::unordered_set<IdType> linkedEdges;
			std::unordered_set<IdType> linkedTriangles;
		};
		struct Edge {
			VertexID begin;
			VertexID end;
			std::array<std::optional<TriangleID>, 2> linkedTriangles;
		};
		struct Triangle {
			VertexID vertices[3];
			EdgeID edges[3];
		};

		void AddTriangle(vec a, vec b, vec c);
		void AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices);
		void AddMesh(const Core::CPUMesh &mesh);

	private:
		Core::AutoIdSparseSet<Vertex, IdType> m_Vertices{50};
		Core::AutoIdSparseSet<Edge, IdType> m_Edges{50};
		Core::AutoIdSparseSet<Triangle, IdType> m_Triangle{50};
	};
} // namespace Imagine::Math

namespace Imagine::Math {
	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddTriangle(vec a, vec b, vec c) {

		const Vertex array[3] = {Vertex{}, Vertex{}, Vertex{}};
		array[0].position = a;
		array[1].position = b;
		array[2].position = c;

		constexpr uint32_t indices[3] = {0, 1, 2};

		AddMesh({array, 3 * sizeof(Vertex)}, {indices, 3 * sizeof(uint32_t)});
	}

	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddMesh(const Core::CPUMesh &mesh) {
		Core::ConstBufferView vertices = Core::ConstBufferView::Make(mesh.Vertices);
		Core::ConstBufferView indices = Core::ConstBufferView::Make(mesh.Indices);
		AddMesh(vertices, indices);
	}

	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices) {
		std::vector<VertexID> m_IDMap;

		{
			std::unordered_map<vec, VertexID> m_IDs;
			m_IDMap.resize(vertices.Count<Imagine::Vertex>());
			const Imagine::Vertex *array = vertices.Get<Imagine::Vertex>();
			for (uint64_t i = 0; i < vertices.Count<Imagine::Vertex>(); ++i) {
				const auto &p = array[i].position;
				if (m_IDs.contains(p)) {
					const auto id = m_IDs.at(p);
					m_IDMap[i] = {id};
				}
				else {
					const auto id = m_Vertices.Create({p});
					m_IDs[p] = id;
					m_IDMap[i] = {id};
				}
			}
		}

		{
			std::unordered_map<ReversiblePair<VertexID, VertexID>, EdgeID> edges;

			const auto count = indices.Count<uint32_t>();
			const auto *array = indices.Get<uint32_t>();
			for (uint64_t i = 0; i < count; i += 3) {
				VertexID vertId1;
				VertexID vertId2;
				VertexID vertId3;

				EdgeID edge1Id;
				EdgeID edge2Id;
				EdgeID edge3Id;

				TriangleID triangleID;

				{
					vertId1 = m_IDMap[array[i + 0]];
					vertId2 = m_IDMap[array[i + 1]];
					vertId3 = m_IDMap[array[i + 2]];

					const Edge edge1 = {vertId1, vertId2};
					const Edge edge2 = {vertId2, vertId3};
					const Edge edge3 = {vertId3, vertId1};

					if (edges.contains({edge1.begin, edge1.end})) {
						edge1Id = edges.at({edge1.begin, edge1.end});
						edge1 = m_Edges.Get(edge1Id);
					}
					else {
						edge1Id = m_Edges.Create(edge1);
						edges[{edge1.begin, edge1.end}] = edge1Id;
					}

					if (edges.contains({edge2.begin, edge2.end})) {
						edge2Id = edges.at({edge2.begin, edge2.end});
						edge2 = m_Edges.Get(edge2Id);
					}
					else {
						edge2Id = m_Edges.Create(edge2);
						edges[{edge2.begin, edge2.end}] = edge2Id;
					}

					if (edges.contains({edge3.begin, edge3.end})) {
						edge3Id = edges.at({edge3.begin, edge3.end});
						edge3 = m_Edges.Get(edge3Id);
					}
					else {
						edge3Id = m_Edges.Create(edge3);
						edges[{edge3.begin, edge3.end}] = edge3Id;
					}

					const Triangle triangle{
							{vertId1, vertId2, vertId3},
							{edge1Id, edge2Id, edge3Id},
					};

					triangleID = m_Triangle.Create(triangle);
				}

				// Backpropagate the links
				{
					m_Vertices.Get(vertId1).linkedEdges.insert(edge3Id.id);
					m_Vertices.Get(vertId1).linkedEdges.insert(vertId1.id);

					m_Vertices.Get(vertId2).linkedEdges.insert(edge1Id.id);
					m_Vertices.Get(vertId2).linkedEdges.insert(edge2Id.id);

					m_Vertices.Get(vertId3).linkedEdges.insert(edge2Id.id);
					m_Vertices.Get(vertId3).linkedEdges.insert(edge3Id.id);

					m_Vertices.Get(vertId1).linkedTriangles.insert(triangleID.id);
					m_Vertices.Get(vertId2).linkedTriangles.insert(triangleID.id);
					m_Vertices.Get(vertId3).linkedTriangles.insert(triangleID.id);

					Edge& edge1 = m_Edges.Get(edge1Id);
					if(!edge1.linkedTriangles[0].has_value()) {edge1.linkedTriangles[0] = triangleID;}
					else {MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled."); edge1.linkedTriangles[1] = triangleID;}
					Edge& edge2 = m_Edges.Get(edge2Id);
					if(!edge1.linkedTriangles[0].has_value()) {edge1.linkedTriangles[0] = triangleID;}
					else {MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled."); edge1.linkedTriangles[1] = triangleID;}
					Edge& edge3 = m_Edges.Get(edge3Id);
					if(!edge1.linkedTriangles[0].has_value()) {edge1.linkedTriangles[0] = triangleID;}
					else {MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled."); edge1.linkedTriangles[1] = triangleID;}
				}
			}
		}

	}

} // namespace Imagine::Math
