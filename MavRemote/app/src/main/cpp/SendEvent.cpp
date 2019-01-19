#include <jni.h>
#include <string>
#include <vector>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/uinput.h>
#include <cerrno>


#define SE_MOUSE_MOVEMENT_SMART_SYNC 0

#define JNI_FUNC(funcName) Java_com_example_maverick_mavremote_Server_Instrumentation_SendEventWrapper_##funcName
#define RET_FALSE_NONZERO(expression) \
{ \
    bool ret = (expression); \
    if(ret != 0) return false; \
}

//JNICALL JNI_FUNC(stringFromJNI)
//        (
//        JNIEnv *env,
//        jobject /* this */
//        )
//{
//    std::string hello = "Hello from C++";
//    return env->NewStringUTF(hello.c_str());
//}

// Declarations

class SendEvent
{
public:

    enum class DeviceType : uint8_t
    {
        Keyboard,
        Mouse,
        NUM
    };

    enum class MouseCodeType : uint8_t
    {
        Key,
        Rel,
        NUM
    };

public:

    SendEvent();
    ~SendEvent();

    bool Initialize(const std::vector<int32_t>& keyCodes, const std::vector<int32_t>& mouseCodes,
                    const std::vector<MouseCodeType>& mouseCodeTypes, int32_t badCode);
    bool Cleanup();

    bool SendInputEvent(DeviceType devType, uint16_t evType, uint16_t evCode, uint32_t evValue);

private:

    bool CreateUinputDevice(const DeviceType deviceType);

    bool OpenDeviceFile(int32_t& outDescriptor);
    bool CloseDeviceFile(int32_t descriptor);

    bool WriteToDevice(DeviceType devType, uint16_t evType, uint16_t evCode, uint32_t evValue);
    bool WriteSync(DeviceType devType);


    static const uint8_t MOUSE_MOVEMENT_SYNC_STATE_CLEAR = 0xFF;

    std::vector<int32_t> _keyCodes;
    std::vector<int32_t> _mouseCodes;
    std::vector<MouseCodeType> _mouseCodeTypes;

    int32_t _badCode;

    int32_t _devDescriptors[(size_t)DeviceType::NUM];
    bool _bIsInitialised;
#if SE_MOUSE_MOVEMENT_SMART_SYNC
    uint8_t _mouseMovementSyncState;
#endif
};


// JNI Interface

static SendEvent g_SendEvent;

template <typename T> void PopulateInitArray(JNIEnv* jEnv, jintArray& inputArray,
        std::vector<T>& outArray, T badValue)
{
    jint num = jEnv->GetArrayLength(inputArray);
    jboolean bIsCopy = JNI_FALSE;
    jint* jKeyCodesPtr = jEnv->GetIntArrayElements(inputArray, &bIsCopy);


    for(jint i = 0; i < num; ++i)
    {
        T val = (T)jKeyCodesPtr[i];
        if(val != badValue)
        {
            outArray.push_back(val);
        }
    }

    jEnv->ReleaseIntArrayElements(inputArray, jKeyCodesPtr, JNI_ABORT);
}

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventInitialize) (JNIEnv* env, jobject _this,
                                                    jintArray jKeyCodes, jintArray jMouseCodes,
                                                    jintArray jMouseCodeTypes, jint jBadCode)
{
    std::vector<int32_t> keyCodes;
    std::vector<int32_t> mouseCodes;
    std::vector<SendEvent::MouseCodeType> mouseCodeTypes;
    int32_t badCode = (int32_t)jBadCode;

    PopulateInitArray<int32_t>(env, jKeyCodes, keyCodes, badCode);
    PopulateInitArray<int32_t>(env, jMouseCodes, mouseCodes, badCode);
    PopulateInitArray<SendEvent::MouseCodeType>(env, jMouseCodeTypes,
                                                mouseCodeTypes, SendEvent::MouseCodeType::NUM);

    const bool ret = g_SendEvent.Initialize(keyCodes, mouseCodes, mouseCodeTypes, badCode);

    return jboolean(ret);
}

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventCleanup) (JNIEnv* env, jobject _this)
{
    const bool ret = g_SendEvent.Cleanup();
    return jboolean(ret);
}

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventSendInputEvent) (JNIEnv* env, jobject _this,
                                                     jint jDeviceType,
                                                     jint jStrType,
                                                     jint jStrCode,
                                                     jint jStrValue)
{
    env->PushLocalFrame(6);

    const jint maxIndex = (jint)SendEvent::DeviceType::NUM;
    if(jDeviceType < 0 || jDeviceType >= maxIndex)
    {
        return jboolean(false);
    }

    const SendEvent::DeviceType devType = (SendEvent::DeviceType)jDeviceType;
    const uint16_t strType = (uint16_t)jStrType;
    const uint16_t strCode = (uint16_t)jStrCode;
    const uint32_t strValue = (uint32_t)jStrValue;

    const bool ret = g_SendEvent.SendInputEvent(devType, strType, strCode, strValue);

    env->PopLocalFrame(nullptr);
    return jboolean(ret);
}

// Func definitions

SendEvent::SendEvent()
    : _bIsInitialised(false)
#if SE_MOUSE_MOVEMENT_SMART_SYNC
    , _mouseMovementSyncState(MOUSE_MOVEMENT_SYNC_STATE_CLEAR)
#endif
{
    for(size_t i = 0; i < (size_t)DeviceType::NUM; ++i)
    {
        _devDescriptors[i] = -1;
    }
}

SendEvent::~SendEvent()
{
}

bool SendEvent::Initialize(const std::vector<int32_t>& keyCodes, const std::vector<int32_t>& mouseCodes,
                           const std::vector<MouseCodeType>& mouseCodeTypes, int32_t badCode)
{
    if(_bIsInitialised)
        return true;

    if(mouseCodes.size() != mouseCodeTypes.size())
        return false;

    _keyCodes = keyCodes;
    _mouseCodes = mouseCodes;
    _mouseCodeTypes = mouseCodeTypes;
    _badCode = badCode;

    // Create and open uinput devices.
    for(int64_t i = 0; i < (int64_t)DeviceType::NUM; ++i)
    {
        bool ret = OpenDeviceFile(_devDescriptors[i]);
        if (!ret)
        {
            for(int64_t j = i - 1; j >= 0; --j)
            {
                CloseDeviceFile(_devDescriptors[j]);
            }
            return false;
        }
    }

    bool ret = CreateUinputDevice(DeviceType::Keyboard);
    ret &= CreateUinputDevice(DeviceType::Mouse);

    if(!ret)
    {
        for(size_t i = 0; i < (size_t)DeviceType::NUM; ++i)
        {
            CloseDeviceFile(_devDescriptors[i]);
        }
        return false;
    }

    _bIsInitialised = true;

    return true;
}

bool SendEvent::Cleanup()
{
    if(!_bIsInitialised)
        return true;

    bool ret = true;

    for(size_t i = 0; i < (size_t)DeviceType::NUM; ++i)
    {
        (ioctl(_devDescriptors[i], UI_DEV_DESTROY));
        ret &= CloseDeviceFile(_devDescriptors[i]);
    }

    _bIsInitialised = !ret;

    return ret;
}

bool SendEvent::SendInputEvent(DeviceType devType, uint16_t evType, uint16_t evCode, uint32_t evValue)
{
//    if(!_bIsInitialised)
//        return false;

    bool bSuccess = WriteToDevice(devType, evType, evCode, evValue);

#if SE_MOUSE_MOVEMENT_SMART_SYNC
    const bool bIsMouseMovement = (devType == DeviceType::Mouse
            && evType == EV_REL && (evCode == REL_X || evCode == REL_Y));

    if(bIsMouseMovement)
    {
        switch(_mouseMovementSyncState)
        {
            case MOUSE_MOVEMENT_SYNC_STATE_CLEAR:
                _mouseMovementSyncState = (uint8_t)evCode;
                break;
            case REL_X:
                if(evCode == REL_Y)
                {
                    bSuccess &= WriteSync(devType);
                    _mouseMovementSyncState = MOUSE_MOVEMENT_SYNC_STATE_CLEAR;
                }
                break;
            case REL_Y:
                if(evCode == REL_X)
                {
                    bSuccess &= WriteSync(devType);
                    _mouseMovementSyncState = MOUSE_MOVEMENT_SYNC_STATE_CLEAR;
                }
                break;
            default:
                break;
        }
    }
    else
#endif
    {
        bSuccess &= WriteSync(devType);
    }

    return bSuccess;
}

bool SendEvent::OpenDeviceFile(int32_t& outDescriptor)
{
    outDescriptor = open("/dev/uinput", O_WRONLY | O_NONBLOCK);
    return outDescriptor >= 0;
}

bool SendEvent::CloseDeviceFile(int32_t descriptor)
{
    int ret = close(descriptor);
    return ret == 0;
}

bool SendEvent::CreateUinputDevice(const DeviceType deviceType)
{
    const int32_t devDescriptor = _devDescriptors[(size_t)deviceType];


    // Append allowed keycodes.

    switch(deviceType)
    {
        case DeviceType::Keyboard:
        {
            // Configure allowed events and keys.
            RET_FALSE_NONZERO(ioctl(devDescriptor, UI_SET_EVBIT, EV_KEY));

            for(int32_t keyCode : _keyCodes)
            {
                RET_FALSE_NONZERO(ioctl(devDescriptor, UI_SET_KEYBIT, keyCode));
            }
        }
            break;
        case DeviceType::Mouse:
        {
            // Configure allowed events and keys.
            RET_FALSE_NONZERO(ioctl(devDescriptor, UI_SET_EVBIT, EV_KEY));
            RET_FALSE_NONZERO(ioctl(devDescriptor, UI_SET_EVBIT, EV_REL));

            const size_t mouseCodeNum = _mouseCodes.size();
            for(size_t i = 0; i < mouseCodeNum; ++i)
            {
                int32_t mouseCode = _mouseCodes[i];
                MouseCodeType mouseCodeType = _mouseCodeTypes[i];
                int32_t codeType;

                switch(mouseCodeType)
                {
                case MouseCodeType::Rel:
                    codeType = UI_SET_RELBIT;
                    break;
                case MouseCodeType::Key:
                default:
                    codeType = UI_SET_KEYBIT;
                    break;
                }

                RET_FALSE_NONZERO(ioctl(devDescriptor, codeType, mouseCode));
            }
        }
            break;
        default:
            break;
    }

    // Create UInput device.

    int32_t version = 0;
    (ioctl(devDescriptor, UI_GET_VERSION, &version));  // Not checked because returns error on BPi.

    static const int INIT_ID_BUS_TYPE = BUS_USB;
    static const int INIT_ID_VENDOR = 0xDEAD;
    static const int INIT_ID_PRODUCT = 0xBEEF;
    static const char* INIT_NAMES[] =
    {
            "MavRemote Keyboard",
            "MavRemove Mouse"
    };

    if(version >= 5)
    {
        uinput_setup usetup = {};
        usetup.id.bustype = INIT_ID_BUS_TYPE;
        usetup.id.vendor = INIT_ID_VENDOR;
        usetup.id.product = INIT_ID_PRODUCT;
        strcpy(usetup.name, INIT_NAMES[(size_t)deviceType]);

        RET_FALSE_NONZERO(ioctl(devDescriptor, UI_DEV_SETUP, &usetup));
        RET_FALSE_NONZERO(ioctl(devDescriptor, UI_DEV_CREATE));
    }
    else if(version < 5)    // for 0 version assume the older method - works on BPi.
    {
        uinput_user_dev usetup = {};
        usetup.id.bustype = INIT_ID_BUS_TYPE;
        usetup.id.vendor = INIT_ID_VENDOR;
        usetup.id.product = INIT_ID_PRODUCT;
        strcpy(usetup.name, INIT_NAMES[(size_t)deviceType]);

        ssize_t writeRetVal = write(devDescriptor, &usetup, sizeof(usetup));
        if(writeRetVal < 0)
        {
            return false;
        }

        int32_t ret = (ioctl(devDescriptor, UI_DEV_CREATE));
        if(ret < 0)
        {
            const char* err = strerror(errno);
            return false;
        }
    }
    else
    {
        return false;
    }


    return true;
}

bool SendEvent::WriteToDevice(SendEvent::DeviceType devType, uint16_t evType, uint16_t evCode,
                              uint32_t evValue)
{
    input_event event = {};
    event.type = evType;
    event.code = evCode;
    event.value = evValue;

    ssize_t written = write(_devDescriptors[(size_t)devType], &event, sizeof(event));
    return written == sizeof(event);
}

bool SendEvent::WriteSync(SendEvent::DeviceType devType)
{
    return WriteToDevice(devType, EV_SYN, SYN_REPORT, 0);
}
