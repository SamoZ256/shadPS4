// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include <deque>

#include <SDL3/SDL_events.h>
#include <imgui.h>

#include "common/config.h"
#include "common/path_util.h"
#include "core/devtools/layer.h"
#include "imgui/imgui_layer.h"
#include "imgui_core.h"
#include "imgui_impl_sdl3.h"
#include "imgui_internal.h"
#include "sdl_window.h"
#include "texture_manager.h"

#include "imgui_fonts/notosansjp_regular.ttf.g.cpp"
#include "imgui_fonts/proggyvector_regular.ttf.g.cpp"

static std::vector<ImGui::Layer*> layers;

// Update layers before rendering to allow layer changes to be applied during rendering.
// Using deque to keep the order of changes in case a Layer is removed then added again between
// frames.
static std::deque<std::pair<bool, ImGui::Layer*>> change_layers{};
static std::mutex change_layers_mutex{};

static ImGuiID dock_id;

namespace ImGui {

namespace Core {

void Initialize(const Frontend::WindowSDL& window) {

    const auto config_path = GetUserPath(Common::FS::PathType::UserDir) / "imgui.ini";
    const auto log_path = GetUserPath(Common::FS::PathType::LogDir) / "imgui_log.txt";

    CreateContext();
    ImGuiIO& io = GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.DisplaySize = ImVec2((float)window.GetWidth(), (float)window.GetHeight());
    PushStyleVar(ImGuiStyleVar_WindowRounding, 6.0f); // Makes the window edges rounded

    auto path = config_path.u8string();
    char* config_file_buf = new char[path.size() + 1]();
    std::memcpy(config_file_buf, path.c_str(), path.size());
    io.IniFilename = config_file_buf;

    path = log_path.u8string();
    char* log_file_buf = new char[path.size() + 1]();
    std::memcpy(log_file_buf, path.c_str(), path.size());
    io.LogFilename = log_file_buf;

    ImFontGlyphRangesBuilder rb{};
    rb.AddRanges(io.Fonts->GetGlyphRangesDefault());
    rb.AddRanges(io.Fonts->GetGlyphRangesGreek());
    rb.AddRanges(io.Fonts->GetGlyphRangesKorean());
    rb.AddRanges(io.Fonts->GetGlyphRangesJapanese());
    rb.AddRanges(io.Fonts->GetGlyphRangesCyrillic());
    ImVector<ImWchar> ranges{};
    rb.BuildRanges(&ranges);
    ImFontConfig font_cfg{};
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 1;
    io.FontDefault = io.Fonts->AddFontFromMemoryCompressedTTF(
        imgui_font_notosansjp_regular_compressed_data,
        imgui_font_notosansjp_regular_compressed_size, 16.0f, &font_cfg, ranges.Data);
    io.Fonts->AddFontFromMemoryCompressedTTF(imgui_font_proggyvector_regular_compressed_data,
                                             imgui_font_proggyvector_regular_compressed_size,
                                             16.0f);

    StyleColorsDark();

    ::Core::Devtools::Layer::SetupSettings();
    Sdl::Init(window.GetSDLWindow());

    TextureManager::StartWorker();

    char label[32];
    ImFormatString(label, IM_ARRAYSIZE(label), "WindowOverViewport_%08X", GetMainViewport()->ID);
    dock_id = ImHashStr(label);

    if (const auto dpi = SDL_GetWindowDisplayScale(window.GetSDLWindow()); dpi > 0.0f) {
        GetIO().FontGlobalScale = dpi;
    }
}

void OnResize() {
    Sdl::OnResize();
}

void Shutdown() {
    TextureManager::StopWorker();

    const ImGuiIO& io = GetIO();
    const auto ini_filename = (void*)io.IniFilename;
    const auto log_filename = (void*)io.LogFilename;

    Sdl::Shutdown();
    DestroyContext();

    delete[] (char*)ini_filename;
    delete[] (char*)log_filename;
}

bool ProcessEvent(SDL_Event* event) {
    Sdl::ProcessEvent(event);
    switch (event->type) {
    // Don't block release/up events
    case SDL_EVENT_MOUSE_MOTION:
    case SDL_EVENT_MOUSE_WHEEL:
    case SDL_EVENT_MOUSE_BUTTON_DOWN: {
        const auto& io = GetIO();
        return io.WantCaptureMouse && io.Ctx->NavWindow != nullptr &&
               io.Ctx->NavWindow->ID != dock_id;
    }
    case SDL_EVENT_TEXT_INPUT:
    case SDL_EVENT_KEY_DOWN: {
        const auto& io = GetIO();
        return io.WantCaptureKeyboard && io.Ctx->NavWindow != nullptr &&
               io.Ctx->NavWindow->ID != dock_id;
    }
    case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
    case SDL_EVENT_GAMEPAD_AXIS_MOTION:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_DOWN:
    case SDL_EVENT_GAMEPAD_TOUCHPAD_MOTION: {
        const auto& io = GetIO();
        return io.NavActive && io.Ctx->NavWindow != nullptr && io.Ctx->NavWindow->ID != dock_id;
    }
    default:
        return false;
    }
}

void NewFrame() {
    {
        std::scoped_lock lock{change_layers_mutex};
        while (!change_layers.empty()) {
            const auto [to_be_added, layer] = change_layers.front();
            if (to_be_added) {
                layers.push_back(layer);
            } else {
                const auto [begin, end] = std::ranges::remove(layers, layer);
                layers.erase(begin, end);
            }
            change_layers.pop_front();
        }
    }

    Sdl::NewFrame();
    ImGui::NewFrame();

    DockSpaceOverViewport(0, GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

    for (auto* layer : layers) {
        layer->Draw();
    }
}

void Render() {
    ImGui::Render();
}

} // namespace Core

void Layer::AddLayer(Layer* layer) {
    std::scoped_lock lock{change_layers_mutex};
    change_layers.emplace_back(true, layer);
}

void Layer::RemoveLayer(Layer* layer) {
    std::scoped_lock lock{change_layers_mutex};
    change_layers.emplace_back(false, layer);
}

} // namespace ImGui
