// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <condition_variable>

#include "core/libraries/videoout/driver.h"
#include "video_core/amdgpu/liverpool.h"

namespace Frontend {
class WindowSDL;
}

namespace AmdGpu {
struct Liverpool;
}

namespace VideoCore {

struct Image;
class Rasterizer;

struct Frame {};

class Presenter {
public:
    virtual ~Presenter() {}

    virtual float& GetGammaRef() = 0;

    virtual Frontend::WindowSDL& GetWindow() const = 0;

    virtual Frame* PrepareFrame(const Libraries::VideoOut::BufferAttributeGroup& attribute,
                                VAddr cpu_address, bool is_eop) = 0;

    virtual Frame* PrepareBlankFrame(bool is_eop) = 0;

    virtual VideoCore::Image& RegisterVideoOutSurface(
        const Libraries::VideoOut::BufferAttributeGroup& attribute, VAddr cpu_address) = 0;

    virtual bool IsVideoOutSurface(const AmdGpu::Liverpool::ColorBuffer& color_buffer) = 0;

    virtual bool ShowSplash(Frame* frame = nullptr) = 0;
    virtual void Present(Frame* frame) = 0;
    virtual void RecreateFrame(Frame* frame, u32 width, u32 height) = 0;

    virtual void FlushDraw() = 0;

    virtual Rasterizer& GetRasterizer() const = 0;
};

} // namespace VideoCore
