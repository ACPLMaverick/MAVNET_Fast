#include "misc.h"

namespace je { namespace util {

    const char* misc::trim_file_name(const char* a_absolute_file_name)
    {
        static const char* root_folder_name = "src";
        static const size_t num_chars_to_advance = strlen(root_folder_name) + get_file_separator().get_size();
        
        const char* root_in_name = strstr(a_absolute_file_name, root_folder_name);
        if(root_in_name != nullptr)
        {
            return root_in_name + num_chars_to_advance;
        }
        else 
        {
            return a_absolute_file_name;
        }
    }

}}