add_subdirectory(buffer)

target_sources(VulkanTutorialApp
    PRIVATE
		Attachment.cpp
        Material.cpp
        Mesh.cpp
        Resource.cpp
        ResourceCommon.cpp
        Shader.cpp
        Texture.cpp
        VertexDeclaration.cpp

		Attachment.h
        Material.h
        Mesh.h
        Resource.h
        ResourceCommon.h
        Shader.h
        Texture.h
        VertexDeclaration.h
)