struct uTime{
boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>time_spana;
boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>time_spanb;
// boost::chrono::duration<boost::compute::double_,boost::chrono::milliseconds::period>time_span_mili_a;
// boost::chrono::duration<boost::compute::double_,boost::chrono::milliseconds::period>time_span_mili_b;
boost::chrono::high_resolution_clock::time_point t1;
boost::chrono::high_resolution_clock::time_point t2;
boost::chrono::high_resolution_clock::time_point t3;
};
static uTime u_time;

struct VertexF{
// GLfloat position[4];
float position[4];
};

VertexF Fvertices[]={
{-1.0,-1.0,1.0,1.0},
{1.0,-1.0,1.0,1.0},
{1.0,1.0,1.0,1.0},
{-1.0,1.0,1.0,1.0},
{-1.0,-1.0,-1.0,1.0},
{1.0,-1.0,-1.0,1.0},
{1.0,1.0,-1.0,1.0},
{-1.0,1.0,1.0,1.0}
};

struct VertexFUV{
float x, y, z, w; // Position
float u, v;       // UV texture coordinates
};

struct Vertex{
// float x, y, z, w; // Position
// emscripten_align2_float x, y, z, w; // Position
emscripten_align1_float x, y, z, w; // Position
};

Vertex vertices[]={
{-1.0f,-1.0f,1.0f,1.0f},
{1.0f,-1.0f,1.0f,1.0f},
{1.0f,1.0f,1.0f,1.0f},
{-1.0f,1.0f,1.0f,1.0f},
{-1.0f,-1.0f,-1.0f,1.0f},
{1.0f,-1.0f,-1.0f,1.0f},
{1.0f,1.0f,-1.0f,1.0f},
{-1.0f,1.0f,1.0f,1.0f}
};

struct VertexUV{
float u, v;       // UV texture coordinates
};

VertexUV UVvertices[]={
{0.0f,0.0f},
{1.0f,0.0f},
{1.0f,1.0f},
{0.0f,1.0f},
{0.0f,0.0f},
{1.0f,0.0f},
{1.0f,1.0f},
{0.0f,1.0f}
};

VertexFUV FUVvertices[]={
{-1.0,-1.0,1.0,1.0,  0.0,0.0},
{1.0,-1.0,1.0,1.0,  1.0,0.0},
{1.0,1.0,1.0,1.0,  1.0,1.0},
{-1.0,1.0,1.0,1.0,  0.0,1.0},
{-1.0,-1.0,-1.0,1.0,  0.0,0.0},
{1.0,-1.0,-1.0,1.0,  1.0,0.0},
{1.0,1.0,-1.0,1.0,  1.0,1.0},
{-1.0,1.0,1.0,1.0,  0.0,1.0}
};

uint32_t OutputBufferBytes=64*4;
uint32_t InputBufferBytes=64*4;
uint64_t WGPU_InputRangeSize=OutputBufferBytes;
const char * Entry="main_image";
WGPU_MAP_MODE_FLAGS mode1=0x1; // READ MODE

float * WGPU_Result_Array=new float[OutputBufferBytes];
float * WGPU_Input_Array=new float[InputBufferBytes];
const char * Fnm2=reinterpret_cast<const char *>("/video/frame.gl");
const char * FnmC=reinterpret_cast<const char *>("/shader/compute.wgsl");
uint8_t * result2=NULL;
float * resultf=NULL;
uint8_t * results2=NULL;
float * resultsf=NULL;

const char * Fnm=reinterpret_cast<const char *>("/shader/shader.wgsl");
const char * FnmF2=reinterpret_cast<const char *>("/shader/frag2.wgsl");
const char * FnmV=reinterpret_cast<const char *>("/shader/vert.wgsl");
static char * result=NULL;
static char8_t * result8=NULL;
static char * results=NULL;

uint32_t indices[35]={3,0,1,1,2,3,4,0,3,3,7,4,1,5,6,6,2,1,4,7,6,6,5,4,2,6,6,7,3,0,4,1,1,4,5};

WGpuBufferDescriptor bufferDescriptor_indice={};
WGpuVertexAttribute vertAtt={};
WGpuVertexAttribute vertAtt2={};
WGpuVertexBufferLayout vertBufLayouts[2];
WGpuVertexBufferLayout vertBufLayout={};
WGpuBufferDescriptor bufferDescriptor_vertex={};
WGpuBufferDescriptor bufferDescriptor_vertex_UV={};
WGpuBufferBindingLayout bufferBindingLayoutV={};
WGpuBufferBindingLayout bufferBindingLayoutF={};

//  egl render (no texture uv)
const char * vertexShaderEG =
R"delimiter(@vertex
fn main(@location(0) position: vec4<f32>) -> @builtin(position) vec4<f32> {
return vec4<f32>(position.xyzw);
})delimiter";

//  with index/vertex buffers and tex
const char * vertexShaderTX =
R"delimiter(
  //   // 
struct VertexInput {
    @location(0) position : vec4<f32>,
    @location(1) uv : vec2<f32>,
};
struct VertexOutput {
    @builtin(position) position : vec4<f32>,
    @location(0) fragUV : vec2<f32>, 
};
@vertex
fn main(vertexInput : VertexInput) -> VertexOutput {
    var output : VertexOutput;
    output.position = vertexInput.position;
    output.fragUV = vertexInput.uv;
    return output;
}
  //   //  
)delimiter";

     // non vertex index / indice index shader
const char * vertexShader=
"struct VertexOutput{\n"
"@builtin(position) Position : vec4<f32>,\n"
"@location(0) fragUV : vec2<f32>\n"
"};\n"
"@vertex\n"
"fn main(@builtin(vertex_index) VertexIndex : u32) -> VertexOutput {\n"
"var pos=array<vec2<f32>,6>(\n"
"vec2<f32>(1.0f,1.0f),\n"
"vec2<f32>(1.0f,-1.0f),\n"
"vec2<f32>(-1.0f,-1.0f),\n"
"vec2<f32>(1.0f,1.0f),\n"
"vec2<f32>(-1.0f,-1.0f),\n"
"vec2<f32>(-1.0f,1.0f)\n"
");\n"
"var uv=array<vec2<f32>,6>(\n"
"vec2<f32>(1.0f,0.0f),\n"
"vec2<f32>(1.0f,1.0f),\n"
"vec2<f32>(0.0f,1.0f),\n"
"vec2<f32>(1.0f,0.0f),\n"
"vec2<f32>(0.0f,1.0f),\n"
"vec2<f32>(0.0f,0.0f)\n"
");\n"
"var output : VertexOutput;\n"
"output.Position=vec4(pos[VertexIndex],0.0f,1.0f);\n"
"output.fragUV=uv[VertexIndex];\n"
"return output;\n"
"}\n";

const char * frag_body=
"@group(0) @binding(0) var <uniform> iTime : f32;\n"
"@group(0) @binding(1) var mySampler : sampler;\n"
"@group(0) @binding(2) var myTexture : texture_2d <f32>;\n"
// "@group(0) @binding(3) var extTexture : texture_external;\n"
"@fragment\n"
"fn main(@location(0) fragUV : vec2<f32>) ->\n"
"@location(0) vec4<f32> {\n"
"return textureSample(myTexture,mySampler,fragUV);"
"}\n";

WGpuVertexAttribute atts[2];

WGpuRenderPassTimestampWrites renderTimestampWrites={};
WGPU_TEXTURE_FORMAT canvasFormat;
WGPU_TEXTURE_FORMAT canvasViewFormat[1];
WGpuCanvasConfiguration config={};
WGpuOrigin3D OriginXYZ={0,0,0};
WGpuOrigin3D originXYZ={};
// OriginXYZ.x=0;
// OriginXYZ.y=0;
// OriginXYZ.z=0;
WGpuOrigin2D OriginXY={0,0};
WGpuOrigin2D originXY={};
// OriginXY.x=0;
// OriginXY.y=0;
WGPU_TEXTURE_FORMAT textureBviewFormats[1];
WGPU_TEXTURE_FORMAT videoViewFormats[1];
WGPU_TEXTURE_FORMAT depthViewFormats[1];
WGPU_TEXTURE_FORMAT depthViewFormats2[1];
WGPU_TEXTURE_FORMAT textureAviewFormats[1];
WGpuColorTargetState colorTarget32={};
WGpuColorTargetState colorTarget={};
WGpuOrigin3D xyz={};
WGpuColor clearColor={};
WGpuCommandEncoder wceA={};
WGpuCommandEncoder wceB={};
WGPUImageCopyBuffer videoFrmBfrSrc={};
WGPUImageCopyBuffer videoFrmBfrDst={};
WGpuExternalTextureBindingLayout extTextureBindingLayout={};
WGpuExternalTextureDescriptor extTextureDescriptor={};
WGpuTextureViewDescriptor depthTextureViewDescriptor={};
WGpuTextureViewDescriptor depthTextureViewDescriptor2={};
WGpuTextureViewDescriptor colorTextureViewDescriptor={};
WGpuTextureViewDescriptor videoTextureViewDescriptor={};
WGpuTextureViewDescriptor INTextureViewDescriptor={};
WGpuTextureViewDescriptor OUTTextureViewDescriptor={};
WGpuTextureViewDescriptor MSTextureViewDescriptor={};
WGpuRenderPassColorAttachment colorAttachment={};
WGpuRenderPassColorAttachment videoAttachment={};
WGpuRenderPassDepthStencilAttachment depthAttachment={};
WGpuRenderPassDepthStencilAttachment depthAttachment2={};
WGpuSampler videoSampler={};
WGpuSampler resizeSampler={};
WGpuTextureDescriptor highbitTextureDescriptor={};
WGpuBufferDescriptor bufferDescriptorSrc={};
WGpuBufferDescriptor bufferDescriptorDst={};
WGpuBufferDescriptor bufferDescriptorVid={};
WGpuSamplerDescriptor resizeSamplerDescriptor={};
WGpuSamplerDescriptor videoSamplerDescriptor={};
WGpuTextureDescriptor depthTextureDescriptor={};
WGpuTextureDescriptor depthTextureDescriptor2={};
WGpuTextureDescriptor colorTextureDescriptor={};
WGpuTextureDescriptor videoTextureDescriptor={};
WGpuTextureDescriptor MSTextureDescriptor={};
  WGpuTextureDescriptor msaaTextureDesc={};
WGpuRenderPassDescriptor passDesc={};
WGpuRenderPassDescriptor passDesc2={};
WGpuShaderModuleDescriptor shaderModuleDescV={};
WGpuShaderModuleDescriptor shaderModuleDescF={};
WGpuShaderModuleDescriptor shaderModuleDescF2={};
WGpuDepthStencilState depthState={};
WGpuDepthStencilState depthState2={};
WGpuVertexState vertState;
WGpuPrimitiveState priState={};
WGpuFragmentState fragState;
WGpuFragmentState fragState2;
WGpuBufferDescriptor bufferDescriptorUni={};
WGpuBufferDescriptor bufferDescriptor_iTime={};
WGpuBufferDescriptor bufferDescriptor_iResolution={};
WGpuBufferDescriptor bufferDescriptor_iResolution_2={};
WGpuBufferDescriptor bufferDescriptor_iFrame={};
WGpuBufferDescriptor bufferDescriptor_iTimeDelta={};
WGpuBindGroupLayout bindgroup_layout=0;
WGpuBindGroupLayout bindgroup_layout_2=0;
WGpuBindGroupLayoutEntry Render_Bindgroup_Layout_Entries[8]={};
WGpuBindGroupLayoutEntry Render_Bindgroup_Layout_Entries_2[8]={};
WGpuBindGroupEntry Render_Bindgroup_Entries[8]={};
WGpuBindGroupEntry Render_Bindgroup_Entries_2[8]={};
WGpuBindGroupEntry bindgroup_entries[8]={};  //  for video.cpp
WGpuRenderBundleEncoderDescriptor renderBundleEncoderDescriptor={};
WGpuDeviceDescriptor deviceDesc={};
WGpuMultisampleState multiSamp={};
WGpuMultisampleState multiSamp2={};
WGpuBufferBindingLayout bufferBindingLayout1={};
WGpuBufferBindingLayout bufferBindingLayoutR={};
WGpuBufferBindingLayout bufferBindingLayoutUVEC={};
WGpuBufferBindingLayout bufferBindingLayoutU={};
WGpuTextureBindingLayout textureBindingLayoutUint32={};
WGpuTextureBindingLayout textureBindingLayoutFloat={};
WGpuTextureBindingLayout textureBindingLayoutFloatM={};
WGpuTextureBindingLayout textureBindingLayoutFloat32={};
WGpuTextureBindingLayout textureBindingLayoutDepth={};
WGpuTextureBindingLayout textureBindingLayout1={}; // for video.cpp
WGpuSamplerBindingLayout samplerBindingLayout={};
WGpuImageCopyExternalImage videoFrm={};

static wdss_tensor wdss=wdss_tensor{2,2};
static wvs_tensor wvs=wvs_tensor{2,2};
static wps_tensor wps=wps_tensor{2,2};
static wfs_tensor wfs=wfs_tensor{2,2};

static wib_tensor wib=wib_tensor{3,3};
static d_tensor d64_uniform=d_tensor{8,8};
static f_tensor f32_uniform=f_tensor{8,8};
static ui32_tensor u32_uniform=ui32_tensor{2,2};
static wvbl_tensor wvbl=wvbl_tensor{2,2};
static wcolor_tensor clearC=wcolor_tensor{2,2};
static xyz_tensor oxyz=xyz_tensor{2,2};
static xy_tensor oxy=xy_tensor{2,2};
static wtbl_tensor wtbl=wtbl_tensor{4,4};
static wetbl_tensor wetbl=wetbl_tensor{4,4};
static i_tensor on=i_tensor{6,6};
static i_tensor on_b=i_tensor{6,6};
static wetd_tensor wetd=wetd_tensor{2,2};
static wet_tensor wet=wet_tensor{2,2};
static i_tensor texid=i_tensor{2,2};
static i_tensor sze=i_tensor{8,8};
static Vi_tensor szeV=Vi_tensor{8,8};
static f_tensor szef=f_tensor{2,2};
static wce_tensor wce=wce_tensor{2,2};
static wrpe_tensor wrpe=wrpe_tensor{2,2};
static wcb_tensor wcb=wcb_tensor{2,2};
static wd_tensor wd=wd_tensor{2,2};
static wq_tensor wq=wq_tensor{2,2};
static wa_tensor wa=wa_tensor{2,2};
static wcc_tensor wcc=wcc_tensor{2,2};
static wccf_tensor wccf=wccf_tensor{2,2};
static wrp_tensor wrp=wrp_tensor{2,2};
static wpl_tensor wrpl=wpl_tensor{2,2};
static wb_tensor wb=wb_tensor{12,12};
static wbgle_tensor wbgle=wbgle_tensor{2,2};
static wbge_tensor wbge=wbge_tensor{2,2};
static wbgl_tensor wbgl=wbgl_tensor{2,2};
static wbg_tensor wbg=wbg_tensor{2,2};
static wrpd_tensor wrpd=wrpd_tensor{2,2};
static wrpca_tensor wrpca=wrpca_tensor{2,2};
static wbbl_tensor wbbl=wbbl_tensor{6,6};
static wsbl_tensor wsbl=wsbl_tensor{2,2};
static wbd_tensor wbd=wbd_tensor{12,12};
static wao_tensor wao=wao_tensor{2,2};
static wdd_tensor wdd=wdd_tensor{2,2};
static u64_tensor u64_uni=u64_tensor{8,8};
static u64_tensor u64_siz=u64_tensor{8,8};
static wrbe_tensor wrbe=wrbe_tensor{2,2};
static wrbed_tensor wrbed=wrbed_tensor{2,2};
static wrpdsa_tensor wrpdsa=wrpdsa_tensor{2,2};
static wt_tensor wt=wt_tensor{8,8};
static wtd_tensor wtd=wtd_tensor{8,8};
static wtvd_tensor wtvd=wtvd_tensor{8,8};
static wtf_tensor wtf=wtf_tensor{5,5};
static wtv_tensor wtv=wtv_tensor{8,8};
static wicb_tensor wicb=wicb_tensor{6,6};
static wicei_tensor wicei=wicei_tensor{2,2};
static js_tensor js_data_pointer=js_tensor{2,2};
static fjs_tensor fjs_data_pointer=fjs_tensor{2,2};
static fjsv_tensor fjsv_data_pointer=fjsv_tensor{2,2};
static js_data_tensor frame_tensor=js_data_tensor{2,2};
static js_data_tensorf frame_tensorf=js_data_tensorf{2,2};
static js_data_tensor64 frame_tensor64=js_data_tensor64{2,2};
static js_data_tensorGL frame_tensorGL=js_data_tensorGL{2,2};
static u64_tensor u64_bfrSze=u64_tensor{4,4};
static u64_tensor compute_xyz=u64_tensor{1,3};
static uniform_vector_tensor u64v=uniform_vector_tensor{4,4};
static wict_tensor wict=wict_tensor{8,8};
static wictt_tensor wictt=wictt_tensor{3,3};
static wsd_tensor wsd=wsd_tensor{2,2};
static ws_tensor wsmp=ws_tensor{4,4};
static v_tensor imgData=v_tensor{2,2};
static wq_tensor WGPU_Queue=wq_tensor{1,1,2};
static wcb_tensor WGPU_CommandBuffer=wcb_tensor{1,1,3};
static wb_tensor WGPU_Buffers=wb_tensor{3,3,3};
static wce_tensor WGPU_CommandEncoder=wce_tensor{1,1,4};
static wcpe_tensor WGPU_ComputePassCommandEncoder=wcpe_tensor{1,1,3};
static wcpd_tensor WGPU_ComputePassDescriptor=wcpd_tensor{1,1,3};
static wcp_tensor WGPU_ComputePipeline=wcp_tensor{3,3,3};
static wpl_tensor WGPU_ComputePipelineLayout=wpl_tensor{1,1,1};
static wsm_tensor WGPU_ComputeModule=wsm_tensor{3,3,3};
static wbg_tensor WGPU_BindGroup=wbg_tensor{1,1,2};
static wbgl_tensor WGPU_BindGroupLayout=wbgl_tensor{1,1,2};
static wbgle_tensor WGPU_Compute_Bindgroup_Layout_Entries=wbgle_tensor{1,1,2};
static wva_tensor wva=wva_tensor{2,2};
static wbge_tensor WGPU_BindGroupEntries=wbge_tensor{1,1,2};
static wbmc_tensor WGPU_MapCallback=wbmc_tensor{1,1,3};
static wdc_tensor WGPU_ComputeDoneCallback=wdc_tensor{1,1,3};
static wbbl_tensor WGPU_BufferBindingLayout=wbbl_tensor{1,1,4};
static wbd_tensor WGPU_BufferDescriptor=wbd_tensor{1,1,4};
static wsmd_tensor WGPU_ShaderModuleDescriptor=wsmd_tensor{3,3,3};
static di_tensor WGPU_BufferMappedRange=di_tensor{1,1,1};
static void_tensor WGPU_UserData=void_tensor{1,1,2};
static fptr_tensor WGPU_ResultBuffer=fptr_tensor{1,1,1};
static fptr_tensor WGPU_InputBuffer=fptr_tensor{1,1,1};
static i53_tensor WGPU_BufferRange=i53_tensor{1,1,2};
static i53_tensor WGPU_BufferSize=i53_tensor{1,1,1};
static wt_tensor WGPU_Texture=wt_tensor{8,8,8};
static wtd_tensor WGPU_TextureDescriptor=wtd_tensor{8,8,8};
static wstbl_tensor WGPU_StorageTextureBindingLayout=wstbl_tensor{3,3,3};
static wetbl_tensor WGPU_ExternalTextureBindingLayout=wetbl_tensor{3,3,3};
static wtvd_tensor WGPU_TextureViewDescriptor=wtvd_tensor{8,8,8};
static uiptr_tensor WGPU_ColorBuffer=uiptr_tensor{1,1,1};
static wced_tensor WGPU_CommandEncoderDescriptor=wced_tensor{1,1,1};
static wbms_tensor WGPU_BufferStatus=wbms_tensor{1,1,1};
static c_tensor wgsl=c_tensor{2,2};
static clk_tensor time_pnt=clk_tensor{4,4};
static timespn_tensor time_spn=timespn_tensor{4,4};

static mouse_tensor mms=mouse_tensor{2,2};
static mouse_tensor mms2=mouse_tensor{2,2};
static vec4_tensor v4f32_uniform=vec4_tensor{2,2};

EMSCRIPTEN_RESULT retCl,retMu,retMd,retMv,retSa,retSb,retSc;

WGpuComputePassDescriptor computePassDescriptor={};
WGpuCommandBufferDescriptor commandBufferDescriptor={};
WGpuCommandEncoderDescriptor commandEncoderDescriptor={};
WGpuDeviceDescriptor deviceDescriptor={};
WGpuBindGroupLayoutEntry Compute_Bindgroup_Layout_Entries[18]={};
WGpuBindGroupLayoutEntry bindgroup_layout_entries[18]={};
WGpuShaderModuleCompilationHint shaderModuleCompilationHint={};
WGpuBindGroupEntry Compute_Bindgroup_Entries[18]={};
WGpuBufferBindingLayout bufferBindingLayoutIn={3};
WGpuBufferBindingLayout bufferBindingLayoutOut={2};
WGpuBufferBindingLayout bufferBindingLayout3={2};
WGpuBufferBindingLayout bufferBindingLayout4={2};
WGpuStorageTextureBindingLayout storageTextureBindingLayoutFloat={};
WGpuStorageTextureBindingLayout storageTextureBindingLayoutFloat32={};
WGpuRequestAdapterOptions options={};
WGpuBufferDescriptor bufferDescriptorI={};
WGpuBufferDescriptor bufferDescriptorO={};
WGpuBufferDescriptor bufferDescriptorM={};
WGpuBufferDescriptor bufferDescriptorC={};
WGpuTextureDescriptor textureDescriptorIn={};
WGpuTextureDescriptor textureDescriptorInV={};
WGpuTextureDescriptor textureDescriptorOut={};
WGpuTextureDescriptor textureDescriptorOut2={};
WGpuTextureDescriptor textureDescriptorBfr={};
WGpuTextureViewDescriptor textureViewDescriptorBfr={};
WGpuTextureViewDescriptor textureViewDescriptorIn={};
WGpuTextureViewDescriptor textureViewDescriptorInV={};
WGpuTextureViewDescriptor textureViewDescriptorOut={};
WGpuTextureViewDescriptor textureViewDescriptorOut2={};
  WGpuTextureViewDescriptor textureViewDescriptorMSAA={};
// char * cmp_bdy=wgl_cmp_src;
WGpuShaderModuleDescriptor shaderModuleDescriptor={};
std::random_device randomizer;
WGpuImageCopyTexture Input_Image_Texture={};
WGpuImageCopyTexture Input_Image_TextureV={};
WGpuImageCopyTexture Output_Image_Texture={};
WGpuImageCopyTexture Output_Image_Texture2={};
WGpuImageCopyTexture WGPU_Output_Image_Bfr={};
WGpuImageCopyTextureTagged External_Image_Texture={};
WGpuImageCopyBuffer Input_Image_Buffer={};
WGpuImageCopyBuffer Output_Image_Buffer={};
WGpuImageCopyBuffer Mapped_Image_Buffer={};


