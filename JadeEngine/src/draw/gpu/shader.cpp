#include "shader.h"
#include "fs/system.h"
#include "fs/file.h"

namespace je { namespace draw { namespace gpu {

    bool shader::is_valid() const
    {
        return m_stage >= static_cast<shader_stage>(0) && m_stage < shader_stage::k_enum_size;
    }

    shader::shader(const shader_params& a_params)
        : m_stage(a_params.m_stage)
    {
    }

    shader::~shader()
    {
    }

    bool shader::init(device& a_device, const shader_params& a_params)
    {
        const data::string file_path(build_file_path(a_params.m_file_name, m_stage));
        fs::data_buffer buffer;
        fs::file module_file(file_path, fs::file::options::k_read);
        if(module_file.is_valid() == false)
        {
            return false;
        }
        if(module_file.read_all(buffer) == false)
        {
            return false;
        }
        if(init_module(a_device, buffer) == false)
        {
            return false;
        }

        return true;
    }

    const data::string& shader::stage_to_string(shader_stage stage)
    {
        static const data::string names[] = 
        {
            "vert",
            "tesc",
            "tese",
            "geom",
            "frag",
            "comp",
            "rgen",
            "rint",
            "rchit",
            "rahit",
            "rmiss"
        };
        return names[static_cast<u8>(stage)];
    }

    data::string shader::build_file_path(const data::string& file_name, shader_stage stage)
    {
        data::string file_path(file_name);
        file_path.append(".");
        file_path.append(stage_to_string(stage));
        file_path.append(JE_DATA_EXTENSION);
        return file_path;
    }

}}}