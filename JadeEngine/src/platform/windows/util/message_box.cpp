#include "util/message_box.h"
#include "platform/windows/platform.h"

namespace je { namespace platform {

    message_box::button_flag message_box::show(const char* a_message, info_level a_information_level, int a_buttons, bool a_is_blocking)
    {
        static const char* title = "Jade Engine Assertion Failed";
        const HWND hwnd = NULL; // TODO Assign the HWND Here.
        UINT contents = MB_OK;
        if(a_buttons & button_flag::k_retry && a_buttons & button_flag::k_cancel && a_buttons & button_flag::k_continue)
        {
            contents = MB_CANCELTRYCONTINUE | MB_DEFBUTTON2;
        }
        else if(a_buttons & button_flag::k_retry && a_buttons & button_flag::k_cancel)
        {
            contents = MB_RETRYCANCEL;
        }
        else if(a_buttons & button_flag::k_yes && a_buttons & button_flag::k_no && a_buttons & button_flag::k_cancel)
        {
            contents = MB_YESNOCANCEL;
        }
        else if(a_buttons & button_flag::k_yes && a_buttons & button_flag::k_no)
        {
            contents = MB_YESNO;
        }
        else if(a_buttons & button_flag::k_yes && a_buttons & button_flag::k_cancel)
        {
            contents = MB_OKCANCEL;
        }
        else
        {
            JE_print("Error! Unsupported message box flag combination.");
#if JE_CONFIG_DEBUG
            je::platform::util::debugbreak();
#endif
        }

        if(a_information_level == info_level::k_error)
        {
            contents |= MB_ICONERROR;
        }
        else if(a_information_level == info_level::k_warning)
        {
            contents |= MB_ICONWARNING;
        }
        else
        {
            contents |= MB_ICONINFORMATION;
        }

        const int ret_flags = MessageBoxA(hwnd, a_message, title, contents);
        
        switch(ret_flags)
        {
        case IDABORT:
            return button_flag::k_cancel;
        case IDCANCEL:
            return button_flag::k_cancel;
        case IDCONTINUE:
            return button_flag::k_continue;
        case IDIGNORE:
            return button_flag::k_continue;
        case IDNO:
            return button_flag::k_no;
        case IDOK:
            return button_flag::k_yes;
        case IDRETRY:
            return button_flag::k_retry;
        case IDTRYAGAIN:
            return button_flag::k_retry;
        case IDYES:
            return button_flag::k_yes;
        default:
            JE_print("Error! Unsupported message box flag combination.");
            return button_flag::k_cancel;
        }
    }

}}