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
		static inline constexpr glm::length_t L = 3;
		// type definitions
		using vec = glm::vec<L, T, Q>;
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
			std::set<EdgeID> linkedEdges;
			std::set<TriangleID> linkedTriangles;
		};
		struct Edge {
			VertexID begin;
			VertexID end;
			std::array<std::optional<TriangleID>, 2> linkedTriangles;

			VertexID GetOtherVertex(VertexID id) const {
				if (begin == id) return end;
				MGN_CORE_ASSERT(id == end, "The point id {} is not part of this edge.", id.id);
				return begin;
			}

			bool IsBegin(VertexID id) const {
				return id == begin;
			}
		};
		struct Triangle {
			std::array<VertexID, 3> vertices;
			std::array<EdgeID, 3> edges;

			VertexID GetOther(VertexID one, VertexID two) {
				auto it = std::find_if(vertices.begin(), vertices.end(), [one,two](auto v) {
					return v != one && v != two;
				});
				return *it;
			}
		};

	public:
		using VertexSparseSet = Core::AutoIdSparseSet<Vertex, IdType>;
		using EdgeSparseSet = Core::AutoIdSparseSet<Edge, IdType>;
		using TriangleSparseSet = Core::AutoIdSparseSet<Triangle, IdType>;

	public:
		struct VertexIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Vertex;
			using difference_type = std::ptrdiff_t;
			using pointer = Vertex *;
			using reference = Vertex &;

		public:
			explicit VertexIterator(MeshGraph3D *graph, VertexID vertex) :
				graph(graph), iterator(EdgeSparseSet::Iterator::FromID(graph->m_Vertices, vertex.id)) {}
			explicit VertexIterator(MeshGraph3D *graph, uint32_t index) :
				graph(graph), iterator(graph->m_Vertices, index) {}

			VertexIterator operator++(int) {
				VertexIterator res{*this};
				++(*this);
				return res;
			}
			VertexIterator &operator++() {
				++iterator;
				return *this;
			}
			reference operator*() const { return GetVertex(); }
			pointer operator->() const { return &GetVertex(); }

			bool operator==(const VertexIterator & o) const {return graph == o.graph && iterator == o.iterator;}
			bool operator!=(const VertexIterator & o) const {return !(*this == o);}
		public:
			Vertex &GetVertex() { return iterator.GetElement(); }
			VertexID GetID() { return {iterator.GetID()}; }

			uint64_t GetEdgeCount() const {
				iterator->linkedEdges.count();
			}

			typename std::set<EdgeID>::iterator GetEdgeBegin() { return iterator->linkedEdges.begin();}
			typename std::set<EdgeID>::iterator GetEdgeEnd() { return iterator->linkedEdges.end();}
		
		private:
			MeshGraph3D *graph{nullptr};
			typename VertexSparseSet::Iterator iterator;
		};

		struct EdgeIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Edge;
			using difference_type = std::ptrdiff_t;
			using pointer = Edge *;
			using reference = Edge &;

		public:
			EdgeIterator() = default;
			~EdgeIterator() = default;
			explicit EdgeIterator(MeshGraph3D *graph, EdgeID edge) :
				graph(graph), iterator(EdgeSparseSet::Iterator::FromID(graph->m_Edges, edge.id)) {}
			explicit EdgeIterator(MeshGraph3D *graph, uint32_t index) :
				graph(graph), iterator(graph->m_Edges, index) {}

		public:
			EdgeIterator operator++(int) {
				EdgeIterator res{*this};
				++(*this);
				return res;
			}
			EdgeIterator &operator++() {
				++iterator;
				return *this;
			}

			bool operator==(const EdgeIterator & o) const {return graph == o.graph && iterator == o.iterator;}
			bool operator!=(const EdgeIterator & o) const {return !(*this == o);}

		public:
			reference operator*() const { return GetEdge(); }
			pointer operator->() const { return &GetEdge(); }

		public:
			Edge &GetEdge() { return iterator.GetElement(); }
			EdgeID GetID() { return {iterator.GetID()}; }

			VertexID GetBeginID() { return GetEdge().begin; }
			VertexID GetEndID() { return GetEdge().end; }

			Vertex &GetBegin() { return graph->m_Vertices.Get(GetBeginID()); }
			Vertex &GetEnd() { return graph->m_Vertices.Get(GetEndID()); }

			std::optional<TriangleID> GetSideTriangleID() { return GetEdge().linkedTriangles[0]; }
			std::optional<TriangleID> GetOtherSideTriangleID() { return GetEdge().linkedTriangles[1]; }

			TriangleID *GetSideTriangle() {
				const std::optional<TriangleID> id = GetSideTriangle();
				if (id.has_value())
					graph->m_Triangle.TryGet(id.value().id);
				else
					return nullptr;
			}
			TriangleID *GetOtherSideTriangle() {
				const std::optional<TriangleID> id = GetOtherSideTriangle();
				if (id.has_value())
					graph->m_Triangle.TryGet(id.value().id);
				else
					return nullptr;
			}

			Line<L, T, Q> GetLine() const { return {GetBegin().position, GetEnd().position}; }

		private:
			MeshGraph3D *graph{nullptr};
			typename EdgeSparseSet::Iterator iterator;
		};

		VertexIterator BeginVertex() { return VertexIterator(this, 0); }
		VertexIterator EndVertex() { return VertexIterator(this, m_Vertices.Count()); }
		EdgeIterator BeginEdge() { return EdgeIterator(this, 0); }
		EdgeIterator EndEdge() { return EdgeIterator(this, m_Edges.Count()); }

	public:
		void AddTriangle(vec a, vec b, vec c);
		void AddMesh(const Core::CPUMesh &mesh);
		void AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices);

	public:
		void SubdivideLoop();

	private:
		VertexSparseSet m_Vertices{50};
		EdgeSparseSet m_Edges{50};
		TriangleSparseSet m_Triangle{50};
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

					Edge &edge1 = m_Edges.Get(edge1Id);
					if (!edge1.linkedTriangles[0].has_value()) {
						edge1.linkedTriangles[0] = triangleID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedTriangles[1] = triangleID;
					}
					Edge &edge2 = m_Edges.Get(edge2Id);
					if (!edge1.linkedTriangles[0].has_value()) {
						edge1.linkedTriangles[0] = triangleID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedTriangles[1] = triangleID;
					}
					Edge &edge3 = m_Edges.Get(edge3Id);
					if (!edge1.linkedTriangles[0].has_value()) {
						edge1.linkedTriangles[0] = triangleID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedTriangles[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedTriangles[1] = triangleID;
					}
				}
			}
		}
	}


	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::SubdivideLoop() {
		std::vector<std::tuple<vec, EdgeID>> newPoints;

		// Create the Edge Points "e"
		newPoints.reserve(m_Edges.Count());
		for (auto it = BeginEdge(); it != EndEdge(); ++it) {
			Triangle* some = it.GetSideTriangle();
			Triangle* other = it.GetOtherSideTriangle();

			vec newPoint;
			// Calculate newPoint based one whether we have adjacent triangles.
			if (!some || !other) {
				newPoint = it.GetLine().GetPoint(0.5);
			} else {
				const auto beginId = it.GetBeginID();
				const auto endId = it.GetBeginID();
				const Vertex& begin = m_Vertices.Get(beginId);
				const Vertex& end = m_Vertices.Get(endId);
				const Vertex& someVert = m_Vertices.Get(some->GetOther(beginId, endId()));
				const Vertex& otherVert = m_Vertices.Get(other->GetOther(beginId, endId()));

				constexpr T threeEight = (T(3.0)/T(8.0));
				constexpr T oneEight = (T(1.0)/T(8.0));
				newPoint = threeEight*(begin.position + end.position) + oneEight*(someVert.position + otherVert.position);
			}

			newPoints.emplace_back(newPoint, it.GetID());
		}


	}

} // namespace Imagine::Math
