#include <jni.h>
#include <string>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#include <linux/uinput.h>
#include <cerrno>


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

    SendEvent();
    ~SendEvent();

    bool Initialize();
    bool Cleanup();

    bool SendInputEvent(uint16_t strType, uint16_t strCode, uint32_t strValue);

private:

    bool CreateUinputDevice();

    bool OpenDeviceFile(int32_t& outDescriptor);
    bool CloseDeviceFile(int32_t descriptor);

    int32_t _devDescriptor;
    bool _bIsInitialised;
};


// JNI Interface

static SendEvent g_SendEvent;

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventInitialize) (JNIEnv* env, jobject _this)
{
    const bool ret = g_SendEvent.Initialize();
    return jboolean(ret);
}

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventCleanup) (JNIEnv* env, jobject _this)
{
    const bool ret = g_SendEvent.Cleanup();
    return jboolean(ret);
}

extern "C" JNIEXPORT jboolean JNICALL JNI_FUNC(SendEventSendInputEvent) (JNIEnv* env, jobject _this,
                                                     jint jStrType,
                                                     jint jStrCode,
                                                     jint jStrValue)
{
    // DeviceIndex ignored for uinput implementation.

    const uint16_t strType = (uint16_t)jStrType;
    const uint16_t strCode = (uint16_t)jStrCode;
    const uint32_t strValue = (uint32_t)jStrValue;

    const bool ret = g_SendEvent.SendInputEvent(strType, strCode, strValue);
    return jboolean(ret);
}

// Func definitions

SendEvent::SendEvent()
    : _devDescriptor(0)
    , _bIsInitialised(false)
{
}

SendEvent::~SendEvent()
{
}

bool SendEvent::Initialize()
{
    if(_bIsInitialised)
        return true;

    // Create and open uinput device.
    bool ret = OpenDeviceFile(_devDescriptor);
    if(!ret) return false;

    ret = CreateUinputDevice();
    if(!ret)
    {
        CloseDeviceFile(_devDescriptor);
        return false;
    }

    _bIsInitialised = true;

    return true;
}

bool SendEvent::Cleanup()
{
    if(!_bIsInitialised)
        return true;

    RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_DEV_DESTROY));

    bool ret = CloseDeviceFile(_devDescriptor);
    if(!ret)
        return false;

    _bIsInitialised = false;

    return true;
}

bool SendEvent::SendInputEvent(uint16_t evType, uint16_t evCode, uint32_t evValue)
{
    if(!_bIsInitialised)
        return false;

    input_event event = {};
    event.type = evType;
    event.code = evCode;
    event.value = evValue;

    const ssize_t written = write(_devDescriptor, &event, sizeof(event));

    const bool bSuccess = written == sizeof(event);

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

bool SendEvent::CreateUinputDevice()
{
    // Configure allowed events and keys.
    RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_SET_EVBIT, EV_KEY));
    RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_SET_EVBIT, EV_REL));

    // TEST
    RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_SET_KEYBIT, KEY_ESC));
    // TODO: Configure via array data.

    // Create UInput device.

    int32_t version = 0;
    (ioctl(_devDescriptor, UI_GET_VERSION, &version));  // Not checked because returns error on BPi.

    static const int INIT_ID_BUS_TYPE = BUS_USB;
    static const int INIT_ID_VENDOR = 0xDEAD;
    static const int INIT_ID_PRODUCT = 0xBEEF;
    static const char* INIT_NAME = "MavRemote UI Device";

    if(version >= 5)
    {
        uinput_setup usetup = {};
        usetup.id.bustype = INIT_ID_BUS_TYPE;
        usetup.id.vendor = INIT_ID_VENDOR;
        usetup.id.product = INIT_ID_PRODUCT;
        strcpy(usetup.name, INIT_NAME);

        RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_DEV_SETUP, &usetup));
        RET_FALSE_NONZERO(ioctl(_devDescriptor, UI_DEV_CREATE));
    }
    else if(version < 5)    // for 0 version assume the older method - works on BPi.
    {
        uinput_user_dev usetup = {};
        usetup.id.bustype = INIT_ID_BUS_TYPE;
        usetup.id.vendor = INIT_ID_VENDOR;
        usetup.id.product = INIT_ID_PRODUCT;
        strcpy(usetup.name, INIT_NAME);

        ssize_t writeRetVal = write(_devDescriptor, &usetup, sizeof(usetup));
        if(writeRetVal < 0)
        {
            return false;
        }

        int32_t ret = (ioctl(_devDescriptor, UI_DEV_CREATE));
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
