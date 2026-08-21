#ifndef SETTINGS_INCLUDED
#  define SETTINGS_INCLUDED

#  include <Hydrogen.hpp>

struct Settings
{
    static inline jmn::Size constexpr DebugBitIndex = 0;
    static inline jmn::Size constexpr VSyncBitIndex = 1;
    static inline jmn::Size constexpr HDRBitIndex   = 2;

    using Flags = jmn::B64;
    enum FlagBits : Flags
    {
        DebugBit = (Flags)1 << DebugBitIndex,
        VSyncBit = (Flags)1 << VSyncBitIndex,
        HDRBit   = (Flags)1 << HDRBitIndex,
    };

    Flags flags;
};

jmn::B8 Create(jmn::MemoryArena::TemporaryMemory &&, jmn::CString8 const &filename, Settings &settings, jmn::Result &result);
void    Destroy(Settings &app, jmn::CString8 const &filename);

#endif // SETTINGS_INCLUDED

#ifdef __INTELLISENSE__
#  define SETTINGS_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef SETTINGS_IMPLEMENTATION
#  ifndef SETTINGS_IMPLEMENTATED
#    define SETTINGS_IMPLEMENTATED

#    include <Windows.h>

jmn::B8 Create(jmn::MemoryArena::TemporaryMemory &&tm, jmn::CString8 const &filename, Settings &settings, jmn::Result &result)
{
    using namespace jmn;

    B8 success = false;
    auto const file = CreateFileA(filename.string, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    JMN_CHECK(file != INVALID_HANDLE_VALUE, result, Result::ErrorGeneric, ex0);

    Size file_size = 0;
    JMN_CHECK(GetFileSizeEx(file, (PLARGE_INTEGER)&file_size), result, Result::ErrorGeneric, ex1);

    auto file_data = NullAddr;
    if (!tm.arena.Push(file_size, file_data, result)) goto ex1;

    Size bytes_read = 0;
    JMN_CHECK(ReadFile(file, (LPVOID)file_data, (DWORD)file_size, (LPDWORD)&bytes_read, NULL), result, Result::ErrorGeneric, ex1);



    success = true;
ex1:JMN_ASSERT(CloseHandle(file));
ex0:return success;
}

void Destroy(Settings &settings, jmn::CString8 const &filename)
{
    auto const file = CreateFileA(filename.string, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_READONLY | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if (file != INVALID_HANDLE_VALUE)
    {


        JMN_ASSERT(CloseHandle(file));
    }
}

#  endif // SETTINGS_IMPLEMENTATED
#endif // SETTINGS_IMPLEMENTATION
