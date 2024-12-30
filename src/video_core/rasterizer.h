// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "video_core/buffer_cache/buffer_cache.h"
#include "video_core/page_manager.h"
#include "video_core/texture_cache/texture_cache.h"

namespace VideoCore {

class Rasterizer {
public:
    virtual ~Rasterizer() {};

    virtual void Draw(bool is_indexed, u32 index_offset = 0) = 0;
    virtual void DrawIndirect(bool is_indexed, VAddr arg_address, u32 offset, u32 size,
                              u32 max_count, VAddr count_address) = 0;

    virtual void DispatchDirect() = 0;
    virtual void DispatchIndirect(VAddr address, u32 offset, u32 size) = 0;

    virtual void ScopeMarkerBegin(const std::string_view& str) = 0;
    virtual void ScopeMarkerEnd() = 0;
    virtual void ScopedMarkerInsert(const std::string_view& str) = 0;
    virtual void ScopedMarkerInsertColor(const std::string_view& str, const u32 color) = 0;

    virtual void InlineData(VAddr address, const void* value, u32 num_bytes, bool is_gds) = 0;
    virtual u32 ReadDataFromGds(u32 gsd_offset) = 0;
    virtual bool InvalidateMemory(VAddr addr, u64 size) = 0;
    virtual bool IsMapped(VAddr addr, u64 size) = 0;
    virtual void MapMemory(VAddr addr, u64 size) = 0;
    virtual void UnmapMemory(VAddr addr, u64 size) = 0;

    virtual void CpSync() = 0;
    virtual u64 Flush() = 0;
    virtual void Finish() = 0;
};

} // namespace VideoCore
