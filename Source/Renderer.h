#pragma once
#include "DGame/Backend.h"

#include "Scene.h"

#include <iostream>
#include <memory>

namespace DGame
{
class Renderer : public Backend
{
	template <class T> struct Uniform
	{
		T value;
		size_t size;
		Renderer *renderer;
		wgpu::Buffer buffer;

		Uniform(const T &uniform, Renderer *renderer)
		    : value(uniform)
		    , size(sizeof(uniform))
		{
			this->renderer = renderer;

			buffer = renderer->createBuffer(&value, size, wgpu::BufferUsage::Uniform);
		}
	};

	void draw()
	{
		wgpu::TextureView backBuffer = swapchain.GetCurrentTextureView();
		wgpu::RenderPassColorAttachment colorDesc{};

		colorDesc.view = backBuffer;
		colorDesc.loadOp = wgpu::LoadOp::Clear;
		colorDesc.storeOp = wgpu::StoreOp::Store;

		colorDesc.clearValue.r = 0.3f;
		colorDesc.clearValue.g = 0.3f;
		colorDesc.clearValue.b = 0.3f;
		colorDesc.clearValue.a = 1.0f;

		wgpu::RenderPassDescriptor renderPass{};
		renderPass.colorAttachmentCount = 1;
		renderPass.colorAttachments = &colorDesc;

		wgpu::CommandEncoder encoder = device.CreateCommandEncoder(nullptr);
		wgpu::RenderPassEncoder pass = encoder.BeginRenderPass(&renderPass);

		// update scene
		// scene.get()->update();
		rotation.value += 0.1f;
		queue.WriteBuffer(rotation.buffer, 0, &rotation.value, rotation.size);
		// draw scene
		pass.SetPipeline(pipeline);
		pass.SetBindGroup(0, bindGroup, 0, 0);

		pass.SetVertexBuffer(0, vertexBuffer, 0, wgpu::kWholeSize);
		pass.SetIndexBuffer(indexBuffer, wgpu::IndexFormat::Uint16, 0, wgpu::kWholeSize);
		pass.DrawIndexed(3, 1, 0, 0, 0);

		// finish
		pass.End();

		wgpu::CommandBuffer commands = encoder.Finish(nullptr);

		/*auto onQueueWorkDone = [](WGPUQueueWorkDoneStatus status, void * userData) {
		    auto _this = reinterpret_cast<Renderer*>(userData);
		    //_this->Start();
		    std::cout << "Queued work finished with status: " << status << std::endl;
		};
		queue.OnSubmittedWorkDone(onQueueWorkDone, this);
		*/
		queue.Submit(1, &commands);
	};

	std::shared_ptr<Scene> scene;

	wgpu::RenderPipeline pipeline;
	wgpu::BindGroup bindGroup;

public:
	Uniform<float> rotation;
	wgpu::Buffer vertexBuffer, indexBuffer;

	Renderer(std::shared_ptr<Scene> loadingScene)
	    : Backend("Dawn Example", 720, 480)
	    , rotation(createUniform<float>(0))
	    , scene(loadingScene)
	{
		// Setup pipeline
		Setup(scene);

		vertexBuffer = createBuffer(
		    scene.get()->vertices.data(),
		    scene.get()->vertices.size() * sizeof(std::decay<decltype(*scene.get()->vertices.begin())>::type),
		    wgpu::BufferUsage::Vertex
		);
		indexBuffer = createBuffer(
		    scene.get()->indices.data(),
		    scene.get()->indices.size() * sizeof(std::decay<decltype(*scene.get()->indices.begin())>::type),
		    wgpu::BufferUsage::Index
		);
	}

	void Setup(std::shared_ptr<Scene> scene)
	{
		wgpu::ShaderModule vertMod = createShader(Shaders::WGSL::vertex);
		wgpu::ShaderModule fragMod = createShader(Shaders::WGSL::fragment);

		wgpu::BufferBindingLayout bgl{};
		bgl.type = wgpu::BufferBindingType::Uniform;

		wgpu::BindGroupLayoutEntry bglEntry{};
		bglEntry.binding = 0;
		bglEntry.visibility = wgpu::ShaderStage::Vertex;
		bglEntry.buffer = bgl;

		wgpu::BindGroupLayoutDescriptor bglDescriptor{};
		bglDescriptor.entryCount = 1;
		bglDescriptor.entries = &bglEntry;
		wgpu::BindGroupLayout bindGroupLayout = device.CreateBindGroupLayout(&bglDescriptor);

		wgpu::PipelineLayoutDescriptor plDescriptor{};
		plDescriptor.bindGroupLayoutCount = 1;
		plDescriptor.bindGroupLayouts = &bindGroupLayout;
		wgpu::PipelineLayout pipelineLayout = device.CreatePipelineLayout(&plDescriptor);

		wgpu::VertexAttribute vAttributes[2];
		vAttributes[0].format = wgpu::VertexFormat::Float32x2;
		vAttributes[0].offset = 0;
		vAttributes[0].shaderLocation = 0;
		vAttributes[1].format = wgpu::VertexFormat::Float32x3;
		vAttributes[1].offset = 2 * sizeof(float);
		vAttributes[1].shaderLocation = 1;
		wgpu::VertexBufferLayout vertexBufferLayout{};
		vertexBufferLayout.arrayStride = 5 * sizeof(float);
		vertexBufferLayout.attributeCount = 2;
		vertexBufferLayout.attributes = vAttributes;

		wgpu::BlendState blendState{};
		blendState.color.operation = wgpu::BlendOperation::Add;
		blendState.color.srcFactor = wgpu::BlendFactor::One;
		blendState.color.dstFactor = wgpu::BlendFactor::One;
		blendState.alpha.operation = wgpu::BlendOperation::Add;
		blendState.alpha.srcFactor = wgpu::BlendFactor::One;
		blendState.alpha.dstFactor = wgpu::BlendFactor::One;

		wgpu::ColorTargetState colorTargetState{};
		colorTargetState.format = wgpu::TextureFormat::BGRA8Unorm;
		colorTargetState.blend = &blendState;
		colorTargetState.writeMask = wgpu::ColorWriteMask::All;

		wgpu::FragmentState fragmentState{};
		fragmentState.module = fragMod;
		fragmentState.entryPoint = "main";
		fragmentState.targetCount = 1;
		fragmentState.targets = &colorTargetState;

		wgpu::RenderPipelineDescriptor pipelineDescriptor{};
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

		pipelineDescriptor.primitive.frontFace = wgpu::FrontFace::CCW;

		// pipelineDescriptor.primitive.cullMode = wgpu::CullMode::Front;
		pipelineDescriptor.primitive.cullMode = wgpu::CullMode::None;
		pipelineDescriptor.primitive.topology = wgpu::PrimitiveTopology::TriangleList;
		pipelineDescriptor.primitive.stripIndexFormat = wgpu::IndexFormat::Undefined;

		pipeline = device.CreateRenderPipeline(&pipelineDescriptor);

		wgpu::BindGroupEntry bgEntry{};
		bgEntry.binding = 0;
		bgEntry.buffer = rotation.buffer;
		bgEntry.offset = 0;
		bgEntry.size = rotation.size;

		wgpu::BindGroupDescriptor bgDescriptor{};
		bgDescriptor.layout = bindGroupLayout;
		bgDescriptor.entryCount = 1;
		bgDescriptor.entries = &bgEntry;

		bindGroup = device.CreateBindGroup(&bgDescriptor);
	}

	wgpu::ShaderModule createShader(std::string code, const char *label = nullptr)
	{
		wgpu::ShaderModuleWGSLDescriptor wgls_descriptor{};
		wgls_descriptor.sType = wgpu::SType::ShaderModuleWGSLDescriptor;
		wgls_descriptor.code = code.c_str();

		wgpu::ShaderModuleDescriptor descriptor{};
		descriptor.nextInChain = &wgls_descriptor;
		descriptor.label = label;

		return device.CreateShaderModule(&descriptor);
	}

	wgpu::Buffer createBuffer(const void *data, size_t size, wgpu::BufferUsage usage)
	{
		wgpu::BufferDescriptor descriptor{};
		descriptor.usage = wgpu::BufferUsage::CopyDst | usage;
		descriptor.size = size;

		wgpu::Buffer buffer = device.CreateBuffer(&descriptor);
		queue.WriteBuffer(buffer, 0, data, size);
		return buffer;
	}

	template <class T> Uniform<T> createUniform(T value)
	{
		Uniform<T> uniform(value, this);
		return uniform;
	}
};
} // namespace DGame