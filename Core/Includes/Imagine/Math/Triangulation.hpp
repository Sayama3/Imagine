//
// Created by ianpo on 31/01/2025.
//

#pragma once

#include "Basics.hpp"
#include "Geometry.hpp"
#include "Shells.hpp"
#include "Mesh.hpp"

namespace Imagine::Math {


	inline std::vector<typename MeshGraph::Vector2> MeshGraphToMesh2D(const MeshGraph& meshGraph) {
		std::vector<MeshGraph::Vector2> mesh;
		mesh.reserve(meshGraph.m_Triangles.size() * 3);
		for (const auto&[trId, ABC] : meshGraph.m_Triangles) {
			const auto& AB = meshGraph.m_Edges.at(ABC.EdgeAB);
			const auto& secondEdge = meshGraph.m_Edges.at(ABC.EdgeBC);

			const auto& A = meshGraph.m_Vertices.at(AB.VertexA);
			const auto& B = meshGraph.m_Vertices.at(AB.VertexB);
			const bool AisInOtherEdge = secondEdge.VertexA == AB.VertexA || secondEdge.VertexB == AB.VertexA;
			const auto& C = meshGraph.m_Vertices.at(secondEdge.VertexA == AB.VertexA || secondEdge.VertexA == AB.VertexB ? secondEdge.VertexB : secondEdge.VertexA);

			if (AisInOtherEdge) {
				mesh.push_back(B.Position);
				mesh.push_back(A.Position);
				mesh.push_back(C.Position);
			} else {
				mesh.push_back(A.Position);
				mesh.push_back(B.Position);
				mesh.push_back(C.Position);
			}
		}
		return mesh;
	}

	inline std::vector<typename MeshGraph::Vector3> MeshGraphToMesh3DXZ(const MeshGraph& meshGraph, MeshGraph::T y = 0) {
		std::vector<MeshGraph::Vector3> mesh;
		mesh.reserve(meshGraph.m_Triangles.size() * 3);
		for (const auto&[trId, ABC] : meshGraph.m_Triangles) {
			const auto& AB = meshGraph.m_Edges.at(ABC.EdgeAB);
			const auto& secondEdge = meshGraph.m_Edges.at(ABC.EdgeBC);

			const auto& A = meshGraph.m_Vertices.at(AB.VertexA);
			const auto& B = meshGraph.m_Vertices.at(AB.VertexB);
			const bool AisInOtherEdge = secondEdge.VertexA == AB.VertexA || secondEdge.VertexB == AB.VertexA;
			const auto& C = meshGraph.m_Vertices.at(secondEdge.VertexA == AB.VertexA || secondEdge.VertexA == AB.VertexB ? secondEdge.VertexB : secondEdge.VertexA);

			if (AisInOtherEdge) {
				mesh.push_back(MeshGraph::Vector3{A.Position.x, y, A.Position.y});
				mesh.push_back(MeshGraph::Vector3{B.Position.x, y, B.Position.y});
				mesh.push_back(MeshGraph::Vector3{C.Position.x, y, C.Position.y});
			} else {
				mesh.push_back(MeshGraph::Vector3{B.Position.x, y, B.Position.y});
				mesh.push_back(MeshGraph::Vector3{A.Position.x, y, A.Position.y});
				mesh.push_back(MeshGraph::Vector3{C.Position.x, y, C.Position.y});
			}
		}
		return mesh;
	}

	inline std::vector<typename MeshGraph::Vector3> MeshGraphToMesh3DXY(const MeshGraph& meshGraph, MeshGraph::T z = 0) {
		std::vector<MeshGraph::Vector3> mesh;
		mesh.reserve(meshGraph.m_Triangles.size() * 3);
		for (const auto&[trId, ABC] : meshGraph.m_Triangles) {
			const auto& AB = meshGraph.m_Edges.at(ABC.EdgeAB);
			const auto& secondEdge = meshGraph.m_Edges.at(ABC.EdgeBC);

			const auto& A = meshGraph.m_Vertices.at(AB.VertexA);
			const auto& B = meshGraph.m_Vertices.at(AB.VertexB);
			const bool AisInOtherEdge = secondEdge.VertexA == AB.VertexA || secondEdge.VertexB == AB.VertexA;
			const auto& C = meshGraph.m_Vertices.at(secondEdge.VertexA == AB.VertexA || secondEdge.VertexA == AB.VertexB ? secondEdge.VertexB : secondEdge.VertexA);

			if (AisInOtherEdge) {
				mesh.push_back(MeshGraph::Vector3{B.Position.x, B.Position.y, z});
				mesh.push_back(MeshGraph::Vector3{A.Position.x, A.Position.y, z});
				mesh.push_back(MeshGraph::Vector3{C.Position.x, C.Position.y, z});
			} else {
				mesh.push_back(MeshGraph::Vector3{A.Position.x, A.Position.y, z});
				mesh.push_back(MeshGraph::Vector3{B.Position.x, B.Position.y, z});
				mesh.push_back(MeshGraph::Vector3{C.Position.x, C.Position.y, z});
			}
		}
		return mesh;
	}



	template<typename Iter>
	std::vector<glm::vec<3,Real>> IncrementalTriangulation(Iter cbegin, Iter cend, Real y = 0) {
		using Vector2 = glm::vec<2,Real>;
		MeshGraph meshGraph;
		std::vector<Vector2> vertices{cbegin, cend};

		std::sort(vertices.begin(), vertices.end(), [](const Vector2& a, const Vector2& b) {
			if (a.x == b.x) {
				return a.y < b.y;
			}
			return a.x < b.x;
		});

		for (const auto& vertex: vertices) {
			meshGraph.AddPoint(vertex);
		}

		return MeshGraphToMesh3DXZ(meshGraph, 0);
	}

}