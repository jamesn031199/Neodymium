#ifndef CONSTANTS_INCLUDED
#  define CONSTANTS_INCLUDED

#  define APPLICATION_NAME_UTF8 "Neodymium"
#  define APPLICATION_NAME_UTF16 u"Neodymium"

#  include <Hydrogen.hpp>
#  include <volk.h>

#  ifdef DEBUG_MODE
static inline jmn::Addr constexpr ApplicationBaseAddr = (jmn::Addr)jmn::TiB(1);
#  else
static inline jmn::Addr constexpr ApplicationBaseAddr = jmn::NullAddr;
#  endif

static inline jmn::CString8  constexpr ApplicationNameUTF8        = jmn::MakeCString(APPLICATION_NAME_UTF8);
static inline jmn::CString16 constexpr ApplicationNameUTF16       = jmn::MakeCString(APPLICATION_NAME_UTF16);
static inline jmn::CString16 constexpr ApplicationWindowClassName = jmn::MakeCString(APPLICATION_NAME_UTF16 u"CustomWindowClass");
static inline jmn::U32       constexpr ApplicationVkVersion       = VK_API_VERSION_1_4;
static inline jmn::Size      constexpr ApplicationArenaSize       = jmn::GiB(1);
static inline jmn::Size      constexpr ApplicationHeapSize        = jmn::GiB(1);

static inline jmn::CString16 constexpr SettingsFileName           = jmn::MakeCString(u"Settings.txt");
static inline jmn::CString8  constexpr DebugSettingName           = jmn::MakeCString("Debug");
static inline jmn::CString8  constexpr VSyncSettingName           = jmn::MakeCString("VSync");
static inline jmn::CString8  constexpr FullScreenSettingName      = jmn::MakeCString("FullScreen");
static inline jmn::CString8  constexpr HDRSettingName             = jmn::MakeCString("HDR");
static inline jmn::CString8  constexpr WindowSizeSettingName      = jmn::MakeCString("WindowResolution");

#endif // CONSTANTS_INCLUDED