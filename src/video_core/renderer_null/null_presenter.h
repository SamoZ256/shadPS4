// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <condition_variable>

#include "video_core/amdgpu/liverpool.h"
#include "video_core/presenter.h"

namespace Frontend {
class WindowSDL;
}

namespace AmdGpu {
struct Liverpool;
}

namespace Null {

class Rasterizer;

class Presenter : public VideoCore::Presenter {
public:
    Presenter(Frontend::WindowSDL& window);
    ~Presenter() override;

    float& GetGammaRef() override {
        return gamma;
    }

    Frontend::WindowSDL& GetWindow() const override {
        return window;
    }

    VideoCore::Frame* PrepareFrame(const Libraries::VideoOut::BufferAttributeGroup& attribute,
                                   VAddr cpu_address, bool is_eop) override {
        return nullptr;
    }

    VideoCore::Frame* PrepareBlankFrame(bool is_eop) override {
        return nullptr;
    }

    void RegisterVideoOutSurface(const Libraries::VideoOut::BufferAttributeGroup& attribute,
                                 VAddr cpu_address) override {}

    bool IsVideoOutSurface(const AmdGpu::Liverpool::ColorBuffer& color_buffer) override {
        return false;
    }

    bool ShowSplash(VideoCore::Frame* frame = nullptr) override {
        return false;
    }
    void Present(VideoCore::Frame* frame) override {}
    void RecreateFrame(VideoCore::Frame* frame, u32 width, u32 height) override {}

    void FlushDraw() override {}

    VideoCore::Rasterizer& GetRasterizer() const override;

private:
    Frontend::WindowSDL& window;
    std::unique_ptr<Rasterizer> rasterizer;

    float gamma = 1.0f;
};

} // namespace Null
