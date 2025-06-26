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
			void RemoveEdge(const EdgeID edge) {
				bool found = false;
				auto it = std::find(linkedEdges.begin(), linkedEdges.end(), edge);
				if (it != linkedEdges.end()) {
					linkedEdges.erase(it);
				}
			}
			void RemoveFace(const FaceID face) {
				bool found = false;
				auto it = std::find(linkedFaces.begin(), linkedFaces.end(), face);
				if (it != linkedFaces.end()) {
					linkedFaces.erase(it);
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
			std::array<std::optional<FaceID>, 2> linkedFaces{std::nullopt, std::nullopt};

			[[nodiscard]] VertexID GetOtherVertexID(VertexID id) const {
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

			[[nodiscard]] FaceID* GetFaceID(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() == faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() == faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}
			[[nodiscard]] FaceID* GetOtherFaceID(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() != faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() != faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}

			[[nodiscard]] bool IsBegin(VertexID id) const {
				return id == begin;
			}

			[[nodiscard]] bool HasTwoTriangle() const {
				return linkedFaces[0].has_value() && linkedFaces[1].has_value();
			}

			[[nodiscard]] ReversiblePair<VertexID> GetPair() { return MGN_MAKE_PAIR_TYPE(begin, end, ReversiblePair<VertexID>); }
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

			std::optional<VertexID> GetFollowing(const VertexID v) const {
				for (uint64_t i = 0; i < VertexCount; ++i) {
					if (vertices[i] == v) return vertices[(i+1)%VertexCount];
				}
				return std::nullopt;
			}

			VertexID GetOther(const VertexID one, const VertexID two) const {
				auto it = std::find_if(vertices.begin(), vertices.end(), [one, two](auto v) {
					return v != one && v != two;
				});
				return *it;
			}
		};

	public:
		using VerticesToEdgeMap = std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash>;
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
			m_VertToEdge.clear();
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
		static void AddTriangle(VerticesToEdgeMap &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3);

	private:
		VerticesToEdgeMap m_VertToEdge{};
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
	void MeshGraph3D<T, Q>::AddTriangle(VerticesToEdgeMap &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3) {
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
				const auto vertId = edge.GetOtherVertexID(vertIt.GetID());
				const auto &vert = m_Vertices.Get(vertId);
				vertPrim += vert.position * alpha;
			}


			newVertices.Get(vertIt.GetID().id).position = vertPrim;
		}

		VerticesToEdgeMap edgeIds;
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

		m_VertToEdge = edgeIds;
		m_Vertices = newVertices;
		m_Edges = newEdges;
		m_Faces = newFaces;
	}
	template<typename T, glm::qualifier Q>
	void MeshGraph3D<T, Q>::SubdivideKobbelt() {

		// We initialize our kobbel with a fresh copy of our data structure but clearing all the linked types.
		//  Theses will be filled in back as we go. Only works as we used kinda set and check for no doubles.
		std::unordered_set<EdgeID, EdgeIDHash> originalEdges;
		VerticesToEdgeMap vertToEdgeMap = m_VertToEdge;
		VertexSparseSet newVertices(m_Vertices.Count());
		EdgeSparseSet newEdges(m_Edges.Count());
		FaceSparseSet newFaces;

		const auto endFaces=EndFace();
		originalEdges.reserve(m_Edges.Count());


		//Perturb the initial position in the initial vertex set and cleanup the vertex link.
		// Copy is not working so manual copy. To fix later.
		for(uint64_t i = 0; i < m_Vertices.Count(); ++i) {
			const auto vertId = m_Vertices.GetID(i);
			newVertices.GetOrCreate(vertId).position = m_Vertices.Get(vertId).position;
		}

		for (uint64_t i = 0; i < m_Edges.Count(); ++i) {
			const auto edgeId = m_Edges.GetID(i);
			const auto& edge = m_Edges.Get(edgeId);
			Edge newEdge{edge.begin, edge.end};
			newEdges.GetOrCreate(edgeId).begin = newEdge.begin;
			newEdges.GetOrCreate(edgeId).end = newEdge.end;
		}

		const auto endNewVert = m_Vertices.end();
		for (auto it = m_Vertices.begin(); it != endNewVert; ++it) {
			constexpr T oneNine = T(1) / T(9);

			const uint64_t neighCount = it->linkedEdges.size();
			const T tNeighCount = T(neighCount);
			const T alpha = oneNine * (4 - (2*Math::Cos(T(Math::Tau)/tNeighCount)));

			vec cumulatedNeigh{0};

			const T alphaOnN = alpha / tNeighCount;

			for (uint64_t i = 0; i < neighCount; ++i) {
				cumulatedNeigh += newVertices.Get(m_Edges.Get(it->linkedEdges[i]).GetOtherVertexID(VertexID{it.GetID()})).position * alphaOnN;
			}
			const vec new_pos = (1 - alpha) * it->position + cumulatedNeigh;
			m_Vertices.Get(it.GetID()).position = new_pos;
		}

		// Iterate on all triangles to create the new triangles. 1 to 3
		for (auto it = BeginFace(); it != endFaces; ++it) {
			const VertexID v1Id = it.GetVertexID(0);
			Vertex& v1 = newVertices.Get(v1Id);


			const VertexID v2Id = it.GetVertexID(1);
			Vertex& v2 = newVertices.Get(v2Id);


			const VertexID v3Id = it.GetVertexID(2);
			Vertex& v3 = newVertices.Get(v3Id);

			VertexID v4Id;
			{
				constexpr T oneThird = T(1) / T(3);
				Vertex v4;
				v4.position = v1.position * oneThird + v2.position * oneThird + v3.position * oneThird;
				v4Id.id = newVertices.Create(v4);
			}
			Vertex& v4 = newVertices.Get(v4Id);

			const EdgeID v1v2Id = it.GetEdgeID(0);
			Edge& v1v2 = newEdges.Get(v1v2Id);
			originalEdges.emplace(v1v2Id);

			const EdgeID v2v3Id = it.GetEdgeID(1);
			Edge& v2v3 = newEdges.Get(v2v3Id);
			newEdges.Create(v2v3Id, v2v3);
			originalEdges.emplace(v2v3Id);

			const EdgeID v3v1Id = it.GetEdgeID(2);
			Edge& v3v1 = newEdges.Get(v3v1Id);
			originalEdges.emplace(v3v1Id);

			EdgeID v1v4Id;
			EdgeID v2v4Id;
			EdgeID v3v4Id;

			{
				Edge v1v4{v1Id, v4Id};
				v1v4Id.id = newEdges.Create(v1v4);
				vertToEdgeMap[{v1Id, v4Id}] = v1v4Id;
				Edge v2v4{v2Id, v4Id};
				v2v4Id.id = newEdges.Create(v2v4);
				vertToEdgeMap[{v2Id, v4Id}] = v2v4Id;
				Edge v3v4{v3Id, v4Id};
				v3v4Id.id = newEdges.Create(v3v4);
				vertToEdgeMap[{v3Id, v4Id}] = v3v4Id;
			}

			Edge& v1v4 = newEdges.Get(v1v4Id);
			Edge& v2v4 = newEdges.Get(v2v4Id);
			Edge& v3v4 = newEdges.Get(v3v4Id);

			const Face f1 {
				{v1Id,v2Id,v4Id},
				{v1v2Id,v2v4Id,v1v4Id}
			};
			FaceID f1Id = FaceID{newFaces.Create(f1)};

			const Face f2 {
				{v2Id, v3Id, v4Id},
				{v2v3Id, v3v4Id, v2v4Id}
			};
			FaceID f2Id = FaceID{newFaces.Create(f2)};

			const Face f3 {
				{v3Id, v1Id, v4Id},
				{v3v1Id, v1v4Id, v3v4Id}
			};
			FaceID f3Id = FaceID{newFaces.Create(f3)};


			// Original Vertices - New Faces
			v1.AddFace(f3Id);
			v1.AddFace(f1Id);

			v2.AddFace(f1Id);
			v2.AddFace(f2Id);

			v3.AddFace(f2Id);
			v3.AddFace(f3Id);

			// Original Vertices - New Edges
			v1.AddEdge(v3v1Id);
			v1.AddEdge(v1v2Id);
			v1.AddEdge(v1v4Id);

			v2.AddEdge(v1v2Id);
			v2.AddEdge(v2v3Id);
			v2.AddEdge(v2v4Id);

			v3.AddEdge(v2v3Id);
			v3.AddEdge(v3v1Id);
			v3.AddEdge(v3v4Id);

			// Original Edges - New Faces
			v1v2.AddFace(f1Id);
			v2v3.AddFace(f2Id);
			v3v1.AddFace(f3Id);

			// New Vertices - New Edges
			v4.AddEdge(v1v4Id);
			v4.AddEdge(v2v4Id);
			v4.AddEdge(v3v4Id);

			// New Vertices - New Faces
			v4.AddFace(f1Id);
			v4.AddFace(f2Id);
			v4.AddFace(f3Id);

			// New Edges - New Faces
			v1v4.AddFace(f3Id);
			v1v4.AddFace(f1Id);

			v2v4.AddFace(f1Id);
			v2v4.AddFace(f2Id);

			v3v4.AddFace(f2Id);
			v3v4.AddFace(f3Id);

		}

		// Apply the perturbed position.
		const auto vertEnd = m_Vertices.end();
		for (auto it = m_Vertices.begin(); it != vertEnd; ++it) {
			newVertices.Get(it.GetID()).position = it->position;
		}


		// We can now move onto the flipping.
		for (EdgeID edgeId : originalEdges) {
			Edge& edge = newEdges.Get(edgeId.id);

			if (!edge.HasTwoTriangle()) continue;

			const VertexID v1Id = edge.begin;
			const VertexID v2Id = edge.end;


			const FaceID f1Id = edge.linkedFaces[0].value();
			const FaceID f2Id = edge.linkedFaces[1].value();

			Face& f1 = newFaces.Get(f1Id);
			Face& f2 = newFaces.Get(f2Id);

			const VertexID v3Id = f1.GetOther(edge.begin, edge.end);
			const VertexID v4Id = f2.GetOther(edge.begin, edge.end);


			// Change edge
			edge.begin = v3Id;
			edge.end = v4Id;

			// Swap the link to this edges
			vertToEdgeMap.erase({v1Id, v2Id});
			vertToEdgeMap[{v3Id, v4Id}] = edgeId;

			// Remove the edge from the vertice
			newVertices.Get(v1Id).RemoveEdge(edgeId);
			newVertices.Get(v2Id).RemoveEdge(edgeId);

			// Remove the face of the vertice before we know which is who
			newVertices.Get(v1Id).RemoveFace(f1Id);
			newVertices.Get(v2Id).RemoveFace(f2Id);

			// Add the new Edge to the vertex
			newVertices.Get(v3Id).AddEdge(edgeId);
			newVertices.Get(v4Id).AddEdge(edgeId);

			// Add the missing face to the Edges.
			newVertices.Get(v3Id).AddFace(f2Id);
			newVertices.Get(v4Id).AddFace(f1Id);

			Face newf1;
			if (f1.GetFollowing(v3Id) == v1Id) {
				MGN_CORE_ASSERT(f1.GetFollowing(v1Id) == v2Id, "WTF is this triangle !");

				newf1.vertices = {v3Id, v1Id, v4Id};
				newf1.edges = {vertToEdgeMap.at({v3Id, v1Id}),vertToEdgeMap.at({v1Id, v4Id}),vertToEdgeMap.at({v4Id, v3Id})};
			} else {
				MGN_CORE_ASSERT(f1.GetFollowing(v3Id) == v2Id, "WTF is this triangle !");
				MGN_CORE_ASSERT(f1.GetFollowing(v2Id) == v1Id, "WTF is this triangle !");
				newf1.vertices = {v3Id, v2Id, v4Id};
				newf1.edges = {vertToEdgeMap.at({v3Id, v2Id}),vertToEdgeMap.at({v2Id, v4Id}),vertToEdgeMap.at({v4Id, v3Id})};
			}

			Face newf2;
			if (f2.GetFollowing(v4Id) == v1Id) {
				MGN_CORE_ASSERT(f2.GetFollowing(v1Id) == v2Id, "WTF is this triangle !");

				newf2.vertices = {v4Id, v1Id, v3Id};
				newf2.edges = {vertToEdgeMap.at({v4Id, v1Id}),vertToEdgeMap.at({v1Id, v3Id}),vertToEdgeMap.at({v3Id, v4Id})};
			} else {
				MGN_CORE_ASSERT(f2.GetFollowing(v4Id) == v2Id, "WTF is this triangle !");
				MGN_CORE_ASSERT(f2.GetFollowing(v2Id) == v1Id, "WTF is this triangle !");
				newf2.vertices = {v4Id, v2Id, v3Id};
				newf2.edges = {vertToEdgeMap.at({v4Id, v2Id}),vertToEdgeMap.at({v2Id, v3Id}),vertToEdgeMap.at({v3Id, v4Id})};
			}

			MGN_CORE_ASSERT(newf1.vertices[1] != newf2.vertices[1], "We used the same vertice for the two triangles, there is a problem somewhere.");

			f1 = newf1;
			f2 = newf2;
		}

		m_VertToEdge = vertToEdgeMap;
		m_Vertices = newVertices;
		m_Edges = newEdges;
		m_Faces = newFaces;
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
