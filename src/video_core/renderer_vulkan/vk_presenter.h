// SPDX-FileCopyrightText: Copyright 2024 shadPS4 Emulator Project
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <condition_variable>

#include "video_core/amdgpu/liverpool.h"
#include "video_core/presenter.h"
#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include "video_core/renderer_vulkan/vk_swapchain.h"
#include "video_core/texture_cache/texture_cache.h"

namespace Frontend {
class WindowSDL;
}

namespace AmdGpu {
struct Liverpool;
}

namespace Vulkan {

struct Frame : public VideoCore::Frame {
    u32 width;
    u32 height;
    VmaAllocation allocation;
    vk::Image image;
    vk::ImageView image_view;
    vk::Fence present_done;
    vk::Semaphore ready_semaphore;
    u64 ready_tick;
};

enum SchedulerType {
    Draw,
    Present,
    CpuFlip,
};

class Rasterizer;

class Presenter : public VideoCore::Presenter {
    struct PostProcessSettings {
        float gamma = 1.0f;
    };

public:
    Presenter(Frontend::WindowSDL& window, AmdGpu::Liverpool* liverpool);
    ~Presenter() override;

    float& GetGammaRef() override {
        return pp_settings.gamma;
    }

    Frontend::WindowSDL& GetWindow() const override {
        return window;
    }

    VideoCore::Frame* PrepareFrame(const Libraries::VideoOut::BufferAttributeGroup& attribute,
                                   VAddr cpu_address, bool is_eop) override {
        auto desc = VideoCore::TextureCache::VideoOutDesc{attribute, cpu_address};
        const auto image_id = texture_cache.FindImage(desc);
        texture_cache.UpdateImage(image_id, is_eop ? nullptr : &flip_scheduler);
        return PrepareFrameInternal(image_id, is_eop);
    }

    VideoCore::Frame* PrepareBlankFrame(bool is_eop) override {
        return PrepareFrameInternal(VideoCore::NULL_IMAGE_ID, is_eop);
    }

    void RegisterVideoOutSurface(const Libraries::VideoOut::BufferAttributeGroup& attribute,
                                 VAddr cpu_address) override {
        vo_buffers_addr.emplace_back(cpu_address);
        auto desc = VideoCore::TextureCache::VideoOutDesc{attribute, cpu_address};
        const auto image_id = texture_cache.FindImage(desc);
        auto& image = texture_cache.GetImage(image_id);
        image.usage.vo_surface = 1u;
    }

    bool IsVideoOutSurface(const AmdGpu::Liverpool::ColorBuffer& color_buffer) override {
        return std::ranges::find_if(vo_buffers_addr, [&](VAddr vo_buffer) {
                   return vo_buffer == color_buffer.Address();
               }) != vo_buffers_addr.cend();
    }

    bool ShowSplash(VideoCore::Frame* frame = nullptr) override;
    void Present(VideoCore::Frame* frame) override;
    void RecreateFrame(VideoCore::Frame* frame, u32 width, u32 height) override;

    void FlushDraw() override {
        SubmitInfo info{};
        draw_scheduler.Flush(info);
    }

    VideoCore::Rasterizer& GetRasterizer() const override;

private:
    void CreatePostProcessPipeline();
    Frame* PrepareFrameInternal(VideoCore::ImageId image_id, bool is_eop = true);
    Frame* GetRenderFrame();

private:
    PostProcessSettings pp_settings{};
    vk::UniquePipeline pp_pipeline{};
    vk::UniquePipelineLayout pp_pipeline_layout{};
    vk::UniqueDescriptorSetLayout pp_desc_set_layout{};
    vk::UniqueSampler pp_sampler{};
    Frontend::WindowSDL& window;
    AmdGpu::Liverpool* liverpool;
    Instance instance;
    Scheduler draw_scheduler;
    Scheduler present_scheduler;
    Scheduler flip_scheduler;
    Swapchain swapchain;
    std::unique_ptr<Rasterizer> rasterizer;
    VideoCore::TextureCache& texture_cache;
    vk::UniqueCommandPool command_pool;
    std::vector<Frame> present_frames;
    std::queue<Frame*> free_queue;
    std::mutex free_mutex;
    std::condition_variable free_cv;
    std::condition_variable_any frame_cv;
    std::optional<VideoCore::Image> splash_img;
    std::vector<VAddr> vo_buffers_addr;
};

} // namespace Vulkan
