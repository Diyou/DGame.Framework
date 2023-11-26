/**
 * Examples/HelloTriangle.cpp - DGame.Framework
 *
 * Copyright (c) 2023 Diyou
 * All rights reserved.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#include "DGame/Backend.h"

using namespace std;
using namespace wgpu;
using namespace DGame;

class HelloTriangle : public Backend
{
  // clang-format off
  const vector<float> Vertices = {
    -0.8f, -0.8f, 0.0f, 0.0f, 1.0f, // BL
     0.8f, -0.8f, 0.0f, 1.0f, 0.0f, // BR
    -0.0f,  0.8f, 1.0f, 0.0f, 0.0f, // top
  };
  const vector<uint16_t> Indices = {
    0, 1, 2,//
    0       //
  };

  // clang-format on
  template <class T>
  struct Uniform
  {
    T value;
    size_t size;
    HelloTriangle *renderer;
    Buffer buffer;

    Uniform(const T &uniform, HelloTriangle *renderer)
    : value(uniform)
    , size(sizeof(uniform))
    {
      this->renderer = renderer;

      buffer = renderer->createBuffer(&value, size, BufferUsage::Uniform);
    }
  };

  void
  draw()
  {
    TextureView backBuffer = swapchain.GetCurrentTextureView();
    RenderPassColorAttachment colorDesc{};

    colorDesc.view = backBuffer;
    colorDesc.loadOp = LoadOp::Clear;
    colorDesc.storeOp = StoreOp::Store;

    colorDesc.clearValue.r = 0.3f;
    colorDesc.clearValue.g = 0.3f;
    colorDesc.clearValue.b = 0.3f;
    colorDesc.clearValue.a = 1.0f;

    RenderPassDescriptor renderPass{};
    renderPass.colorAttachmentCount = 1;
    renderPass.colorAttachments = &colorDesc;

    CommandEncoder encoder = device.CreateCommandEncoder(nullptr);
    RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);

    // update scene
    // scene.get()->update();
    rotation.value += 0.1f;
    queue.WriteBuffer(rotation.buffer, 0, &rotation.value, rotation.size);
    // draw scene
    pass.SetPipeline(pipeline);
    pass.SetBindGroup(0, bindGroup, 0, 0);

    pass.SetVertexBuffer(0, vertexBuffer, 0, kWholeSize);
    pass.SetIndexBuffer(indexBuffer, IndexFormat::Uint16, 0, kWholeSize);
    pass.DrawIndexed(3, 1, 0, 0, 0);

    // finish
    pass.End();

    CommandBuffer commands = encoder.Finish(nullptr);

    /*auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void * userData) {
      auto _this = reinterpret_cast<Renderer*>(userData);
      //_this->Start();
      std::cout << "Queued work finished with status: " << status << std::endl;
    };
    queue.OnSubmittedWorkDone(onQueueWorkDone, this);
    */
    queue.Submit(1, &commands);
  };

  RenderPipeline pipeline;
  BindGroup bindGroup;

public:
  Uniform<float> rotation;
  Buffer vertexBuffer, indexBuffer;

  HelloTriangle()
  : Backend("Dawn Example", 720, 480)
  , rotation(createUniform<float>(0))
  {
    // Setup pipeline
    Setup();

    vertexBuffer = createBuffer(
      Vertices.data(),
      Vertices.size() * sizeof(decay<decltype(*Vertices.begin())>::type),
      BufferUsage::Vertex
    );
    indexBuffer = createBuffer(
      Indices.data(),
      Indices.size() * sizeof(decay<decltype(*Indices.begin())>::type),
      BufferUsage::Index
    );
  }

  void
  Setup()
  {
    ShaderModule vertMod = createShader(Shaders::WGSL::vertex);
    ShaderModule fragMod = createShader(Shaders::WGSL::fragment);

    BufferBindingLayout bgl{};
    bgl.type = BufferBindingType::Uniform;

    BindGroupLayoutEntry bglEntry{};
    bglEntry.binding = 0;
    bglEntry.visibility = wgpu::ShaderStage::Vertex;
    bglEntry.buffer = bgl;

    BindGroupLayoutDescriptor bglDescriptor{};
    bglDescriptor.entryCount = 1;
    bglDescriptor.entries = &bglEntry;
    BindGroupLayout bindGroupLayout = device.CreateBindGroupLayout(&bglDescriptor);

    PipelineLayoutDescriptor plDescriptor{};
    plDescriptor.bindGroupLayoutCount = 1;
    plDescriptor.bindGroupLayouts = &bindGroupLayout;
    PipelineLayout pipelineLayout = device.CreatePipelineLayout(&plDescriptor);

    VertexAttribute vAttributes[2];
    vAttributes[0].format = VertexFormat::Float32x2;
    vAttributes[0].offset = 0;
    vAttributes[0].shaderLocation = 0;
    vAttributes[1].format = VertexFormat::Float32x3;
    vAttributes[1].offset = 2 * sizeof(float);
    vAttributes[1].shaderLocation = 1;
    VertexBufferLayout vertexBufferLayout{};
    vertexBufferLayout.arrayStride = 5 * sizeof(float);
    vertexBufferLayout.attributeCount = 2;
    vertexBufferLayout.attributes = vAttributes;

    BlendState blendState{};
    blendState.color.operation = BlendOperation::Add;
    blendState.color.srcFactor = BlendFactor::One;
    blendState.color.dstFactor = BlendFactor::One;
    blendState.alpha.operation = BlendOperation::Add;
    blendState.alpha.srcFactor = BlendFactor::One;
    blendState.alpha.dstFactor = BlendFactor::One;

    ColorTargetState colorTargetState{};
    colorTargetState.format = TextureFormat::BGRA8Unorm;
    colorTargetState.blend = &blendState;
    colorTargetState.writeMask = ColorWriteMask::All;

    FragmentState fragmentState{};
    fragmentState.module = fragMod;
    fragmentState.entryPoint = "main";
    fragmentState.targetCount = 1;
    fragmentState.targets = &colorTargetState;

    RenderPipelineDescriptor pipelineDescriptor{};
    pipelineDescriptor.fragment = &fragmentState;

    pipelineDescriptor.layout = pipelineLayout;
    pipelineDescriptor.depthStencil = nullptr;

    pipelineDescriptor.vertex.module = vertMod;
    pipelineDescriptor.vertex.entryPoint = "main";
    pipelineDescriptor.vertex.bufferCount = 1;
    pipelineDescriptor.vertex.buffers = &vertexBufferLayout;

    pipelineDescriptor.multisample.count = 1;
    pipelineDescriptor.multisample.mask = 0xFFFFFFFF;
    pipelineDescriptor.multisample.alphaToCoverageEnabled = false;

    pipelineDescriptor.primitive.frontFace = FrontFace::CCW;

    // pipelineDescriptor.primitive.cullMode = wgpu::CullMode::Front;
    pipelineDescriptor.primitive.cullMode = CullMode::None;
    pipelineDescriptor.primitive.topology = PrimitiveTopology::TriangleList;
    pipelineDescriptor.primitive.stripIndexFormat = IndexFormat::Undefined;

    pipeline = device.CreateRenderPipeline(&pipelineDescriptor);

    BindGroupEntry bgEntry{};
    bgEntry.binding = 0;
    bgEntry.buffer = rotation.buffer;
    bgEntry.offset = 0;
    bgEntry.size = rotation.size;

    BindGroupDescriptor bgDescriptor{};
    bgDescriptor.layout = bindGroupLayout;
    bgDescriptor.entryCount = 1;
    bgDescriptor.entries = &bgEntry;

    bindGroup = device.CreateBindGroup(&bgDescriptor);
  }

  ShaderModule
  createShader(string code, const char *label = nullptr)
  {
    ShaderModuleWGSLDescriptor wgls_descriptor{};
    wgls_descriptor.sType = SType::ShaderModuleWGSLDescriptor;
    wgls_descriptor.code = code.c_str();

    ShaderModuleDescriptor descriptor{};
    descriptor.nextInChain = &wgls_descriptor;
    descriptor.label = label;

    return device.CreateShaderModule(&descriptor);
  }

  Buffer
  createBuffer(const void *data, size_t size, BufferUsage usage)
  {
    BufferDescriptor descriptor{};
    descriptor.usage = BufferUsage::CopyDst | usage;
    descriptor.size = size;

    Buffer buffer = device.CreateBuffer(&descriptor);
    queue.WriteBuffer(buffer, 0, data, size);
    return buffer;
  }

  template <class T>
  Uniform<T>
  createUniform(T value)
  {
    Uniform<T> uniform(value, this);
    return uniform;
  }
};

int
main(int argc, const char *argv[])
{
  // Runtime _(argc, argv);
  auto triangle = new HelloTriangle();
  auto triangle2 = new HelloTriangle();

  return 0;
}
