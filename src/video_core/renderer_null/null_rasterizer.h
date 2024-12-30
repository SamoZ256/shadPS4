// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "video_core/buffer_cache/buffer_cache.h"
#include "video_core/page_manager.h"
#include "video_core/rasterizer.h"

namespace AmdGpu {
struct Liverpool;
}

namespace Core {
class MemoryManager;
}

namespace Null {

class Rasterizer : public VideoCore::Rasterizer {
public:
    explicit Rasterizer();
    ~Rasterizer() override;

    void Draw(bool is_indexed, u32 index_offset = 0) override {}
    void DrawIndirect(bool is_indexed, VAddr arg_address, u32 offset, u32 size, u32 max_count,
                      VAddr count_address) override {}

    void DispatchDirect() override {}
    void DispatchIndirect(VAddr address, u32 offset, u32 size) override {}

    void ScopeMarkerBegin(const std::string_view& str) override {}
    void ScopeMarkerEnd() override {}
    void ScopedMarkerInsert(const std::string_view& str) override {}
    void ScopedMarkerInsertColor(const std::string_view& str, const u32 color) override {}

    void InlineData(VAddr address, const void* value, u32 num_bytes, bool is_gds) override {}
    u32 ReadDataFromGds(u32 gsd_offset) override {
        return 0;
    }
    bool InvalidateMemory(VAddr addr, u64 size) override {
        return false;
    }
    bool IsMapped(VAddr addr, u64 size) override {
        return false;
    }
    void MapMemory(VAddr addr, u64 size) override {}
    void UnmapMemory(VAddr addr, u64 size) override {}

    void CpSync() override {}
    u64 Flush() override {
        return 0;
    }
    void Finish() override {}
};

} // namespace Null
