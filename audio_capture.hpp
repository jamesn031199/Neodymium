#ifndef AUDIO_CAPTURE_INCLUDED
#  define AUDIO_CAPTURE_INCLUDED

#  include <Windows.h>
#  include <mmdeviceapi.h>
#  include <Audioclient.h>
#  include <Hydrogen.hpp>

struct AudioCapture
{
    HANDLE                  sync_event;
    HANDLE                  thread;
    IMMDeviceEnumerator     *dev_enum;
    IMMDevice               *dev;
    IAudioClient            *ac;
    WAVEFORMATEX            *fmt;
    IAudioCaptureClient     *acc;
    CRITICAL_SECTION         cs;
    jmn::U32                 index, count;
    jmn::V2F32              *raw_buffer;
};

jmn::B8 Create(jmn::Allocator allocator, AudioCapture &ac, jmn::Result &result);
void    Destroy(AudioCapture &ac);

#endif // AUDIO_CAPTURE_INCLUDED

#ifdef __INTELLISENSE__
#  define AUDIO_CAPTURE_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef AUDIO_CAPTURE_IMPLEMENTATION
#  ifndef AUDIO_CAPTURE_IMPLEMENTATED
#    define AUDIO_CAPTURE_IMPLEMENTATED

#    define HR_CHECK(expr, resvar, resval, jmplbl) JMN_CHECK(SUCCEEDED(expr), resvar, resval, jmplbl)
#    define SafeRelease(ptr) if (ptr) { ptr->Release(); ptr = NULL; } else {}

namespace AudioCaptureInternal
{

    struct EntryPointData
    {
        jmn::Allocator allocator;
        AudioCapture  &audio_capture;
    };

    static jmn::B8 CreateAudioObjects(AudioCapture &ac, jmn::Result &result)
    {
        using namespace jmn;

        HR_CHECK(CoInitializeEx(NULL, COINIT_SPEED_OVER_MEMORY | COINIT_DISABLE_OLE1DDE), result, Result::ErrorGeneric, ex0);
        HR_CHECK(CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, IID_PPV_ARGS(&ac.dev_enum)), result, Result::ErrorGeneric, ex1);
        HR_CHECK(ac.dev_enum->GetDefaultAudioEndpoint(eRender, eConsole, &ac.dev), result, Result::ErrorGeneric, ex2);
        HR_CHECK(ac.dev->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **)&ac.ac), result, Result::ErrorGeneric, ex3);
        HR_CHECK(ac.ac->GetMixFormat(&ac.fmt), result, Result::ErrorGeneric, ex4);
        HR_CHECK(ac.ac->Initialize(AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, 0, 0, ac.fmt, NULL), result, Result::ErrorGeneric, ex5);
        HR_CHECK(ac.ac->GetService(IID_PPV_ARGS(&ac.acc)), result, Result::ErrorGeneric, ex5);
        HR_CHECK(ac.ac->Start(), result, Result::ErrorGeneric, ex6);
        JMN_CHECK(InitializeCriticalSectionAndSpinCount(&ac.cs, 4096), result, Result::ErrorGeneric, ex7);

        return true;
    //ex8:DeleteCriticalSection(&ac.cs);
    ex7:JMN_ASSERT(SUCCEEDED(ac.ac->Stop()));
    ex6:SafeRelease(ac.acc);
    ex5:CoTaskMemFree(ac.fmt);
    ex4:SafeRelease(ac.ac);
    ex3:SafeRelease(ac.dev);
    ex2:SafeRelease(ac.dev_enum);
    ex1:CoUninitialize();
    ex0:return false;
    }

    static void DestroyAudioObjects(AudioCapture &ac)
    {
        DeleteCriticalSection(&ac.cs);
        JMN_ASSERT(SUCCEEDED(ac.ac->Stop()));
        SafeRelease(ac.acc);
        CoTaskMemFree(ac.fmt);
        SafeRelease(ac.ac);
        SafeRelease(ac.dev);
        SafeRelease(ac.dev_enum);
        CoUninitialize();
    }

    static jmn::B8 CreateAudioBuffers(AudioCapture &ac, jmn::Allocator allocator, jmn::Result &result)
    {
        using namespace jmn;

        ac.count = (U32)ac.fmt->nSamplesPerSec;

        if (!allocator.Alloc(ac.count, ac.raw_buffer, result)) goto ex0;

        return true;
    ex0:return false;
    }

    static void DestroyAudioBuffers(AudioCapture &ac, jmn::Allocator allocator)
    {
        allocator.Free(ac.raw_buffer, ac.count);
    }

    static jmn::B8 ProcessPCM(AudioCapture &, jmn::Result &result)
    {
        result = jmn::Result::ErrorNotSupported;
        return false;
    }

    static jmn::B8 ProcessFloat(AudioCapture &ac, jmn::Result &result)
    {
        using namespace jmn;

        Addr  packet_addr   = NullAddr;
        U32   packet_length = 0;
        DWORD packet_flags  = 0;

        HR_CHECK(ac.acc->GetNextPacketSize((UINT32 *)&packet_length), result, Result::ErrorGeneric, ex0);

        if (!packet_length)
        {
        }
        else while (packet_length)
        {
            HR_CHECK(ac.acc->GetBuffer((BYTE **)&packet_addr, (UINT32 *)&packet_length, &packet_flags, NULL, NULL), result, Result::ErrorGeneric, ex0);

            auto const packet_ptr = (V2F32 const *)packet_addr;

            EnterCriticalSection(&ac.cs);
            {
                ac.index = JMN_WRAPPED_DEC(ac.index, packet_length, ac.count);
                if (packet_flags & AUDCLNT_BUFFERFLAGS_SILENT)
                {
                    for (U32 i = 0; i < packet_length; ++i)
                    {
                        ac.raw_buffer[JMN_WRAPPED_INC(ac.index, i, ac.count)] ={};
                    }
                }
                else
                {
                    for (U32 i = 0; i < packet_length; ++i)
                    {
                        ac.raw_buffer[JMN_WRAPPED_INC(ac.index, i, ac.count)] = packet_ptr[i];
                    }
                }
            }
            LeaveCriticalSection(&ac.cs);

            HR_CHECK(ac.acc->ReleaseBuffer((UINT32)packet_length), result, Result::ErrorGeneric, ex0);
            HR_CHECK(ac.acc->GetNextPacketSize((UINT32 *)&packet_length), result, Result::ErrorGeneric, ex0);
        }

        return true;
    ex0:return false;
    }

    jmn::B8 Process(AudioCapture &ac, jmn::Result &result)
    {
        using namespace jmn;

        switch (ac.fmt->wFormatTag)
        {
            case WAVE_FORMAT_PCM:        return AudioCaptureInternal::ProcessPCM(ac, result);
            case WAVE_FORMAT_IEEE_FLOAT: return AudioCaptureInternal::ProcessFloat(ac, result);
            case WAVE_FORMAT_EXTENSIBLE:
            {
                auto const fmtex = (WAVEFORMATEXTENSIBLE *)ac.fmt;
                if (IsEqualGUID(fmtex->SubFormat, KSDATAFORMAT_SUBTYPE_PCM))
                {
                    return AudioCaptureInternal::ProcessPCM(ac, result);
                }
                else if (IsEqualGUID(fmtex->SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT))
                {
                    return AudioCaptureInternal::ProcessFloat(ac, result);
                }
            } break;
        }

        result = jmn::Result::ErrorNotSupported;
        return false;
    }

    static DWORD CALLBACK ThreadEntryPoint(LPVOID lpParameter)
    {
        using namespace jmn;

        auto       result    = Result::Success;
        auto const allocator = ((EntryPointData *)lpParameter)->allocator;
        auto      &ac        = ((EntryPointData *)lpParameter)->audio_capture;

        if (!CreateAudioObjects(ac, result)) goto ex0;
        if (!CreateAudioBuffers(ac, allocator, result)) goto ex1;

        // Currently checking audio format every tick, which isn't needed but not impactful in performance
        for (SetEvent(ac.sync_event); WaitForSingleObject(ac.sync_event, 0) == WAIT_OBJECT_0;)
            if (!Process(ac, result)) ResetEvent(ac.sync_event);

        DestroyAudioBuffers(ac, allocator);
    ex1:DestroyAudioObjects(ac);
    ex0:return (DWORD)result;
    }

}

jmn::B8 Create(jmn::Allocator allocator, AudioCapture &ac, jmn::Result &result)
{
    using namespace jmn;

    AudioCaptureInternal::EntryPointData data ={ allocator, ac };

    JMN_CHECK(ac.sync_event = CreateEvent(NULL, TRUE, FALSE, NULL), result, Result::ErrorGeneric, ex0);
    JMN_CHECK(ac.thread = CreateThread(NULL, 0, AudioCaptureInternal::ThreadEntryPoint, &data, 0, NULL), result, Result::ErrorGeneric, ex1);

    WaitForSingleObject(ac.sync_event, INFINITE);

    return true;
//ex2:JMN_ASSERT(CloseHandle(ac.sync_event));
ex1:JMN_ASSERT(CloseHandle(ac.thread));
ex0:return false;
}

void Destroy(AudioCapture &ac)
{
    ResetEvent(ac.sync_event);
    JMN_ASSERT(WaitForSingleObject(ac.thread, INFINITE) == WAIT_OBJECT_0);
    JMN_ASSERT(CloseHandle(ac.thread));
    JMN_ASSERT(CloseHandle(ac.sync_event));
}

#  endif // AUDIO_CAPTURE_IMPLEMENTATED
#endif // AUDIO_CAPTURE_IMPLEMENTATION
