#include "Mesh.h"

#include "Core/HelloTriangle.h"
#include "Rendering/Helper.h"

#define USE_STAGING_BUFFER 1

namespace Rendering
{
	Mesh::Mesh()
		: _indexBuffer(VK_NULL_HANDLE)
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

	void Mesh::InitializeCommon(const LoadOptions* loadOptions)
	{
		CreateVertexBufferArray();
		CreateIndexBuffer();

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

		_info.VertexCount = static_cast<uint32_t>(_info.VertexArrays[0].GetArrayComponentCount());
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


		_info.VertexCount = static_cast<uint32_t>(_info.VertexArrays[0].GetArrayComponentCount());
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
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(-halfSize);

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
			arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(a);
			/*
			arrayColor->Array.push_back(arrayPosition->Array[3 * i]); 
			arrayColor->Array.push_back(arrayPosition->Array[3 * i + 1]); 
			arrayColor->Array.push_back(arrayPosition->Array[3 * i + 2]); 
			arrayColor->Array.push_back(a);
			*/
		}

		// Counter-clockwise -> Because we flip vertices on Y axis with projection matrix.
		_info.IndexArray.push_back(0);
		_info.IndexArray.push_back(1);
		_info.IndexArray.push_back(2);
		_info.IndexArray.push_back(0);
		_info.IndexArray.push_back(2);
		_info.IndexArray.push_back(3);
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
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		colR = 0.0f; colG = 0.0f; colB = 1.0f; // BLUE
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(-1.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Top
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		colR = 1.0f; colG = 1.0f; colB = 0.0f; // YELLOW
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(1.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Left
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		colR = 0.0f; colG = 1.0f; colB = 1.0f; // CYAN
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(-1.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Right
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(-halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		colR = 1.0f; colG = 0.0f; colB = 0.0f; // RED
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(1.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		// Front 
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(-halfSize); arrayPosition->Array.push_back(halfSize);
		arrayPosition->Array.push_back(-halfSize);	arrayPosition->Array.push_back(halfSize); arrayPosition->Array.push_back(halfSize);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
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
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(1.0f);
		arrayUv->Array.push_back(1.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(0.0f);
		arrayUv->Array.push_back(0.0f); arrayUv->Array.push_back(1.0f);
		colR = 0.0f; colG = 1.0f; colB = 0.0f; // GREEN
		for (size_t i = 0; i < 4; ++i)
		{
			arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(0.0f); arrayNormal->Array.push_back(-1.0f);
			arrayColor->Array.push_back(colR); arrayColor->Array.push_back(colG); arrayColor->Array.push_back(colB); arrayColor->Array.push_back(a);
		}

		for (unsigned int i = 0; i < 6 * 4; i += 4)
		{
			_info.IndexArray.push_back(i + 0);
			_info.IndexArray.push_back(i + 1);
			_info.IndexArray.push_back(i + 2);
			_info.IndexArray.push_back(i + 0);
			_info.IndexArray.push_back(i + 2);
			_info.IndexArray.push_back(i + 3);
		}
	}

	void Mesh::GenerateSphere(const LoadOptions* loadOptions)
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		static const float DEFAULT_RADIUS = 1.0f;
		static const uint32_t DEFAULT_NUM_EDGES_VERT = 26;
		static const uint32_t DEFAULT_NUM_EDGES_HOR = 32;
		const bool bIsHalf = loadOptions->AutoGenerateOptions.SphereMode != AutoGenSphereMode::Whole;
		const uint32_t numEdgesVert = (loadOptions->AutoGenerateOptions.SphereEdgesVert >= 3 ? loadOptions->AutoGenerateOptions.SphereEdgesVert : DEFAULT_NUM_EDGES_VERT);

		uint32_t numEdgesHor = (loadOptions->AutoGenerateOptions.SphereEdgesHor >= 1 ? loadOptions->AutoGenerateOptions.SphereEdgesHor : DEFAULT_NUM_EDGES_HOR);
		if (bIsHalf)
		{
			if (numEdgesHor % 2 == 0 || numEdgesHor == 1)
			{
				++numEdgesHor;
			}
			numEdgesHor /= 2;
		}

		const float radius = DEFAULT_RADIUS;

		arrayPosition->ComponentCount = arrayNormal->ComponentCount = arrayUv->ComponentCount = arrayColor->ComponentCount = (numEdgesVert + 1) * numEdgesHor + (bIsHalf ? 1 : 2);

		const float stepAngleVert = bIsHalf ? (90.0f / (float)(numEdgesHor)) : (180.0f / (float)(numEdgesHor + 1));
		const float stepAngleHor = 360.0f / (float)(numEdgesVert);
		const glm::mat4 stepRotateVert = glm::rotate(glm::mat4(1.0f), glm::radians(stepAngleVert), glm::vec3(1.0f, 0.0f, 0.0f));
		const glm::mat4 stepRotateHor = glm::rotate(glm::mat4(1.0f), glm::radians(stepAngleHor), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 vertex;
		if (loadOptions->AutoGenerateOptions.SphereMode == AutoGenSphereMode::UpperHalf)
		{
			vertex = glm::vec4(0.0f, 0.0f, -radius, 0.0f);	
		}
		else
		{
			vertex = glm::vec4(0.0f, -radius, 0.0f, 0.0f);
		}

		const uint32_t numStep = numEdgesHor + 2;

		// Place lowest vertex.
		if (loadOptions->AutoGenerateOptions.SphereMode != AutoGenSphereMode::UpperHalf)
		{
			arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(-radius); arrayPosition->Array.push_back(0.0f);
			arrayUv->Array.push_back(0.5f); arrayUv->Array.push_back(1.0f);

			vertex = stepRotateVert * vertex;
		}

		// Place vertices along horizontal rings and draw a horizontal strip with lower level.
		for (uint32_t i = 1; i < numStep - 1; ++i)
		{
			const float currentUvY = 1.0f - ((float)i / (float)(numStep - 1)) * (bIsHalf ? 0.5f : 1.0f) - (loadOptions->AutoGenerateOptions.SphereMode == AutoGenSphereMode::UpperHalf ? 0.5f : 0.0f);
			const uint32_t currentBaseIndex = arrayPosition->GetArrayComponentCount();
			
			for (uint32_t j = 0; j <= numEdgesVert; ++j)	// Duplicating first horizontal edge because we need to have UV seam there.
			{
				arrayPosition->Array.push_back(vertex.x); arrayPosition->Array.push_back(vertex.y); arrayPosition->Array.push_back(vertex.z);
				arrayUv->Array.push_back(1.0f - (float)j / (float)(numEdgesVert));
				arrayUv->Array.push_back(currentUvY);

				if (j < numEdgesVert)
				{
					vertex = stepRotateHor * vertex;
				}
			}

			vertex = stepRotateVert * vertex;

			if (i == 1) // Make faces with lowest vertex.
			{
				if (loadOptions->AutoGenerateOptions.SphereMode != AutoGenSphereMode::UpperHalf)
				{
					for (uint32_t j = 0; j < numEdgesVert; ++j)
					{
						_info.IndexArray.push_back(0);
						_info.IndexArray.push_back(j + 1);
						_info.IndexArray.push_back(j + 2);
					}
				}
				// Else do nothing at this ring, hence this is the first one.
			}
			else // Make faces with lower ring.
			{
				for (uint32_t j = 0; j < numEdgesVert; ++j)
				{
					const uint32_t thisIndex = currentBaseIndex + j;
					const uint32_t thisNextIndex = currentBaseIndex + j + 1;
					const uint32_t lowerIndex = thisIndex - (numEdgesVert + 1);
					const uint32_t lowerNextIndex = thisNextIndex - (numEdgesVert + 1);

					_info.IndexArray.push_back(thisIndex);
					_info.IndexArray.push_back(thisNextIndex);
					_info.IndexArray.push_back(lowerIndex);
					_info.IndexArray.push_back(thisNextIndex);
					_info.IndexArray.push_back(lowerNextIndex);
					_info.IndexArray.push_back(lowerIndex);
				}
			}
		}

		// Place highest vertex.
		if (loadOptions->AutoGenerateOptions.SphereMode != AutoGenSphereMode::LowerHalf)
		{
			const uint32_t lastRingBaseIndex = (uint32_t)arrayPosition->GetArrayComponentCount() - (numEdgesVert + 1);
			arrayPosition->Array.push_back(0.0f); arrayPosition->Array.push_back(radius); arrayPosition->Array.push_back(0.0f);
			arrayUv->Array.push_back(0.5f); arrayUv->Array.push_back(0.0f);
			const uint32_t finalVertexIndex = (uint32_t)arrayPosition->GetArrayComponentCount() - 1;
			// Make faces with previous ring.
			for (uint32_t j = 0; j < numEdgesVert; ++j)
			{
				_info.IndexArray.push_back(lastRingBaseIndex + j + 1);
				_info.IndexArray.push_back(lastRingBaseIndex + j);
				_info.IndexArray.push_back(finalVertexIndex);
			}
		}

		// Fill white color.
		const float col = 1.0f;
		const float a = 1.0f;
		const uint32_t currCompCount = arrayPosition->GetArrayComponentCount();
		const uint32_t currPosStride = arrayPosition->ComponentSize / sizeof(float);
		for (uint32_t i = 0; i < currCompCount; ++i)
		{
			arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(a);

			glm::vec3 normal(arrayPosition->Array[currPosStride * i], arrayPosition->Array[currPosStride * i + 1], arrayPosition->Array[currPosStride * i + 2]);
			normal = glm::normalize(normal);
			arrayNormal->Array.push_back(normal.x); arrayNormal->Array.push_back(normal.y); arrayNormal->Array.push_back(normal.z);
		}

		// Fix seam edge normals.
		{
			const uint32_t normStride = arrayNormal->ComponentSize / sizeof(float);
			const uint32_t start = loadOptions->AutoGenerateOptions.SphereMode == AutoGenSphereMode::UpperHalf ? 0 : normStride;	// Ignore first vertex if Whole or LowerHalf
			const uint32_t stop = (uint32_t)arrayNormal->Array.size() - normStride;
			const uint32_t step = normStride * (numEdgesVert + 1);
			for (uint32_t i = start; i < stop; i += step) // Omitting first and last vertex, stepping by numEdgesVert+1
			{
				const glm::vec3 firstNormal
				(
					arrayNormal->Array[i],
					arrayNormal->Array[i + 1],
					arrayNormal->Array[i + 2]
				);
				const glm::vec3 lastNormal
				(
					arrayNormal->Array[i + normStride * numEdgesVert],
					arrayNormal->Array[i + normStride * numEdgesVert + 1],
					arrayNormal->Array[i + normStride * numEdgesVert + 2]
				);

				glm::vec3 avg = glm::normalize(firstNormal + lastNormal);

				arrayNormal->Array[i] = arrayNormal->Array[i + normStride * numEdgesVert] = avg.x;
				arrayNormal->Array[i + 1] = arrayNormal->Array[i + normStride * numEdgesVert + 1] = avg.y;
				arrayNormal->Array[i + 2] = arrayNormal->Array[i + normStride * numEdgesVert + 2] = avg.z;
			}
		}
	}

	void Mesh::GenerateCylinder(const LoadOptions* loadOptions)
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];
		VertexArray* arrayUv = &_info.VertexArrays[2];
		VertexArray* arrayColor = &_info.VertexArrays[3];

		static const float DEFAULT_HEIGHT = 1.0f;
		static const float DEFAULT_CAP_RADIUS = 0.5f;
		static const uint32_t DEFAULT_NUM_EDGES_VERT = 32;
		const uint32_t numEdgesVert = loadOptions->AutoGenerateOptions.CylinderEdgesVert >= 3 ? loadOptions->AutoGenerateOptions.CylinderEdgesVert : DEFAULT_NUM_EDGES_VERT;
		const float halfHeight = DEFAULT_HEIGHT * 0.5f;
		const float capRadius = DEFAULT_CAP_RADIUS;
		const bool bCaps = !loadOptions->AutoGenerateOptions.CylinderNoCaps;

		arrayPosition->ComponentCount = arrayNormal->ComponentCount = arrayUv->ComponentCount = arrayColor->ComponentCount = (numEdgesVert + 1) * (bCaps ? 4 : 2);


		const float stepAngleHor = 360.0f / (float)(numEdgesVert);
		const glm::mat4 stepRotateHor = glm::rotate(glm::mat4(1.0f), glm::radians(stepAngleHor), glm::vec3(0.0f, 1.0f, 0.0f));
		const glm::mat4 stepRotateHorUv = glm::rotate(glm::mat4(1.0f), glm::radians(stepAngleHor), glm::vec3(0.0f, 0.0f, 1.0f));

		// Make caps.

		if (bCaps)
		{
			glm::vec3 vertices[2] = { glm::vec3(0.0f, -halfHeight, capRadius), glm::vec3(0.0f, halfHeight, -capRadius) };
			glm::vec3 normals[2] = { glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f) };
			glm::vec2 uv(0.0f, -0.5f);
			for (uint32_t i = 0; i < 2; ++i)
			{
				// Middle vertex.
				const glm::vec3 middle = normals[i] * halfHeight;
				arrayPosition->Array.push_back(middle.x); arrayPosition->Array.push_back(middle.y); arrayPosition->Array.push_back(middle.z);
				arrayUv->Array.push_back(0.5f); arrayUv->Array.push_back(0.5f);
				arrayNormal->Array.push_back(normals[i].x); arrayNormal->Array.push_back(normals[i].y); arrayNormal->Array.push_back(normals[i].z);

				for (uint32_t j = 0; j < numEdgesVert; ++j)
				{
					const glm::vec2 currUv = uv + glm::vec2(0.5f, 0.5f);
					arrayPosition->Array.push_back(vertices[i].x); arrayPosition->Array.push_back(vertices[i].y); arrayPosition->Array.push_back(vertices[i].z);
					arrayUv->Array.push_back(currUv.x); arrayUv->Array.push_back(currUv.y);
					arrayNormal->Array.push_back(normals[i].x); arrayNormal->Array.push_back(normals[i].y); arrayNormal->Array.push_back(normals[i].z);

					vertices[i] = stepRotateHor * glm::vec4(vertices[i], 0.0f);
					uv = stepRotateHorUv * glm::vec4(uv, 0.0f, 0.0f);
				}
			}

			const uint32_t upperCapOffset = numEdgesVert + 1;

			// Lower cap face.
			for (uint32_t i = 0; i < numEdgesVert; ++i)
			{
				_info.IndexArray.push_back(0);
				_info.IndexArray.push_back(i + 1);
				_info.IndexArray.push_back(((i + 1) % numEdgesVert) + 1);
			}

			// Upper cap face.
			for (uint32_t i = 0; i < numEdgesVert; ++i)
			{
				_info.IndexArray.push_back(upperCapOffset);
				_info.IndexArray.push_back(((i + 1) % numEdgesVert) + upperCapOffset + 1);
				_info.IndexArray.push_back(i + upperCapOffset + 1);
			}
		}

		// Make side.
		const uint32_t indexOffset = (uint32_t)(arrayPosition->GetArrayComponentCount());
		glm::vec3 vertex(0.0f, 0.0f, capRadius);
		for (uint32_t i = 0; i <= numEdgesVert; ++i)
		{
			// Lower and upper vertex.
			arrayPosition->Array.push_back(vertex.x); arrayPosition->Array.push_back(-halfHeight); arrayPosition->Array.push_back(vertex.z);
			arrayPosition->Array.push_back(vertex.x); arrayPosition->Array.push_back(halfHeight); arrayPosition->Array.push_back(vertex.z);
			
			const float uvX = 1.0f - (float)i / (float)numEdgesVert;
			arrayUv->Array.push_back(uvX); arrayUv->Array.push_back(1.0f);
			arrayUv->Array.push_back(uvX); arrayUv->Array.push_back(0.0f);
			
			const glm::vec3 normal(glm::normalize(vertex));
			arrayNormal->Array.push_back(normal.x); arrayNormal->Array.push_back(normal.y); arrayNormal->Array.push_back(normal.z);
			arrayNormal->Array.push_back(normal.x); arrayNormal->Array.push_back(normal.y); arrayNormal->Array.push_back(normal.z);

			vertex = stepRotateHor * glm::vec4(vertex, 0.0f);

			if (i == 0)
			{
				continue;
			}
			else
			{
				const uint32_t indexThisLower = indexOffset + i * 2;
				const uint32_t indexThisUpper = indexOffset + i * 2 + 1;
				const uint32_t indexPrevLower = indexOffset + (i - 1) * 2;
				const uint32_t indexPrevUpper = indexOffset + (i - 1) * 2 + 1;

				_info.IndexArray.push_back(indexThisLower);
				_info.IndexArray.push_back(indexPrevLower);
				_info.IndexArray.push_back(indexThisUpper);
				_info.IndexArray.push_back(indexPrevLower);
				_info.IndexArray.push_back(indexPrevUpper);
				_info.IndexArray.push_back(indexThisUpper);
			}
		}

		// Fill white color.
		const float col = 1.0f;
		const float a = 1.0f;
		for (uint32_t i = 0; i < arrayColor->ComponentCount; ++i)
		{
			arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(col); arrayColor->Array.push_back(a);
		}
	}

	void Mesh::ComputeNormalsSmooth()
	{
		VertexArray* arrayPosition = &_info.VertexArrays[0];
		VertexArray* arrayNormal = &_info.VertexArrays[1];

		arrayNormal->Array.resize(arrayPosition->Array.size(), 0.0f);

		const size_t indexNum = _info.IndexArray.size();
		uint32_t compStride = arrayPosition->ComponentSize / sizeof(float);
		glm::vec3 positions[3];
		glm::vec3 edges[2];
		
		for (size_t i = 0; i < indexNum; i += 3)
		{
			positions[0] = glm::vec3
			(
				arrayPosition->Array[_info.IndexArray[i] * compStride], 
				arrayPosition->Array[_info.IndexArray[i] * compStride + 1], 
				arrayPosition->Array[_info.IndexArray[i] * compStride + 2]
			);
			positions[1] = glm::vec3
			(
				arrayPosition->Array[_info.IndexArray[i + 1] * compStride], 
				arrayPosition->Array[_info.IndexArray[i + 1] * compStride + 1], 
				arrayPosition->Array[_info.IndexArray[i + 1] * compStride + 2]
			);
			positions[2] = glm::vec3
			(
				arrayPosition->Array[_info.IndexArray[i + 2] * compStride], 
				arrayPosition->Array[_info.IndexArray[i + 2] * compStride + 1], 
				arrayPosition->Array[_info.IndexArray[i + 2] * compStride + 2]
			);

			edges[0] = glm::normalize(positions[1] - positions[0]);
			edges[1] = glm::normalize(positions[2] - positions[0]);

			glm::vec3 cross = glm::normalize(glm::cross(edges[1], edges[0]));


			arrayNormal->Array[_info.IndexArray[i] * compStride] += cross.x;
			arrayNormal->Array[_info.IndexArray[i] * compStride + 1] += cross.y;
			arrayNormal->Array[_info.IndexArray[i] * compStride + 2] += cross.z;

			arrayNormal->Array[_info.IndexArray[i + 1] * compStride] += cross.x;
			arrayNormal->Array[_info.IndexArray[i + 1] * compStride + 1] += cross.y;
			arrayNormal->Array[_info.IndexArray[i + 1] * compStride + 2] += cross.z;

			arrayNormal->Array[_info.IndexArray[i + 2] * compStride] += cross.x;
			arrayNormal->Array[_info.IndexArray[i + 2] * compStride + 1] += cross.y;
			arrayNormal->Array[_info.IndexArray[i + 2] * compStride + 2] += cross.z;
		}

		const size_t normalArraySize = arrayNormal->Array.size();
		compStride = arrayNormal->ComponentSize / sizeof(float);
		for (size_t i = 0; i < normalArraySize; i += compStride)
		{
			glm::vec3 normal
			(
				arrayNormal->Array[i],
				arrayNormal->Array[i + 1],
				arrayNormal->Array[i + 2]
			);
			normal = glm::normalize(normal);
			arrayNormal->Array[i] = normal.x;
			arrayNormal->Array[i + 1] = normal.y;
			arrayNormal->Array[i + 2] = normal.z;
		}
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
		&Mesh::GenerateSphere,
		&Mesh::GenerateCylinder
	};

	const char* Rendering::Mesh::AutogenPostfixes[] = { "", "Quad", "Box", "Sphere", "Cylinder" };
	const char* Rendering::Mesh::AutogenSphereModePostfixes[] = { "Whole", "LowerHalf", "UpperHalf" };
	const char* Rendering::Mesh::AutogenNoCapsPostfix = "NoCaps";
}