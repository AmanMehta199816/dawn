// Copyright 2023 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef SRC_DAWN_NATIVE_D3D11_DEVICED3D11_H_
#define SRC_DAWN_NATIVE_D3D11_DEVICED3D11_H_

#include <memory>
#include <vector>

#include "dawn/common/SerialQueue.h"
#include "dawn/native/d3d/DeviceD3D.h"
#include "dawn/native/d3d11/CommandRecordingContextD3D11.h"
#include "dawn/native/d3d11/DeviceInfoD3D11.h"
#include "dawn/native/d3d11/Forward.h"

namespace dawn::native::d3d11 {

class Fence;

// Definition of backend types
class Device final : public d3d::Device {
  public:
    static ResultOrError<Ref<Device>> Create(Adapter* adapter,
                                             const DeviceDescriptor* descriptor,
                                             const TogglesState& deviceToggles);
    ~Device() override;

    MaybeError Initialize(const DeviceDescriptor* descriptor);

    ID3D11Device* GetD3D11Device() const;
    ID3D11Device5* GetD3D11Device5() const;

    ResultOrError<CommandRecordingContext*> GetPendingCommandContext(
        Device::SubmitMode submitMode = Device::SubmitMode::Normal);

    const DeviceInfo& GetDeviceInfo() const;

    MaybeError NextSerial();
    MaybeError WaitForSerial(ExecutionSerial serial);

    void ReferenceUntilUnused(ComPtr<IUnknown> object);
    MaybeError ExecutePendingCommandContext();
    HANDLE GetFenceHandle() const;
    Ref<TextureBase> CreateD3D11ExternalTexture(const TextureDescriptor* descriptor,
                                                ComPtr<ID3D11Resource> d3d11Texture,
                                                std::vector<Ref<Fence>> waitFences,
                                                bool isSwapChainTexture,
                                                bool isInitialized);

    ResultOrError<Ref<CommandBufferBase>> CreateCommandBuffer(
        CommandEncoder* encoder,
        const CommandBufferDescriptor* descriptor) override;
    MaybeError TickImpl() override;
    void ForceEventualFlushOfCommands() override;
    MaybeError CopyFromStagingToBufferImpl(BufferBase* source,
                                           uint64_t sourceOffset,
                                           BufferBase* destination,
                                           uint64_t destinationOffset,
                                           uint64_t size) override;
    MaybeError CopyFromStagingToTextureImpl(const BufferBase* source,
                                            const TextureDataLayout& src,
                                            const TextureCopy& dst,
                                            const Extent3D& copySizePixels) override;
    uint32_t GetOptimalBytesPerRowAlignment() const override;
    uint64_t GetOptimalBufferToTextureCopyOffsetAlignment() const override;
    float GetTimestampPeriodInNS() const override;
    bool ShouldDuplicateNumWorkgroupsForDispatchIndirect(
        ComputePipelineBase* computePipeline) const override;
    bool MayRequireDuplicationOfIndirectParameters() const override;
    bool ShouldDuplicateParametersForDrawIndirect(
        const RenderPipelineBase* renderPipelineBase) const override;
    uint64_t GetBufferCopyOffsetAlignmentForDepthStencil() const override;
    void SetLabelImpl() override;

  private:
    using Base = d3d::Device;
    using Base::Base;

    ResultOrError<Ref<BindGroupBase>> CreateBindGroupImpl(
        const BindGroupDescriptor* descriptor) override;
    ResultOrError<Ref<BindGroupLayoutBase>> CreateBindGroupLayoutImpl(
        const BindGroupLayoutDescriptor* descriptor,
        PipelineCompatibilityToken pipelineCompatibilityToken) override;
    ResultOrError<Ref<BufferBase>> CreateBufferImpl(const BufferDescriptor* descriptor) override;
    ResultOrError<Ref<PipelineLayoutBase>> CreatePipelineLayoutImpl(
        const PipelineLayoutDescriptor* descriptor) override;
    ResultOrError<Ref<QuerySetBase>> CreateQuerySetImpl(
        const QuerySetDescriptor* descriptor) override;
    ResultOrError<Ref<SamplerBase>> CreateSamplerImpl(const SamplerDescriptor* descriptor) override;
    ResultOrError<Ref<ShaderModuleBase>> CreateShaderModuleImpl(
        const ShaderModuleDescriptor* descriptor,
        ShaderModuleParseResult* parseResult,
        OwnedCompilationMessages* compilationMessages) override;
    ResultOrError<Ref<SwapChainBase>> CreateSwapChainImpl(
        const SwapChainDescriptor* descriptor) override;
    ResultOrError<Ref<NewSwapChainBase>> CreateSwapChainImpl(
        Surface* surface,
        NewSwapChainBase* previousSwapChain,
        const SwapChainDescriptor* descriptor) override;
    ResultOrError<Ref<TextureBase>> CreateTextureImpl(const TextureDescriptor* descriptor) override;
    ResultOrError<Ref<TextureViewBase>> CreateTextureViewImpl(
        TextureBase* texture,
        const TextureViewDescriptor* descriptor) override;
    Ref<ComputePipelineBase> CreateUninitializedComputePipelineImpl(
        const ComputePipelineDescriptor* descriptor) override;
    Ref<RenderPipelineBase> CreateUninitializedRenderPipelineImpl(
        const RenderPipelineDescriptor* descriptor) override;
    void InitializeComputePipelineAsyncImpl(Ref<ComputePipelineBase> computePipeline,
                                            WGPUCreateComputePipelineAsyncCallback callback,
                                            void* userdata) override;
    void InitializeRenderPipelineAsyncImpl(Ref<RenderPipelineBase> renderPipeline,
                                           WGPUCreateRenderPipelineAsyncCallback callback,
                                           void* userdata) override;
    void DestroyImpl() override;
    MaybeError WaitForIdleForDestruction() override;
    bool HasPendingCommands() const override;
    MaybeError CheckDebugLayerAndGenerateErrors();
    void AppendDebugLayerMessages(ErrorData* error) override;
    ResultOrError<ExecutionSerial> CheckAndUpdateCompletedSerials() override;

    ComPtr<ID3D11Fence> mFence;
    HANDLE mFenceHandle = nullptr;
    HANDLE mFenceEvent = nullptr;

    ComPtr<ID3D11Device> mD3d11Device;
    ComPtr<ID3D11Device5> mD3d11Device5;
    CommandRecordingContext mPendingCommands;
    SerialQueue<ExecutionSerial, ComPtr<IUnknown>> mUsedComObjectRefs;
};

}  // namespace dawn::native::d3d11

#endif  // SRC_DAWN_NATIVE_D3D11_DEVICED3D11_H_