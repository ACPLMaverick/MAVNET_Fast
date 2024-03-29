#pragma once

#include "global.h"
#include "mem/allocatable.h"
#include "params.h"

namespace je { namespace draw { namespace gpu {

    class device;

    using shader_hash = data::string::hash;

    // Represents a single shader module. 
    // Used for creating a render state.
    // TODO This should be inside a "resource" managed by the Resource Manager later on.
    class shader : public mem::allocatable
    {
    public:

        bool is_valid() const;
        shader_stage get_stage() const { return m_stage; }
        shader_hash get_hash() const { return m_hash; }

    protected:

        // File name with cooked format (_ instead of dir separator), without the stage specific extension.
        shader(const shader_params& params);
        virtual ~shader();

        bool init(device& a_device, const shader_params& params);
        virtual void shutdown(device& a_device) = 0;
        virtual bool init_module(device& a_device, const data::array<u8>& data) = 0;

        static const data::string& stage_to_string(shader_stage stage);
        static data::string build_file_path(const data::string& file_name, shader_stage stage);

    protected:

        shader_stage m_stage;
        shader_hash m_hash;

    protected:

        friend class factory;

    };

}}}