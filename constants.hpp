#ifndef CONSTANTS_INCLUDED
#  define CONSTANTS_INCLUDED

#  define APPLICATION_NAME "Neodymium"

#  include <Hydrogen.hpp>
#  include <volk.h>

#  ifdef DEBUG_MODE
static inline jmn::Addr constexpr ApplicationBaseAddr = (jmn::Addr)jmn::TiB(1);
#  else
static inline jmn::Addr constexpr ApplicationBaseAddr = jmn::NullAddr;
#  endif

static inline jmn::CString8 constexpr ApplicationName      = jmn::MakeCString(APPLICATION_NAME);
static inline jmn::U32      constexpr ApplicationVkVersion = VK_API_VERSION_1_4;
static inline jmn::Size     constexpr ApplicationArenaSize = jmn::GiB(1);
static inline jmn::Size     constexpr ApplicationHeapSize  = jmn::GiB(1);
static inline jmn::CString8 constexpr SettingsFileName     = jmn::MakeCString("settings");

#endif // CONSTANTS_INCLUDED