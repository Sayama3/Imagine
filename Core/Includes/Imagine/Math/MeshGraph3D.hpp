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

#include <algorithm>
#include <vector>

#define MGN_MAKE_PAIR(a, b) ((a < b) ? {a, b} : {b, a})
#define MGN_MAKE_PAIR_TYPE(a, b, type) ((a < b) ? type{a, b} : type{b, a})

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
				if (x.first < x.second)
					return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
				else
					return std::hash<IdType>()(x.second) ^ std::hash<IdType>()(x.first);
			}
		};
		struct EdgeIDPairHash {
		public:
			std::size_t operator()(const ReversiblePair<EdgeID> &x) const {
				if (x.first < x.second)
					return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
				else
					return std::hash<IdType>()(x.second) ^ std::hash<IdType>()(x.first);
			}
		};
		struct FaceIDPairHash {
		public:
			std::size_t operator()(const ReversiblePair<FaceID> &x) const {
				if (x.first < x.second)
					return std::hash<IdType>()(x.first) ^ std::hash<IdType>()(x.second);
				else
					return std::hash<IdType>()(x.second) ^ std::hash<IdType>()(x.first);
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
			Vertex() = default;
			~Vertex() = default;
			Vertex(const Vertex &) = default;
			Vertex &operator=(const Vertex &) = default;
			Vertex(const vec &p) :
				position(p), linkedEdges(), linkedFaces() {}
			vec position{};

			void AddEdge(const EdgeID edge) {
				if (linkedEdges.empty()) {
					linkedEdges.push_back(edge);
				}
				else {
					bool found = false;
					for (uint64_t i = 0; i < linkedEdges.size(); ++i) {
						if (edge == linkedEdges[i]) {
							found = true;
							break;
						}
					}
					if (!found) {
						linkedEdges.push_back(edge);
					}
				}
			}

			void AddFace(const FaceID face) {
				if (linkedFaces.empty()) {
					linkedFaces.push_back(face);
				}
				else {
					bool found = false;
					for (uint64_t i = 0; i < linkedFaces.size(); ++i) {
						if (face == linkedFaces[i]) {
							found = true;
							break;
						}
					}
					if (!found) {
						linkedFaces.push_back(face);
					}
				}
			}

			std::vector<EdgeID> linkedEdges{};
			std::vector<FaceID> linkedFaces{};
		};
		struct Edge {
			Edge() = default;
			~Edge() = default;
			Edge(const Edge &) = default;
			Edge &operator=(const Edge &) = default;
			Edge(VertexID b, VertexID e) :
				begin(b), end(e) {}

			VertexID begin;
			VertexID end;
			std::array<std::optional<FaceID>, 2> linkedFaces;

			VertexID GetOtherVertex(VertexID id) const {
				if (begin == id) return end;
				MGN_CORE_ASSERT(id == end, "The point id {} is not part of this edge.", id.id);
				return begin;
			}

			void AddFace(FaceID faceID) {
				if (!linkedFaces[0].has_value()) {
					linkedFaces[0] = faceID;
				}
				else {
					MGN_CORE_ASSERT(!linkedFaces[1].has_value(), "The two possible triangle of the edge are filled.");
					linkedFaces[1] = faceID;
				}
			}

			FaceID* GetFace(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() == faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() == faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}
			FaceID* GetOtherFace(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() != faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() != faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}

			bool IsBegin(VertexID id) const {
				return id == begin;
			}
			ReversiblePair<VertexID> GetPair() { return MGN_MAKE_PAIR_TYPE(begin, end, ReversiblePair<VertexID>); }
		};
		struct Face {
			inline static constexpr uint64_t VertexCount = 3;

			Face() = default;
			~Face() = default;
			Face(const Face &) = default;
			Face &operator=(const Face &) = default;
			Face(std::array<VertexID, VertexCount> v) :
				vertices(v) {}
			Face(std::array<EdgeID, VertexCount> e) :
				edges(e) {}
			Face(std::array<VertexID, VertexCount> v, std::array<EdgeID, VertexCount> e) :
				vertices(v), edges(e) {}
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
		using VertexSparseSet = Core::AutoIdSparseSet<Vertex, IdType, true>;
		using EdgeSparseSet = Core::AutoIdSparseSet<Edge, IdType, false>;
		using FaceSparseSet = Core::AutoIdSparseSet<Face, IdType, false>;

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
			IdType GetIndex() { return iterator.GetIndex(); }

			uint64_t GetEdgeCount() const {
				return iterator->linkedEdges.size();
			}

			typename std::vector<EdgeID>::iterator GetEdgeBegin() { return iterator->linkedEdges.begin(); }
			typename std::vector<EdgeID>::iterator GetEdgeEnd() { return iterator->linkedEdges.end(); }

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

		bool EnsureLink();

	private:
		bool EnsureLink(VertexSparseSet &newVertices, EdgeSparseSet &newEdges);

	public:
		void SubdivideLoop();
		void SubdivideKobbelt();
		void SubdivideButterfly();

		Core::CPUMesh GetSmoothCPUMesh();
		Core::CPUMesh GetHardCPUMesh();

	private:
		static void AddTriangle(std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash> &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3);

	private:
		std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash> m_VertToEdge;
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
				const vec position = array[i].position;
				if (m_IDs.contains(position)) {
					const auto id = m_IDs.at(position);
					m_IDMap[i] = VertexID{id};
				}
				else {
					Vertex newVertex{};
					newVertex.position = position;
					const auto id = m_Vertices.Create(newVertex);
					m_IDs[position] = VertexID{id};
					m_IDMap[i] = VertexID{id};
				}
			}
		}

		{

			const auto count = indices.Count<uint32_t>();
			const auto *array = indices.Get<uint32_t>();
			for (uint64_t i = 0; i < count; i += 3) {
				AddTriangle(m_VertToEdge, m_Vertices, m_Edges, m_Faces, m_IDMap[array[i + 0]], m_IDMap[array[i + 1]], m_IDMap[array[i + 2]]);
				//EnsureLink(m_Vertices, m_Edges);
			}
		}
	}
	template<typename T, glm::qualifier Q>
	bool MeshGraph3D<T, Q>::EnsureLink() {
		return EnsureLink(m_Vertices, m_Edges);
	}

	template<typename T, glm::qualifier Q>
	bool MeshGraph3D<T, Q>::EnsureLink(VertexSparseSet &newVertices, EdgeSparseSet &newEdges) {
		bool valid = true;
		for (auto it = newVertices.begin(); it != newVertices.end(); ++it) {
			const VertexID id = VertexID{it.GetID()};
			const Vertex &v = it.GetElement();
			for (int i = 0; i < v.linkedEdges.size(); ++i) {
				EdgeID eId = v.linkedEdges[i];
				Edge &edge = newEdges.Get(eId);
				if (!(edge.begin == id || edge.end == id)) {
					MGN_CORE_ERROR("The Edge id {} has an Edge {}", id.id, eId.id);
					valid = false;
				}
			}
		}
		return valid;
	}


	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::AddTriangle(std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash> &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3) {
		EdgeID edge1Id;
		EdgeID edge2Id;
		EdgeID edge3Id;

		FaceID faceID;

		{
			Edge edge1 = {vertId1, vertId2};
			Edge edge2 = {vertId2, vertId3};
			Edge edge3 = {vertId3, vertId1};

			if (edgeIds.contains(MGN_MAKE_PAIR_TYPE(edge1.begin, edge1.end, ReversiblePair<VertexID>))) {
				edge1Id = edgeIds.at(MGN_MAKE_PAIR_TYPE(edge1.begin, edge1.end, ReversiblePair<VertexID>));
				edge1 = newEdges.Get(edge1Id);
				MGN_CORE_ASSERT(edge1.begin == vertId1 || edge1.end == vertId1 && edge1.begin == vertId2 || edge1.end == vertId2, "The Edge is not the correct one.");
			}
			else {
				edge1Id = EdgeID{newEdges.Create(edge1)};
				edgeIds[MGN_MAKE_PAIR_TYPE(edge1.begin, edge1.end, ReversiblePair<VertexID>)] = edge1Id;
			}

			if (edgeIds.contains(MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>))) {
				edge2Id = edgeIds.at(MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>));
				edge2 = newEdges.Get(edge2Id);
				MGN_CORE_ASSERT(edge2.begin == vertId3 || edge2.end == vertId3 && edge2.begin == vertId2 || edge2.end == vertId2, "The Edge is not the correct one.");
			}
			else {
				edge2Id = EdgeID{newEdges.Create(edge2)};
				edgeIds[MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>)] = edge2Id;
			}

			if (edgeIds.contains(MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>))) {
				edge3Id = edgeIds.at(MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>));
				edge3 = newEdges.Get(edge3Id);
				MGN_CORE_ASSERT(edge3.begin == vertId1 || edge3.end == vertId1 && edge3.begin == vertId3 || edge3.end == vertId3, "The Edge is not the correct one.");
			}
			else {
				edge3Id = EdgeID{newEdges.Create(edge3)};
				edgeIds[MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>)] = edge3Id;
			}

			const Face face{
					{vertId1, vertId2, vertId3},
					{edge1Id, edge2Id, edge3Id},
			};

			faceID = FaceID{newFaces.Create(face)};
		}

		// Backpropagate the links
		{
			Vertex &vert1 = newVertices.Get(vertId1.id);
			Vertex &vert2 = newVertices.Get(vertId2.id);
			Vertex &vert3 = newVertices.Get(vertId3.id);

			vert1.AddEdge(edge3Id);
			vert1.AddEdge(edge1Id);

			vert2.AddEdge(edge1Id);
			vert2.AddEdge(edge2Id);

			vert3.AddEdge(edge2Id);
			vert3.AddEdge(edge3Id);

			vert1.AddFace(faceID);
			vert2.AddFace(faceID);
			vert3.AddFace(faceID);

			Edge &edge1 = newEdges.Get(edge1Id);
			edge1.AddFace(faceID);

			Edge &edge2 = newEdges.Get(edge2Id);
			edge2.AddFace(faceID);

			Edge &edge3 = newEdges.Get(edge3Id);
			edge3.AddFace(faceID);
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

		VertexSparseSet newVertices;
		newVertices.Prepare(m_Vertices.Count() + edgePoints.size());

		for (auto it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
			IdType id = it.GetID();
			MGN_CORE_ASSERT(id < m_Vertices.Count(), "wtf");
			Vertex v{it->position};
			newVertices.Create(id, v);
		}

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


			const Vertex newVertex{newPoint};
			const IdType newVertId = newVertices.Create(newVertex);
			if (edgeIt->begin < edgeIt->end)
				edgePoints[{edgeIt->begin, edgeIt->end}] = {newPoint, edgeIt.GetID(), VertexID{newVertId}};
			else
				edgePoints[{edgeIt->end, edgeIt->begin}] = {newPoint, edgeIt.GetID(), VertexID{newVertId}};
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

			for (uint64_t i = 0; i < vertIt->linkedEdges.size(); ++i) {
				EdgeID edgeId = vertIt->linkedEdges[i];
				Edge &edge = m_Edges.Get(edgeId);
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

			const VertexID p1ID = it.GetVertexID(0);
			const VertexID p2ID = it.GetVertexID(1);
			const VertexID p3ID = it.GetVertexID(2);

			// TODO: Handle other than 3 vertex faces
			const auto &edge1 = it.GetEdge(0);
			EdgePoint *ppE1;
			if (edge1.begin < edge1.end)
				ppE1 = &edgePoints.at({edge1.begin, edge1.end});
			else
				ppE1 = &edgePoints.at({edge1.end, edge1.begin});
			EdgePoint &pE1 = *ppE1;

			const auto &edge2 = it.GetEdge(1);
			EdgePoint *ppE2;
			if (edge2.begin < edge2.end)
				ppE2 = &edgePoints.at({edge2.begin, edge2.end});
			else
				ppE2 = &edgePoints.at({edge2.end, edge2.begin});
			EdgePoint &pE2 = *ppE2;

			const auto &edge3 = it.GetEdge(2);
			EdgePoint *ppE3;
			if (edge3.begin < edge3.end)
				ppE3 = &edgePoints.at({edge3.begin, edge3.end});
			else
				ppE3 = &edgePoints.at({edge3.end, edge3.begin});
			EdgePoint &pE3 = *ppE3;

			AddTriangle(edgeIds, newVertices, newEdges, newFaces, pE1.id, pE2.id, pE3.id);
			// EnsureLink(newVertices, newEdges);
			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p1ID, pE1.id, pE3.id);
			// EnsureLink(newVertices, newEdges);
			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p2ID, pE2.id, pE1.id);
			// EnsureLink(newVertices, newEdges);
			AddTriangle(edgeIds, newVertices, newEdges, newFaces, p3ID, pE3.id, pE2.id);
			// EnsureLink(newVertices, newEdges);
		}

		m_Vertices = newVertices;
		m_Edges = newEdges;
		m_Faces = newFaces;
	}
	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::SubdivideKobbelt() {
		FaceSparseSet newFaces;
		const auto endFaces=EndFace();
		for (auto it = BeginFace(); it != endFaces; ++it) {
			const VertexID v1Id = it.GetVertexID(0);
			Vertex& v1 = m_Vertices.Get(v1Id);
			const VertexID v2Id = it.GetVertexID(1);
			Vertex& v2 = m_Vertices.Get(v2Id);
			const VertexID v3Id = it.GetVertexID(2);
			Vertex& v3 = m_Vertices.Get(v3Id);

			const EdgeID v1v2Id = it.GetEdgeID(0);
			Edge& v1v2 = m_Edges.Get(v1v2Id);
			const EdgeID v2v3Id = it.GetEdgeID(1);
			Edge& v2v3 = m_Edges.Get(v2v3Id);
			const EdgeID v3v1Id = it.GetEdgeID(2);
			Edge& v3v1 = m_Edges.Get(v3v1Id);


			VertexID v4Id;
			{
				constexpr T oneThird = T(1) / T(3);
				Vertex v4;
				v4.position = v1.position * oneThird + v2.position * oneThird + v3.position * oneThird;
				v4Id = m_Vertices.Create(v4);
			}
			Vertex& v4 = m_Vertices.Get(v4Id);

			EdgeID v1v4Id;
			EdgeID v2v4Id;
			EdgeID v3v4Id;

			{
				Edge v1v4{v1Id, v4Id};
				v1v4Id = m_Edges.Create(v1v4);
				m_VertToEdge[{v1Id, v4Id}] = v1v4Id;
				Edge v2v4{v2Id, v4Id};
				v2v4Id = m_Edges.Create(v2v4);
				m_VertToEdge[{v2Id, v4Id}] = v2v4Id;
				Edge v3v4{v3Id, v4Id};
				v3v4Id = m_Edges.Create(v3v4);
				m_VertToEdge[{v3Id, v4Id}] = v3v4Id;
			}

			Edge& v1v4 = m_Edges.Get(v1v4Id);
			Edge& v2v4 = m_Edges.Get(v2v4Id);
			Edge& v3v4 = m_Edges.Get(v3v4Id);

			Face f1 {
				{v1Id,v2Id,v4Id},
				{v1v2Id,v2v4Id,v1v4Id}
			};
			FaceID f1Id = newFaces.Create(f1);

			Face f2 {
				{v2Id, v3Id, v4Id},
				{v2v3Id, v3v4Id, v2v4Id}
			};
			FaceID f2Id = newFaces.Create(f2);

			Face f3 {
				{v3Id, v1Id, v4Id},
				{v3v1Id, v1v4Id, v3v4Id}
			};
			FaceID f3Id = newFaces.Create(f3);

			//TODO: Backpropagate the new faceID and the links of this current face.
		}
	}

	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::SubdivideButterfly() {
		MGN_CORE_ERROR("Function 'void MeshGraph3D<T, Q>::SubdivideButterfly(void);' not implemented.");
	}

	template<typename T, glm::qualifier Q>
	Core::CPUMesh MeshGraph3D<T, Q>::GetSmoothCPUMesh() {
		Core::CPUMesh mesh;
		mesh.Vertices.resize(m_Vertices.Count());

		const auto vertBeg = BeginVertex();
		const auto vertEnd = EndVertex();
		for (auto vertIt = vertBeg; vertIt != vertEnd; ++vertIt) {
			const VertexID vertId = vertIt.GetID();
			const Vertex &vert = *vertIt;

			vec normal{0};

			if (true) {
				for (uint64_t i = 0; i < vert.linkedFaces.size(); ++i) {
					FaceID faceId = vert.linkedFaces[i];
					Face& face = m_Faces.Get(faceId.id);
					const Vertex& one = m_Vertices.Get(face.vertices[0]);
					const Vertex& two = m_Vertices.Get(face.vertices[1]);
					const Vertex& three = m_Vertices.Get(face.vertices[2]);
					normal += Math::CalculateTriangleNormal(one.position, two.position,three.position);
				}
				Math::NormalizeInPlace(normal);
			}

			Imagine::Vertex vertex;
			vertex.position = vert.position;
			vertex.normal = normal;
			mesh.Vertices[vertIt.GetIndex()] = vertex;
		}

		mesh.Indices.reserve(m_Faces.Count() * ((Face::VertexCount - 3) * 3 + 3));
		for (auto it = m_Faces.cbegin(); it != m_Faces.cend(); ++it) {
			const auto first = m_Vertices.GetIndex(it->vertices[0]);
			auto last = m_Vertices.GetIndex(it->vertices[1]);
			for (uint64_t i = 2; i < Face::VertexCount; ++i) {
				auto newLast = m_Vertices.GetIndex(it->vertices[i]);
				mesh.Indices.push_back(first);
				mesh.Indices.push_back(last);
				mesh.Indices.push_back(newLast);
				last = newLast;
			}
		}

		return mesh;
	}

	template<typename T, glm::qualifier Q>
	Core::CPUMesh MeshGraph3D<T, Q>::GetHardCPUMesh() {
		Core::CPUMesh mesh;
		const auto triangleCount = m_Faces.Count() * ((Face::VertexCount - 3) * 3 + 3);
		mesh.Vertices.resize(triangleCount*3);
		mesh.Indices.reserve(triangleCount);

		for (auto it = m_Faces.cbegin(); it != m_Faces.cend(); ++it) {
			const VertexID firstId = it->vertices[0];
			VertexID lastId = it->vertices[1];
			for (uint64_t i = 2; i < Face::VertexCount; ++i) {
				const VertexID newLastId = it->vertices[i];

				const Vertex& first = m_Vertices.Get(firstId);
				Imagine::Vertex vFirst{first.position};

				const Vertex& last = m_Vertices.Get(lastId);
				Imagine::Vertex vLast{last.position};

				const Vertex& newLast = m_Vertices.Get(newLastId);
				Imagine::Vertex vNewLast{newLast.position};

				vec normal = Math::CalculateTriangleNormal(first.position, last.position, newLast.position);

				vFirst.normal = normal;
				vLast.normal = normal;
				vNewLast.normal = normal;

				const uint32_t offset = mesh.Vertices.size();

				mesh.Vertices.push_back(vFirst);
				mesh.Vertices.push_back(vLast);
				mesh.Vertices.push_back(vNewLast);

				mesh.Indices.push_back(offset + 0);
				mesh.Indices.push_back(offset + 1);
				mesh.Indices.push_back(offset + 2);

				lastId = newLastId;
			}
		}

		return mesh;
	}

} // namespace Imagine::Math
