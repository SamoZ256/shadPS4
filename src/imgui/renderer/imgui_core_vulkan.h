// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "video_core/renderer_vulkan/vk_instance.h"
#include "vulkan/vulkan_handles.hpp"

union SDL_Event;

namespace Vulkan {
struct Frame;
}

namespace ImGui::Core::Vulkan {

void Initialize(const ::Vulkan::Instance& instance, const Frontend::WindowSDL& window,
                u32 image_count, vk::Format surface_format,
                const vk::AllocationCallbacks* allocator = nullptr);

void Shutdown(const vk::Device& device);

void Render(const vk::CommandBuffer& cmdbuf, ::Vulkan::Frame* frame);

} // namespace ImGui::Core::Vulkan
