//
// Created by Iannis on 21/05/2025.
//

#pragma once

#include "Buffer.hpp"
#include "BufferView.hpp"
#include "HeapArray.hpp"
#include "MemoryHelper.hpp"
#include "SparseSet.hpp"

namespace Imagine::Core {
	struct DataType {
		uint32_t id;
		uint32_t size;

		bool operator==(const DataType &other) { return id == other.id && size == other.size; }
		bool operator!=(const DataType &other) { return !(*this == other) }
	};

	struct Member {
		std::string name;
		DataType type;
		uint64_t offset;
		uint64_t size() const { return type.size; }
	};

	class Struct {
	public:
		void AddType(DataType type, std::string name) {
			m_Members.push_back({std::move(name), type, m_Size});
			m_Size += type.size;
		}

	public:
		uint64_t count() const { return m_Members.size(); }
		uint64_t size() const {
			uint64_t size{0};
			for (uint32_t i = 0; i < m_Members.size(); ++i;) size += m_Members[i].size;
			return size;
		}

		void allocate() {
			m_Data.Reallocate(size());
		}

		BufferView get(uint32_t memberIndex) {
			if (memberIndex >= count()) return BufferView{};
			uint64_t offset{0};
			for (uint32_t i = 0; i < memberIndex; ++i;) offset += m_Members[i].size;
			const uint64_t size = m_Members[memberIndex].size;
			return BufferView{m_Data, offset, size};
		}

		ConstBufferView get(uint32_t memberIndex) const {
			if (memberIndex >= count()) return ConstBufferView{};
			uint64_t offset{0};
			for (uint32_t i = 0; i < memberIndex; ++i;) offset += m_Members[i].size;
			const uint64_t size = m_Members[memberIndex].size;
			return ConstBufferView{m_Data, offset, size};
		}

	private:
		HeapArray<Member> m_Members;
		uint64_t m_Size{0};
		Buffer m_Data{};
	};

	class Graph {
	public:
		struct Input {
			uint32_t nodeId;
			uint32_t outputId;
			DataType dataType;
		};

		struct Output {
			uint32_t nodeId;
			uint32_t inputId;
			DataType dataType;
		};

		struct Node {
			HeapArray<Input> inputs;
			HeapArray<Output> outputs;
			std::function<Struct(const Struct &)> function;
		};

	public:
	private:
		AutoIdSparseSet<Node> m_Nodes;
	};


} // namespace Imagine::Core
