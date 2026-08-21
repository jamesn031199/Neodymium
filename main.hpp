#pragma once

#ifdef DEBUG_MODE
#  define JMN_DEBUG_MODE
#endif
#ifdef NDEBUG
#  define JMN_FORCE_INLINE
#endif

#include <Windows.h>
#include <mmdeviceapi.h>
#include <Audioclient.h>

#include <Hydrogen.hpp>
#include <Helium.hpp>
#include <Lithium.hpp>
#include <Beryllium.hpp>
#include <Boron.hpp>

#pragma warning(push)
#pragma warning(disable:4820)
#include <stb_sprintf.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4820)
#include <volk.h>
#pragma warning(pop)

#pragma warning(push)
#pragma warning(disable:4820)
#include <imgui.h>
#include <backends/imgui_impl_win32.h>
#include <backends/imgui_impl_vulkan.h>
#pragma warning(pop)

#include "common.hpp"
#include "constants.hpp"
#include "settings.hpp"
#include "vulkan_common.hpp"
#include "vulkan_context.hpp"
#include "vulkan_swap_chain.hpp"
#include "application.hpp"
