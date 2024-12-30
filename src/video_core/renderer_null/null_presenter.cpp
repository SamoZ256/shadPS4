// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#include "sdl_window.h"
#include "video_core/renderer_null/null_presenter.h"
#include "video_core/renderer_null/null_rasterizer.h"

namespace Null {

Presenter::Presenter(Frontend::WindowSDL& window_)
    : window{window_}, rasterizer{std::make_unique<Rasterizer>()} {}

Presenter::~Presenter() = default;

VideoCore::Rasterizer& Presenter::GetRasterizer() const {
    return *rasterizer.get();
}

} // namespace Null
