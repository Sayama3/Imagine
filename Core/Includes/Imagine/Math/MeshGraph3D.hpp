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
			explicit VertexID(const IdType _id) :
				id(_id) {}
			IdType id;

			operator IdType() const { return id; }

			bool operator==(const VertexID &o) const { return id == o.id; }
			bool operator!=(const VertexID &o) const { return !(*this == o); }
			auto operator<=>(const VertexID &o) const { return id <=> o.id; }
		};
		struct EdgeID {
			EdgeID() = default;
			~EdgeID() = default;
			explicit EdgeID(const IdType _id) :
				id(_id) {}
			IdType id;

			operator IdType() const { return id; }

			bool operator==(const EdgeID &o) const { return id == o.id; }
			bool operator!=(const EdgeID &o) const { return !(*this == o); }
			auto operator<=>(const EdgeID &o) const { return id <=> o.id; }
		};
		struct FaceID {
			FaceID() = default;
			~FaceID() = default;
			explicit FaceID(const IdType _id) :
				id(_id) {}
			IdType id;

			operator IdType() const { return id; }

			bool operator==(const FaceID &o) const { return id == o.id; }
			bool operator!=(const FaceID &o) const { return !(*this == o); }
			auto operator<=>(const FaceID &o) const { return id <=> o.id; }
		};


		struct VertexIDPairHash {
		public:
			std::size_t operator()(const ReversiblePair<VertexID> &x) const {
				return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
			}
		};
		struct EdgeIDPairHash {
		public:
			std::size_t operator()(const ReversiblePair<EdgeID> &x) const {
				return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
			}
		};
		struct FaceIDPairHash {
		public:
			std::size_t operator()(const ReversiblePair<FaceID> &x) const {
				return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
			}
		};
		struct VertexIDHash {
		public:
			std::size_t operator()(const VertexID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};
		struct EdgeIDHash {
		public:
			std::size_t operator()(const EdgeID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};
		struct FaceIDHash {
		public:
			std::size_t operator()(const FaceID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};

		struct Vertex {
			vec position;
			std::set<EdgeID> linkedEdges;
			std::set<FaceID> linkedFaces;
		};
		struct Edge {
			VertexID begin;
			VertexID end;
			std::array<std::optional<FaceID>, 2> linkedFaces;

			VertexID GetOtherVertex(VertexID id) const {
				if (begin == id) return end;
				MGN_CORE_ASSERT(id == end, "The point id {} is not part of this edge.", id.id);
				return begin;
			}

			bool IsBegin(VertexID id) const {
				return id == begin;
			}
		};
		struct Face {
			inline static constexpr uint64_t VertexCount = 3;
			std::array<VertexID, VertexCount> vertices;
			std::array<EdgeID, VertexCount> edges;

			VertexID GetOther(VertexID one, VertexID two) {
				auto it = std::find_if(vertices.begin(), vertices.end(), [one, two](auto v) {
					return v != one && v != two;
				});
				return *it;
			}
		};

	public:
		using VertexSparseSet = Core::AutoIdSparseSet<Vertex, IdType>;
		using EdgeSparseSet = Core::AutoIdSparseSet<Edge, IdType>;
		using FaceSparseSet = Core::AutoIdSparseSet<Face, IdType>;

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
				graph(graph), iterator(&graph->m_Vertices, index) {}

			VertexIterator operator++(int) {
				VertexIterator res{*this};
				++(*this);
				return res;
			}
			VertexIterator &operator++() {
				++iterator;
				return *this;
			}
			reference operator*() { return GetVertex(); }
			pointer operator->() { return &GetVertex(); }

			bool operator==(const VertexIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			bool operator!=(const VertexIterator &o) const { return !(*this == o); }
			auto operator<=>(const VertexIterator &o) const { return iterator <=> o.iterator; }

		public:
			Vertex &GetVertex() { return iterator.GetElement(); }
			VertexID GetID() { return VertexID{iterator.GetID()}; }

			uint64_t GetEdgeCount() const {
				return iterator->linkedEdges.size();
			}

			typename std::set<EdgeID>::iterator GetEdgeBegin() { return iterator->linkedEdges.begin(); }
			typename std::set<EdgeID>::iterator GetEdgeEnd() { return iterator->linkedEdges.end(); }

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
				graph(graph), iterator(&graph->m_Edges, index) {}

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

			bool operator==(const EdgeIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			bool operator!=(const EdgeIterator &o) const { return !(*this == o); }
			auto operator<=>(const EdgeIterator &o) const { return iterator <=> o.iterator; }

		public:
			reference operator*() { return GetEdge(); }
			pointer operator->() { return &GetEdge(); }

		public:
			Edge &GetEdge() { return iterator.GetElement(); }
			EdgeID GetID() { return EdgeID{iterator.GetID()}; }

			VertexID GetBeginID() { return GetEdge().begin; }
			VertexID GetEndID() { return GetEdge().end; }

			Vertex &GetBegin() { return graph->m_Vertices.Get(GetBeginID()); }
			Vertex &GetEnd() { return graph->m_Vertices.Get(GetEndID()); }

			std::optional<FaceID> GetSideFaceID() { return GetEdge().linkedFaces[0]; }
			std::optional<FaceID> GetOtherSideFaceID() { return GetEdge().linkedFaces[1]; }

			Face *GetSideFace() {
				const std::optional<FaceID> id = GetSideFaceID();
				if (id.has_value())
					return graph->m_Faces.TryGet(id.value().id);
				else
					return nullptr;
			}
			Face *GetOtherSideFace() {
				const std::optional<FaceID> id = GetOtherSideFaceID();
				if (id.has_value())
					return graph->m_Faces.TryGet(id.value().id);
				else
					return nullptr;
			}

			Line<L, T, Q> GetLine() { return Line<L, T, Q>{GetBegin().position, GetEnd().position}; }

		private:
			MeshGraph3D *graph{nullptr};
			typename EdgeSparseSet::Iterator iterator;
		};

		struct FaceIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Face;
			using difference_type = std::ptrdiff_t;
			using pointer = Face *;
			using reference = Face &;

		public:
			explicit FaceIterator(MeshGraph3D *graph, FaceID face) :
				graph(graph), iterator(FaceSparseSet::Iterator::FromID(graph->m_Faces, face.id)) {}
			explicit FaceIterator(MeshGraph3D *graph, uint32_t index) :
				graph(graph), iterator(&graph->m_Faces, index) {}

		public:
			FaceIterator operator++(int) {
				EdgeIterator res{*this};
				++(*this);
				return res;
			}
			FaceIterator &operator++() {
				++iterator;
				return *this;
			}

			bool operator==(const FaceIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			bool operator!=(const FaceIterator &o) const { return !(*this == o); }
			auto operator<=>(const FaceIterator &o) const { return iterator <=> o.iterator; }

			reference operator*() { return GetFace(); }
			pointer operator->() { return &GetFace(); }

		public:
			Face &GetFace() { return iterator.GetElement(); }
			FaceID GetID() { return FaceID{iterator.GetID()}; }

			constexpr uint64_t GetEdgeCount() const { return Face::VertexCount; }
			constexpr uint64_t GetVertexCount() const { return Face::VertexCount; }

			VertexID &GetVertexID(const uint64_t index) { return iterator->vertices[index]; }
			EdgeID &GetEdgeID(const uint64_t index) { return iterator->edges[index]; }

			std::array<VertexID, Face::VertexCount>::iterator GetVertexIDBegin() { return iterator->vertices.begin(); }
			std::array<VertexID, Face::VertexCount>::iterator GetVertexIDEnd() { return iterator->vertices.end(); }

			std::array<EdgeID, Face::VertexCount>::iterator GetEdgeIDBegin() { return iterator->edges.begin(); }
			std::array<EdgeID, Face::VertexCount>::iterator GetEdgeIDEnd() { return iterator->edges.end(); }

			Vertex &GetVertex(const uint64_t index) { return graph->m_Vertices.Get(iterator->vertices[index]); }
			Edge &GetEdge(const uint64_t index) { return graph->m_Edges.Get(iterator->edges[index]); }

		private:
			MeshGraph3D *graph{nullptr};
			typename FaceSparseSet::Iterator iterator;
		};

		VertexIterator BeginVertex() { return VertexIterator(this, 0); }
		VertexIterator EndVertex() { return VertexIterator(this, m_Vertices.Count()); }

		EdgeIterator BeginEdge() { return EdgeIterator(this, 0); }
		EdgeIterator EndEdge() { return EdgeIterator(this, m_Edges.Count()); }

		FaceIterator BeginFace() { return FaceIterator(this, 0); }
		FaceIterator EndFace() { return FaceIterator(this, m_Faces.Count()); }

	public:
		void Clear() {
			m_Vertices.Clear();
			m_Edges.Clear();
			m_Faces.Clear();
		}
		void AddFace(vec a, vec b, vec c);
		void AddMesh(const Core::CPUMesh &mesh);
		void AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices);

	public:
		void SubdivideLoop();

		Core::CPUMesh GetCPUMesh() const;

	private:
		void AddTriangle(std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash>& edgeIds, VertexSparseSet& newVertices, EdgeSparseSet& newEdges, FaceSparseSet& newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3);
	private:
		VertexSparseSet m_Vertices{50};
		EdgeSparseSet m_Edges{50};
		FaceSparseSet m_Faces{50};
	};
} // namespace Imagine::Math

namespace Imagine::Math {
	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddFace(vec a, vec b, vec c) {

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
					m_IDMap[i] = VertexID{id};
				}
				else {
					const auto id = m_Vertices.Create({p});
					m_IDs[p] = VertexID{id};
					m_IDMap[i] = VertexID{id};
				}
			}
		}

		{
			std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash> edges;

			const auto count = indices.Count<uint32_t>();
			const auto *array = indices.Get<uint32_t>();
			for (uint64_t i = 0; i < count; i += 3) {
				VertexID vertId1;
				VertexID vertId2;
				VertexID vertId3;

				EdgeID edge1Id;
				EdgeID edge2Id;
				EdgeID edge3Id;

				FaceID faceID;

				{
					vertId1 = m_IDMap[array[i + 0]];
					vertId2 = m_IDMap[array[i + 1]];
					vertId3 = m_IDMap[array[i + 2]];

					Edge edge1 = {vertId1, vertId2};
					Edge edge2 = {vertId2, vertId3};
					Edge edge3 = {vertId3, vertId1};

					if (edges.contains({edge1.begin, edge1.end})) {
						edge1Id = edges.at({edge1.begin, edge1.end});
						edge1 = m_Edges.Get(edge1Id);
					}
					else {
						edge1Id = EdgeID{m_Edges.Create(edge1)};
						edges[{edge1.begin, edge1.end}] = edge1Id;
					}

					if (edges.contains({edge2.begin, edge2.end})) {
						edge2Id = edges.at({edge2.begin, edge2.end});
						edge2 = m_Edges.Get(edge2Id);
					}
					else {
						edge2Id = EdgeID{m_Edges.Create(edge2)};
						edges[{edge2.begin, edge2.end}] = edge2Id;
					}

					if (edges.contains({edge3.begin, edge3.end})) {
						edge3Id = edges.at({edge3.begin, edge3.end});
						edge3 = m_Edges.Get(edge3Id);
					}
					else {
						edge3Id = EdgeID{m_Edges.Create(edge3)};
						edges[{edge3.begin, edge3.end}] = edge3Id;
					}

					const Face face{
							{vertId1, vertId2, vertId3},
							{edge1Id, edge2Id, edge3Id},
					};

					faceID = FaceID{m_Faces.Create(face)};
				}

				// Backpropagate the links
				{
					m_Vertices.Get(vertId1.id).linkedEdges.insert(edge3Id);
					m_Vertices.Get(vertId1.id).linkedEdges.insert(edge1Id);

					m_Vertices.Get(vertId2.id).linkedEdges.insert(edge1Id);
					m_Vertices.Get(vertId2.id).linkedEdges.insert(edge2Id);

					m_Vertices.Get(vertId3.id).linkedEdges.insert(edge2Id);
					m_Vertices.Get(vertId3.id).linkedEdges.insert(edge3Id);

					m_Vertices.Get(vertId1.id).linkedFaces.insert(faceID);
					m_Vertices.Get(vertId2.id).linkedFaces.insert(faceID);
					m_Vertices.Get(vertId3.id).linkedFaces.insert(faceID);

					Edge &edge1 = m_Edges.Get(edge1Id);
					if (!edge1.linkedFaces[0].has_value()) {
						edge1.linkedFaces[0] = faceID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedFaces[1] = faceID;
					}
					Edge &edge2 = m_Edges.Get(edge2Id);
					if (!edge1.linkedFaces[0].has_value()) {
						edge1.linkedFaces[0] = faceID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedFaces[1] = faceID;
					}
					Edge &edge3 = m_Edges.Get(edge3Id);
					if (!edge1.linkedFaces[0].has_value()) {
						edge1.linkedFaces[0] = faceID;
					}
					else {
						MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
						edge1.linkedFaces[1] = faceID;
					}
				}
			}
		}
	}


	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddTriangle( std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash>& edgeIds, VertexSparseSet& newVertices, EdgeSparseSet& newEdges, FaceSparseSet& newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3) {
		EdgeID edge1Id;
		EdgeID edge2Id;
		EdgeID edge3Id;

		FaceID faceID;

		{
			Edge edge1 = {vertId1, vertId2};
			Edge edge2 = {vertId2, vertId3};
			Edge edge3 = {vertId3, vertId1};

			if (edgeIds.contains({edge1.begin, edge1.end})) {
				edge1Id = edgeIds.at({edge1.begin, edge1.end});
				edge1 = newEdges.Get(edge1Id);
			}
			else {
				edge1Id = EdgeID{newEdges.Create(edge1)};
				edgeIds[{edge1.begin, edge1.end}] = edge1Id;
			}

			if (edgeIds.contains({edge2.begin, edge2.end})) {
				edge2Id = edgeIds.at({edge2.begin, edge2.end});
				edge2 = newEdges.Get(edge2Id);
			}
			else {
				edge2Id = EdgeID{newEdges.Create(edge2)};
				edgeIds[{edge2.begin, edge2.end}] = edge2Id;
			}

			if (edgeIds.contains({edge3.begin, edge3.end})) {
				edge3Id = edgeIds.at({edge3.begin, edge3.end});
				edge3 = newEdges.Get(edge3Id);
			}
			else {
				edge3Id = EdgeID{newEdges.Create(edge3)};
				edgeIds[{edge3.begin, edge3.end}] = edge3Id;
			}

			const Face face{
					{vertId1, vertId2, vertId3},
					{edge1Id, edge2Id, edge3Id},
			};

			faceID = FaceID{newFaces.Create(face)};
		}

		// Backpropagate the links
		{
			newVertices.Get(vertId1.id).linkedEdges.insert(edge3Id);
			newVertices.Get(vertId1.id).linkedEdges.insert(edge1Id);

			newVertices.Get(vertId2.id).linkedEdges.insert(edge1Id);
			newVertices.Get(vertId2.id).linkedEdges.insert(edge2Id);

			newVertices.Get(vertId3.id).linkedEdges.insert(edge2Id);
			newVertices.Get(vertId3.id).linkedEdges.insert(edge3Id);

			newVertices.Get(vertId1.id).linkedFaces.insert(faceID);
			newVertices.Get(vertId2.id).linkedFaces.insert(faceID);
			newVertices.Get(vertId3.id).linkedFaces.insert(faceID);

			Edge &edge1 = newEdges.Get(edge1Id);
			if (!edge1.linkedFaces[0].has_value()) {
				edge1.linkedFaces[0] = faceID;
			}
			else {
				MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
				edge1.linkedFaces[1] = faceID;
			}
			Edge &edge2 = newEdges.Get(edge2Id);
			if (!edge1.linkedFaces[0].has_value()) {
				edge1.linkedFaces[0] = faceID;
			}
			else {
				MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
				edge1.linkedFaces[1] = faceID;
			}
			Edge &edge3 = newEdges.Get(edge3Id);
			if (!edge1.linkedFaces[0].has_value()) {
				edge1.linkedFaces[0] = faceID;
			}
			else {
				MGN_CORE_ASSERT(!edge1.linkedFaces[1].has_value(), "The two possible trianlge of the edge are filled.");
				edge1.linkedFaces[1] = faceID;
			}
		}
	}

	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::SubdivideLoop() {
		struct EdgePoint {
			vec position;
			EdgeID edge;
			VertexID id;
		};
		std::unordered_map<ReversiblePair<VertexID>, EdgePoint, VertexIDPairHash> edgePoints;
		edgePoints.reserve(m_Edges.Count());

		VertexSparseSet newVertices = m_Vertices;
		newVertices.Prepare(edgePoints.size());

		// Create the Edge Points "e"
		for (auto edgeIt = BeginEdge(); edgeIt != EndEdge(); ++edgeIt) {
			Face *some = edgeIt.GetSideFace();
			Face *other = edgeIt.GetOtherSideFace();

			vec newPoint;
			// Calculate newPoint based one whether we have adjacent faces.
			if (!some || !other) {
				newPoint = edgeIt.GetLine().GetPoint(0.5);
			}
			else {
				const VertexID beginId = edgeIt.GetBeginID();
				const VertexID endId = edgeIt.GetEndID();

				const Vertex &begin = m_Vertices.Get(beginId.id);
				const Vertex &end = m_Vertices.Get(endId.id);

				const Vertex &someVert = m_Vertices.Get(some->GetOther(beginId, endId).id);
				const Vertex &otherVert = m_Vertices.Get(other->GetOther(beginId, endId).id);

				constexpr T threeEight = (T(3.0) / T(8.0));
				constexpr T oneEight = (T(1.0) / T(8.0));
				newPoint = threeEight * (begin.position + end.position) + oneEight * (someVert.position + otherVert.position);
			}


			Vertex newVertex;
			newVertex.position = newPoint;
			const auto newVertId = newVertices.Create(newVertex);
			edgePoints[{edgeIt->begin, edgeIt->end}] = {newPoint, edgeIt.GetID(), VertexID{newVertId}};
			// newPoints.emplace_back(newPoint, edgeIt.GetID());
		}

		for (VertexIterator vertIt = BeginVertex(); vertIt != EndVertex(); ++vertIt) {
			Vertex &vertex = *vertIt;
			vec &vertPos = vertex.position;
			const uint64_t adjacentVertices = vertIt.GetEdgeCount();
			const T tAdj = T(adjacentVertices);

			T alpha{0.25};
			if (adjacentVertices == 2) {
				alpha = T(1) / T(8);
			}
			else if (adjacentVertices == 3) {
				alpha = T(3) / T(16);
			}
			else if (adjacentVertices > 3) {
				const T oneN = T(1) / tAdj;
				constexpr T fiveHeight = (T(5) / T(8));
				constexpr T threeHeight = (T(3) / T(8));
				constexpr T oneFourth = (T(1) / T(4));
				const T tauN = Math::Tau / tAdj;
				const T cosTauN = std::cos(tauN);
				alpha = threeHeight + (oneFourth * cosTauN);
				alpha = alpha * alpha;
				alpha = fiveHeight - alpha;
				alpha = oneN * alpha;
			}

			vec vertPrim = (T(1) - (tAdj * alpha)) * vertPos;

			const auto begin = vertIt.GetEdgeBegin();
			const auto end = vertIt.GetEdgeEnd();
			for (auto it = begin; it != end; ++it) {
				Edge &edge = m_Edges.Get(*it);
				const auto vertId = edge.GetOtherVertex(vertIt.GetID());
				const auto &vert = m_Vertices.Get(vertId);
				vertPrim += vert.position * alpha;
			}

			newVertices.Get(vertIt.GetID().id).position = vertPrim;
		}

		std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash> edgeIds;
		edgeIds.reserve(m_Edges.Count() + edgePoints.size() * 3);
		EdgeSparseSet newEdges{static_cast<IdType>(m_Edges.Count() + edgePoints.size() * 3)};
		FaceSparseSet newFaces{static_cast<IdType>(m_Faces.Count() * 3)};


		const auto begFace = BeginFace();
		const auto endFace = EndFace();

		for (FaceIterator it = begFace; it != endFace; ++it) {
			Face face;

			auto p1ID = it.GetVertexID(0);
			auto p2ID = it.GetVertexID(1);
			auto p3ID = it.GetVertexID(2);

			// TODO: Handle other than 3 vertex faces
			const auto &edge1 = it.GetEdge(0);
			auto &pE1 = edgePoints.at({edge1.begin, edge1.end});

			const auto &edge2 = it.GetEdge(1);
			auto &pE2 = edgePoints.at({edge2.begin, edge2.end});

			const auto &edge3 = it.GetEdge(2);
			auto &pE3 = edgePoints.at({edge3.begin, edge3.end});

			AddTriangle(edgeIds, newVertices, newEdges, newFaces, pE1.id, pE2.id, pE3.id);

			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p1ID, pE1.id, pE3.id);
			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p2ID, pE2.id, pE1.id);
			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p3ID, pE3.id, pE2.id);
		}

		std::swap(m_Vertices, newVertices);
		std::swap(m_Edges, newEdges);
		std::swap(m_Faces, newFaces);
	}

	template<typename T, glm::qualifier Q>
	Core::CPUMesh MeshGraph3D<T, Q>::GetCPUMesh() const {
		return {};
	}

} // namespace Imagine::Math
