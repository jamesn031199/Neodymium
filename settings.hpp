#ifndef SETTINGS_INCLUDED
#  define SETTINGS_INCLUDED

#  include <Hydrogen.hpp>
#  include <Helium.hpp>

struct Settings
{
    static inline jmn::Size constexpr DebugBitIndex      = 0;
    static inline jmn::Size constexpr VSyncBitIndex      = 1;
    static inline jmn::Size constexpr FullScreenBitIndex = 2;
    static inline jmn::Size constexpr HDRBitIndex        = 3;

    using Flags = jmn::B64;
    enum FlagBits : Flags
    {
        DebugBit      = (Flags)1 << DebugBitIndex,
        VSyncBit      = (Flags)1 << VSyncBitIndex,
        FullScreenBit = (Flags)1 << FullScreenBitIndex,
        HDRBit        = (Flags)1 << HDRBitIndex,
    };

    Flags      flags;
    jmn::V2U32 window_size;
};

jmn::B8 Create(jmn::Allocator allocator, jmn::CString16 const &filename, Settings &settings, jmn::Result &result);
void    Destroy(Settings &app, jmn::CString16 const &filename);

#endif // SETTINGS_INCLUDED

#ifdef __INTELLISENSE__
#  define SETTINGS_IMPLEMENTATION
#endif // __INTELLISENSE__

#ifdef SETTINGS_IMPLEMENTATION
#  ifndef SETTINGS_IMPLEMENTATED
#    define SETTINGS_IMPLEMENTATED

#    include <Windows.h>
#    include <stb_sprintf.h>

namespace SettingsInternal
{

    static inline jmn::CString8 constexpr BooleanTrueTxt  = jmn::MakeCString("true");
    static inline jmn::CString8 constexpr BooleanFalseTxt = jmn::MakeCString("false");

    static char *WriteFileSTBSPCallback(char const *buffer, void *user_data, int length)
    {
        auto const file = (HANDLE)user_data;

        jmn::U64 bytes_written = 0;
        JMN_ASSERT(WriteFile(file, (LPCVOID)buffer, (DWORD)length, (LPDWORD)&bytes_written, NULL));

        JMN_ASSERT(SetFilePointerEx(file, *(PLARGE_INTEGER)&bytes_written, NULL, FILE_CURRENT));

        return (char *)buffer;
    }

    static void WriteFileSTBSP(HANDLE file, jmn::C8 const *format, ...)
    {
        char buffer[STB_SPRINTF_MIN];
        va_list ap;
        va_start(ap, format);
        stbsp_vsprintfcb(WriteFileSTBSPCallback, (void *)file, buffer, format, ap);
        va_end(ap);
    }

    static jmn::B8 ParseBoolean(jmn::C8 const *txt_beg, jmn::C8 const *txt_end)
    {
        auto const txt_len = (jmn::Size)(txt_end - txt_beg);
        if (txt_len == 1)
        {
            switch (*txt_beg)
            {
                case '1': return true;
                case '0': return false;
            }
        }
        if ((txt_len == BooleanTrueTxt.length) && (strncmp(txt_beg, BooleanTrueTxt.string, BooleanTrueTxt.length) == 0))
        {
            return true;
        }
        if ((txt_len == BooleanFalseTxt.length) && (strncmp(txt_beg, BooleanFalseTxt.string, BooleanFalseTxt.length) == 0))
        {
            return false;
        }
        return false;
    }

    static void InitializeDefault(Settings &settings)
    {
        settings.flags       = 0;
        settings.flags      |= Settings::VSyncBit;
        settings.flags      |= Settings::HDRBit;
        settings.window_size = { 1280, 720 };
    }

    static void ReadSetting(Settings &settings, jmn::C8 const *key_beg, jmn::C8 const *key_end, jmn::C8 const *val_beg, jmn::C8 const *val_end)
    {
        auto const key_len = (jmn::Size)(key_end - key_beg);
        if ((key_len == DebugSettingName.length) && (strncmp(key_beg, DebugSettingName.string, DebugSettingName.length) == 0))
        {
            if (ParseBoolean(val_beg, val_end))
            {
                settings.flags |=  Settings::DebugBit;
            }
            else
            {
                settings.flags &=~ Settings::DebugBit;
            }
        }
        else if ((key_len == VSyncSettingName.length) && (strncmp(key_beg, VSyncSettingName.string, VSyncSettingName.length) == 0))
        {
            if (ParseBoolean(val_beg, val_end))
            {
                settings.flags |=  Settings::VSyncBit;
            }
            else
            {
                settings.flags &=~ Settings::VSyncBit;
            }
        }
        else if ((key_len == FullScreenSettingName.length) && (strncmp(key_beg, FullScreenSettingName.string, FullScreenSettingName.length) == 0))
        {
            if (ParseBoolean(val_beg, val_end))
            {
                settings.flags |=  Settings::FullScreenBit;
            }
            else
            {
                settings.flags &=~ Settings::FullScreenBit;
            }
        }
        else if ((key_len == HDRSettingName.length) && (strncmp(key_beg, HDRSettingName.string, HDRSettingName.length) == 0))
        {
            if (ParseBoolean(val_beg, val_end))
            {
                settings.flags |=  Settings::HDRBit;
            }
            else
            {
                settings.flags &=~ Settings::HDRBit;
            }
        }
        else if ((key_len == WindowSizeSettingName.length) && (strncmp(key_beg, WindowSizeSettingName.string, WindowSizeSettingName.length) == 0))
        {
        }
    }

    static void ReadFileData(Settings &settings, jmn::Addr file_data, jmn::Size file_size)
    {
        using namespace jmn;

        auto const file_beg = (C8 const *)file_data;
        auto const file_end = (C8 const *)(file_data + file_size);

        C8 const *line_beg = NULL;
        C8 const *line_end = NULL;

        for (line_beg = file_beg; line_beg < file_end; line_beg = (line_end + 1))
        {
            // Find end of line, indicated by newline character
            for (line_end = line_beg; line_end < file_end; ++line_end)
                if (*line_end == '\n') break;

            // Trim whitespace from the end of the line
            for (; line_beg < line_end; --line_end)
                if ((*(line_end - 1) != ' ') && (*(line_end - 1) != '\t')) break;

            // Skip empty lines
            if ((line_end - line_beg) == 0) continue;

            // Trim whitespace from the beginning of the line
            for (; line_beg < line_end; ++line_beg)
                if ((*line_beg != ' ') && (*line_beg != '\t')) break;

            // Skip comments
            if (*line_beg == '#') continue;

            C8 const *key_beg = line_beg;
            C8 const *key_end = NULL;
            C8 const *val_beg = NULL;
            C8 const *val_end = NULL;

            // Find end of key, indicated by a colon
            for (key_end = line_beg; key_end <= line_end; ++key_end)
                if (*key_end == ':') break;

            // Skip keys with no vals
            if ((key_end + 1) >= line_end) continue;

            // Trim whitespace from the beginning of the val
            for (val_beg = (key_end + 1); val_beg < line_end; ++val_beg)
                if ((*val_beg != ' ') && (*val_beg != '\t')) break;

            // Skip keys with empty vals
            if (val_beg >= line_end) continue;

            // Trim whitespace from the end of the val
            for (val_end = line_end; val_beg < val_end; --val_end)
                if ((*(val_end - 1) != ' ') && (*(val_end - 1) != '\t')) break;

            // Trim whitespace from the end of the key
            for (; key_beg < key_end; --key_end)
                if ((*(key_end - 1) != ' ') && (*(key_end - 1) != '\t')) break;

            ReadSetting(settings, key_beg, key_end, val_beg, val_end);
        }
    }

    static void WriteFileData(Settings const &settings, HANDLE file)
    {
        WriteFileSTBSP(file,
        "# Settings file for Neodymium\n"
        "%s : %s\n"
        "%s : %s\n"
        "%s : %s\n"
        "%s : %s\n"
        "%s : [%d, %d]\n"
          , DebugSettingName.string, (settings.flags & Settings::DebugBit) ? "1" : "0",
            VSyncSettingName.string, (settings.flags & Settings::VSyncBit) ? "1" : "0",
            FullScreenSettingName.string, (settings.flags & Settings::FullScreenBit) ? "1" : "0",
            HDRSettingName.string, (settings.flags & Settings::HDRBit) ? "1" : "0",
            WindowSizeSettingName.string, settings.window_size.x, settings.window_size.y
        );
    }

}

jmn::B8 Create(jmn::Allocator allocator, jmn::CString16 const &filename, Settings &settings, jmn::Result &result)
{
    using namespace jmn;

    B8 success = false;
    auto const file = CreateFile((LPCWSTR)filename.string, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    JMN_CHECK(file != INVALID_HANDLE_VALUE, result, Result::ErrorGeneric, ex0);

    Size file_size = 0;
    JMN_CHECK(GetFileSizeEx(file, (PLARGE_INTEGER)&file_size), result, Result::ErrorGeneric, ex1);

    auto file_data = NullAddr;
    if (!allocator.Alloc(file_size, alignof(void *), file_data, result)) goto ex1;

    Size bytes_read = 0;
    JMN_CHECK(ReadFile(file, (LPVOID)file_data, (DWORD)file_size, (LPDWORD)&bytes_read, NULL), result, Result::ErrorGeneric, ex2);

    SettingsInternal::InitializeDefault(settings);
    SettingsInternal::ReadFileData(settings, file_data, file_size);

    success = true;
ex2:allocator.Free(file_data, file_size);
ex1:JMN_ASSERT(CloseHandle(file));
ex0:return success;
}

void Destroy(Settings &settings, jmn::CString16 const &filename)
{
    auto const file = CreateFile((LPCWSTR)filename.string, GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    if (file != INVALID_HANDLE_VALUE)
    {
        SettingsInternal::WriteFileData(settings, file);
        JMN_ASSERT(CloseHandle(file));
    }
}

#  endif // SETTINGS_IMPLEMENTATED
#endif // SETTINGS_IMPLEMENTATION
