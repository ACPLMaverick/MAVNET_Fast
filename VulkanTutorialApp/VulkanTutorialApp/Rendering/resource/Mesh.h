#pragma once

#include "Resource.h"
#include "VertexDeclaration.h"

namespace Rendering
{
	typedef uint32_t IndexType;
	struct WeightIndexType 
	{ 
		uint16_t Indices[4] = { 0, 0, 0, 0 }; 
		bool operator==(const WeightIndexType& other) const
		{
			return
				Indices[0] == other.Indices[0] &&
				Indices[1] == other.Indices[1] &&
				Indices[2] == other.Indices[2] &&
				Indices[3] == other.Indices[3];
		}
	};

#define JE_IndexTypeVk VK_INDEX_TYPE_UINT32

	class Mesh : public Resource
	{
	public:

		JE_EnumBegin(AutoGenMode)
			None,
			Quad,
			Box,
			Sphere
		JE_EnumEnd()

		struct LoadOptions
		{
			AutoGenMode AutoGenerateMode;
			bool bReadOnly = true;
		};

		struct VertexArray
		{
			std::vector<float> Array;
			uint32_t ComponentSize = 0;
			uint32_t ComponentCount = 0;
			VertexDeclaration::ComponentType Type;

			VertexArray()
			{
			}

			VertexArray(VertexDeclaration::ComponentType type)
				: Type(type)
				, ComponentSize(VertexDeclaration::GetComponentSize(type))
			{
			}

			JE_Inline uint32_t GetArraySizeBytes() const { return ComponentSize * ComponentCount; }
		};

		// This contains ALL possible vertex elements. Used for vertex comparison when loading from OBJ.
		// TODO: Think of a better solution.
		struct VertexTotal
		{
			glm::vec3 Position = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec4 Color = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			glm::vec3 Normal = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec2 Uv = glm::vec2(0.0f, 0.0f);
			glm::vec3 Tangent = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec3 Binormal = glm::vec3(0.0f, 0.0f, 0.0f);
			glm::vec4 Weight = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
			WeightIndexType WeightIndex = WeightIndexType();

			bool operator==(const VertexTotal& other) const
			{
				return
					Position == other.Position &&
					Color == other.Color &&
					Normal == other.Normal &&
					Uv == other.Uv &&
					Tangent == other.Tangent &&
					Binormal == other.Binormal &&
					Weight == other.Weight &&
					WeightIndex == other.WeightIndex;
			}
		};

		struct Info
		{
			std::vector<VertexArray> VertexArrays;
			std::vector<IndexType> IndexArray;

			uint32_t VertexCount = 0;
			uint32_t IndexCount = 0;
		};

	public:
		
		Mesh();
		~Mesh();

		JE_Inline bool IsReadOnly() const { return _info.VertexArrays.size() == 0; }
		JE_Inline const Info* GetInfo() const { return &_info; }

		JE_Inline const VkBuffer* GetVertexBuffers() const { return _adjVertexBufferArray.data(); }
		JE_Inline const VkDeviceSize* GetVertexBufferOffsets() const { return _adjOffsetArray.data(); }
		JE_Inline uint32_t GetVertexBufferCount() const { return static_cast<uint32_t>(_adjVertexBufferArray.size()); }

		JE_Inline const VkBuffer* GetOriginalVertexBuffers() const { return _vertexBufferArray.data(); }
		JE_Inline const VkDeviceSize* GetOriginalVertexBufferOffsets() const { return _offsetArray.data(); }
		JE_Inline uint32_t GetOriginalVertexBufferCount() const { return static_cast<uint32_t>(_vertexBufferMemoryArray.size()); }

		JE_Inline VkBuffer GetIndexBuffer() const { return _indexBuffer; }
		JE_Inline uint32_t GetIndexCount() const { return static_cast<uint32_t>(_info.IndexCount); }

		JE_Inline const VertexDeclaration* GetVertexDeclarationIsAdjustedTo() const { return _adjustment; }

		void Load(const std::string& name, const LoadOptions* loadOptions);
		void Cleanup();

		void AdjustBuffersForVertexDeclaration(const VertexDeclaration* declaration);

	protected:

		void InitializeCommon(const LoadOptions* loadOptions);

		void LoadData(const std::string& name, const LoadOptions* loadOptions);
		void LoadDataObj(const std::string& name, const LoadOptions* loadOptions);

		void GenerateData(AutoGenMode generatedType, const LoadOptions* loadOptions);
		void GenerateQuad(const LoadOptions* loadOptions);
		void GenerateBox(const LoadOptions* loadOptions);
		void GenerateSphere(const LoadOptions* loadOptions);

		void CleanupData();

		void CreateVertexBufferArray();
		void CreateVertexBuffer(const VertexArray* arr, VkDeviceMemory* outDeviceMemory, VkBuffer* outBuffer);
		void CreateIndexBuffer();

	protected:

		typedef void (Mesh::*GenerateFunc)(const LoadOptions*);
		static GenerateFunc _generateFunctions[(size_t)AutoGenMode::ENUM_SIZE];

	protected:

		std::vector<VkDeviceMemory> _vertexBufferMemoryArray;
		VkDeviceMemory _indexBufferMemory;

		std::vector<VkBuffer> _vertexBufferArray;
		VkBuffer _indexBuffer;

		std::vector<VkDeviceSize> _offsetArray;

		std::vector<VkBuffer> _adjVertexBufferArray;
		std::vector<VkDeviceSize> _adjOffsetArray;

		Info _info;

		const VertexDeclaration* _adjustment;
	};
}

namespace std
{
	template<> struct hash<Rendering::WeightIndexType>
	{
		size_t operator()(const Rendering::WeightIndexType& weightIndex) const
		{
			return ((hash<uint16_t>()(weightIndex.Indices[0]) ^
				(hash<uint16_t>()(weightIndex.Indices[1]) << 1)) >> 1) ^
				((hash<uint16_t>()(weightIndex.Indices[2]) << 2) >> 2) ^
				((hash<uint16_t>()(weightIndex.Indices[3]) << 3) >> 3);
		}
	};

	template<> struct hash<Rendering::Mesh::VertexTotal>
	{
		size_t operator()(const Rendering::Mesh::VertexTotal& vertex) const
		{
			return ((hash<glm::vec3>()(vertex.Position) ^
				(hash<glm::vec4>()(vertex.Color) << 1)) >> 1) ^
				((hash<glm::vec3>()(vertex.Normal) << 2) >> 2) ^
				((hash<glm::vec3>()(vertex.Tangent) << 3) >> 3) ^
				((hash<glm::vec3>()(vertex.Binormal) << 4) >> 4) ^
				((hash<glm::vec4>()(vertex.Weight) << 5) >> 5) ^
				((hash<Rendering::WeightIndexType>()(vertex.WeightIndex) << 6) >> 6) ^
				(hash<glm::vec2>()(vertex.Uv) << 1);

		}
	};
}