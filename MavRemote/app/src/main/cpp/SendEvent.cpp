#include <jni.h>
#include <string>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>


#define JNI_FUNC(funcName) Java_com_example_maverick_mavremote_Server_Instrumentation_SendEventWrapper_##funcName

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

    enum class DeviceIndex : uint8_t
    {
        Keyboard = 0,
        Mouse = 1,
        NUM,
    };

private:

    struct InputData
    {
        timeval Timestamp;
        uint16_t Type;
        uint16_t Code;
        int32_t Value;
    };

    const char* DeviceNames[(uint32_t)DeviceIndex::NUM] =
    {
        "/dev/input/event4", // TODO: This is hw-keyboard, for sw-keyboard, use event1.
        "/dev/input/event0" // vmouse
    };

public:

    SendEvent();
    ~SendEvent();

    bool Initialize();
    bool Cleanup();

    bool SendInputEvent(DeviceIndex deviceIndex,
                        const char* strType, const char* strCode, const char* strValue);

    const char* GetDeviceName(DeviceIndex deviceIndex);

private:

    bool OpenDeviceFile(const char* path, int32_t& outDescriptor);
    bool CloseDeviceFile(int32_t descriptor);


    int32_t _deviceFiles[(uint32_t)DeviceIndex::NUM];

    bool _bIsInitialised;
};


// JNI Interface

static SendEvent g_SendEvent;

void JCharArrayToCharArray(JNIEnv* env, char* dstChars, const jcharArray& srcJChars, const uint32_t arrayLength)
{
    for(uint32_t i = 0; i < arrayLength; ++i)
    {
        jchar srcChar;
        env->GetCharArrayRegion(srcJChars, i, 1, &srcChar);

        char dstChar = (char)(srcChar);
        dstChars[i] = dstChar;
    }
}

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
                                                    jint deviceIndex,
                                                     jcharArray jStrType,
                                                     jcharArray jStrCode,
                                                     jcharArray jStrValue)
{
    static const uint32_t STR_TYPE_SIZE = 4;
    static const uint32_t STR_CODE_SIZE = 4;
    static const uint32_t STR_VALUE_SIZE = 8;

    if(deviceIndex < 0 || deviceIndex >= (jint)SendEvent::DeviceIndex::NUM)
        return jboolean(false);

    if(env->GetArrayLength(jStrType) != STR_TYPE_SIZE)
        return jboolean(false);
    if(env->GetArrayLength(jStrCode) != STR_CODE_SIZE)
        return jboolean(false);
    if(env->GetArrayLength(jStrValue) != STR_VALUE_SIZE)
        return jboolean(false);

    char strType[STR_TYPE_SIZE] = {};
    char strCode[STR_CODE_SIZE] = {};
    char strValue[STR_VALUE_SIZE] = {};

    JCharArrayToCharArray(env, strType, jStrType, STR_TYPE_SIZE);
    JCharArrayToCharArray(env, strCode, jStrCode, STR_CODE_SIZE);
    JCharArrayToCharArray(env, strValue, jStrValue, STR_VALUE_SIZE);

    const bool ret = g_SendEvent.SendInputEvent(
            (SendEvent::DeviceIndex)deviceIndex, strType, strCode, strValue);
    return jboolean(ret);
}

extern "C" JNIEXPORT jstring JNICALL JNI_FUNC(SendEventGetDeviceName)(JNIEnv* env, jobject _this, jint deviceIndex)
{
    if(deviceIndex < 0 || deviceIndex >= (jint)SendEvent::DeviceIndex::NUM)
        return env->NewStringUTF("/dev/null");

    const char* ret = g_SendEvent.GetDeviceName((SendEvent::DeviceIndex)deviceIndex);
    jstring retStr = env->NewStringUTF(ret);
    return retStr;
}


// Func definitions

SendEvent::SendEvent()
    : _bIsInitialised(false)
{
    for(uint32_t i = 0; i < (uint32_t)DeviceIndex::NUM; ++i)
    {
        _deviceFiles[i] = -1;
    }
}

SendEvent::~SendEvent()
{

}

bool SendEvent::Initialize()
{
    if(_bIsInitialised)
        return true;

    for(uint32_t i = 0; i < (uint32_t)DeviceIndex::NUM; ++i)
    {
        bool ret = OpenDeviceFile(DeviceNames[i], _deviceFiles[i]);
        if(!ret)
            return false;
    }

    _bIsInitialised = true;

    return true;
}

bool SendEvent::Cleanup()
{
    if(!_bIsInitialised)
        return true;

    for(uint32_t i = 0; i < (uint32_t)DeviceIndex::NUM; ++i)
    {
        if(_deviceFiles[i] < 0)
            continue;

        bool ret = CloseDeviceFile(_deviceFiles[i]);
        if(!ret)
            return false;
    }

    _bIsInitialised = false;

    return true;
}

bool SendEvent::SendInputEvent(DeviceIndex deviceIndex,
                               const char* strType, const char* strCode, const char* strValue)
{
    if(!_bIsInitialised)
        return false;

    InputData event = {};
    event.Type = (uint16_t)atoi(strType);
    event.Code = (uint16_t)atoi(strCode);
    event.Value = atoi(strValue);

    ssize_t ret = write(_deviceFiles[(uint32_t)deviceIndex], &event, sizeof(event));

    return ret == sizeof(event);
}

bool SendEvent::OpenDeviceFile(const char* path, int32_t &outDescriptor)
{
    outDescriptor = open(path, O_RDWR);
    return outDescriptor >= 0;
}

bool SendEvent::CloseDeviceFile(int32_t descriptor)
{
    int ret = close(descriptor);
    return ret == 0;
}

const char* SendEvent::GetDeviceName(SendEvent::DeviceIndex deviceIndex)
{
    return DeviceNames[(uint32_t)deviceIndex];
}
