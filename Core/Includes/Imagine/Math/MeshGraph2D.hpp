//
// Created by ianpo on 31/01/2025.
//

#pragma once

#include "Basics.hpp"
#include <queue>

namespace Imagine::Math {


	class MeshGraph {
	public:
		using T = Real;
		using Vector2 = glm::vec<2, T>;
		using Vector3 = glm::vec<3, T>;

		struct Vertex {
			Vector2 Position;
		};

		struct Edge {
			uint32_t VertexA;
			uint32_t VertexB;
			std::optional<uint32_t> TriangleLeft{std::nullopt};
			std::optional<uint32_t> TriangleRight{std::nullopt};
		};

		struct Triangle {
			uint32_t EdgeAB;
			uint32_t EdgeBC;
			uint32_t EdgeCA;
		};

	public:
		MeshGraph() = default;

		~MeshGraph() = default;

		template<typename const_iter>
		MeshGraph(const_iter vec2Begin, const_iter vec2End, const bool optimize = true) {
			for (const_iter it = vec2Begin; it != vec2End; ++it) {
				if (optimize) {
					AddDelaunayPoint(*it);
				} else {
					AddPoint(*it);
				}
			}
		}

	public:
		void AddPoint(Vector2 point);
		void AddDelaunayPoint(Vector2 point);
		void DelaunayTriangulation();
		void RemoveDelaunayPoint(Vector2 point);
		void RemoveDelaunayPoint(uint32_t pointId);
	public:
		std::pair<std::unordered_map<uint32_t, MeshGraph::Vector2>, std::unordered_set<ReversiblePair<uint32_t, uint32_t>, ReversiblePairHash>> GetVoronoi();
	public:
		std::optional<uint32_t> GetClosestPoint(Vector2 point);
	public:
		void clear();

	private:
		void ReverseEdge(uint32_t edgeId);

		std::tuple<bool, uint32_t, uint32_t, uint32_t, uint32_t> RespectDelaunay(uint32_t edgeId);

		std::optional<std::tuple<uint32_t, uint32_t, uint32_t> > GetOrientedVerticesOfTriangle(uint32_t triangleId);

	private:
		[[nodiscard]] uint32_t GenerateVertexId() { return m_VertexIdGenerator++; };
		[[nodiscard]] uint32_t GenerateEdgeId() { return m_EdgeIdGenerator++; };
		[[nodiscard]] uint32_t GenerateTriangleId() { return m_TriangleIdGenerator++; };

	public:
		std::map<uint32_t, Vertex> m_Vertices;
		std::map<uint32_t, Edge> m_Edges;
		std::map<uint32_t, Triangle> m_Triangles;

	private:
		uint32_t m_VertexIdGenerator{0};
		uint32_t m_EdgeIdGenerator{0};
		uint32_t m_TriangleIdGenerator{0};
	};

	inline std::pair<std::unordered_map<uint32_t, MeshGraph::Vector2>, std::unordered_set<ReversiblePair<uint32_t, uint32_t>, ReversiblePairHash>> MeshGraph::GetVoronoi() {
		uint32_t localGenerator = m_TriangleIdGenerator;
		std::unordered_map<uint32_t, uint32_t> exteriorsPoints;
		std::unordered_map<uint32_t, Vector2> trianglePoints;
		std::unordered_set<ReversiblePair<uint32_t, uint32_t>, ReversiblePairHash> lines;

		for (const auto& [triangleId, triangle]: m_Triangles) {
			const auto abId = triangle.EdgeAB;
			const auto bcId = triangle.EdgeBC;
			const auto caId = triangle.EdgeCA;

			const auto& AB = m_Edges.at(abId);
			const auto& BC = m_Edges.at(bcId);
			const auto& CA = m_Edges.at(caId);

			const auto aId = AB.VertexA;
			const auto bId = AB.VertexB;
			const auto cId = BC.VertexA == aId || BC.VertexA == bId ? BC.VertexB : BC.VertexA;

			const auto& A = m_Vertices.at(aId);
			const auto& B = m_Vertices.at(bId);
			const auto& C = m_Vertices.at(cId);

			const auto barycenter = (A.Position + B.Position + C.Position) * static_cast<T>(1.0f/3.0f);
			const auto circle = Math::GetCircle(A.Position, B.Position, C.Position);
			trianglePoints[triangleId] = circle.Center;
			if (AB.TriangleLeft && AB.TriangleRight) {
				lines.insert({AB.TriangleLeft.value(), AB.TriangleRight.value()});
			} else {
				assert(AB.TriangleLeft || AB.TriangleRight);
				const uint32_t exteriorId = exteriorsPoints.contains(abId) ? exteriorsPoints.at(abId) : localGenerator++;
				if(!exteriorsPoints.contains(abId)) {
					exteriorsPoints[abId] = exteriorId;
					const Vector2 middle = (m_Vertices.at(AB.VertexA).Position + m_Vertices.at(AB.VertexB).Position) * static_cast<T>(0.5);
					const Vector2 middleToCircle = circle.Center - middle;
					const Vector2 middleToBarycenter = barycenter - middle;
					const bool mtcPointInside = Math::Dot(Math::Normalize(middleToBarycenter), Math::Normalize(middleToCircle)) >= 0;
					const Vector2 exterior = circle.Center + (mtcPointInside ? -middleToCircle * static_cast<T>(2) : middleToCircle * static_cast<T>(2));
					trianglePoints[exteriorId] = exterior;
				}
				const uint32_t otherTriangleId = AB.TriangleLeft ? AB.TriangleLeft.value() : AB.TriangleRight.value();
				lines.insert({otherTriangleId, exteriorId});
			}
			if (BC.TriangleLeft && BC.TriangleRight) {
				lines.insert({BC.TriangleLeft.value(), BC.TriangleRight.value()});
			} else {
				assert(BC.TriangleLeft || BC.TriangleRight);
				const uint32_t exteriorId = exteriorsPoints.contains(bcId) ? exteriorsPoints.at(bcId) : localGenerator++;
				if(!exteriorsPoints.contains(bcId)) {
					exteriorsPoints[bcId] = exteriorId;
					const Vector2 middle = (m_Vertices.at(BC.VertexA).Position + m_Vertices.at(BC.VertexB).Position) * static_cast<T>(0.5);
					const Vector2 middleToCircle = circle.Center - middle;
					const Vector2 middleToBarycenter = barycenter - middle;
					const bool mtcPointInside = Math::Dot(Math::Normalize(middleToBarycenter), Math::Normalize(middleToCircle)) >= 0;
					const Vector2 exterior = circle.Center + (mtcPointInside ? -middleToCircle * static_cast<T>(2) : middleToCircle * static_cast<T>(2));
					trianglePoints[exteriorId] = exterior;
				}
				const uint32_t otherTriangleId = BC.TriangleLeft ? BC.TriangleLeft.value() : BC.TriangleRight.value();
				lines.insert({otherTriangleId, exteriorId});
			}
			if (CA.TriangleLeft && CA.TriangleRight) {
				lines.insert({CA.TriangleLeft.value(), CA.TriangleRight.value()});
			} else {
				assert(CA.TriangleLeft || CA.TriangleRight);
				const uint32_t exteriorId = exteriorsPoints.contains(caId) ? exteriorsPoints.at(caId) : localGenerator++;
				if(!exteriorsPoints.contains(caId)) {
					exteriorsPoints[caId] = exteriorId;
					const Vector2 middle = (m_Vertices.at(CA.VertexA).Position + m_Vertices.at(CA.VertexB).Position) * static_cast<T>(0.5);
					const Vector2 middleToCircle = circle.Center - middle;
					const Vector2 middleToBarycenter = barycenter - middle;
					const bool mtcPointInside = Math::Dot(Math::Normalize(middleToBarycenter), Math::Normalize(middleToCircle)) >= 0;
					const Vector2 exterior = circle.Center + (mtcPointInside ? -middleToCircle * static_cast<T>(2) : middleToCircle * static_cast<T>(2));
					trianglePoints[exteriorId] = exterior;
				}
				const uint32_t otherTriangleId = CA.TriangleLeft ? CA.TriangleLeft.value() : CA.TriangleRight.value();
				lines.insert({otherTriangleId, exteriorId});
			}
		}

		return {trianglePoints, lines};
	}

	inline void MeshGraph::AddPoint(const Vector2 point) {
		auto it = std::find_if(m_Vertices.begin(), m_Vertices.end(), [point](std::pair<uint32_t, Vertex> vert) {
			return vert.second.Position == point;
		});
		if (it != m_Vertices.end()) return;


		const uint32_t newVertId = GenerateVertexId();
		m_Vertices[newVertId] = {point};

		if (m_Vertices.size() > 2) {
			// Check if outside
			std::unordered_set<uint32_t> compatibleVertices;
			compatibleVertices.reserve(m_Edges.size() - 1);

			std::vector<uint32_t> compatibleEdges{};
			compatibleEdges.reserve(m_Edges.size());

			// Check if inside
			for (auto [trId, ABC]: m_Triangles) {
				Edge &AB = m_Edges[ABC.EdgeAB];
				Edge &secondEdge = m_Edges[ABC.EdgeBC];
				Edge &thirdEdge = m_Edges[ABC.EdgeCA];

				const uint32_t aId = AB.VertexA;
				const uint32_t bId = AB.VertexB;
				const uint32_t cId = (secondEdge.VertexA != AB.VertexB) && (secondEdge.VertexA != AB.VertexA)
					                     ? secondEdge.VertexA
					                     : secondEdge.VertexB;

				const Vertex &A = m_Vertices[aId];
				const Vertex &B = m_Vertices[bId];
				const Vertex &C = m_Vertices[cId];

				if (Math::PointIsInsideTriangle(A.Position, B.Position, C.Position, point)) {
					// Remove existance of triangle.
					m_Triangles.erase(m_Triangles.find(trId));

					compatibleVertices.insert(aId);
					compatibleVertices.insert(bId);
					compatibleVertices.insert(cId);

					if (AB.TriangleLeft == trId) AB.TriangleLeft = std::nullopt;
					else AB.TriangleRight = std::nullopt;

					if (secondEdge.TriangleLeft == trId) secondEdge.TriangleLeft = std::nullopt;
					else secondEdge.TriangleRight = std::nullopt;

					if (thirdEdge.TriangleLeft == trId) thirdEdge.TriangleLeft = std::nullopt;
					else thirdEdge.TriangleRight = std::nullopt;

					compatibleEdges.push_back(ABC.EdgeAB);
					compatibleEdges.push_back(ABC.EdgeBC);
					compatibleEdges.push_back(ABC.EdgeCA);
					break;
				}
			}

			if (compatibleEdges.empty()) {
				for (auto &[ABId, edgeAB]: m_Edges) {
					if (edgeAB.TriangleLeft && edgeAB.TriangleRight) continue;

					const auto &vertA = m_Vertices.at(edgeAB.VertexA);
					const auto &vertB = m_Vertices.at(edgeAB.VertexB);

					const auto aToB = vertB.Position - vertA.Position;
					const auto aToC = point - vertA.Position;

					const bool isAligned = std::abs(Math::Dot(Math::Normalize(aToB), Math::Normalize(aToC))) >= 1 -
					                       REAL_EPSILON;
					if (isAligned) continue;

					const bool isLeft = Math::IsTriangleOriented(aToB, aToC);
					if (isLeft && edgeAB.TriangleLeft) continue;
					if (!isLeft && edgeAB.TriangleRight) continue;

					compatibleEdges.push_back(ABId);
					compatibleVertices.insert(edgeAB.VertexA);
					compatibleVertices.insert(edgeAB.VertexB);
				}
			}

			if (compatibleVertices.empty()) {
				T distance = 0;
				uint32_t closest = m_Vertices.begin()->first;
				for (const auto &[vId, vert]: m_Vertices) {
					if (vId == newVertId) continue;
					const auto d = Math::Magnitude(vert.Position - point);
					if (distance == 0 || d < distance) {
						closest = vId;
						distance = d;
					}
				}
				m_Edges[GenerateEdgeId()] = {closest, newVertId};
			} else {
				std::unordered_map<uint32_t, uint32_t> verticeToPromotedVertices;
				verticeToPromotedVertices.reserve(compatibleVertices.size());

				for (const auto vertId: compatibleVertices) {
					const auto newEdgeId = GenerateEdgeId();
					verticeToPromotedVertices[vertId] = newEdgeId;
					m_Edges[newEdgeId] = {vertId, newVertId};
				}

				for (const auto ABId: compatibleEdges) {
					auto &AB = m_Edges[ABId];
					const auto BCId = verticeToPromotedVertices[AB.VertexB];
					const auto ACId = verticeToPromotedVertices[AB.VertexA];
					auto &BC = m_Edges[BCId];
					auto &AC = m_Edges[ACId];

					const auto &A = m_Vertices[AB.VertexA];
					const auto &B = m_Vertices[AB.VertexB];
					const auto &C = m_Vertices[newVertId];

					if (Math::IsTriangleOriented(A.Position, B.Position, C.Position)) {
						const auto ABCId = GenerateTriangleId();
						const auto ABC = Triangle{ABId, BCId, ACId};
						m_Triangles[ABCId] = ABC;

						AB.TriangleLeft = ABCId;
						BC.TriangleLeft = ABCId;
						AC.TriangleRight = ABCId;
					} else {
						const auto ABCId = GenerateTriangleId();
						const auto ABC = Triangle{ABId, ACId, BCId};
						m_Triangles[ABCId] = ABC;

						AB.TriangleRight = ABCId;
						BC.TriangleRight = ABCId;
						AC.TriangleLeft = ABCId;
					}
				}
			}
		} else if (m_Vertices.size() == 2) {
			uint32_t otherId = -1;
			for (const auto &[id,vert]: m_Vertices) {
				if (id != newVertId) {
					otherId = id;
					break;
				}
			}
			if (otherId == -1) return; // Too much safety but is okay.
			const uint32_t newEdge = GenerateEdgeId();
			m_Edges[newEdge] = {otherId, newVertId};
		}
	}

	inline void MeshGraph::AddDelaunayPoint(const Vector2 point) {
		const uint32_t newVertId = GenerateVertexId();
		m_Vertices[newVertId] = {point};
		if (m_Vertices.size() > 2 && !m_Triangles.empty()) {
			std::vector<uint32_t> edgeToTriangulate;
			std::unordered_map<ReversiblePair<uint32_t, uint32_t>, uint32_t, ReversiblePairHash> vertexPairToEdge;
			edgeToTriangulate.reserve(1024);
			vertexPairToEdge.reserve(1024);

			// Handle Point is inside a triangle
			for (auto [trId, triangle]: m_Triangles) {
				Edge &AB = m_Edges[triangle.EdgeAB];
				Edge &secondEdge = m_Edges[triangle.EdgeBC];
				Edge &thirdEdge = m_Edges[triangle.EdgeCA];

				const uint32_t aId = AB.VertexA;
				const uint32_t bId = AB.VertexB;
				const uint32_t cId = (secondEdge.VertexA != AB.VertexB) && (secondEdge.VertexA != AB.VertexA)
					                     ? secondEdge.VertexA
					                     : secondEdge.VertexB;

				const Vertex &A = m_Vertices[aId];
				const Vertex &B = m_Vertices[bId];
				const Vertex &C = m_Vertices[cId];


				if (Math::PointIsInsideTriangle(A.Position, B.Position, C.Position, point)) {
					// Remove existence of triangle.
					m_Triangles.erase(m_Triangles.find(trId));

					edgeToTriangulate.push_back(triangle.EdgeAB);
					vertexPairToEdge[ReversiblePair{aId, bId}] = triangle.EdgeAB;
					edgeToTriangulate.push_back(triangle.EdgeBC);
					vertexPairToEdge[ReversiblePair{bId, cId}] = triangle.EdgeBC;
					edgeToTriangulate.push_back(triangle.EdgeCA);
					vertexPairToEdge[ReversiblePair{cId, aId}] = triangle.EdgeCA;

					if (AB.TriangleLeft == trId) AB.TriangleLeft = std::nullopt;
					else AB.TriangleRight = std::nullopt;

					if (secondEdge.TriangleLeft == trId) secondEdge.TriangleLeft = std::nullopt;
					else secondEdge.TriangleRight = std::nullopt;

					if (thirdEdge.TriangleLeft == trId) thirdEdge.TriangleLeft = std::nullopt;
					else thirdEdge.TriangleRight = std::nullopt;
					break;
				}
			}

			// Handle Point is outside triangles
			if (edgeToTriangulate.empty()) {
				for (auto &[edgeId, edge]: m_Edges) {
					if (edge.TriangleLeft && edge.TriangleRight) continue;

					const auto &A = m_Vertices.at(edge.VertexA);
					const auto &B = m_Vertices.at(edge.VertexB);

					const auto ABCIsOriented = Math::IsTriangleOriented(A.Position, B.Position, point);
					if (!edge.TriangleLeft && ABCIsOriented) {
						edgeToTriangulate.push_back(edgeId);
						vertexPairToEdge[ReversiblePair{edge.VertexA, edge.VertexB}] = edgeId;
					} else if (!edge.TriangleRight && !ABCIsOriented) {
						edgeToTriangulate.push_back(edgeId);
						vertexPairToEdge[ReversiblePair{edge.VertexA, edge.VertexB}] = edgeId;
					}
				}
			}

			while (!edgeToTriangulate.empty()) {
				const uint32_t edgeId = edgeToTriangulate.back();
				edgeToTriangulate.pop_back();
				Edge &edge = m_Edges.at(edgeId);

				const uint32_t aId = edge.VertexA;
				const uint32_t bId = edge.VertexB;
				const auto &[APos] = m_Vertices.at(aId);
				const auto &[BPos] = m_Vertices.at(bId);

				const bool isOriented = Math::IsTriangleOriented(APos, BPos, point);
				const std::optional<uint32_t> triangleToCheck = isOriented ? edge.TriangleRight : edge.TriangleLeft;

				bool edgeIsValid = true;
				if (triangleToCheck) {
					const auto triangleId = triangleToCheck.value();
					const Triangle &ABC = m_Triangles.at(triangleId);
					const uint32_t abId = ABC.EdgeAB;
					const uint32_t bcId = ABC.EdgeBC;
					const uint32_t caId = ABC.EdgeCA;
					const bool edgeIsAB = edgeId == abId;
					const bool edgeIsBC = edgeId == bcId;
					const bool edgeIsCA = edgeId == caId;

					uint32_t cId; {
						const Edge &AB = m_Edges.at(abId);
						const Edge &BC = m_Edges.at(bcId);
						if (edgeIsAB) {
							cId = BC.VertexA == aId || BC.VertexA == bId ? BC.VertexB : BC.VertexA;
						} else {
							cId = AB.VertexA == aId || AB.VertexA == bId ? AB.VertexB : AB.VertexA;
						}
					}
					const auto &[CPos] = m_Vertices.at(cId);
					const Circle circle = Math::GetCircle(APos, BPos, CPos);
					edgeIsValid = !Math::IsPointInsideCircle(circle, point);
					if (!edgeIsValid) {
						{
							auto &firstEdge = m_Edges.at(ABC.EdgeAB);
							if (firstEdge.TriangleLeft == triangleId) firstEdge.TriangleLeft = std::nullopt;
							else firstEdge.TriangleRight = std::nullopt;
						} {
							auto &secondEdge = m_Edges.at(ABC.EdgeBC);
							if (secondEdge.TriangleLeft == triangleId) secondEdge.TriangleLeft = std::nullopt;
							else secondEdge.TriangleRight = std::nullopt;
						} {
							auto &thirdEdge = m_Edges.at(ABC.EdgeCA);
							if (thirdEdge.TriangleLeft == triangleId) thirdEdge.TriangleLeft = std::nullopt;
							else thirdEdge.TriangleRight = std::nullopt;
						}
						m_Edges.erase(m_Edges.find(edgeId));
						m_Triangles.erase(m_Triangles.find(triangleId));
						if (!edgeIsAB) edgeToTriangulate.push_back(abId);
						if (!edgeIsBC) edgeToTriangulate.push_back(bcId);
						if (!edgeIsCA) edgeToTriangulate.push_back(caId);
					}
				}

				if (edgeIsValid) {
					uint32_t edgeBCId;
					if (vertexPairToEdge.contains({bId, newVertId})) {
						edgeBCId = vertexPairToEdge.at({bId, newVertId});
					} else {
						const uint32_t bcId = GenerateEdgeId();
						m_Edges[bcId] = {bId, newVertId};
						edgeBCId = (vertexPairToEdge[{bId, newVertId}] = bcId);
					}

					uint32_t edgeACId;
					if (vertexPairToEdge.contains({aId, newVertId})) {
						edgeACId = vertexPairToEdge.at({aId, newVertId});
					} else {
						const uint32_t acId = GenerateEdgeId();
						m_Edges[acId] = {aId, newVertId};
						edgeACId = (vertexPairToEdge[{aId, newVertId}] = acId);
					}
					Edge &edgeAC = m_Edges[edgeACId];
					Edge &edgeBC = m_Edges[edgeBCId];

					uint32_t trId = GenerateTriangleId();
					if (isOriented) {
						m_Triangles[trId] = {edgeId, edgeBCId, edgeACId};
						edge.TriangleLeft = trId;

						if (edgeBC.VertexA == bId) edgeBC.TriangleLeft = trId;
						else edgeBC.TriangleRight = trId;

						if (edgeAC.VertexA == aId) edgeAC.TriangleRight = trId;
						else edgeAC.TriangleLeft = trId;
					} else {
						m_Triangles[trId] = {edgeId, edgeACId, edgeBCId};
						edge.TriangleRight = trId;

						if (edgeBC.VertexA == bId) edgeBC.TriangleRight = trId;
						else edgeBC.TriangleLeft = trId;

						if (edgeAC.VertexA == aId) edgeAC.TriangleLeft = trId;
						else edgeAC.TriangleRight = trId;
					}
				}
			}
		} else if (m_Vertices.size() > 2 && m_Triangles.empty()) {

			bool createAllTheTriangles = false;
			for (auto it = m_Vertices.begin(); it != m_Vertices.end(); ++it) {
				if (it->first == newVertId) continue;
				const auto a = it->second.Position;
				auto oit = it;
				++oit;
				for (; oit != m_Vertices.end(); ++oit) {
					if (oit->first == newVertId) continue;
					const auto b = oit->second.Position;

					if (std::abs(Math::Dot(Math::Normalize(b - a), Math::Normalize(point - a))) < 1-std::numeric_limits<T>::epsilon()) {
						createAllTheTriangles = true;
						break;
					}
				}
				if (createAllTheTriangles) break;
			}

			if (createAllTheTriangles) {

				std::unordered_map<ReversiblePair<uint32_t, uint32_t>, uint32_t, ReversiblePairHash> VertexPairToEdge;
				std::vector<std::pair<Vector2, uint32_t>> pointOrdered;
				pointOrdered.reserve(m_Vertices.size() - 1);

				for (auto[id, vert] : m_Vertices) {
					if (id == newVertId) continue;
					pointOrdered.push_back({vert.Position, id});
				}

				std::sort(pointOrdered.begin(), pointOrdered.end(), [](std::pair<Vector2, uint32_t> a, std::pair<Vector2, uint32_t> b) {
					if (a.first.x < b.first.x) {
						return true;
					} else if (a.first.x == b.first.x) {
						return a.first.x <= b.first.y;
					} else /* if (a.first.x > b.first.x)*/ {
						return false;
					}
				});

				for (uint32_t i = 1; i < pointOrdered.size(); ++i) {
					m_Edges.insert({GenerateEdgeId(), {pointOrdered[i-1].second, pointOrdered[i].second}});
				}

				for (auto& [abId, AB]: m_Edges) {
					if (AB.VertexA == newVertId || AB.VertexB == newVertId) continue;
					auto aId = AB.VertexA;
					auto a = m_Vertices.at(aId);
					auto bId = AB.VertexB;
					auto b = m_Vertices.at(bId);

					uint32_t bcId;
					if(VertexPairToEdge.contains({bId, newVertId})) {
						bcId = VertexPairToEdge.at({bId, newVertId});
					} else {
						bcId = GenerateEdgeId();
						m_Edges[bcId] = {bId, newVertId};
						VertexPairToEdge[{bId, newVertId}] = bcId;

					}

					uint32_t acId;
					if(VertexPairToEdge.contains({aId, newVertId})) {
						acId = VertexPairToEdge.at({aId, newVertId});
					} else {
						acId = GenerateEdgeId();
						m_Edges[acId] = {aId, newVertId};
						VertexPairToEdge[{aId, newVertId}] = acId;
					}

					auto& BC = m_Edges[bcId];
					auto& AC = m_Edges[acId];


					if (Math::IsTriangleOriented(a.Position, b.Position, point)) {
						const uint32_t trId = GenerateTriangleId();
						m_Triangles[trId] = {abId, bcId, acId};
						AB.TriangleLeft = trId;
						BC.TriangleLeft = trId;
						AC.TriangleRight = trId;
					} else {
						const uint32_t trId = GenerateTriangleId();
						m_Triangles[trId] = {abId, acId, bcId};
						AB.TriangleRight = trId;
						BC.TriangleRight = trId;
						AC.TriangleLeft = trId;
					}
				}
			}
		}
	}

	inline void MeshGraph::RemoveDelaunayPoint(const Vector2 point) {
		const auto it = std::find_if(m_Vertices.cbegin(), m_Vertices.cend(), [point](const std::pair<uint32_t, Vertex>& vertices) {
			return vertices.second.Position == point;
		});
		if (it != m_Vertices.end()) {
			RemoveDelaunayPoint(it->first);
		}
	}

	inline void MeshGraph::RemoveDelaunayPoint(const uint32_t pointId) {
		if (!m_Vertices.contains(pointId)) return;
		if (m_Triangles.empty()) {
			assert(m_Edges.empty());
			m_Vertices.erase(m_Vertices.find(pointId));
		} else {
			std::set<uint32_t> edgeList2;
			std::unordered_map<uint32_t, std::unordered_set<uint32_t>> vertexToEdgeMap;

			{
				std::set<uint32_t> edgeList1;
				std::set<uint32_t> triangleList;

				for (const auto& [edgeId, edge] : m_Edges) {
					if (edge.VertexA != pointId && edge.VertexB != pointId) {
						continue;
					}

					edgeList1.insert(edgeId);
					if (edge.TriangleLeft) triangleList.insert(edge.TriangleLeft.value());
					if (edge.TriangleRight) triangleList.insert(edge.TriangleRight.value());
				}

				for (const auto triangleId : triangleList) {
					const auto& triangle = m_Triangles.at(triangleId);

					if(!edgeList1.contains(triangle.EdgeAB)) {
						Edge& AB = m_Edges.at(triangle.EdgeAB);
						if (AB.TriangleLeft && triangleList.contains(AB.TriangleLeft.value())) AB.TriangleLeft = std::nullopt;
						if (AB.TriangleRight && triangleList.contains(AB.TriangleRight.value())) AB.TriangleRight = std::nullopt;
						vertexToEdgeMap[AB.VertexA].insert(triangle.EdgeAB);
						vertexToEdgeMap[AB.VertexB].insert(triangle.EdgeAB);
						edgeList2.insert(triangle.EdgeAB);
					}
					if(!edgeList1.contains(triangle.EdgeBC)) {
						Edge& BC = m_Edges.at(triangle.EdgeBC);
						if (BC.TriangleLeft && triangleList.contains(BC.TriangleLeft.value())) BC.TriangleLeft = std::nullopt;
						if (BC.TriangleRight && triangleList.contains(BC.TriangleRight.value())) BC.TriangleRight = std::nullopt;
						vertexToEdgeMap[BC.VertexA].insert(triangle.EdgeBC);
						vertexToEdgeMap[BC.VertexB].insert(triangle.EdgeBC);
						edgeList2.insert(triangle.EdgeBC);
					}
					if(!edgeList1.contains(triangle.EdgeCA)) {
						Edge& CA = m_Edges.at(triangle.EdgeCA);
						if (CA.TriangleLeft && triangleList.contains(CA.TriangleLeft.value())) CA.TriangleLeft = std::nullopt;
						if (CA.TriangleRight && triangleList.contains(CA.TriangleRight.value())) CA.TriangleRight = std::nullopt;
						vertexToEdgeMap[CA.VertexA].insert(triangle.EdgeCA);
						vertexToEdgeMap[CA.VertexB].insert(triangle.EdgeCA);
						edgeList2.insert(triangle.EdgeCA);
					}
				}

				m_Vertices.erase(m_Vertices.find(pointId));
				for (const auto edgeId : edgeList1) {
					const auto it = m_Edges.find(edgeId);
					if (it != m_Edges.end()) m_Edges.erase(it);
				}

				for (const auto triangleId : triangleList) {
					const auto it = m_Triangles.find(triangleId);
					if (it != m_Triangles.end()) m_Triangles.erase(it);
				}
			}

			const bool polygonIsClosed = std::all_of(vertexToEdgeMap.begin(), vertexToEdgeMap.end(), [](const std::pair<uint32_t, std::unordered_set<uint32_t>>& vToEdge){return vToEdge.second.size() == 2;});

			if (!polygonIsClosed) {
				throw std::runtime_error("Case where the polygon is not closed is not handled");
			}

			while (edgeList2.size() > 3) {
				for (const uint32_t edgeId : edgeList2) {
					Edge& edge = m_Edges.at(edgeId);
					const uint32_t aId = edge.VertexA;
					const uint32_t bId = edge.VertexB;
					const Vertex& A = m_Vertices.at(edge.VertexA);
					const Vertex& B = m_Vertices.at(edge.VertexB);
					const std::array<uint32_t, 2> edgeVerticeIds{aId, bId};

					bool hasAddedTriangle = false;

					for (const auto verticeId : edgeVerticeIds) {
						if(vertexToEdgeMap.contains(verticeId)) {
							const auto& vertList = vertexToEdgeMap.at(verticeId);
							const std::unordered_set<uint32_t>::const_iterator it = std::find_if(vertList.begin(), vertList.end(), [edgeId](const uint32_t id){return id!=edgeId;});
							if (it == vertList.end()) continue;

							const uint32_t nextEdgeId = *it;
							Edge& nextEdge = m_Edges.at(nextEdgeId);

							const uint32_t cId = nextEdge.VertexA == edge.VertexA || nextEdge.VertexA == edge.VertexB ? nextEdge.VertexB : nextEdge.VertexA;
							const Vertex& C = m_Vertices.at(cId);

							const Circle ABC = Math::GetCircle(A.Position, B.Position, C.Position);

							const bool invalidTriangle = std::any_of(vertexToEdgeMap.begin(), vertexToEdgeMap.end(), [this, &ABC,aId,bId,cId](const std::pair<uint32_t, std::unordered_set<uint32_t>>& vertEdgesPair) {
								const uint32_t pId = vertEdgesPair.first;
								if (pId == aId) return false;
								if (pId == bId) return false;
								if (pId == cId) return false;

								const Vertex& p = m_Vertices.at(vertEdgesPair.first);
								return Math::IsPointInsideCircle(ABC, p.Position);
							});

							if (!invalidTriangle) {
								const bool isOriented = Math::IsTriangleOriented(A.Position,B.Position,C.Position);
								const bool isAttachedThroughA = aId == nextEdge.VertexA || aId == nextEdge.VertexB;

								const uint32_t newEdgeId = GenerateEdgeId();
								Edge newEdge = isAttachedThroughA ? Edge{bId, cId} : Edge{aId, cId};

								const uint32_t newTriangleId = GenerateTriangleId();
								Triangle newTriangle;
								if (isOriented) {
									edge.TriangleLeft = newTriangleId;
									if (isAttachedThroughA) {
										newEdge.TriangleLeft = newTriangleId; // BC
										newTriangle = {edgeId, newEdgeId, nextEdgeId}; //AB - BC - AC
									} else {
										newEdge.TriangleRight = newTriangleId; // AC;
										newTriangle = {edgeId, nextEdgeId, newEdgeId}; //AB - BC - AC
									}
								} else {
									edge.TriangleRight = newTriangleId;
									if (isAttachedThroughA) {
										newEdge.TriangleRight = newTriangleId; // BC
										newTriangle = {edgeId, nextEdgeId, newEdgeId}; //AB - AC - BC
									} else {
										newEdge.TriangleLeft = newTriangleId; // AC
										newTriangle = {edgeId, newEdgeId, nextEdgeId}; //AB - AC - BC
									}
								}

								{
									const auto nAId = nextEdge.VertexA;
									const auto nBId = nextEdge.VertexB;
									const auto nCId = edge.VertexA == nAId || edge.VertexA == nBId ? edge.VertexB : edge.VertexA;

									if (Math::IsTriangleOriented(m_Vertices.at(nAId).Position,m_Vertices.at(nBId).Position,m_Vertices.at(nCId).Position)) {
										nextEdge.TriangleLeft = newTriangleId;
									} else {
										nextEdge.TriangleRight = newTriangleId;
									}
								}

								m_Triangles[newTriangleId] = newTriangle;
								m_Edges[newEdgeId] = newEdge;

								edgeList2.erase(edgeList2.find(edgeId));
								edgeList2.erase(edgeList2.find(nextEdgeId));
								edgeList2.insert(newEdgeId);

								if (isAttachedThroughA) {
									vertexToEdgeMap.erase(vertexToEdgeMap.find(aId));
									auto& lstB = vertexToEdgeMap.at(bId);
									auto& lstC = vertexToEdgeMap.at(cId);
									lstB.erase(lstB.find(edgeId));
									lstC.erase(lstC.find(nextEdgeId));
									lstB.insert(newEdgeId);
									lstC.insert(newEdgeId);
								} else {
									vertexToEdgeMap.erase(vertexToEdgeMap.find(bId));
									auto& lstA = vertexToEdgeMap.at(aId);
									auto& lstC = vertexToEdgeMap.at(cId);
									lstA.erase(lstA.find(edgeId));
									lstC.erase(lstC.find(nextEdgeId));
									lstA.insert(newEdgeId);
									lstC.insert(newEdgeId);
								}

								hasAddedTriangle = true;
								break;
							}
						}
					}

					if (hasAddedTriangle) {
						break;
					}
				}
			}

			assert(edgeList2.size() == 3);
			auto it = edgeList2.cbegin();
			const uint32_t edge1Id = *(it++);
			const uint32_t edge2Id =  *(it++);
			const uint32_t edge3Id =  *(it++);

			Edge& edge1 = m_Edges.at(edge1Id);
			Edge& edge2 = m_Edges.at(edge2Id);
			Edge& edge3 = m_Edges.at(edge3Id);

			bool edge2IsLinkToEdge1ThroughA = edge1.VertexA == edge2.VertexA || edge1.VertexA == edge2.VertexB;
			const uint32_t aId = edge1.VertexA;
			const uint32_t bId = edge1.VertexB;

			const uint32_t triangleId = GenerateTriangleId();
			Triangle triangle;// = ABCIsOriented ? Triangle{edge1Id, edge2Id, edge3Id} : Triangle{edge1Id, edge3Id, edge2Id};

			{
				const bool edge2ConnectedToEdge1ThroughB = edge1.VertexB == edge2.VertexA || edge1.VertexB == edge2.VertexB;
				const uint32_t cId = edge2.VertexA == edge1.VertexA || edge2.VertexA == edge1.VertexB ? edge2.VertexB : edge2.VertexA;
				const bool ABCOriented = Math::IsTriangleOriented(m_Vertices.at(edge1.VertexA).Position,m_Vertices.at(edge1.VertexB).Position,m_Vertices.at(cId).Position);

				if (ABCOriented) {
					if (edge2ConnectedToEdge1ThroughB) {
						triangle = Triangle{edge1Id, edge2Id, edge3Id};
					} else {
						triangle = Triangle{edge1Id, edge3Id, edge2Id};
					}
				} else {
					if (edge2ConnectedToEdge1ThroughB) {
						triangle = Triangle{edge1Id, edge3Id, edge2Id};
					} else {
						triangle = Triangle{edge1Id, edge2Id, edge3Id};
					}
				}

				if (ABCOriented) {
					edge1.TriangleLeft = triangleId;
				} else {
					edge1.TriangleRight = triangleId;
				}
			}

			{
				const uint32_t cId = edge3.VertexA == edge2.VertexA || edge3.VertexA == edge2.VertexB ? edge3.VertexB : edge3.VertexA;
				const bool ABCOriented = Math::IsTriangleOriented(m_Vertices.at(edge2.VertexA).Position,m_Vertices.at(edge2.VertexB).Position,m_Vertices.at(cId).Position);

				if (ABCOriented) {
					edge2.TriangleLeft = triangleId;
				} else {
					edge2.TriangleRight = triangleId;
				}
			}

			{
				const uint32_t cId = edge1.VertexA == edge3.VertexA || edge1.VertexA == edge3.VertexB ? edge1.VertexB : edge1.VertexA;
				const bool ABCOriented = Math::IsTriangleOriented(m_Vertices.at(edge3.VertexA).Position,m_Vertices.at(edge3.VertexB).Position,m_Vertices.at(cId).Position);

				if (ABCOriented) {
					edge3.TriangleLeft = triangleId;
				} else {
					edge3.TriangleRight = triangleId;
				}
			}

			m_Triangles[triangleId] = triangle;
		}
	}

	inline std::optional<uint32_t> MeshGraph::GetClosestPoint(const Vector2 point) {
		std::optional<uint32_t> result{std::nullopt};
		T distance = std::numeric_limits<T>::max();
		for (const auto& [vertId, vert] : m_Vertices) {
			const auto dist = Math::Distance(point, vert.Position);
			if (Math::Distance(point, vert.Position) < distance) {
				result = vertId;
				distance = dist;
			}
		}
		return result;
	}

	inline std::tuple<bool, uint32_t, uint32_t, uint32_t, uint32_t> MeshGraph::RespectDelaunay(const uint32_t edgeId) {
		const auto &edge = m_Edges.at(edgeId);
		if (!edge.TriangleLeft || !edge.TriangleRight) return {true, -1, -1, -1, -1};

		const uint32_t s1Id = edge.VertexB;
		const uint32_t s2Id = edge.VertexA;
		const Vertex& s1 = m_Vertices.at(s1Id);
		const Vertex& s2 = m_Vertices.at(s2Id);

		const uint32_t t1Id = edge.TriangleLeft.value();
		const Triangle &t1 = m_Triangles.at(t1Id);
		uint32_t a1Id = -1;
		uint32_t a4Id = -1;
		if (t1.EdgeAB == edgeId) {
			a1Id = t1.EdgeBC;
			a4Id = t1.EdgeCA;
		} else if (t1.EdgeBC == edgeId) {
			a1Id = t1.EdgeCA;
			a4Id = t1.EdgeAB;
		} else /* t1.EdgeCA == edgeId */ {
			a1Id = t1.EdgeAB;
			a4Id = t1.EdgeBC;
		}

		const Edge &a1 = m_Edges.at(a1Id);
		const uint32_t s4Id = a1.VertexA == s1Id || a1.VertexA == s2Id ? a1.VertexB : a1.VertexA;
		const Vertex &s4 = m_Vertices.at(s4Id);
		const Circle t1Circle = Math::GetCircle(s1.Position, s4.Position, s2.Position);

		const uint32_t t2Id = edge.TriangleRight.value();
		const Triangle &t2 = m_Triangles.at(t2Id);
		uint32_t a2Id = -1;
		uint32_t a3Id = -1;
		if (t2.EdgeAB == edgeId) {
			a3Id = t2.EdgeBC;
			a2Id = t2.EdgeCA;
		} else if (t2.EdgeBC == edgeId) {
			a3Id = t2.EdgeCA;
			a2Id = t2.EdgeAB;
		} else /* t2.EdgeCA == edgeId */ {
			a3Id = t2.EdgeAB;
			a2Id = t2.EdgeBC;
		}
		const Edge &a2 = m_Edges.at(a2Id);
		const uint32_t s3Id = a2.VertexA == s1Id || a2.VertexA == s2Id ? a2.VertexB : a2.VertexA;
		const Vertex &s3 = m_Vertices.at(s3Id);
		const Circle t2Circle = Math::GetCircle(s1.Position, s2.Position, s3.Position);


		const bool shouldInvert = IsPointInsideCircle(t1Circle, s3.Position) || IsPointInsideCircle(
			                          t2Circle, s4.Position);
		return {!shouldInvert, a1Id, a2Id, a3Id, a4Id};
	}

	inline std::optional<std::tuple<uint32_t, uint32_t, uint32_t> > MeshGraph::GetOrientedVerticesOfTriangle(
		const uint32_t triangleId) {
		if (!m_Triangles.contains(triangleId)) return std::nullopt;
		const Triangle &ABC = m_Triangles.at(triangleId);

		const uint32_t abId = ABC.EdgeAB;
		const uint32_t bcId = ABC.EdgeBC;

		const Edge &AB = m_Edges.at(abId);
		const Edge &BC = m_Edges.at(bcId);

		const uint32_t aId = AB.VertexA;
		const uint32_t bId = BC.VertexB;
		const uint32_t cId = BC.VertexA == AB.VertexA || BC.VertexA == AB.VertexB ? BC.VertexB : BC.VertexA;
		const bool ABCIsOriented = AB.VertexB == BC.VertexA || AB.VertexB == BC.VertexB;

		if (ABCIsOriented) {
			return std::tuple{aId, bId, cId};
		} else {
			return std::tuple{aId, cId, bId};
		}
	}

	inline void MeshGraph::DelaunayTriangulation() {
		std::queue<uint32_t> edgeToCheck;

		for (auto [id, edge]: m_Edges) {
			if (edge.TriangleLeft && edge.TriangleRight)
				edgeToCheck.push(id);
		}

		while (!edgeToCheck.empty()) {
			const auto edgeId = edgeToCheck.front();
			edgeToCheck.pop();

			auto [respectDelaunay, a1Id, a2Id, a3Id, a4Id] = RespectDelaunay(edgeId);

			if (!respectDelaunay) {
				ReverseEdge(edgeId);
				edgeToCheck.push(a1Id);
				edgeToCheck.push(a2Id);
				edgeToCheck.push(a3Id);
				edgeToCheck.push(a4Id);
			}
		}
	}

	inline void MeshGraph::clear() {
		m_Vertices.clear();
		m_Edges.clear();
		m_Triangles.clear();
	}

	inline void MeshGraph::ReverseEdge(const uint32_t edgeId) {
		if (!m_Edges.contains(edgeId)) return;
		const Edge &edge = m_Edges.at(edgeId);
		if (!edge.TriangleLeft || !edge.TriangleRight) return;

		const auto t1Id = edge.TriangleLeft.value();
		const auto t2Id = edge.TriangleRight.value();

		if (!m_Triangles.contains(t1Id)) return;
		if (!m_Triangles.contains(t2Id)) return;


		const uint32_t s2Id = edge.VertexA;
		const uint32_t s1Id = edge.VertexB;
		const Vertex &s1 = m_Vertices.at(s1Id);
		const Vertex &s2 = m_Vertices.at(s2Id);

		std::unordered_map<ReversiblePair<uint32_t, uint32_t>, std::pair<uint32_t, Edge *>, ReversiblePairHash>
				VertexPairToEdge;
		VertexPairToEdge[ReversiblePair{s1Id, s2Id}] = {edgeId, &m_Edges.at(edgeId)};

		Triangle &t1 = m_Triangles.at(t1Id);
		Triangle &t2 = m_Triangles.at(t2Id);

		uint32_t a1Id = -1;
		uint32_t a4Id = -1;
		if (t1.EdgeAB == edgeId) {
			a1Id = t1.EdgeBC;
			a4Id = t1.EdgeCA;
		} else if (t1.EdgeBC == edgeId) {
			a1Id = t1.EdgeCA;
			a4Id = t1.EdgeAB;
		} else /* t1.EdgeCA == edgeId */ {
			a1Id = t1.EdgeAB;
			a4Id = t1.EdgeBC;
		}

		uint32_t a2Id = -1;
		uint32_t a3Id = -1;
		if (t2.EdgeAB == edgeId) {
			a3Id = t2.EdgeBC;
			a2Id = t2.EdgeCA;
		} else if (t2.EdgeBC == edgeId) {
			a3Id = t2.EdgeCA;
			a2Id = t2.EdgeAB;
		} else /* t2.EdgeCA == edgeId */ {
			a3Id = t2.EdgeAB;
			a2Id = t2.EdgeBC;
		}

		Edge &a1 = m_Edges.at(a1Id);
		Edge &a4 = m_Edges.at(a4Id);
		const uint32_t s4Id = a1.VertexA == s1Id || a1.VertexA == s2Id ? a1.VertexB : a1.VertexA;

		Edge &a2 = m_Edges.at(a2Id);
		Edge &a3 = m_Edges.at(a3Id);
		const uint32_t s3Id = a2.VertexA == s1Id || a2.VertexA == s2Id ? a2.VertexB : a2.VertexA;

		VertexPairToEdge[ReversiblePair{a1.VertexA, a1.VertexB}] = {a1Id, &m_Edges.at(a1Id)};
		VertexPairToEdge[ReversiblePair{a4.VertexA, a4.VertexB}] = {a4Id, &m_Edges.at(a4Id)};
		VertexPairToEdge[ReversiblePair{a2.VertexA, a2.VertexB}] = {a2Id, &m_Edges.at(a2Id)};
		VertexPairToEdge[ReversiblePair{a3.VertexA, a3.VertexB}] = {a3Id, &m_Edges.at(a3Id)};

		const Vertex &s3 = m_Vertices.at(s3Id);
		const Vertex &s4 = m_Vertices.at(s4Id);

		VertexPairToEdge[{s3Id, s4Id}] = {edgeId, &m_Edges.at(edgeId)};

		if (Math::IsTriangleOriented(s2.Position, s3.Position, s4.Position)) {
			// T1 = s2-s3-s4
			t1 = {
				VertexPairToEdge.at(ReversiblePair{s2Id, s3Id}).first,
				VertexPairToEdge.at(ReversiblePair{s3Id, s4Id}).first,
				VertexPairToEdge.at(ReversiblePair{s4Id, s2Id}).first
			};

			auto &t1AB = *VertexPairToEdge.at(ReversiblePair{s2Id, s3Id}).second;
			if (t1AB.TriangleLeft == t1Id || t1AB.TriangleLeft == t2Id) { t1AB.TriangleLeft = t1Id; } else if (
				t1AB.TriangleRight == t1Id || t1AB.TriangleRight == t2Id) { t1AB.TriangleRight = t1Id; }

			auto &t1CA = *VertexPairToEdge.at(ReversiblePair{s4Id, s2Id}).second;
			if (t1CA.TriangleLeft == t1Id || t1CA.TriangleLeft == t2Id) { t1CA.TriangleLeft = t1Id; } else if (
				t1CA.TriangleRight == t1Id || t1CA.TriangleRight == t2Id) { t1CA.TriangleRight = t1Id; }


			// T2 s4-s3-s1
			t2 = {
				VertexPairToEdge.at(ReversiblePair{s4Id, s3Id}).first,
				VertexPairToEdge.at(ReversiblePair{s3Id, s1Id}).first,
				VertexPairToEdge.at(ReversiblePair{s1Id, s4Id}).first
			};

			auto &t2BC = *VertexPairToEdge.at(ReversiblePair{s3Id, s1Id}).second;
			if (t2BC.TriangleLeft == t1Id || t2BC.TriangleLeft == t2Id) { t2BC.TriangleLeft = t2Id; } else if (
				t2BC.TriangleRight == t1Id || t2BC.TriangleRight == t2Id) { t2BC.TriangleRight = t2Id; }

			auto &t2CA = *VertexPairToEdge.at(ReversiblePair{s1Id, s4Id}).second;
			if (t2CA.TriangleLeft == t1Id || t2CA.TriangleLeft == t2Id) { t2CA.TriangleLeft = t2Id; } else if (
				t2CA.TriangleRight == t1Id || t2CA.TriangleRight == t2Id) { t2CA.TriangleRight = t2Id; }

			m_Edges[edgeId] = {s3Id, s4Id, t1Id, t2Id};
		} else {
			// T1 s1-s3-s4
			t1 = {
				VertexPairToEdge.at(ReversiblePair{s1Id, s3Id}).first,
				VertexPairToEdge.at(ReversiblePair{s3Id, s4Id}).first,
				VertexPairToEdge.at(ReversiblePair{s4Id, s1Id}).first
			};

			auto &t2BC = *VertexPairToEdge.at(ReversiblePair{s1Id, s3Id}).second;
			if (t2BC.TriangleLeft == t1Id || t2BC.TriangleLeft == t2Id) { t2BC.TriangleLeft = t1Id; } else if (
				t2BC.TriangleRight == t1Id || t2BC.TriangleRight == t2Id) { t2BC.TriangleRight = t1Id; }

			auto &t2CA = *VertexPairToEdge.at(ReversiblePair{s4Id, s1Id}).second;
			if (t2CA.TriangleLeft == t1Id || t2CA.TriangleLeft == t2Id) { t2CA.TriangleLeft = t1Id; } else if (
				t2CA.TriangleRight == t1Id || t2CA.TriangleRight == t2Id) { t2CA.TriangleRight = t1Id; }

			// T2 s2-s4-s3
			t2 = {
				VertexPairToEdge.at(ReversiblePair{s2Id, s4Id}).first,
				VertexPairToEdge.at(ReversiblePair{s4Id, s3Id}).first,
				VertexPairToEdge.at(ReversiblePair{s3Id, s2Id}).first
			};

			auto &t1AB = *VertexPairToEdge.at(ReversiblePair{s3Id, s2Id}).second;
			if (t1AB.TriangleLeft == t1Id || t1AB.TriangleLeft == t2Id) { t1AB.TriangleLeft = t2Id; } else if (
				t1AB.TriangleRight == t1Id || t1AB.TriangleRight == t2Id) { t1AB.TriangleRight = t2Id; }

			auto &t1CA = *VertexPairToEdge.at(ReversiblePair{s2Id, s4Id}).second;
			if (t1CA.TriangleLeft == t1Id || t1CA.TriangleLeft == t2Id) { t1CA.TriangleLeft = t2Id; } else if (
				t1CA.TriangleRight == t1Id || t1CA.TriangleRight == t2Id) { t1CA.TriangleRight = t2Id; }

			m_Edges[edgeId] = {s3Id, s4Id, t1Id, t2Id};
		}
	}
}
