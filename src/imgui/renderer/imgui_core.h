// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "video_core/renderer_vulkan/vk_instance.h"
#include "vulkan/vulkan_handles.hpp"

union SDL_Event;

namespace Vulkan {
struct Frame;
}

namespace ImGui::Core {

void Initialize(const Frontend::WindowSDL& window);

void OnResize();

void Shutdown();

bool ProcessEvent(SDL_Event* event);

void NewFrame();

void Render();

} // namespace ImGui::Core
