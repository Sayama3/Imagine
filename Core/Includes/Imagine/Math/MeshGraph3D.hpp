//
// Created by Iannis on 24/06/2025.
//

#pragma once

#include "Imagine/Rendering/CPU/CPUMesh.hpp"
#include "Imagine/Core/BufferView.hpp"
#include "Imagine/Core/SparseSet.hpp"
#include "Imagine/Core/UUID.hpp"
#include "Imagine/Rendering/MeshParameters.hpp"
#include "Types.hpp"

#include <algorithm>
#include <utility>
#include <vector>

#define MGN_MAKE_PAIR(a, b) ((a < b) ? {a, b} : {b, a})
#define MGN_MAKE_PAIR_TYPE(a, b, type) ((a < b) ? type{a, b} : type{b, a})

namespace Imagine::Math {
	// template<typename T = Real, glm::qualifier Q = glm::defaultp>
	class MeshGraph3D {
	public:
		using T = Real;
		static inline constexpr glm::qualifier Q = glm::defaultp;
		static inline constexpr glm::length_t L = 3;
		// type definitions
		using vec = glm::vec<L, T, Q>;
		using IdType = Imagine::Core::UUID;

	public:
		// struct definition
		struct VertexID {
			VertexID() = default;
			~VertexID() = default;
			inline VertexID(const IdType _id) :
				id(_id) {}
			IdType id{};
			inline std::string string() const {return id.string();}

			inline bool operator==(const VertexID &o) const { return id == o.id; }
			inline bool operator!=(const VertexID &o) const { return !(*this == o); }
			inline auto operator<=>(const VertexID &o) const { return id <=> o.id; }
		};
		struct EdgeID {
			EdgeID() = default;
			~EdgeID() = default;
			inline EdgeID(const IdType _id) :
				id(_id) {}
			IdType id{};
			inline std::string string() const {return id.string();}

			inline bool operator==(const EdgeID &o) const { return id == o.id; }
			inline bool operator!=(const EdgeID &o) const { return !(*this == o); }
			inline auto operator<=>(const EdgeID &o) const { return id <=> o.id; }
		};
		struct FaceID {
			FaceID() = default;
			~FaceID() = default;
			inline FaceID(const IdType _id) :
				id(_id) {}
			IdType id{};
			inline std::string string() const {return id.string();}

			inline bool operator==(const FaceID &o) const { return id == o.id; }
			inline bool operator!=(const FaceID &o) const { return !(*this == o); }
			inline std::strong_ordering operator<=>(const FaceID &o) const { return id <=> o.id; }
		};


		struct VertexIDPairHash {
		public:
			inline std::size_t operator()(const ReversiblePair<VertexID> &x) const {
				if (x.first < x.second)
					return std::hash<IdType>()(x.first.id) ^ std::hash<IdType>()(x.second.id);
				else
					return std::hash<IdType>()(x.second.id) ^ std::hash<IdType>()(x.first.id);
			}
		};
		struct EdgeIDPairHash {
		public:
			inline std::size_t operator()(const ReversiblePair<EdgeID> &x) const {
				if (x.first < x.second)
					return std::hash<IdType>()(x.first.id) ^ std::hash<IdType>()(x.second.id);
				else
					return std::hash<IdType>()(x.second.id) ^ std::hash<IdType>()(x.first.id);
			}
		};
		struct FaceIDPairHash {
		public:
			inline std::size_t operator()(const ReversiblePair<FaceID> &x) const {
				if (x.first < x.second)
					return std::hash<IdType>()(x.first.id) ^ std::hash<IdType>()(x.second.id);
				else
					return std::hash<IdType>()(x.second.id) ^ std::hash<IdType>()(x.first.id);
			}
		};
		struct VertexIDHash {
		public:
			inline std::size_t operator()(const VertexID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};
		struct EdgeIDHash {
		public:
			inline std::size_t operator()(const EdgeID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};
		struct FaceIDHash {
		public:
			inline std::size_t operator()(const FaceID &x) const {
				return std::hash<IdType>()(x.id);
			}
		};

		struct Vertex {
			Vertex() = default;
			~Vertex() = default;
			Vertex(const Vertex &) = default;
			Vertex &operator=(const Vertex &) = default;
			inline Vertex(const vec &p) :
				position(p), linkedEdges(), linkedFaces() {}

			inline void AddEdge(const EdgeID edge) {
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
			inline void AddFace(const FaceID face) {
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
			inline void RemoveEdge(const EdgeID edge) {
				bool found = false;
				auto it = std::find(linkedEdges.begin(), linkedEdges.end(), edge);
				if (it != linkedEdges.end()) {
					linkedEdges.erase(it);
				}
			}
			inline void RemoveFace(const FaceID face) {
				bool found = false;
				auto it = std::find(linkedFaces.begin(), linkedFaces.end(), face);
				if (it != linkedFaces.end()) {
					linkedFaces.erase(it);
				}
			}

			vec position{};
			std::vector<EdgeID> linkedEdges{};
			std::vector<FaceID> linkedFaces{};
		};
		struct Edge {
			Edge() = default;
			~Edge() = default;
			Edge(const Edge &) = default;
			Edge &operator=(const Edge &) = default;
			inline Edge(VertexID b, VertexID e) :
				begin(b), end(e) {}


			[[nodiscard]] inline VertexID GetOtherVertexID(VertexID id) const {
				if (begin == id) return end;
				// MGN_CORE_ASSERT(id == end, "The point id {} is not part of this edge.", id.string());
				return begin;
			}

			inline void AddFace(FaceID faceID) {
				if (!linkedFaces[0].has_value()) {
					linkedFaces[0] = faceID;
				}
				else {
					// MGN_CORE_ASSERT(!linkedFaces[1].has_value(), "The two possible triangle of the edge are filled.");
					linkedFaces[1] = faceID;
				}
			}

			[[nodiscard]] inline FaceID* GetFaceID(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() == faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() == faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}
			[[nodiscard]] inline FaceID* GetOtherFaceID(FaceID faceId) {
				if (linkedFaces[0].has_value() && linkedFaces[0].value() != faceId) {return &(linkedFaces[0].value());}
				if (linkedFaces[1].has_value() && linkedFaces[1].value() != faceId) {return &(linkedFaces[1].value());}
				return nullptr;
			}

			[[nodiscard]] inline bool IsBegin(VertexID id) const {
				return id == begin;
			}

			[[nodiscard]] inline bool HasTwoTriangle() const {
				return linkedFaces[0].has_value() && linkedFaces[1].has_value();
			}

			[[nodiscard]] inline ReversiblePair<VertexID> GetPair() { return MGN_MAKE_PAIR_TYPE(begin, end, ReversiblePair<VertexID>); }

			VertexID begin;
			VertexID end;
			std::array<std::optional<FaceID>, 2> linkedFaces{std::nullopt, std::nullopt};
		};
		struct Face {
			inline static constexpr uint64_t VertexCount = 3;

			Face() = default;
			~Face() = default;
			Face(const Face &) = default;
			Face &operator=(const Face &) = default;
			inline Face(std::array<VertexID, VertexCount> v) :
				vertices(v) {}
			inline Face(std::array<EdgeID, VertexCount> e) :
				edges(e) {}
			inline Face(std::array<VertexID, VertexCount> v, std::array<EdgeID, VertexCount> e) :
				vertices(v), edges(e) {}

			inline std::optional<VertexID> GetFollowing(const VertexID v) const {
				for (uint64_t i = 0; i < VertexCount; ++i) {
					if (vertices[i] == v) return vertices[(i+1)%VertexCount];
				}
				return std::nullopt;
			}
			inline VertexID GetOther(const VertexID one, const VertexID two) const {
				auto it = std::find_if(vertices.begin(), vertices.end(), [one, two](auto v) {
					return v != one && v != two;
				});
				return *it;
			}

			std::array<VertexID, VertexCount> vertices;
			std::array<EdgeID, VertexCount> edges;
		};

	public:
		using VerticesToEdgeMap = std::unordered_map<ReversiblePair<VertexID>, EdgeID, VertexIDPairHash>;
		using VertexSparseSet = std::unordered_map<VertexID, Vertex, VertexIDHash>;
		using EdgeSparseSet = std::unordered_map<EdgeID, Edge, EdgeIDHash>;
		using FaceSparseSet = std::unordered_map<FaceID, Face, FaceIDHash>;

	public:
		struct VertexIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Vertex;
			using difference_type = std::ptrdiff_t;
			using pointer = Vertex *;
			using reference = Vertex &;

		public:
			inline explicit VertexIterator(MeshGraph3D *graph, VertexID vertex) :
				graph(graph), iterator(graph->m_Vertices.find(vertex)) {}
			inline explicit VertexIterator(MeshGraph3D *graph, const VertexSparseSet::iterator iterator) : graph(graph), iterator(std::move(iterator)) {}

			inline VertexIterator operator++(int) {
				VertexIterator res{*this};
				++(*this);
				return res;
			}
			inline VertexIterator &operator++() {
				++iterator;
				return *this;
			}
			inline reference operator*() { return GetVertex(); }
			inline pointer operator->() { return &GetVertex(); }

			inline bool operator==(const VertexIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			inline bool operator!=(const VertexIterator &o) const { return !(*this == o); }

		public:
			inline Vertex &GetVertex() { return iterator->second; }
			inline VertexID GetID() { return VertexID{iterator->first}; }
			// IdType GetIndex() { return iterator->first; }

			[[nodiscard]] inline uint64_t GetEdgeCount() const {
				return iterator->second.linkedEdges.size();
			}

			inline typename std::vector<EdgeID>::iterator GetEdgeBegin() { return iterator->second.linkedEdges.begin(); }
			inline typename std::vector<EdgeID>::iterator GetEdgeEnd() { return iterator->second.linkedEdges.end(); }

		private:
			MeshGraph3D *graph{nullptr};
			typename VertexSparseSet::iterator iterator;
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
			EdgeIterator(const EdgeIterator&) = default;
			EdgeIterator& operator=(const EdgeIterator&) = default;
			inline explicit EdgeIterator(MeshGraph3D *graph, EdgeID edge) : graph(graph), iterator(graph->m_Edges.find(edge.id)) {}
			inline explicit EdgeIterator(MeshGraph3D *graph, const EdgeSparseSet::iterator iterator) : graph(graph), iterator(std::move(iterator)) {}

		public:
			inline EdgeIterator operator++(int) {
				EdgeIterator res{*this};
				++(*this);
				return res;
			}
			inline EdgeIterator &operator++() {
				++iterator;
				return *this;
			}

			inline bool operator==(const EdgeIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			inline bool operator!=(const EdgeIterator &o) const { return !(*this == o); }

		public:
			inline reference operator*() { return GetEdge(); }
			inline pointer operator->() { return &GetEdge(); }

		public:
			inline Edge &GetEdge() { return iterator->second; }
			inline EdgeID GetID() { return EdgeID{iterator->first}; }

			inline VertexID GetBeginID() { return GetEdge().begin; }
			inline VertexID GetEndID() { return GetEdge().end; }

			inline Vertex &GetBegin() { return graph->m_Vertices.at(GetBeginID()); }
			inline Vertex &GetEnd() { return graph->m_Vertices.at(GetEndID()); }

			inline std::optional<FaceID> GetSideFaceID() { return GetEdge().linkedFaces[0]; }
			inline std::optional<FaceID> GetOtherSideFaceID() { return GetEdge().linkedFaces[1]; }

			inline Face *GetSideFace() {
				const std::optional<FaceID> id = GetSideFaceID();
				if (id.has_value() && graph->m_Faces.contains(id.value()))
					return &graph->m_Faces.at(id.value());
				else
					return nullptr;
			}
			inline Face *GetOtherSideFace() {
				const std::optional<FaceID> id = GetOtherSideFaceID();
				if (id.has_value() && graph->m_Faces.contains(id.value()))
					return &graph->m_Faces.at(id.value());
				else
					return nullptr;
			}

			inline Line<L, T, Q> GetLine() { return Line<L, T, Q>{GetBegin().position, GetEnd().position}; }

		private:
			MeshGraph3D *graph{nullptr};
			typename EdgeSparseSet::iterator iterator;
		};

		struct FaceIterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using value_type = Face;
			using difference_type = std::ptrdiff_t;
			using pointer = Face *;
			using reference = Face &;

		public:
			inline explicit FaceIterator(MeshGraph3D *graph, FaceID face) :
				graph(graph), iterator(graph->m_Faces.find(face.id)) {}
			inline explicit FaceIterator(MeshGraph3D *graph, const FaceSparseSet::iterator iterator) : graph(graph), iterator(std::move(iterator)) {}


		public:
			inline FaceIterator operator++(int) {
				FaceIterator res{*this};
				++(*this);
				return res;
			}
			inline FaceIterator &operator++() {
				++iterator;
				return *this;
			}

			inline bool operator==(const FaceIterator &o) const { return graph == o.graph && iterator == o.iterator; }
			inline bool operator!=(const FaceIterator &o) const { return !(*this == o); }

			inline reference operator*() { return GetFace(); }
			inline pointer operator->() { return &GetFace(); }

		public:
			inline Face &GetFace() { return iterator->second; }
			inline FaceID GetID() { return FaceID{iterator->first}; }

			inline constexpr uint64_t GetEdgeCount() const { return Face::VertexCount; }
			inline constexpr uint64_t GetVertexCount() const { return Face::VertexCount; }

			inline VertexID &GetVertexID(const uint64_t index) { return iterator->second.vertices[index]; }
			inline EdgeID &GetEdgeID(const uint64_t index) { return iterator->second.edges[index]; }

			inline std::array<VertexID, Face::VertexCount>::iterator GetVertexIDBegin() { return iterator->second.vertices.begin(); }
			inline std::array<VertexID, Face::VertexCount>::iterator GetVertexIDEnd() { return iterator->second.vertices.end(); }

			inline std::array<EdgeID, Face::VertexCount>::iterator GetEdgeIDBegin() { return iterator->second.edges.begin(); }
			inline std::array<EdgeID, Face::VertexCount>::iterator GetEdgeIDEnd() { return iterator->second.edges.end(); }

			inline Vertex &GetVertex(const uint64_t index) { return graph->m_Vertices.at(iterator->second.vertices[index]); }
			inline Edge &GetEdge(const uint64_t index) { return graph->m_Edges.at(iterator->second.edges[index]); }

		private:
			MeshGraph3D *graph{nullptr};
			typename FaceSparseSet::iterator iterator;
		};

		inline VertexIterator BeginVertex() { return VertexIterator(this, m_Vertices.begin()); }
		inline VertexIterator EndVertex() { return VertexIterator(this, m_Vertices.end()); }

		inline EdgeIterator BeginEdge() { return EdgeIterator(this, m_Edges.begin()); }
		inline EdgeIterator EndEdge() { return EdgeIterator(this, m_Edges.end()); }

		inline FaceIterator BeginFace() { return FaceIterator(this, m_Faces.begin()); }
		inline FaceIterator EndFace() { return FaceIterator(this, m_Faces.end()); }

	public:
		MeshGraph3D() {
			m_VertToEdge.reserve(100);
			m_Vertices.reserve(100);
			m_Edges.reserve(100);
			m_Faces.reserve(100);
		}
		~MeshGraph3D() {
			m_VertToEdge.clear();
			m_Vertices.clear();
			m_Edges.clear();
			m_Faces.clear();
		}
	public:
		inline void Clear() {
			m_VertToEdge.clear();
			m_Vertices.clear();
			m_Edges.clear();
			m_Faces.clear();
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
		template<bool UseRandomColor = false>
		Core::CPUMesh GetHardCPUMesh();

	private:
		static void AddTriangle(VerticesToEdgeMap &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3);
	private:
		VerticesToEdgeMap m_VertToEdge{};
		VertexSparseSet m_Vertices{};
		EdgeSparseSet m_Edges{};
		FaceSparseSet m_Faces{};
	};
} // namespace Imagine::Math

namespace Imagine::Math {
	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::AddFace(vec a, vec b, vec c) {

		const Vertex array[3] = {Vertex{a}, Vertex{b}, Vertex{c}};

		constexpr uint32_t indices[3] = {0, 1, 2};

		AddMesh({array, 3 * sizeof(Vertex)}, {indices, 3 * sizeof(uint32_t)});
	}

	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::AddMesh(const Core::CPUMesh &mesh) {
		Core::ConstBufferView vertices = Core::ConstBufferView::Make(mesh.Vertices);
		Core::ConstBufferView indices = Core::ConstBufferView::Make(mesh.Indices);
		AddMesh(vertices, indices);
	}

	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::AddMesh(Core::ConstBufferView vertices, Core::ConstBufferView indices) {
		std::vector<VertexID> m_IDMap;

		{
			std::unordered_map<vec, VertexID> m_IDs;
			m_IDMap.resize(vertices.Count<Imagine::Core::Vertex>());
			const Imagine::Core::Vertex *array = vertices.Get<Imagine::Core::Vertex>();
			for (uint64_t i = 0; i < vertices.Count<Imagine::Core::Vertex>(); ++i) {
				const vec position = array[i].position;
				if (m_IDs.contains(position)) {
					const auto id = m_IDs.at(position);
					m_IDMap[i] = VertexID{id};
				}
				else {
					Vertex newVertex{};
					newVertex.position = position;
					const VertexID id = VertexID{IdType{}};
					m_Vertices.try_emplace(id, newVertex);
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
	// template<typename T, glm::qualifier Q>
	inline bool MeshGraph3D::EnsureLink() {
		return EnsureLink(m_Vertices, m_Edges);
	}

	// template<typename T, glm::qualifier Q>
	inline bool MeshGraph3D::EnsureLink(VertexSparseSet &newVertices, EdgeSparseSet &newEdges) {
		bool valid = true;
		for (auto it = newVertices.begin(); it != newVertices.end(); ++it) {
			const VertexID id = it->first;
			const Vertex &v = it->second;
			for (int i = 0; i < v.linkedEdges.size(); ++i) {
				EdgeID eId = v.linkedEdges[i];
				Edge &edge = newEdges.at(eId);
				if (!(edge.begin == id || edge.end == id)) {
					// MGN_CORE_ERROR("The Edge id {} has an Edge {}", id.string(), eId.string());
					valid = false;
				}
			}
		}
		return valid;
	}


	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::AddTriangle(VerticesToEdgeMap &edgeIds, VertexSparseSet &newVertices, EdgeSparseSet &newEdges, FaceSparseSet &newFaces, VertexID vertId1, VertexID vertId2, VertexID vertId3) {
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
				edge1 = newEdges.at(edge1Id);
				// MGN_CORE_ASSERT(edge1.begin == vertId1 || edge1.end == vertId1 && edge1.begin == vertId2 || edge1.end == vertId2, "The Edge is not the correct one.");
			}
			else {
				edge1Id = EdgeID{};
				newEdges.emplace(edge1Id, edge1);
				edgeIds[MGN_MAKE_PAIR_TYPE(edge1.begin, edge1.end, ReversiblePair<VertexID>)] = edge1Id;
			}

			if (edgeIds.contains(MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>))) {
				edge2Id = edgeIds.at(MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>));
				edge2 = newEdges.at(edge2Id);
				// MGN_CORE_ASSERT(edge2.begin == vertId3 || edge2.end == vertId3 && edge2.begin == vertId2 || edge2.end == vertId2, "The Edge is not the correct one.");
			}
			else {
				edge2Id = EdgeID{};
				newEdges.emplace(edge2Id, edge2);
				edgeIds[MGN_MAKE_PAIR_TYPE(edge2.begin, edge2.end, ReversiblePair<VertexID>)] = edge2Id;
			}

			if (edgeIds.contains(MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>))) {
				edge3Id = edgeIds.at(MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>));
				edge3 = newEdges.at(edge3Id);
				// MGN_CORE_ASSERT(edge3.begin == vertId1 || edge3.end == vertId1 && edge3.begin == vertId3 || edge3.end == vertId3, "The Edge is not the correct one.");
			}
			else {
				edge3Id = EdgeID{};
				newEdges.emplace(edge3Id, edge3);
				edgeIds[MGN_MAKE_PAIR_TYPE(edge3.begin, edge3.end, ReversiblePair<VertexID>)] = edge3Id;
			}

			const Face face{
					{vertId1, vertId2, vertId3},
					{edge1Id, edge2Id, edge3Id},
			};

			faceID = FaceID{IdType{}};
			newFaces.emplace(faceID, face);
		}

		// Backpropagate the links
		{
			Vertex &vert1 = newVertices.at(vertId1.id);
			Vertex &vert2 = newVertices.at(vertId2.id);
			Vertex &vert3 = newVertices.at(vertId3.id);

			vert1.AddEdge(edge3Id);
			vert1.AddEdge(edge1Id);

			vert2.AddEdge(edge1Id);
			vert2.AddEdge(edge2Id);

			vert3.AddEdge(edge2Id);
			vert3.AddEdge(edge3Id);

			vert1.AddFace(faceID);
			vert2.AddFace(faceID);
			vert3.AddFace(faceID);

			Edge &edge1 = newEdges.at(edge1Id);
			edge1.AddFace(faceID);

			Edge &edge2 = newEdges.at(edge2Id);
			edge2.AddFace(faceID);

			Edge &edge3 = newEdges.at(edge3Id);
			edge3.AddFace(faceID);
		}
	}

	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::SubdivideLoop() {
		struct EdgePoint {
			vec position;
			EdgeID edge;
			VertexID id;
		};

		std::unordered_map<ReversiblePair<VertexID>, EdgePoint, VertexIDPairHash> edgePoints;
		edgePoints.reserve(m_Edges.size());

		VertexSparseSet newVertices;
		newVertices.reserve(m_Vertices.size() + edgePoints.size());

		for (auto it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
			VertexID id = it->first;
			Vertex v{it->second.position};
			newVertices[id] = v;
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

				const Vertex &begin = m_Vertices.at(beginId.id);
				const Vertex &end = m_Vertices.at(endId.id);

				const Vertex &someVert = m_Vertices.at(some->GetOther(beginId, endId).id);
				const Vertex &otherVert = m_Vertices.at(other->GetOther(beginId, endId).id);

				constexpr T threeEight = (T(3.0) / T(8.0));
				constexpr T oneEight = (T(1.0) / T(8.0));
				newPoint = threeEight * (begin.position + end.position) + oneEight * (someVert.position + otherVert.position);
			}


			const Vertex newVertex{newPoint};
			const VertexID newVertId{IdType{}};
			newVertices[newVertId] = newVertex;
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
				Edge &edge = m_Edges.at(edgeId);
				const auto vertId = edge.GetOtherVertexID(vertIt.GetID());
				const auto &vert = m_Vertices.at(vertId);
				vertPrim += vert.position * alpha;
			}


			newVertices.at(vertIt.GetID().id).position = vertPrim;
		}

		VerticesToEdgeMap edgeIds;
		edgeIds.reserve(m_Edges.size() + edgePoints.size() * 3);
		EdgeSparseSet newEdges{static_cast<uint64_t>(m_Edges.size() + edgePoints.size() * 3)};
		FaceSparseSet newFaces{static_cast<uint64_t>(m_Faces.size() * 3)};


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

		m_VertToEdge.swap(edgeIds);
		m_Vertices.swap(newVertices);
		m_Edges.swap(newEdges);
		m_Faces.swap(newFaces);
	}
	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::SubdivideKobbelt() {

		// We initialize our kobbel with a fresh copy of our data structure but clearing all the linked types.
		//  Theses will be filled in back as we go. Only works as we used kinda set and check for no doubles.
		std::unordered_set<EdgeID, EdgeIDHash> originalEdges;
		VerticesToEdgeMap vertToEdgeMap = m_VertToEdge;
		VertexSparseSet newVertices = m_Vertices;
		EdgeSparseSet newEdges = m_Edges;
		FaceSparseSet newFaces;
		newFaces.reserve(m_Faces.size() * 3);

		const auto endFaces=EndFace();
		originalEdges.reserve(m_Edges.size());


		//Perturb the initial position in the initial vertex set and cleanup the vertex link.
		// Copy is not working so manual copy. To fix later.
		for (auto& [id, v] : newVertices) {
			v.linkedEdges.clear();
			v.linkedFaces.clear();
		}

		for (auto& [id, e] : newEdges) {
			e.linkedFaces[0] = std::nullopt;
			e.linkedFaces[1] = std::nullopt;
		}

		const auto endNewVert = m_Vertices.end();
		for (auto it = m_Vertices.begin(); it != endNewVert; ++it) {
			constexpr T oneNine = T(1) / T(9);

			const uint64_t neighCount = it->second.linkedEdges.size();
			const T tNeighCount = T(neighCount);
			const T alpha = oneNine * (4 - (2*Math::Cos(T(Math::Tau)/tNeighCount)));

			vec cumulatedNeigh{0};

			const T alphaOnN = alpha / tNeighCount;

			for (uint64_t i = 0; i < neighCount; ++i) {
				cumulatedNeigh += newVertices.at(m_Edges.at(it->second.linkedEdges[i]).GetOtherVertexID(it->first)).position * alphaOnN;
			}
			const vec new_pos = (1 - alpha) * newVertices.at(it->first).position + cumulatedNeigh;
			it->second.position = new_pos;
		}

		// Iterate on all triangles to create the new triangles. 1 to 3
		for (auto it = BeginFace(); it != endFaces; ++it) {
			const VertexID v1Id = it.GetVertexID(0);
			Vertex& v1 = newVertices.at(v1Id);


			const VertexID v2Id = it.GetVertexID(1);
			Vertex& v2 = newVertices.at(v2Id);


			const VertexID v3Id = it.GetVertexID(2);
			Vertex& v3 = newVertices.at(v3Id);

			VertexID v4Id;
			{
				constexpr T oneThird = T(1) / T(3);
				Vertex v4;
				v4.position = v1.position * oneThird + v2.position * oneThird + v3.position * oneThird;
				v4Id.id = IdType{};
				newVertices[v4Id] = v4;
			}
			Vertex& v4 = newVertices.at(v4Id);

			const EdgeID v1v2Id = it.GetEdgeID(0);
			Edge& v1v2 = newEdges[v1v2Id];
			originalEdges.emplace(v1v2Id);

			const EdgeID v2v3Id = it.GetEdgeID(1);
			Edge& v2v3 = newEdges[v2v3Id];
			originalEdges.emplace(v2v3Id);

			const EdgeID v3v1Id = it.GetEdgeID(2);
			Edge& v3v1 = newEdges[v3v1Id];
			originalEdges.emplace(v3v1Id);

			EdgeID v1v4Id;
			EdgeID v2v4Id;
			EdgeID v3v4Id;

			{
				Edge v1v4{v1Id, v4Id};
				v1v4Id.id = IdType{};
				newEdges[v1v4Id.id] = v1v4;
				vertToEdgeMap[{v1Id, v4Id}] = v1v4Id;
				Edge v2v4{v2Id, v4Id};
				v2v4Id.id = IdType{};
				newEdges[v2v4Id.id] = v2v4;
				vertToEdgeMap[{v2Id, v4Id}] = v2v4Id;
				Edge v3v4{v3Id, v4Id};
				v3v4Id.id = IdType{};
				newEdges[v3v4Id.id] = v3v4;
				vertToEdgeMap[{v3Id, v4Id}] = v3v4Id;
			}

			Edge& v1v4 = newEdges.at(v1v4Id);
			Edge& v2v4 = newEdges.at(v2v4Id);
			Edge& v3v4 = newEdges.at(v3v4Id);

			const Face f1 {
				{v1Id,v2Id,v4Id},
				{v1v2Id,v2v4Id,v1v4Id}
			};
			FaceID f1Id = IdType{};
			newFaces[f1Id] = f1;

			const Face f2 {
				{v2Id, v3Id, v4Id},
				{v2v3Id, v3v4Id, v2v4Id}
			};
			FaceID f2Id = IdType{};
			newFaces[f2Id] = f2;

			const Face f3 {
				{v3Id, v1Id, v4Id},
				{v3v1Id, v1v4Id, v3v4Id}
			};
			FaceID f3Id = IdType{};
			newFaces[f3Id] = f3;


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
			newVertices.at(it->first).position = it->second.position;
		}


		// We can now move onto the flipping.
		for (EdgeID edgeId : originalEdges) {
			Edge& edge = newEdges.at(edgeId.id);

			if (!edge.HasTwoTriangle()) continue;

			const VertexID v1Id = edge.begin;
			const VertexID v2Id = edge.end;


			const FaceID f1Id = edge.linkedFaces[0].value();
			const FaceID f2Id = edge.linkedFaces[1].value();

			Face& f1 = newFaces.at(f1Id);
			Face& f2 = newFaces.at(f2Id);

			const VertexID v3Id = f1.GetOther(edge.begin, edge.end);
			const VertexID v4Id = f2.GetOther(edge.begin, edge.end);


			// Change edge
			edge.begin = v3Id;
			edge.end = v4Id;

			// Swap the link to this edges
			vertToEdgeMap.erase({v1Id, v2Id});
			vertToEdgeMap[{v3Id, v4Id}] = edgeId;

			// Remove the edge from the vertice
			newVertices.at(v1Id).RemoveEdge(edgeId);
			newVertices.at(v2Id).RemoveEdge(edgeId);

			// Remove the face of the vertice before we know which is who
			newVertices.at(v1Id).RemoveFace(f1Id);
			newVertices.at(v2Id).RemoveFace(f2Id);

			// Add the new Edge to the vertex
			newVertices.at(v3Id).AddEdge(edgeId);
			newVertices.at(v4Id).AddEdge(edgeId);

			// Add the missing face to the Edges.
			newVertices.at(v3Id).AddFace(f2Id);
			newVertices.at(v4Id).AddFace(f1Id);

			Face newf1;
			if (f1.GetFollowing(v3Id) == v1Id) {
				// MGN_CORE_ASSERT(f1.GetFollowing(v1Id) == v2Id, "WTF is this triangle !");

				newf1.vertices = {v3Id, v1Id, v4Id};
				newf1.edges = {vertToEdgeMap.at({v3Id, v1Id}),vertToEdgeMap.at({v1Id, v4Id}),vertToEdgeMap.at({v4Id, v3Id})};
			} else {
				// MGN_CORE_ASSERT(f1.GetFollowing(v3Id) == v2Id, "WTF is this triangle !");
				// MGN_CORE_ASSERT(f1.GetFollowing(v2Id) == v1Id, "WTF is this triangle !");
				newf1.vertices = {v3Id, v2Id, v4Id};
				newf1.edges = {vertToEdgeMap.at({v3Id, v2Id}),vertToEdgeMap.at({v2Id, v4Id}),vertToEdgeMap.at({v4Id, v3Id})};
			}

			Face newf2;
			if (f2.GetFollowing(v4Id) == v1Id) {
				// MGN_CORE_ASSERT(f2.GetFollowing(v1Id) == v2Id, "WTF is this triangle !");

				newf2.vertices = {v4Id, v1Id, v3Id};
				newf2.edges = {vertToEdgeMap.at({v4Id, v1Id}),vertToEdgeMap.at({v1Id, v3Id}),vertToEdgeMap.at({v3Id, v4Id})};
			} else {
				// MGN_CORE_ASSERT(f2.GetFollowing(v4Id) == v2Id, "WTF is this triangle !");
				// MGN_CORE_ASSERT(f2.GetFollowing(v2Id) == v1Id, "WTF is this triangle !");
				newf2.vertices = {v4Id, v2Id, v3Id};
				newf2.edges = {vertToEdgeMap.at({v4Id, v2Id}),vertToEdgeMap.at({v2Id, v3Id}),vertToEdgeMap.at({v3Id, v4Id})};
			}

			// MGN_CORE_ASSERT(newf1.vertices[1] != newf2.vertices[1], "We used the same vertice for the two triangles, there is a problem somewhere.");

			f1 = newf1;
			f2 = newf2;
		}

		m_VertToEdge.swap(vertToEdgeMap);
		m_Vertices.swap(newVertices);
		m_Edges.swap(newEdges);
		m_Faces.swap(newFaces);
	}

	// template<typename T, glm::qualifier Q>
	inline void MeshGraph3D::SubdivideButterfly() {
		// MGN_CORE_ERROR("Function 'void MeshGraph3D::SubdivideButterfly(void);' not implemented.");
	}

	// template<typename T, glm::qualifier Q>
	inline Core::CPUMesh MeshGraph3D::GetSmoothCPUMesh() {
		Core::CPUMesh mesh;
		mesh.Vertices.resize(m_Vertices.size());
		std::unordered_map<VertexID, uint32_t, VertexIDHash> vertexMap{};
		vertexMap.reserve(m_Vertices.size());

		const auto vertBeg = BeginVertex();
		const auto vertEnd = EndVertex();
		for (auto vertIt = vertBeg; vertIt != vertEnd; ++vertIt) {
			const VertexID vertId = vertIt.GetID();
			const Vertex &vert = *vertIt;

			vec normal{0};

			if (true) {
				for (uint64_t i = 0; i < vert.linkedFaces.size(); ++i) {
					FaceID faceId = vert.linkedFaces[i];
					Face& face = m_Faces.at(faceId.id);
					const Vertex& one = m_Vertices.at(face.vertices[0]);
					const Vertex& two = m_Vertices.at(face.vertices[1]);
					const Vertex& three = m_Vertices.at(face.vertices[2]);
					normal += Math::CalculateTriangleNormal(one.position, two.position,three.position);
				}
				Math::NormalizeInPlace(normal);
			}

			Imagine::Core::Vertex vertex;
			vertex.position = vert.position;
			vertex.normal = normal;
			vertexMap[vertIt.GetID()] = static_cast<uint32_t>(mesh.Vertices.size());
			mesh.Vertices.push_back(vertex);
		}

		mesh.Indices.reserve(m_Faces.size() * ((Face::VertexCount - 3) * 3 + 3));
		for (auto it = m_Faces.cbegin(); it != m_Faces.cend(); ++it) {
			const auto first = vertexMap.at(it->second.vertices[0]);
			auto last = vertexMap.at(it->second.vertices[1]);
			for (uint64_t i = 2; i < Face::VertexCount; ++i) {
				auto newLast = vertexMap.at(it->second.vertices[i]);
				mesh.Indices.push_back(first);
				mesh.Indices.push_back(last);
				mesh.Indices.push_back(newLast);
				last = newLast;
			}
		}

		mesh.Lods.emplace_back(0u, static_cast<uint32_t>(mesh.Indices.size()));
		return mesh;
	}

	// template<typename T, glm::qualifier Q>
	template<bool UseRandomColor>
	inline Core::CPUMesh MeshGraph3D::GetHardCPUMesh() {
		Core::CPUMesh mesh;
		const auto triangleCount = m_Faces.size() * ((Face::VertexCount - 3) * 3 + 3);
		mesh.Vertices.resize(triangleCount*3);
		mesh.Indices.reserve(triangleCount);

		for (auto it = m_Faces.cbegin(); it != m_Faces.cend(); ++it) {
			const VertexID firstId = it->second.vertices[0];
			VertexID lastId = it->second.vertices[1];
			for (uint64_t i = 2; i < Face::VertexCount; ++i) {
				const VertexID newLastId = it->second.vertices[i];

				const Vertex& first = m_Vertices.at(firstId);
				Imagine::Core::Vertex vFirst{first.position};

				const Vertex& last = m_Vertices.at(lastId);
				Imagine::Core::Vertex vLast{last.position};

				const Vertex& newLast = m_Vertices.at(newLastId);
				Imagine::Core::Vertex vNewLast{newLast.position};

				glm::vec<4,T,Q> color{1,1,1,1};

				if constexpr (UseRandomColor) {
					color.r = (T)(rand()) / (T)(RAND_MAX);
					color.g = (T)(rand()) / (T)(RAND_MAX);
					color.b = (T)(rand()) / (T)(RAND_MAX);
				}

				vFirst.color = color;
				vLast.color = color;
				vNewLast.color = color;

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

		mesh.Lods.emplace_back(0, static_cast<uint32_t>(mesh.Indices.size()));
		return mesh;
	}

} // namespace Imagine::Math
