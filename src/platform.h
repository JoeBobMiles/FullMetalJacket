/**
 * @file platform.h
 * @author Joseph Miles <josephmiles2015@gmail.com>
 * @date 2019-04-23
 *
 * This file contains declarations for all functions that require somekind of
 * platform specific implementation, but also need to be used by the rest of
 * the program.
 */

#ifndef _PLATFORM_H_
#define _PLATFORM_H_

// TODO[joe] Create size macros for better handling memory things?
#define PLATFORM_MAX_SHADER_SIZE 16384

#ifdef DEBUG
static void Assert(bool, const char*);
#else
// TODO[joe] Make this do something?
// Right now this just gobbles Assert() calls where they appear, meaning that
// if the application fails for a reason that an Assert() would've caught, it
// will crash inexplicably. Which will frustrate the user.
#define Assert(F,M)
#endif

static void Abort(const char*);

static inline
VkShaderModule PlatformLoadShader(vulkan_context, const char*);

#endif
