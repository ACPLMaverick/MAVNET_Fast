#include "Mesh.h"

#include "Core/HelloTriangle.h"
#include "Rendering/Helper.h"

#define USE_STAGING_BUFFER 1

namespace Rendering
{
	Mesh::Mesh()
		: _indexBuffer(VK_NULL_HANDLE)
		, _adjustment(nullptr)
	{
		_type = ResourceCommon::Type::Mesh;
	}

	Mesh::~Mesh()
	{
	}

	void Mesh::Load(const std::string& name, const LoadOptions * loadOptions)
	{
		JE_Assert(loadOptions != nullptr);

		if (loadOptions->AutoGenerateMode == AutoGenMode::None)
		{
			LoadData(name, loadOptions);
		}
		else
		{
			GenerateData(loadOptions->AutoGenerateMode, loadOptions);
		}

		InitializeCommon(loadOptions);
	}

	void Mesh::Cleanup()
	{
		if (!IsReadOnly())
		{
			CleanupData();
		}

		for (auto& buffer : _vertexBufferArray)
		{
			vkDestroyBuffer(JE_GetRenderer()->GetDevice(), buffer, JE_GetRenderer()->GetAllocatorPtr());
		}
		_vertexBufferArray.clear();

		for (auto& mem : _vertexBufferMemoryArray)
		{
			vkFreeMemory(JE_GetRenderer()->GetDevice(), mem, JE_GetRenderer()->GetAllocatorPtr());
		}
		_vertexBufferMemoryArray.clear();

		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), _indexBuffer, JE_GetRenderer()->GetAllocatorPtr());
		vkFreeMemory(JE_GetRenderer()->GetDevice(), _indexBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
		_indexBuffer = VK_NULL_HANDLE;
	}

	void Mesh::AdjustBuffersForVertexDeclaration(const VertexDeclaration * declaration)
	{
		if (declaration == nullptr)
		{
			_adjustment = nullptr;

			_adjVertexBufferArray = _vertexBufferArray;
			_adjOffsetArray = _offsetArray;

			return;
		}

		const size_t componentCount = declaration->GetComponents()->size();
		_adjVertexBufferArray.clear();
		_adjVertexBufferArray.resize(componentCount);
		_adjOffsetArray.clear();
		_adjOffsetArray.resize(componentCount);

		for(size_t i = 0; i < componentCount; ++i)
		{
			VertexDeclaration::ComponentType thisType = (*declaration->GetComponents())[i];

			// Find this type in our vertex arrays.
			size_t foundIndex = -1;
			for (size_t j = 0; j < _info.VertexArrays.size(); ++j)
			{
				if (_info.VertexArrays[j].Type == thisType)
				{
					foundIndex = j;
					break;
				}
			}

			if (foundIndex != -1)
			{
				// Found this amongst vertex arrays. Assuming indices in vertex arrays correspond to these in vertex buffer array.
				// Simply place this buffer in this position.

				_adjVertexBufferArray[i] = _vertexBufferArray[foundIndex];
				_adjOffsetArray[i] = _offsetArray[foundIndex];
			}
			else
			{
				// This component type is not present among the vertex arrays of this mesh.
				// Place dummy vertex buffer from the helper.

				uint32_t neededSizeBytes = VertexDeclaration::GetComponentSize(thisType) * _info.VertexCount;

				_adjVertexBufferArray[i] = Helper::GetInstance()->GetVoidVertexBuffer(neededSizeBytes);
				_adjOffsetArray[i] = 0;
			}
		}
	}

	void Mesh::InitializeCommon(const LoadOptions* loadOptions)
	{
		CreateVertexBufferArray();
		CreateIndexBuffer();

		AdjustBuffersForVertexDeclaration(nullptr);

		if (loadOptions->bReadOnly)
		{
			CleanupData();
		}
	}

	void Mesh::LoadData(const std::string& name, const LoadOptions * loadOptions)
	{
		// TODO: implement different formats support (FBX?)
		LoadDataObj(name, loadOptions);
	}

	void Mesh::LoadDataObj(const std::string& name, const LoadOptions * loadOptions)
	{
		JE_Assert(IsReadOnly());	// i.e. !IsLoaded()

		const std::string finalPath = (::Core::HelloTriangle::RESOURCE_PATH + "Meshes\\Source\\" + name);

		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string err;

		JE_AssertThrow(tinyobj::LoadObj(&attrib, &shapes, &materials, &err, finalPath.c_str()), err);

		glm::vec3 dummyNormal = glm::vec3(0.0f, 0.0f, 0.0f);
		const bool bIncludeNormals = attrib.normals.size() > 0;

		VertexArray* arrayPosition = nullptr;
		VertexArray* arrayNormal = nullptr;
		VertexArray* arrayUv = nullptr;
		if (bIncludeNormals)
		{
			_info.VertexArrays.resize(3);
			arrayPosition = &_info.VertexArrays[0];
			arrayNormal = &_info.VertexArrays[1];
			arrayUv = &_info.VertexArrays[2];
		}
		else
		{
			_info.VertexArrays.resize(2);
			arrayPosition = &_info.VertexArrays[0];
			arrayUv = &_info.VertexArrays[1];
		}

		*arrayPosition = VertexArray(VertexDeclaration::ComponentType::Position);
		if (bIncludeNormals)
		{
			*arrayNormal = VertexArray(VertexDeclaration::ComponentType::Normal);
		}
		*arrayUv = VertexArray(VertexDeclaration::ComponentType::Uv);



		std::unordered_map<VertexTotal, uint32_t> uniqueVertices = {};

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				VertexTotal vertex;

				vertex.Position =
				{
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				// No loading color from OBJ.

				if (bIncludeNormals)
				{
					vertex.Normal =
					{
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}
				else
				{
					vertex.Normal = dummyNormal;
				}

				vertex.Uv =
				{
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				if (uniqueVertices.count(vertex) == 0)
				{
					uniqueVertices[vertex] = arrayPosition->ComponentCount;

					arrayPosition->Array.push_back(vertex.Position.x);
					arrayPosition->Array.push_back(vertex.Position.y);
					arrayPosition->Array.push_back(vertex.Position.z);
					++(arrayPosition->ComponentCount);

					if (bIncludeNormals)
					{
						arrayNormal->Array.push_back(vertex.Normal.x);
						arrayNormal->Array.push_back(vertex.Normal.y);
						arrayNormal->Array.push_back(vertex.Normal.z);
						++(arrayNormal->ComponentCount);
					}

					arrayUv->Array.push_back(vertex.Uv.x);
					arrayUv->Array.push_back(vertex.Uv.y);
					++(arrayUv->ComponentCount);
				}

				_info.IndexArray.push_back(uniqueVertices[vertex]);
			}
		}

		_info.VertexCount = static_cast<uint32_t>(_info.VertexArrays[0].Array.size() / (_info.VertexArrays[0].ComponentSize / 4)); // One float has 4 bytes.
		_info.IndexCount = static_cast<uint32_t>(_info.IndexArray.size());
		JE_Assert(_info.VertexCount != 0);
		JE_Assert(_info.IndexCount != 0);
	}

	void Mesh::GenerateData(AutoGenMode generatedType, const LoadOptions* loadOptions)
	{
		_info.VertexArrays.resize(4);
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		*arrayPosition = VertexArray(VertexDeclaration::ComponentType::Position);
		*arrayNormal = VertexArray(VertexDeclaration::ComponentType::Normal);
		*arrayUv = VertexArray(VertexDeclaration::ComponentType::Uv);
		*arrayColor = VertexArray(VertexDeclaration::ComponentType::Color);
		

		const size_t index = (size_t)generatedType;
		JE_Assert(index < JE_ArrayLength(_generateFunctions));
		GenerateFunc func = _generateFunctions[index];
		if (func != nullptr)
		{
			(this->*(func))(loadOptions);
		}


		_info.VertexCount = static_cast<uint32_t>(_info.VertexArrays[0].Array.size() / (_info.VertexArrays[0].ComponentSize / 4)); // One float has 4 bytes.
		_info.IndexCount = static_cast<uint32_t>(_info.IndexArray.size());
		JE_Assert(_info.VertexCount != 0);
		JE_Assert(_info.IndexCount != 0);
	}

	void Mesh::GenerateQuad(const LoadOptions* loadOptions)
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		arrayPosition->ComponentCount = arrayNormal->ComponentCount = arrayUv->ComponentCount = arrayColor->ComponentCount = 4;

		const float halfSize = 0.5f;
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(0.0f);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(0.0f);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(0.0f);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(0.0f);

		for (uint32_t i = 0; i < arrayNormal->ComponentCount; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(1.0f);
		}

		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);

		const float col = 1.0f;
		const float a = 1.0f;
		for (uint32_t i = 0; i < arrayColor->ComponentCount; ++i)
		{
			//arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(a);
			arrayColor->Array.push_back(arrayPosition->Array[3 * i]); 
			arrayColor->Array.push_back(arrayPosition->Array[3 * i + 1]); 
			arrayColor->Array.push_back(arrayPosition->Array[3 * i + 2]); 
			arrayColor->Array.push_back(a);
		}

		_info.IndexArray.push_back(0);
		_info.IndexArray.push_back(2);
		_info.IndexArray.push_back(1);
		_info.IndexArray.push_back(0);
		_info.IndexArray.push_back(3);
		_info.IndexArray.push_back(2);
	}

	void Mesh::GenerateBox(const LoadOptions* loadOptions)
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		arrayPosition->ComponentCount = arrayNormal->ComponentCount = arrayUv->ComponentCount = arrayColor->ComponentCount = 24;

		const float halfSize = 0.5f;
		float colR, colG, colB;
		const float a = 1.0f;
		
		// Bottom
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		colR = 0.0f; colG = 0.0f; colB = 1.0f; // BLUE
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(-1.0f); arrayNormal->Array.push_back(-1.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Top
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		colR = 1.0f; colG = 1.0f; colB = 0.0f; // YELLOW
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(1.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Left
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		colR = 0.0f; colG = 1.0f; colB = 1.0f; // CYAN
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(1.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Right
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		colR = 1.0f; colG = 0.0f; colB = 0.0f; // RED
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(-1.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Front 
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		colR = 1.0f; colG = 0.0f; colB = 1.0f; // MAGENTA
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(1.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Back
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize);
		colR = 0.0f; colG = 1.0f; colB = 0.0f; // GREEN
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(-1.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		for (size_t i = 0; i < 6; ++i)
		{
			arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
			arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
			arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
			arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		}


		for (unsigned int i = 0; i < 6 * 4; i += 4)
		{
			_info.IndexArray.push_back(i + 0);
			_info.IndexArray.push_back(i + 2);
			_info.IndexArray.push_back(i + 1);
			_info.IndexArray.push_back(i + 0);
			_info.IndexArray.push_back(i + 3);
			_info.IndexArray.push_back(i + 2);
		}
	}

	void Mesh::GenerateSphere(const LoadOptions* loadOptions)
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		JE_TODO();
	}

	void Mesh::CleanupData()
	{
		for (auto& arr : _info.VertexArrays)
		{
			arr.Array.clear();
		}

		_info.IndexArray.clear();
	}

	void Mesh::CreateVertexBufferArray()
	{
		JE_Assert(_vertexBufferArray.size() == 0 && _info.VertexArrays.size() > 0);
		for (auto& arr : _info.VertexArrays)
		{
			VkDeviceMemory mem;
			VkBuffer buf;
			CreateVertexBuffer(&arr, &mem, &buf);
			_vertexBufferArray.push_back(buf);
			_vertexBufferMemoryArray.push_back(mem);
			_offsetArray.push_back(0); // TODO: Use one buffer and offsetting instead of a buffer for each component.
		}
	}

	void Mesh::CreateVertexBuffer(const VertexArray* arr, VkDeviceMemory* outDeviceMemory, VkBuffer* outBuffer)
	{
#if USE_STAGING_BUFFER
		VkDeviceSize bufferSize = arr->GetArraySizeBytes();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		JE_GetRenderer()->CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(arr->Array.data()), stagingBufferMemory, static_cast<size_t>(bufferSize));

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, *outBuffer, *outDeviceMemory);

		JE_GetRenderer()->CopyBuffer_GPU_GPU(stagingBuffer, *outBuffer, bufferSize);

		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), stagingBuffer, JE_GetRenderer()->GetAllocatorPtr());
		vkFreeMemory(JE_GetRenderer()->GetDevice(), stagingBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
#else
		VkDeviceSize bufferSize = arr->GetArraySizeBytes();

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, *outBuffer, *outDeviceMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(arr->Array.data()), *outDeviceMemory, static_cast<size_t>(bufferSize));
#endif
	}

	void Mesh::CreateIndexBuffer()
	{
		JE_Assert(_indexBuffer == VK_NULL_HANDLE && _info.IndexArray.size() > 0);
#if USE_STAGING_BUFFER
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_info.IndexArray);

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		JE_GetRenderer()->CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_info.IndexArray.data()), stagingBufferMemory, static_cast<size_t>(bufferSize));

		JE_GetRenderer()->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, _indexBuffer, _indexBufferMemory);

		JE_GetRenderer()->CopyBuffer_GPU_GPU(stagingBuffer, _indexBuffer, bufferSize);

		vkDestroyBuffer(JE_GetRenderer()->GetDevice(), stagingBuffer, JE_GetRenderer()->GetAllocatorPtr());
		vkFreeMemory(JE_GetRenderer()->GetDevice(), stagingBufferMemory, JE_GetRenderer()->GetAllocatorPtr());
#else
		VkDeviceSize bufferSize = JE_VectorSizeBytes(_info.IndexArray);

		CreateBuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, _indexBuffer, _indexBufferMemory);

		CopyBuffer_CPU_GPU(reinterpret_cast<const void*>(_info.IndexArray.data()), _indexBufferMemory, static_cast<size_t>(bufferSize));
#endif
	}

	Rendering::Mesh::GenerateFunc Mesh::_generateFunctions[] = 
	{
		nullptr,
		&Mesh::GenerateQuad,
		&Mesh::GenerateBox,
		&Mesh::GenerateSphere
	};

}