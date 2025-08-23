#include <boost/cstdfloat.hpp>  // must be first include
// #include <stdfloat.hpp>  // wait for clang 16
#include <emscripten/bind.h>

// #include "/usr/include/eigen3/Eigen/Eigen"
#include <omp.h>

#include "../../include/vanilla/avx.hpp"
#include "../../include/vanilla/defs.hpp"
#include "../../include/vanilla/gl.hpp"
#include "../../include/vanilla/boost_defs.hpp"
#include "../../lib/lib_webgpu_cpp20.cpp"
// #include <SDL2/SDL.h>
#include "../../include/vanilla/egl.hpp"

// #include "../../highway/hwy/foreach_target.h"
// #include "../../highway/hwy/highway.h"

using namespace std;
#include <cstdio> // C++ style
#include <cstdarg> // C++ style

/*   aubio needs  < c++17
#include "/content/RAMDRIVE2/aubio/src/aubio.h"
#include "/content/RAMDRIVE2/aubio/src/utils/parameter.c"
#include "/content/RAMDRIVE2/aubio/src/types.h"
#include "/content/RAMDRIVE2/aubio/src/fvec.c"
#include "/content/RAMDRIVE2/aubio/src/lvec.c"
#include "/content/RAMDRIVE2/aubio/src/cvec.c"
#include "/content/RAMDRIVE2/aubio/src/mathutils.c"
#include "/content/RAMDRIVE2/aubio/src/tempo/tempo.c"
#include "/content/RAMDRIVE2/aubio/src/tempo/beattracking.c"
#include "/content/RAMDRIVE2/aubio/src/spectral/specdesc.c"
#include "/content/RAMDRIVE2/aubio/src/onset/peakpicker.c"
#include "/content/RAMDRIVE2/aubio/src/utils/log.c"
#include "/content/RAMDRIVE2/aubio/src/utils/hist.c"
#include "/content/RAMDRIVE2/aubio/src/utils/scale.c"
#include "/content/RAMDRIVE2/aubio/src/spectral/phasevoc.c"
#include "/content/RAMDRIVE2/aubio/src/spectral/fft.c"
#include "/content/RAMDRIVE2/aubio/src/spectral/statistics.c"
#include "/content/RAMDRIVE2/aubio/src/spectral/ooura_fft8g.c"
#include "/content/RAMDRIVE2/aubio/src/temporal/biquad.c"
#include "/content/RAMDRIVE2/aubio/src/temporal/filter.c"
*/

/*   //  other aubio
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchmcomb.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchyin.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchfcomb.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchschmitt.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchyinfft.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchyinfast.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitchspecacf.c"
#include "/content/RAMDRIVE2/aubio/src/pitch/pitch.c"
*/

// #define __EMCSCRIPTEN__ 1

#include <cstdint>
// #include <pthread.h>
#include <boost/container/vector.hpp>

#include <boost/array.hpp>

#include <boost/integer.hpp>
#include <boost/atomic.hpp>
#include <boost/numeric/ublas/tensor.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/vector.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/chrono.hpp>

// #include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
 
#include <boost/compute/core.hpp>   //  requires CL.h

#include <experimental/simd>

#include <iterator> // For std::distance
#include <array>
#include <chrono>
#include <cmath>
#include <exception>
#include <fstream>
#include <limits>
#include <numeric>
#include <string>

#include <algorithm>
#include <stdarg.h>
#include <stdio.h>
#include <ctime>
// #include <vector>
#include <memory>
#include <streambuf>

#include <cassert>
#include <random>
#include <cfloat>
#include <new>
#include <emscripten.h>

// #include <emscripten/threading.h>
#include <emscripten/em_types.h>
#include <emscripten/val.h>

// #include <emscripten/wasmfs.h>
#include <emscripten/html5.h>
#include <emscripten/html5_webgpu.h>
#include <iostream>
#include "../../lib/lib_webgpu.h"
/*
#include <thrust/host_vector.h>
#include <thrust/device_vector.h>
#include <thrust/generate.h>
#include <thrust/reduce.h>
#include <thrust/functional.h>
  */
#include <cstdlib>

#include <functional>

template<class ArgumentType,class ResultType>

#include <boost/function.hpp>

struct unary_function{
typedef ArgumentType argument_type;
typedef ResultType result_type;
};

inline int rNd4(int);
// static void WGPU_Run();
// static void ObtainedWebGpuDeviceStart(WGpuDevice,void *);
static void ObtainedWebGpuAdapterStart(WGpuAdapter,void *);
static void ObtainedWebGpuDeviceStart(WGpuDevice,void *);
// const char * rd_fl(const char *);
EM_BOOL getCode(const char *);
void raf();
// static void WGPU_Start();
// int cltest();

extern"C"{

void startWebGPU();
void startWebGPUi(int vsz,int sz,int sr);
void startWebGPUbi(int vsz,int sz,int sr);
void startWebGPUC(int vsz,int sz,int sr);
void runWebGPU();

}

constexpr uint32_t compute_x = 32;
constexpr uint32_t compute_y = 8;
constexpr uint32_t compute_z = 1;

WGpuBuffer indice_Buffer;
WGpuBuffer vertex_Buffer;
WGpuBuffer vertex_Buffer_UV;

WGpuImageBitmap bmpImage;

WGpuImageCopyTexture videoTextureCopy;
WGpuImageCopyTexture MSTextureCopy;
WGpuExternalTexture extTexture;
WGpuTextureView depthTextureView;
WGpuTextureView depthTextureView2;
WGpuTextureView colorTextureView;
WGpuTextureView videoTextureView;
WGpuTextureView INTextureView;
WGpuTextureView BfrTextureView;
WGpuTextureView INVTextureView;
WGpuTextureView OUTTextureView;
WGpuTextureView OUTTexture2View;
WGpuTextureView MSTextureView;
  WGpuTextureView msaaTextureView;

WGpuTexture depthTexture;
WGpuTexture depthTexture2;
WGpuTexture colorTexture;
WGpuTexture videoTexture;
WGpuTexture MSTexture;
WGpuTexture textureBfr;
WGpuTexture textureIn;
WGpuTexture textureInV;
WGpuTexture textureOut;
WGpuTexture textureOut2;
WGpuTexture __128bit_Texture__;
  WGpuTexture msaaTexture;

 WGpuRenderPipelineDescriptor renderPipelineDesc;
 WGpuRenderPipelineDescriptor renderPipelineDesc2;
 WGpuShaderModule vs, fs, fs2;

WGpuBindGroup bindgroup;
WGpuBindGroup bindgroup_2;
WGpuRenderBundleEncoder renderBundleEncoder;
WGpuBuffer uniBuffer;
WGpuBuffer srcBuffer;
WGpuBuffer dstBuffer;
WGpuBuffer vidBuffer;
WGpuBuffer uni_iTime_Buffer;
WGpuBuffer uni_iTimeDelta_Buffer;
WGpuBuffer uni_iResolution_Buffer;
WGpuBuffer uni_iResolution_Buffer_2;
WGpuBuffer uni_iFrame_Buffer;

double szh,szw;
int szhI,szwI;
double szhDv,szwDv;
float szhFv,szwFv;
uint64_t tme;
void * userDataA;
void * userDataB;
WGpuTexture textureA;
WGpuBindGroupLayout bindGroupLayout;
WGpuBindGroupLayout bindGroupLayoutB;
WGpuComputePipeline computePipeline;
WGpuBuffer inputBuffer;
WGpuBuffer outputBuffer;
WGpuBuffer mapBuffer;
// WGpuBuffer uniBuffer;
WGpuShaderModule cs;
WGpuCommandBuffer commandBuffer;
WGpuCommandEncoder encoder;
WGpuComputePassEncoder computePass;
WGpuBindGroup bindGroup;
WGpuPipelineLayout pipelineLayout;
WGpuQuerySet querySet;
int randomNumber,entropySeed;
int raN;
int raND;
uint32_t outP;
double_int53_t WGPU_Range_PointerB;
double_int53_t WGPU_Range_PointerC;
long int length2;
void * fram;
static long int length;

using namespace boost::chrono;

namespace boost::numeric::ublas {
using vec4=vector<float,boost::numeric::ublas::bounded_array<float,4>>;
}

// using dv_tensor=boost::numeric::ublas::tensor<SDL_AudioDeviceID>;
using void_tensor=boost::numeric::ublas::tensor<boost::atomic<void *>>;
using gi_tensor=boost::numeric::ublas::tensor<boost::atomic<long>>;
using ub_tensor=boost::numeric::ublas::tensor<boost::atomic<unsigned char *>>;
using lu_tensor=boost::numeric::ublas::tensor<boost::atomic<unsigned long>>;
using v_tensor=boost::numeric::ublas::tensor<v128_t>;
using d_tensor=boost::numeric::ublas::tensor<emscripten_align1_double>;
using fjs_tensor=boost::numeric::ublas::tensor<emscripten_align1_float *>;
using fjsv_tensor=boost::numeric::ublas::tensor<std::vector<emscripten_align1_float> *>;
using wcolor_tensor=boost::numeric::ublas::tensor<WGpuColor>;
using uiptr_tensor=boost::numeric::ublas::tensor<uint32_t *>;
using wced_tensor=boost::numeric::ublas::tensor<WGpuCommandEncoderDescriptor>;
using wbmc_tensor=boost::numeric::ublas::tensor<WGpuBufferMapCallback>;
using wcpe_tensor=boost::numeric::ublas::tensor<WGpuComputePassEncoder>;
using wcp_tensor=boost::numeric::ublas::tensor<WGpuComputePipeline>;
using i53_tensor=boost::numeric::ublas::tensor<double_int53_t>;
using wstbl_tensor=boost::numeric::ublas::tensor<WGpuStorageTextureBindingLayout>;
using wbms_tensor=boost::numeric::ublas::tensor<WGPU_BUFFER_MAP_STATE>;
using di_tensor=boost::numeric::ublas::tensor<boost::uint_t<64>::exact>;
// using void_tensor=boost::numeric::ublas::tensor<void *>;
using wdc_tensor=boost::numeric::ublas::tensor<WGpuOnSubmittedWorkDoneCallback>;
using mouse_tensor=boost::numeric::ublas::tensor<boost::compute::double_>;
using c_tensor=boost::numeric::ublas::tensor<const char *>;
using c32_tensor=boost::numeric::ublas::tensor<const char32_t *>;
using f_tensor=boost::numeric::ublas::tensor<emscripten_align1_float>;
// using f_tensor=boost::numeric::ublas::tensor<std::bfloat16_t>; // wait for clang 16
using vec4_tensor = boost::numeric::ublas::tensor<boost::numeric::ublas::vec4>;
using fptr_tensor=boost::numeric::ublas::tensor<emscripten_align1_float *>;
using wce_tensor=boost::numeric::ublas::tensor<WGpuCommandEncoder>;
using wrpe_tensor=boost::numeric::ublas::tensor<WGpuRenderPassEncoder>;
using wcb_tensor=boost::numeric::ublas::tensor<WGpuCommandBuffer>;
using wd_tensor=boost::numeric::ublas::tensor<WGpuDevice>;
using wq_tensor=boost::numeric::ublas::tensor<WGpuQueue>;
using wb_tensor=boost::numeric::ublas::tensor<WGpuBuffer>;
using wa_tensor=boost::numeric::ublas::tensor<WGpuAdapter>;
using wcc_tensor=boost::numeric::ublas::tensor<WGpuCanvasContext>;
using wccf_tensor=boost::numeric::ublas::tensor<WGpuCanvasConfiguration>;
using wrp_tensor=boost::numeric::ublas::tensor<WGpuRenderPipeline>;
using wrpid_tensor=boost::numeric::ublas::tensor<WGpuRenderPipelineDescriptor>;
using wpl_tensor=boost::numeric::ublas::tensor<WGpuPipelineLayout>;
using wbg_tensor=boost::numeric::ublas::tensor<WGpuBindGroup>;
using wbgl_tensor=boost::numeric::ublas::tensor<WGpuBindGroupLayout>;
using wbgle_tensor=boost::numeric::ublas::tensor<WGpuBindGroupLayoutEntry *>;
using wva_tensor=boost::numeric::ublas::tensor<WGpuVertexAttribute *>;
using wbge_tensor=boost::numeric::ublas::tensor<WGpuBindGroupEntry *>;
using wrpd_tensor=boost::numeric::ublas::tensor<WGpuRenderPassDescriptor>;
using wcpd_tensor=boost::numeric::ublas::tensor<WGpuComputePassDescriptor>;
using wrpca_tensor=boost::numeric::ublas::tensor<WGpuRenderPassColorAttachment>;
using wbbl_tensor=boost::numeric::ublas::tensor<WGpuBufferBindingLayout>;
using wtbl_tensor=boost::numeric::ublas::tensor<WGpuTextureBindingLayout>;
using wetbl_tensor=boost::numeric::ublas::tensor<WGpuExternalTextureBindingLayout>;
using wsbl_tensor=boost::numeric::ublas::tensor<WGpuSamplerBindingLayout>;
using wbd_tensor=boost::numeric::ublas::tensor<WGpuBufferDescriptor>;
using wao_tensor=boost::numeric::ublas::tensor<WGpuRequestAdapterOptions>;
using wdd_tensor=boost::numeric::ublas::tensor<WGpuDeviceDescriptor>;
using wrbe_tensor=boost::numeric::ublas::tensor<WGpuRenderBundleEncoder>;
using wrbed_tensor=boost::numeric::ublas::tensor<WGpuRenderBundleEncoderDescriptor>;
using wrpdsa_tensor=boost::numeric::ublas::tensor<WGpuRenderPassDepthStencilAttachment>;
using u64_tensor=boost::numeric::ublas::tensor<uint32_t>;
using i_tensor=boost::numeric::ublas::tensor<emscripten_align1_int>;
using Vi_tensor=boost::numeric::ublas::tensor<emscripten_align1_int>;
// using i_tensor=boost::numeric::ublas::tensor<boost::int_t<64>::exact>;
using ui32_tensor=boost::numeric::ublas::tensor<boost::uint_t<32>::exact>;
using wt_tensor=boost::numeric::ublas::tensor<WGpuTexture>;
using wtd_tensor=boost::numeric::ublas::tensor<WGpuTextureDescriptor>;
using wtvd_tensor=boost::numeric::ublas::tensor<WGpuTextureViewDescriptor>;
using wtf_tensor=boost::numeric::ublas::tensor<WGPU_TEXTURE_FORMAT>;
using wtv_tensor=boost::numeric::ublas::tensor<WGpuTextureView>;
using js_tensor=boost::numeric::ublas::tensor<uint8_t *>;
using js_data_tensor=boost::numeric::ublas::tensor<std::vector<uint8_t>>;
using js_data_tensorf=boost::numeric::ublas::tensor<std::vector<emscripten_align1_float>>;
using js_data_tensorGL=boost::numeric::ublas::tensor<std::vector<GLubyte>>;
using js_data_tensor64=boost::numeric::ublas::tensor<std::vector<uint64_t>>;
using uniform_vector_tensor=boost::numeric::ublas::tensor<std::vector<uint32_t>>;
using xyz_tensor=boost::numeric::ublas::tensor<WGpuOrigin3D>;
using xy_tensor=boost::numeric::ublas::tensor<WGpuOrigin2D>;
using wicb_tensor=boost::numeric::ublas::tensor<WGpuImageCopyBuffer>;
using wicei_tensor=boost::numeric::ublas::tensor<WGpuImageCopyExternalImage>;
using wict_tensor=boost::numeric::ublas::tensor<WGpuImageCopyTexture>;
using wictt_tensor=boost::numeric::ublas::tensor<WGpuImageCopyTextureTagged>;
using ws_tensor=boost::numeric::ublas::tensor<WGpuSampler>;
using wsd_tensor=boost::numeric::ublas::tensor<WGpuSamplerDescriptor>;
using wms_tensor=boost::numeric::ublas::tensor<WGpuMultisampleState>;
using wsm_tensor=boost::numeric::ublas::tensor<WGpuShaderModule>;
using wsmd_tensor=boost::numeric::ublas::tensor<WGpuShaderModuleDescriptor>;
using wcts_tensor=boost::numeric::ublas::tensor<WGpuColorTargetState>;
using wdss_tensor=boost::numeric::ublas::tensor<WGpuDepthStencilState>;
using wvs_tensor=boost::numeric::ublas::tensor<WGpuVertexState>;
using wps_tensor=boost::numeric::ublas::tensor<WGpuPrimitiveState>;
using wfs_tensor=boost::numeric::ublas::tensor<WGpuFragmentState>;
using wcts_tensor=boost::numeric::ublas::tensor<WGpuColorTargetState>;
using wetd_tensor=boost::numeric::ublas::tensor<WGpuExternalTextureDescriptor>;
using wet_tensor=boost::numeric::ublas::tensor<WGpuExternalTexture>;
using wvbl_tensor=boost::numeric::ublas::tensor<WGpuVertexBufferLayout>;
using wib_tensor=boost::numeric::ublas::tensor<WGpuImageBitmap>;
using clk_tensor=boost::numeric::ublas::tensor<boost::chrono::high_resolution_clock::time_point>;
using timespn_tensor=boost::numeric::ublas::tensor<boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>>;

EM_BOOL ms_l,clk_l;
