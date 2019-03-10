add_subdirectory(buffer)

target_sources(VulkanTutorialApp
    PRIVATE
        Material.cpp
        Mesh.cpp
        Resource.cpp
        ResourceCommon.cpp
        Shader.cpp
        Texture.cpp
        VertexDeclaration.cpp

        Material.h
        Mesh.h
        Resource.h
        ResourceCommon.h
        Shader.h
        Texture.h
        VertexDeclaration.h
)