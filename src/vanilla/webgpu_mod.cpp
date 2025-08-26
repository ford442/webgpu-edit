
#include "../../include/vanilla/webgpu_em.hpp"
#include "../../src/vanilla/webgpu_compute_vars_em.cpp"

namespace stdx = std::experimental;

namespace bfs = boost::filesystem;

using float_simd = stdx::simd<float>;
using uint8_simd = stdx::native_simd<uint8_t>; // Use native uint8_t SIMD size
using uint16_simd = stdx::native_simd<uint16_t>;
using uint32_simd = stdx::native_simd<uint32_t>;

namespace fsm = boost::filesystem;

static boost::container::vector<emscripten_align1_float> pixel_buffer;

EM_BOOL buffer_resize(emscripten_align1_int sz){
compute_xyz.at(0,0)=std::max(1,(sze.at(1,1)+15)/16);
compute_xyz.at(0,1)=std::max(1,(sze.at(1,1)+15)/16);
compute_xyz.at(0,2)=2;
size_t num_elements = (size_t)sz * sz * 4;
pixel_buffer.resize(num_elements);
return EM_TRUE;
}

/**
 * @brief Resizes a specific input texture by recreating it and its associated resources.
 * * In WebGPU, textures are immutable. To resize a texture, we must release the old
 * texture and its view, update the texture's descriptor with the new size,
 * and then recreate the texture, its view, and any bind groups that use it.
 *
 * @param newSize The new width and height for the texture. The texture is assumed to be square.
 */
void resizeInputTexture(emscripten_align1_int newSize) {
    emscripten_log(EM_LOG_CONSOLE, "Resizing input texture to %dx%d", newSize, newSize);
    if (WGPU_BindGroup.at(0,0,0)) {
        wgpu_object_destroy(WGPU_BindGroup.at(0,0,0));
    }
    if (wtv.at(6,6)) { // wtv.at(6,6) holds INVTextureView
        wgpu_object_destroy(wtv.at(6,6));
    }
    if (WGPU_Texture.at(0,0,3)) { // WGPU_Texture.at(0,0,3) holds textureInV
        wgpu_object_destroy(WGPU_Texture.at(0,0,3));
    }
    szeV.at(7,7) = newSize; // Update the global size variable
    sze.at(3,3)=static_cast<emscripten_align1_int>(newSize);
    textureDescriptorInV.width = newSize;
    textureDescriptorInV.height = newSize;
    WGPU_TextureDescriptor.at(0,0,3) = textureDescriptorInV; // Store it back in the global array
    textureInV = wgpu_device_create_texture(wd.at(0,0), &WGPU_TextureDescriptor.at(0,0,3));
    WGPU_Texture.at(0,0,3) = textureInV;
    INVTextureView = wgpu_texture_create_view(WGPU_Texture.at(0,0,3), &WGPU_TextureViewDescriptor.at(0,0,3));
    wtv.at(6,6) = INVTextureView;
    Compute_Bindgroup_Entries[8].resource = wtv.at(6,6); // wtv.at(6,6) is INVTextureView
    wict.at(4,4).texture = WGPU_Texture.at(0,0,3);
    WGPU_BindGroup.at(0,0,0) = wgpu_device_create_bind_group(wd.at(0,0), WGPU_BindGroupLayout.at(0,0,0), WGPU_BindGroupEntries.at(0,0,0), 10);
    emscripten_log(EM_LOG_CONSOLE, "Input texture resize complete.");
}


void process_image(const char * img_data, int size) {
    int width, height, channels;
    unsigned char* pixels = stbi_load_from_memory(reinterpret_cast<const unsigned char*>(img_data), size, &width, &height, &channels, 0);
    if (pixels) {
       // std::cout << "Image decoded: " << width << "x" << height << " with " << channels << " channels." << std::endl;
        int decoded_size = width * height * 4;
     //   buffer_resize(height);
     //   pixel_buffer.insert(pixel_buffer.end(), pixels, pixels + decoded_size);

        std::ofstream outfile("/video/frame.gl", std::ios::binary);
        if (outfile) {
            outfile.write((char*)pixels, decoded_size);
            outfile.close();
          //  std::cout << "File 'decoded_image.raw' saved to the virtual filesystem." << std::endl;
       //     on_b.at(5,5)=1;
        } else {
            std::cerr << "Failed to open 'decoded_image.raw' for writing in the VFS." << std::endl;
        }
     
      //  resizeInputTexture(height);
        // szeV.at(7,7) = height;
        // on_b.at(4,4)=1;
    //    texOn();
        stbi_image_free(pixels);
    } else {
        std::cerr << "Failed to decode image from memory." << std::endl;
    }
}

void downloadSucceeded(emscripten_fetch_t * fetch) {
    std::cout << "Finished downloading " << fetch->numBytes << " bytes from " << fetch->url << std::endl;
    process_image(fetch->data,fetch->numBytes);
    emscripten_fetch_close(fetch);
}

void downloadFailed(emscripten_fetch_t * fetch) {
   // std::cerr << "Downloading " << fetch->url << " failed! HTTP status code: " << fetch->status << std::endl;
    emscripten_fetch_close(fetch);
}

void fetcher(const std::string & fl_nm) {
    emscripten_fetch_attr_t attr;
    emscripten_fetch_attr_init(&attr);
    strcpy(attr.requestMethod, "GET");
    attr.attributes = EMSCRIPTEN_FETCH_LOAD_TO_MEMORY;
    attr.onsuccess = downloadSucceeded;
    attr.onerror = downloadFailed;
   // std::cout << "Requesting file from server..." << std::endl;
    emscripten_fetch(&attr, fl_nm.c_str());
    return;
}

emscripten::val getPixelBufferView() {
return emscripten::val(emscripten::typed_memory_view(pixel_buffer.size(), pixel_buffer.data()));
}

uintptr_t get_buffer_ptr() {
return reinterpret_cast<uintptr_t>(pixel_buffer.data());
}

void process_copied_data_val(emscripten::val js_typed_array_val) {
std::vector<emscripten_align1_float> cpp_copy = emscripten::vecFromJSArray<emscripten_align1_float>(js_typed_array_val);
size_t num_elements = (size_t)cpp_copy.size();
pixel_buffer.resize(num_elements);
std::transform(cpp_copy.begin(),cpp_copy.end(),pixel_buffer.begin(),[](float val){return val;});
if(on.at(3,3)==1){
on_b.at(4,4)=1;
}
}

EM_BOOL ms_clk(int32_t eventType,const EmscriptenMouseEvent * e,void * userData){
if(e->screenX!=0&&e->screenY!=0&&e->clientX!=0&&e->clientY!=0&&e->targetX!=0&&e->targetY!=0){
if(eventType==EMSCRIPTEN_EVENT_MOUSEDOWN&&e->buttons!=0){
ms_l=true;
}
if(eventType==EMSCRIPTEN_EVENT_MOUSEUP){
ms_l=false;
}}
return EM_TRUE;
}

EM_BOOL ms_mv(int32_t eventType,const EmscriptenMouseEvent * e,void * userData){
if(e->screenX!=0&&e->screenY!=0&&e->clientX!=0&&e->clientY!=0&&e->targetX!=0&&e->targetY!=0){
if(eventType==EMSCRIPTEN_EVENT_MOUSEMOVE&&(e->movementX!=0||e->movementY!=0)){
mms2.at(0,0)=e->clientX;
mms2.at(0,1)=e->clientY;
}}
return EM_TRUE;
}

WGpuBufferMapCallback mapCallbackStart=[](WGpuBuffer buffer,void * userData,WGPU_MAP_MODE_FLAGS mode,double_int53_t offset,double_int53_t size){
return;
};

/*
WGpuLoadImageBitmapCallback imageCallbackStart=[](WGpuImageBitmap bitmap,emscripten_align1_int width,emscripten_align1_int height,void *userData){
bmpImage=bitmap;
wib.at(0,0)=bmpImage;
return;
};
*/

WGpuOnSubmittedWorkDoneCallback onComputeDoneStart=[](WGpuQueue queue,void *userData){
return;
};

int rNd4(emscripten_align1_int randomMax){
entropySeed=(randomMax)*randomizer();
std::srand(entropySeed);
randomNumber=std::rand()%randomMax;
return randomNumber;
}

const char * rd_fl(const char * Fnm){
FILE * file=fopen(Fnm,"r");
::boost::tuples::tie(result,results,file);
if(file){
int32_t stat=fseek(file,(int32_t)0,SEEK_END);
if(stat!=0){
fclose(file);
return nullptr;
}
length=ftell(file);
stat=fseek(file,(int32_t)0,SEEK_SET);
if(stat!=0){
fclose(file);
return nullptr;
}
result=static_cast<char *>(malloc((length+1)*sizeof(char)));
if(result){
size_t actual_length=fread(result,sizeof(char),length,file);
result[actual_length++]={'\0'};
}
fclose(file);
return result;
}
return nullptr;
}

EM_BOOL getCode(const char * Fnm){
wgsl.at(0,0)=frag_body;
return EM_TRUE;
}

EM_BOOL texOn(){
if(on.at(3,3)==1){
on_b.at(4,4)=1;
}
return EM_TRUE;
}

EM_BOOL cnvOn(){
if(on.at(3,3)==1){
on_b.at(5,5)=1;
}
return EM_TRUE;
}

EMSCRIPTEN_BINDINGS(my_video_module) {
emscripten::function("ftch", &fetcher);
emscripten::function("frmOn", &texOn);
emscripten::function("cnvOn", &cnvOn);
emscripten::function("getPixelBufferView", &getPixelBufferView);
emscripten::function("processCopiedDataVal", &process_copied_data_val);
emscripten::function("get_buffer_ptr", &get_buffer_ptr);
emscripten::function("sizeBuffer", &buffer_resize);
emscripten::function("resizeInputTexture", &resizeInputTexture); 
// emscripten::register_vector<float>("VectorFloat"); // Needed for vecFromJSArray
}

EM_BOOL framesOff(){
on.at(3,3)=0;
on_b.at(4,4)=0;
return EM_TRUE;
}

EM_BOOL framesOn(){
on.at(3,3)=1;
return EM_TRUE;
}

EM_BOOL PanRight(){
u64v.at(0,0)[1]++;
return EM_TRUE;
}

EM_BOOL PanLeft(){
u64v.at(0,0)[1]--;
return EM_TRUE;
}

EM_BOOL PanUp(){
u64v.at(0,0)[2]++;
return EM_TRUE;
}
EM_BOOL PanDown(){
u64v.at(0,0)[2]--;
return EM_TRUE;
}

EM_BOOL ZoomIn(){
u64v.at(0,0)[0]++;
return EM_TRUE;
}

EM_BOOL ZoomOut(){
u64v.at(0,0)[0]--;
return EM_TRUE;
}

/**
 * @brief Converts a vector of 8-bit unsigned integers to a vector of single-precision floats using 128-bit SSE instructions.
 *
 * This function is optimized for Emscripten by using 128-bit SSE intrinsics which translate
 * directly to WebAssembly's 128-bit SIMD instructions. It processes 4 elements per iteration.
 *
 * @param data The input vector of uint8_t values (0-255).
 * @param pixel_buffer The output vector where the converted float values (0.0-1.0) will be stored.
 */
void convert_u8_to_float_sse(const boost::container::vector<uint8_t>& data, boost::container::vector<float>& pixel_buffer) {
    size_t num_elements = data.size();
    if (num_elements == 0) {
        pixel_buffer.clear();
        return;
    }
    pixel_buffer.resize(num_elements);
    const float scale = 1.0f / 255.0f;
    const __m128 inv_255_ps_sse = _mm_set1_ps(scale); // 128-bit scaling vector
    const uint8_t* data_ptr = data.data();
    float* buffer_ptr = pixel_buffer.data();
    const size_t limit = (num_elements / 4) * 4;
    for (size_t i = 0; i < limit; i += 4) {
        // The upper 96 bits will be zero.
        __m128i data_u8_sse = _mm_loadu_si32(data_ptr + i);
        // Convert the 4 uint8_t values to 4 int32_t values.
        // SSE4.1's _mm_cvtepu8_epi32 is perfect for this.
        __m128i data_i32_sse = _mm_cvtepu8_epi32(data_u8_sse);
        // Convert the 4 int32_t values to 4 single-precision floats.
        __m128 data_f32_sse = _mm_cvtepi32_ps(data_i32_sse);
        // Scale the float values to the 0.0-1.0 range.
        data_f32_sse = _mm_mul_ps(data_f32_sse, inv_255_ps_sse);
        // Store the 4 resulting floats in the output buffer.
        _mm_storeu_ps(buffer_ptr + i, data_f32_sse);
    }
    // Process any remaining elements (less than 4) with a standard scalar loop.
    for (size_t i = limit; i < num_elements; ++i) {
        buffer_ptr[i] = static_cast<float>(data_ptr[i]) * scale;
    }
}

/**
 * @brief Converts a vector of 8-bit unsigned integers to a vector of single-precision floats using AVX2 instructions.
 *
 * This function is optimized for performance by processing data in chunks using SIMD (Single Instruction, Multiple Data)
 * instructions provided by the AVX2 instruction set. An OpenMP simd pragma is used to assist the compiler
 * in vectorizing the main loop.
 *
 * @param data The input vector of uint8_t values (0-255).
 * @param pixel_buffer The output vector where the converted float values (0.0-1.0) will be stored.
 */
void convert_u8_to_float_avx2(const boost::container::vector<uint8_t>& data,boost::container::vector<float>& pixel_buffer){
size_t num_elements = data.size();
if (num_elements == 0) {
        pixel_buffer.clear();
        return;
}
    pixel_buffer.resize(num_elements);
    const float scale = 1.0f / 255.0f;
    const __m256 inv_255_ps_avx = _mm256_set1_ps(scale);
    const uint8_t* data_ptr = data.data();
    float* buffer_ptr = pixel_buffer.data();
    size_t i;
    const size_t limit = (num_elements / 8) * 8;
    #pragma omp simd
    for (i = 0; i < limit; i += 8) {
        // Load 8 uint8_t values into the lower 64 bits of a 128-bit SSE register.
        __m128i data_u8_sse = _mm_loadl_epi64(reinterpret_cast<const __m128i*>(data_ptr + i));
        // --- Conversion from uint8 to float ---
        __m128i data_i16 = _mm_unpacklo_epi8(data_u8_sse, _mm_setzero_si128());
        __m256i data_i32_avx = _mm256_cvtepi16_epi32(data_i16);
        __m256 data_f32_avx = _mm256_cvtepi32_ps(data_i32_avx);
        // --- Scaling (Normalization) ---
        data_f32_avx = _mm256_mul_ps(data_f32_avx, inv_255_ps_avx);
        // --- Storage ---
        _mm256_storeu_ps(buffer_ptr + i, data_f32_avx);
    }
    #pragma omp simd
    for (i = 0; i < num_elements; ++i) {
        buffer_ptr[i] = static_cast<float>(data_ptr[i]) * scale;
    }
}

boost::function<EM_BOOL()>render=[](){
u64_uni.at(3,3)++; 

if(ms_l==true){
mms.at(0,1)=round(mms2.at(0,0)*(emscripten_align1_float)sze.at(0,0));
mms.at(1,0)=round((mms2.at(0,1))*(emscripten_align1_float)sze.at(0,0));
}
// retCl=emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,(EM_BOOL)0,ms_clk);
// retMd=emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,(EM_BOOL)0,ms_clk);
if(ms_l==true){
// retMv=emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,(EM_BOOL)0,ms_mv);
// retMu=emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,(EM_BOOL)0,ms_clk);
if(clk_l==true){
const long xxx=mms2.at(0,0);
const long yyy=mms2.at(0,1);
mms.at(0,1)=float(xxx);
mms.at(1,0)=float((float)sze.at(0,0)-((yyy-sze.at(0,0))/2));
clk_l=false;
}
mms.at(2,0)=float((emscripten_align1_float)sze.at(0,0)-mms2.at(0,0));
mms.at(2,1)=float((float)sze.at(0,0)-mms2.at(0,1));
// v4f32_uniform.at(0,0)=vector<emscripten_align1_float>({static_cast<emscripten_align1_float>(mms.at(2,0)),static_cast<emscripten_align1_float>(mms.at(2,1)),static_cast<emscripten_align1_float>(mms.at(0,1)),static_cast<emscripten_align1_float>(mms.at(1,0))});
// glUniform4f(uni_mse,mms.at(2,0),mms.at(2,1),mms.at(0,1),mms.at(1,0));
}
else{
clk_l=true;
}
u_time.t3=u_time.t2;
u_time.t2=boost::chrono::high_resolution_clock::now();
u_time.time_spana=boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>(u_time.t2-u_time.t1);
u_time.time_spanb=boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>(u_time.t2-u_time.t3);
// u64_uni.at(0,0)=u_time.time_spana.count()*100u;
// u64_uni.at(1,1)=u_time.time_spanb.count()*1000u;
// f32_uniform.at(0,0)=static_cast<emscripten_align1_float>(u_time.time_spana.count())*100000.0f;
// f32_uniform.at(0,0)=static_cast<emscripten_align1_float>(u_time.time_spana.count());
f32_uniform.at(0,0)=u_time.time_spana.count();
// u64_uni.at(2,2)=u_time.time_spanb.count()/1.0f;
colorTexture=wgpu_canvas_context_get_current_texture(wcc.at(0,0));
wt.at(1,1)=colorTexture;
colorTextureView=wgpu_texture_create_view(wt.at(1,1),&wtvd.at(1,1));
wtv.at(1,1)=colorTextureView;
  colorAttachment.view=wtv.at(7,7);
  colorAttachment.resolveTarget=wtv.at(1,1);        // <-- RESOLVE to the canvas texture
colorAttachment.depthSlice=-1;
colorAttachment.storeOp=WGPU_STORE_OP_DISCARD; // WGPU_STORE_OP_DISCARD; 
// colorAttachment.loadOp=WGPU_LOAD_OP_LOAD;
colorAttachment.loadOp=WGPU_LOAD_OP_CLEAR;
colorAttachment.clearValue=clearC.at(0,0);
wrpca.at(0,0)=colorAttachment;
INTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,0),&WGPU_TextureViewDescriptor.at(0,0,0));
wtv.at(3,3)=INTextureView;
OUTTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,1),&WGPU_TextureViewDescriptor.at(0,0,1));
wtv.at(4,4)=OUTTextureView;
OUTTexture2View=wgpu_texture_create_view(WGPU_Texture.at(0,0,2),&WGPU_TextureViewDescriptor.at(0,0,2));
wtv.at(5,5)=OUTTexture2View;
videoAttachment.view=wtv.at(3,3);
videoAttachment.depthSlice=-1;
videoAttachment.storeOp=WGPU_STORE_OP_STORE;
// videoAttachment.loadOp=WGPU_LOAD_OP_LOAD;
videoAttachment.loadOp=WGPU_LOAD_OP_CLEAR;
videoAttachment.clearValue=clearC.at(0,0);
wrpca.at(1,1)=videoAttachment;
videoTextureView=wgpu_texture_create_view(wt.at(2,2),&wtvd.at(2,2));
wtv.at(2,2)=videoTextureView;
passDesc.numColorAttachments=1;
passDesc.colorAttachments=&wrpca.at(1,1); // &wrpca.at(0,0); // 
passDesc.occlusionQuerySet=0;
// passDesc.maxDrawCount=6;
renderTimestampWrites.querySet=0;
renderTimestampWrites.beginningOfPassWriteIndex=-1;
renderTimestampWrites.endOfPassWriteIndex=-1;
passDesc.timestampWrites=renderTimestampWrites;
wrpd.at(0,0)=passDesc;
passDesc2.numColorAttachments=1;
passDesc2.colorAttachments=&wrpca.at(0,0); // &wrpca.at(1,1); //
passDesc2.occlusionQuerySet=0;
// passDesc2.maxDrawCount=6;
passDesc2.timestampWrites=renderTimestampWrites;
wrpd.at(1,1)=passDesc2;
      
if(on_b.at(5,5)==1){
fsm::ifstream fram(Fnm2,std::ios::binary);
boost::container::vector<uint8_t>data((std::istreambuf_iterator<char>(fram)),(std::istreambuf_iterator<char>()));
fram.close();
      
 // AVX 2
// convert_u8_to_float_avx2(data, pixel_buffer);
convert_u8_to_float_sse(data, pixel_buffer);
const size_t bytesPerRow=szeV.at(7,7)*4*sizeof(emscripten_align1_float);

/*      // regular
std::transform(data.begin(),data.end(),pixel_buffer.begin(),[](uint8_t val){return val/255.0f;});
const size_t bytesPerRow=szeV.at(7,7)*4*sizeof(emscripten_align1_float);
      
    //  SIMD
size_t num_elements = data.size();
pixel_buffer.resize(num_elements); // Resize pixel_buffer to hold floats
const size_t simd_size = float_simd::size(); // How many floats fit in one SIMD register
const size_t vec_size = data.size();
size_t i = 0;
float_simd inv_255(1.0f / 255.0f);
for (; i + simd_size <= vec_size; i += simd_size) {
    alignas(float_simd) std::array<uint8_t, simd_size> temp_u8;
    std::copy(data.begin() + i, data.begin() + i + simd_size, temp_u8.begin());
    float_simd data_chunk_f;
    for(size_t k=0; k < simd_size; ++k) {
        data_chunk_f[k] = static_cast<float>(temp_u8[k]); // Element-wise assignment for conversion
    }
    auto result_chunk = data_chunk_f * inv_255; // SIMD multiplication
    result_chunk.copy_to(pixel_buffer.data() + i, std::experimental::element_aligned);
}
for (; i < vec_size; ++i) {
    pixel_buffer[i] = static_cast<float>(data[i]) / 255.0f;
}
const size_t bytesPerRow = szeV.at(7,7) * 4 * sizeof(emscripten_align1_float); // Should this be pixel_buffer.size() * sizeof(float) / height? Or width*4*sizeof(float)? Check calculation.
*/
      
wgpu_queue_write_texture(WGPU_Queue.at(0,0,0),&wict.at(4,4),pixel_buffer.data(),bytesPerRow,szeV.at(7,7),szeV.at(7,7),szeV.at(7,7),1);
on_b.at(5,5)=0;
}
      
if(on_b.at(4,4)==1){

INVTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,3),&WGPU_TextureViewDescriptor.at(0,0,3));
wtv.at(6,6)=INVTextureView;
      
      //  Frame Data 
// std::ifstream fram(Fnm2,std::ios::binary);
// fsm::ifstream fram(Fnm2,std::ios::binary);

// boost::container::vector<uint8_t>data((std::istreambuf_iterator<char>(fram)),(std::istreambuf_iterator<char>()));
// boost::container::vector<emscripten_align1_float>floatData(data.size());
// boost::container::vector<emscripten_align1_float>floatData(pixel_buffer.size());
    
// std::vector<float> outputData(data.size()); // Pre-allocate output data
// std::transform(data.begin(),data.end(),floatData.begin(),[](uint8_t val){return val/255.0f;});  // for RGBA32FLOAT
// std::transform(pixel_buffer.begin(),pixel_buffer.end(),floatData.begin(),[](uint8_t val){return val/255.0f;});  // for RGBA32FLOAT

// Eigen::VectorXf floatData(data.size());
// for (Eigen::Index i = 0; i < data.size(); ++i) {
// floatData(i) = static_cast<float>(data[i]) / 255.0f;
// }
  /*    
    size_t num_to_print = std::min((size_t)16, pixel_buffer.size()); // Print max 16 floats
    for (size_t i = 0; i < num_to_print; ++i) {
        // Print index and value, format float to a few decimal places
        printf("pixel_buffer[%zu] = %.4f\n", i, pixel_buffer[i]);
    }
*/
      
const size_t bytesPerRow=szeV.at(7,7)*4*sizeof(emscripten_align1_float);
// frame_tensor.at(0,0)=data;
// fjs_data_pointer.at(0,0)=floatData.data();
// fjsv_data_pointer.at(0,0)=&floatData; // (std::vector<float*>)
//     frame_tensorf.at(0,0)=floatData;
// frame_tensorGL.at(0,0)=data;
// wetd.at(0,0).source=texid.at(0,0);
//   wgpu_queue_write_texture(WGPU_Queue.at(0,0,0),&wict.at(4,4),&frame_tensor.at(0,0),bytesPerRow,szeV.at(7,7),sze.at(6,6),szeV.at(7,7),1);
wgpu_queue_write_texture(WGPU_Queue.at(0,0,0),&wict.at(4,4),pixel_buffer.data(),bytesPerRow,szeV.at(7,7),szeV.at(7,7),szeV.at(7,7),1);

/*    //  highway way
const HWY_FULL(uint8_t) d;
const size_t N = data.size();  
std::vector<emscripten_align1_float> floatData(4 * N); 
    // SIMD conversion loop
for (size_t i = 0; i < N; i += 1) {
const auto v = Load(d, &data[i]); 
const HWY_FULL(float) f = v / Set(d, 255.0f); // Divide as before
Store(f, d, &floatData[i]); 
}
*/

on_b.at(4,4)=0;
}   // end if on 4,4
// void wgpu_queue_copy_external_image_to_texture(WGpuQueue queue, const WGpuImageCopyExternalImage *source NOTNULL, const WGpuImageCopyTextureTagged *destination NOTNULL, uint32_t copyWidth, uint32_t copyHeight _WGPU_DEFAULT_VALUE(1), uint32_t copyDepthOrArrayLayers _WGPU_DEFAULT_VALUE(1));
// wgpu_queue_copy_external_image_to_texture(WGPU_Queue.at(0,0,0), ,&wictt.at(0,0) ,szeV.at(7,7),sze.at(6,6),szeV.at(7,7),1);
 //  Render Pass
wceA=wgpu_device_create_command_encoder(wd.at(0,0),0);
wce.at(0,0)=wceA;
wrpe.at(0,0)=wgpu_command_encoder_begin_render_pass(wce.at(0,0),&wrpd.at(0,0));
wgpu_render_pass_encoder_set_pipeline(wrpe.at(0,0),wrp.at(0,0));
wgpu_encoder_set_bind_group(wrpe.at(0,0),0,wbg.at(0,0),0,0);
 // wgpu_queue_write_buffer(wq.at(0,0),wb.at(0,0),0,&u64_uni.at(0,0),sizeof(uint64_t));
// wgpu_queue_write_buffer(wq.at(0,0),wb.at(2,2),0,&u64_siz.at(3,3),sizeof(uint64_t));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(2,2),0,&f32_uniform.at(2,2),sizeof(emscripten_align1_float));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(1,1),0,&u64_uni.at(3,3),sizeof(uint64_t));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(0,0),0,&f32_uniform.at(0,0),sizeof(emscripten_align1_float));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(8,8),0,u64v.at(0,0).data(),sizeof(uint32_t)*3);
  //  wgpu_queue_write_buffer(wq.at(0,0),wb.at(0,0),0,&v4f32_uniform.at(0,0),sizeof(emscripten_align1_float)*4);
wgpu_render_pass_encoder_set_index_buffer(wrpe.at(0,0),wb.at(7,7),WGPU_INDEX_FORMAT_UINT32,0,36*sizeof(uint32_t));
wgpu_render_pass_encoder_set_vertex_buffer(wrpe.at(0,0),0,wb.at(6,6),0,sizeof(vertices));
wgpu_render_pass_encoder_set_viewport(wrpe.at(0,0),0.0f,0.0f,szef.at(1,1),szef.at(1,1),0.0f,1.0f);
wgpu_render_pass_encoder_set_scissor_rect(wrpe.at(0,0),0.0f,0.0f,sze.at(1,1),sze.at(1,1));
wgpu_render_pass_encoder_draw_indexed(wrpe.at(0,0),36,1,0,0,0);
wgpu_render_pass_encoder_end(wrpe.at(0,0));
wcb.at(0,0)=wgpu_command_encoder_finish(wce.at(0,0));
wgpu_queue_submit_one_and_destroy(wq.at(0,0),wcb.at(0,0));
  //  Render Pass 2  (sampler)
wceA={};
wceB=wgpu_device_create_command_encoder(wd.at(0,0),0);
wce.at(1,1)=wceB;
wrpe.at(1,1)=wgpu_command_encoder_begin_render_pass(wce.at(1,1),&wrpd.at(1,1));
wgpu_render_pass_encoder_set_pipeline(wrpe.at(1,1),wrp.at(1,1));
wgpu_encoder_set_bind_group(wrpe.at(1,1),0,wbg.at(1,1),0,0);
// wgpu_queue_write_buffer(wq.at(0,0),wb.at(5,5),0,&u64_siz.at(2,2),sizeof(uint64_t));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(5,5),0,&f32_uniform.at(1,1),sizeof(emscripten_align1_float));
wgpu_render_pass_encoder_set_index_buffer(wrpe.at(1,1),wb.at(7,7),WGPU_INDEX_FORMAT_UINT32,0,36*sizeof(uint32_t));
wgpu_render_pass_encoder_set_vertex_buffer(wrpe.at(1,1),0,wb.at(6,6),0,sizeof(vertices));
wgpu_render_pass_encoder_set_viewport(wrpe.at(1,1),0.0f,0.0f,szef.at(0,0),szef.at(0,0),0.0f,1.0f);
wgpu_render_pass_encoder_set_scissor_rect(wrpe.at(1,1),0.0f,0.0f,sze.at(0,0),sze.at(0,0));
wgpu_render_pass_encoder_draw_indexed(wrpe.at(1,1),36,1,0,0,0);
wgpu_render_pass_encoder_end(wrpe.at(1,1));
wcb.at(1,1)=wgpu_command_encoder_finish(wce.at(1,1));
wgpu_queue_submit_one_and_destroy(wq.at(0,0),wcb.at(1,1));
 // Compute Pass
WGPU_CommandEncoder.at(0,0,0)=wgpu_device_create_command_encoder_simple(wd.at(0,0));
WGPU_ComputePassCommandEncoder.at(0,0,0)=wgpu_command_encoder_begin_compute_pass(WGPU_CommandEncoder.at(0,0,0),&WGPU_ComputePassDescriptor.at(0,0,0));
wgpu_compute_pass_encoder_set_pipeline(WGPU_ComputePassCommandEncoder.at(0,0,0),WGPU_ComputePipeline.at(0,0,0));
wgpu_encoder_set_bind_group(WGPU_ComputePassCommandEncoder.at(0,0,0),0,WGPU_BindGroup.at(0,0,0),0,0);
wgpu_compute_pass_encoder_dispatch_workgroups(WGPU_ComputePassCommandEncoder.at(0,0,0),compute_xyz.at(0,0),compute_xyz.at(0,1),compute_xyz.at(0,2));
wgpu_encoder_end(WGPU_ComputePassCommandEncoder.at(0,0,0));
wgpu_command_encoder_copy_texture_to_texture(WGPU_CommandEncoder.at(0,0,0),&wict.at(1,1),&wict.at(3,3),sze.at(3,3),sze.at(3,3),1);
/*  //  Buffer Data View
if(WGPU_BufferStatus.at(0,0,0)!=3&&on.at(1,1)==0){
  // wgpu_queue_write_buffer(WGPU_Queue.at(0,0,0),WGPU_Buffers.at(1,1,1),0,&WGPU_InputBuffer.at(0,0,0),InputBufferBytes);
  wgpu_command_encoder_copy_buffer_to_buffer(WGPU_CommandEncoder.at(0,0,0),WGPU_Buffers.at(0,0,0),0,WGPU_Buffers.at(2,0,2),0,OutputBufferBytes);
    // void wgpu_load_image_bitmap_from_url_async(const char *url NOTNULL, EM_BOOL flipY, WGpuLoadImageBitmapCallback callback, void *userData);
   // const char url_address="https://test.1ink.us/gpu/  ";
  // wgpu_load_image_bitmap_from_url_async(url_address,EM_TRUE,imageCallbackStart,WGPU_UserData.at(0,0,0));
on.at(1,1)=1;
wgpu_buffer_map_sync(WGPU_Buffers.at(2,0,2),mode1,0,OutputBufferBytes);  
// wgpu_buffer_map_async(WGPU_Buffers.at(2,0,2),WGPU_MapCallback.at(0,0,0),&WGPU_UserData.at(0,0,0),mode1,0,OutputBufferBytes);
}
WGPU_BufferStatus.at(0,0,0)=wgpu_buffer_map_state(WGPU_Buffers.at(2,0,2));
if(WGPU_BufferStatus.at(0,0,0)==3){
WGPU_Range_PointerB=wgpu_buffer_get_mapped_range(WGPU_Buffers.at(2,0,2),0,OutputBufferBytes);
WGPU_BufferRange.at(0,0,1)=WGPU_Range_PointerB;
wgpu_buffer_read_mapped_range(WGPU_Buffers.at(2,0,2),WGPU_BufferRange.at(0,0,1),0,WGPU_ResultBuffer.at(0,0,0),OutputBufferBytes);
EM_ASM({
document.querySelector('#outText').innerHTML='Buffer at [2]:'+$0.toFixed(2);
document.querySelector('#outText').innerHTML+='Buffer at [3]:'+$1.toFixed(2);
},WGPU_ResultBuffer.at(0,0,0)[2],WGPU_ResultBuffer.at(0,0,0)[3]);
}
*/
WGPU_CommandBuffer.at(0,0,0)=wgpu_encoder_finish(WGPU_CommandEncoder.at(0,0,0));
WGPU_BufferStatus.at(0,0,0)=wgpu_buffer_map_state(WGPU_Buffers.at(2,0,2));
if(WGPU_BufferStatus.at(0,0,0)!=1){
wgpu_buffer_unmap(WGPU_Buffers.at(2,0,2));
on.at(1,1)=3;
}
// wgpu_queue_set_on_submitted_work_done_callback(WGPU_Queue.at(0,0,0),WGPU_ComputeDoneCallback.at(0,0,0),0);
wgpu_queue_submit_one_and_destroy(WGPU_Queue.at(0,0,0),WGPU_CommandBuffer.at(0,0,0));
return EM_TRUE;
};

void raf(){
render();
return;
}

static void ObtainedWebGpuDeviceStart(WGpuDevice result,void *userData){
if(on.at(0,0)==0){wd.at(0,0)=result;}
on_b.at(4,4)=0;
on_b.at(5,5)=0;
on.at(3,3)=1;
js_data_pointer.at(0,0)=0;
fjs_data_pointer.at(0,0)=0;
wcc.at(0,0)=wgpu_canvas_get_webgpu_context("#scanvas");
const char * vert_body = rd_fl(FnmV);
const char * frag_body_main = rd_fl(Fnm); // Your main shader
const char * frag_body_sampler = rd_fl(FnmF2); // Your sampler shader
const char * comp_body = rd_fl(FnmC);
// canvasFormat=navigator_gpu_get_preferred_canvas_format();
wtf.at(2,2)=WGPU_TEXTURE_FORMAT_RGBA32FLOAT;
// wtf.at(0,0)=navigator_gpu_get_preferred_canvas_format();
// wtf.at(0,0)=WGPU_TEXTURE_FORMAT_RGBA8UNORM;
wtf.at(0,0)=WGPU_TEXTURE_FORMAT_RGBA16FLOAT;
wtf.at(1,1)=WGPU_TEXTURE_FORMAT_RGBA32FLOAT;
// wtf.at(0,0)=WGPU_TEXTURE_FORMAT_RG11B10UFLOAT;
// wtf.at(0,0)=WGPU_TEXTURE_FORMAT_RGBA8UNORM;
wtf.at(4,4)=WGPU_TEXTURE_FORMAT_INVALID;
// wtf.at(5,5)=WGPU_TEXTURE_FORMAT_DEPTH32FLOAT_STENCIL8;
// wtf.at(5,5)=WGPU_TEXTURE_FORMAT_DEPTH24PLUS_STENCIL8;
wtf.at(5,5)=WGPU_TEXTURE_FORMAT_DEPTH16UNORM;
// wtf.at(0,0)=canvasFormat;
canvasViewFormat[0]={wtf.at(0,0)};
config.device=wd.at(0,0);
config.format=wtf.at(0,0);
config.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT;
// config.numViewFormats=1;
config.viewFormats=&canvasViewFormat[0];
config.alphaMode=WGPU_CANVAS_ALPHA_MODE_PREMULTIPLIED;
// config.alphaMode=WGPU_CANVAS_ALPHA_MODE_OPAQUE;
//config.colorSpace=HTML_PREDEFINED_COLOR_SPACE_DISPLAY_P3;
config.colorSpace=HTML_PREDEFINED_COLOR_SPACE_SRGB;
wccf.at(0,0)=config;
clearColor.r=0.5;
clearColor.g=0.5;
clearColor.b=0.5;
clearColor.a=1.0;
clearC.at(0,0)=clearColor;
wgpu_canvas_context_configure(wcc.at(0,0),&wccf.at(0,0));
emscripten_get_canvas_element_size("canvas",&szwI,&szhI);
emscripten_get_element_css_size("#scanvas",&szw,&szh);
// u64_siz.at(3,3)=sze.at(1,1);
sze.at(0,0)=static_cast<emscripten_align1_int>(szhI);
emscripten_log(EM_LOG_CONSOLE,"C got canvas size: %d", sze.at(0,0));
sze.at(3,3)=static_cast<emscripten_align1_int>(std::max(sze.at(0,0),sze.at(1,1))*(float(u64_uni.at(4,4)/1000.0f)));
emscripten_log(EM_LOG_CONSOLE,"C setting main texture size: %d", sze.at(3,3));
emscripten_log(EM_LOG_CONSOLE,"C got secondary size: %d", szh);
// u64_siz.at(2,2)=static_cast<emscripten_align1_int>(szhI);
f32_uniform.at(1,1)=static_cast<emscripten_align1_float>(sze.at(1,1));
f32_uniform.at(2,2)=static_cast<emscripten_align1_float>(sze.at(1,1));
szef.at(0,0)=static_cast<emscripten_align1_float>(szhI);
szef.at(1,1)=static_cast<emscripten_align1_float>(sze.at(1,1));

     //  mouse setup
clk_l=true;
mms.at(0,0)=0.5*szef.at(0,0);
mms.at(0,1)=0.5*(mms2.at(0,1)-szef.at(0,0));
mms.at(1,0)=0.5*szef.at(0,0);
mms.at(1,1)=0.5*(mms2.at(0,1)-szef.at(0,0));
mms.at(2,0)=szef.at(0,0)*0.5;
mms.at(2,1)=(mms2.at(0,1)-szef.at(0,0))*0.5;

emscripten_set_click_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,EM_FALSE,ms_clk);
emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,EM_FALSE,ms_clk);
emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,EM_FALSE,ms_mv);
emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW,0,EM_FALSE,ms_clk);

u64_bfrSze.at(0,0)=256; // (floor((sze.at(0,0))/256)+1)*256;
u64_bfrSze.at(1,1)=256; // (floor((sze.at(1,1))/256)+1)*256;
originXYZ.x=0;
originXYZ.y=0;
originXYZ.z=0;
originXY.x=0;
originXY.y=0;
oxyz.at(0,0)=originXYZ;
oxy.at(0,0)=originXY;
WGPU_UserData.at(0,0,0)=userData;
WGPU_ComputeDoneCallback.at(0,0,0)=onComputeDoneStart;
WGPU_MapCallback.at(0,0,0)=mapCallbackStart;
textureDescriptorIn.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorIn.format=wtf.at(2,2);
textureDescriptorIn.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT|WGPU_TEXTURE_USAGE_TEXTURE_BINDING;
textureDescriptorIn.width=sze.at(1,1);
textureDescriptorIn.height=sze.at(1,1); // default = 1;
textureDescriptorIn.depthOrArrayLayers=1;
textureDescriptorIn.mipLevelCount=1;
textureDescriptorIn.sampleCount=1;
textureDescriptorIn.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureAviewFormats[0]={wtf.at(2,2)};
textureDescriptorIn.numViewFormats=0;
textureDescriptorIn.viewFormats=nullptr; // &textureAviewFormats[0];
textureDescriptorInV.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorInV.format=wtf.at(1,1);
textureDescriptorInV.usage=WGPU_TEXTURE_USAGE_TEXTURE_BINDING|WGPU_TEXTURE_USAGE_COPY_DST;
textureDescriptorInV.width=szeV.at(7,7);
textureDescriptorInV.height=szeV.at(7,7); // default = 1;
emscripten_log(EM_LOG_CONSOLE,"Input texture size: %d", szeV.at(7,7));
textureDescriptorInV.depthOrArrayLayers=1;
textureDescriptorInV.mipLevelCount=1;
textureDescriptorInV.sampleCount=1;
textureDescriptorInV.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorInV.numViewFormats=0;
textureDescriptorInV.viewFormats=nullptr; // &textureAviewFormats[0];
textureDescriptorOut.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorOut.format=wtf.at(2,2);
textureDescriptorOut.usage=WGPU_TEXTURE_USAGE_STORAGE_BINDING|WGPU_TEXTURE_USAGE_COPY_SRC;
textureDescriptorOut.width=sze.at(3,3);
textureDescriptorOut.height=sze.at(3,3); // default = 1;
textureDescriptorOut.depthOrArrayLayers=1;
textureDescriptorOut.mipLevelCount=1;
textureDescriptorOut.sampleCount=1;
textureDescriptorOut.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorOut.numViewFormats=0;
textureDescriptorOut.viewFormats=nullptr;
textureDescriptorOut2.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorOut2.format=wtf.at(2,2);
textureDescriptorOut2.usage=WGPU_TEXTURE_USAGE_TEXTURE_BINDING|WGPU_TEXTURE_USAGE_COPY_DST;
textureDescriptorOut2.width=sze.at(3,3);
textureDescriptorOut2.height=sze.at(3,3); // default = 1;
textureDescriptorOut2.depthOrArrayLayers=1;
textureDescriptorOut2.mipLevelCount=1;
textureDescriptorOut2.sampleCount=1;
textureDescriptorOut2.dimension=WGPU_TEXTURE_DIMENSION_2D;
textureDescriptorOut2.numViewFormats=0;
textureDescriptorOut2.viewFormats=nullptr;
WGPU_TextureDescriptor.at(0,0,0)=textureDescriptorIn;
WGPU_TextureDescriptor.at(0,0,1)=textureDescriptorOut;
WGPU_TextureDescriptor.at(0,0,2)=textureDescriptorOut2;
WGPU_TextureDescriptor.at(0,0,3)=textureDescriptorInV;
WGPU_CommandEncoderDescriptor.at(0,0,0)=commandEncoderDescriptor;
textureBindingLayoutFloat.sampleType=WGPU_TEXTURE_SAMPLE_TYPE_FLOAT;
textureBindingLayoutFloat.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureBindingLayoutFloat.multisampled=0;
textureBindingLayoutFloatM.sampleType=WGPU_TEXTURE_SAMPLE_TYPE_FLOAT;
textureBindingLayoutFloatM.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureBindingLayoutFloatM.multisampled=1;
textureBindingLayoutUint32.sampleType=WGPU_TEXTURE_SAMPLE_TYPE_UINT;
textureBindingLayoutUint32.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureBindingLayoutUint32.multisampled=0;
wtbl.at(1,1)=textureBindingLayoutFloat;
wtbl.at(3,3)=textureBindingLayoutFloatM;
wtbl.at(4,4)=textureBindingLayoutUint32;
textureBindingLayoutDepth.sampleType=WGPU_TEXTURE_SAMPLE_TYPE_DEPTH;
textureBindingLayoutDepth.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureBindingLayoutDepth.multisampled=0;
wtbl.at(2,2)=textureBindingLayoutDepth;
textureViewDescriptorIn.format=wtf.at(2,2);
textureViewDescriptorIn.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureViewDescriptorIn.aspect=WGPU_TEXTURE_ASPECT_ALL;
textureViewDescriptorIn.baseMipLevel=0; // default = 0
textureViewDescriptorIn.mipLevelCount=1;
textureViewDescriptorIn.baseArrayLayer=0; // default = 0
textureViewDescriptorIn.arrayLayerCount=1;
textureViewDescriptorInV.format=wtf.at(1,1);
textureViewDescriptorInV.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureViewDescriptorInV.aspect=WGPU_TEXTURE_ASPECT_ALL;
textureViewDescriptorInV.baseMipLevel=0; // default = 0
textureViewDescriptorInV.mipLevelCount=1; // (std::floor(std::log2(szeV.at(7,7)))) + 1;
textureViewDescriptorInV.baseArrayLayer=0; // default = 0
textureViewDescriptorInV.arrayLayerCount=1;
textureViewDescriptorOut.format=wtf.at(2,2);
textureViewDescriptorOut.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureViewDescriptorOut.aspect=WGPU_TEXTURE_ASPECT_ALL;
textureViewDescriptorOut.baseMipLevel=0; // default = 0
textureViewDescriptorOut.mipLevelCount=1;
textureViewDescriptorOut.baseArrayLayer=0; // default = 0
textureViewDescriptorOut.arrayLayerCount=1;
textureViewDescriptorOut2.format=wtf.at(2,2);
textureViewDescriptorOut2.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureViewDescriptorOut2.aspect=WGPU_TEXTURE_ASPECT_ALL;
textureViewDescriptorOut2.baseMipLevel=0; // default = 0
textureViewDescriptorOut2.mipLevelCount=1; // (std::floor(std::log2(sze.at(3,3)))) + 1;
textureViewDescriptorOut2.baseArrayLayer=0; // default = 0
textureViewDescriptorOut2.arrayLayerCount=1;
WGPU_TextureViewDescriptor.at(0,0,0)=textureViewDescriptorIn;
WGPU_TextureViewDescriptor.at(0,0,1)=textureViewDescriptorOut;
WGPU_TextureViewDescriptor.at(0,0,2)=textureViewDescriptorOut2;
WGPU_TextureViewDescriptor.at(0,0,3)=textureViewDescriptorInV;
WGPU_ResultBuffer.at(0,0,0)=WGPU_Result_Array;
WGPU_InputBuffer.at(0,0,0)=WGPU_Input_Array;
bufferDescriptorI={u64_bfrSze.at(1,1),WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
bufferDescriptorO={u64_bfrSze.at(0,0),WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_SRC,WGPU_FALSE};
bufferDescriptorM={OutputBufferBytes,WGPU_BUFFER_USAGE_MAP_READ|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
bufferDescriptorC={OutputBufferBytes,WGPU_BUFFER_USAGE_MAP_READ|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
WGPU_BufferDescriptor.at(0,0,0)=bufferDescriptorI;
WGPU_BufferDescriptor.at(0,0,1)=bufferDescriptorO;
WGPU_BufferDescriptor.at(0,0,2)=bufferDescriptorM;
WGPU_BufferDescriptor.at(0,0,3)=bufferDescriptorC;
WGPU_Buffers.at(1,1,1)=wgpu_device_create_buffer(wd.at(0,0),&WGPU_BufferDescriptor.at(0,0,0));
WGPU_Buffers.at(0,0,0)=wgpu_device_create_buffer(wd.at(0,0),&WGPU_BufferDescriptor.at(0,0,1));
WGPU_Buffers.at(1,0,1)=wgpu_device_create_buffer(wd.at(0,0),&WGPU_BufferDescriptor.at(0,0,2));
WGPU_Buffers.at(2,0,2)=wgpu_device_create_buffer(wd.at(0,0),&WGPU_BufferDescriptor.at(0,0,3));
// bufferDescriptor_iTime={sizeof(uint64_t),WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,EM_FALSE};
bufferDescriptor_iTime={sizeof(emscripten_align1_float),WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
wbd.at(0,0)=bufferDescriptor_iTime;
uni_iTime_Buffer=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(0,0));
wb.at(0,0)=uni_iTime_Buffer;
bufferDescriptor_iFrame={sizeof(uint64_t),WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
wbd.at(1,1)=bufferDescriptor_iFrame;
uni_iFrame_Buffer=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(1,1));
wb.at(1,1)=uni_iFrame_Buffer;
bufferDescriptor_iResolution={sizeof(emscripten_align1_float),WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
wbd.at(2,2)=bufferDescriptor_iResolution;
uni_iResolution_Buffer=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(2,2));
wb.at(2,2)=uni_iResolution_Buffer;
bufferDescriptor_iResolution_2={sizeof(emscripten_align1_float),WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
wbd.at(5,5)=bufferDescriptor_iResolution_2;
uni_iResolution_Buffer_2=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(5,5));
wb.at(5,5)=uni_iResolution_Buffer_2;
bufferBindingLayoutR.type=WGPU_BUFFER_BINDING_TYPE_UNIFORM;
bufferBindingLayoutR.hasDynamicOffset=0,
bufferBindingLayoutR.minBindingSize=sizeof(uint64_t);
wbbl.at(0,0)=bufferBindingLayoutR;
bufferBindingLayoutUVEC.type=WGPU_BUFFER_BINDING_TYPE_UNIFORM;
bufferBindingLayoutUVEC.hasDynamicOffset=0,
bufferBindingLayoutUVEC.minBindingSize=sizeof(uint32_t)*3;
wbbl.at(1,1)=bufferBindingLayoutUVEC;
bufferBindingLayoutF.type=WGPU_BUFFER_BINDING_TYPE_UNIFORM;
bufferBindingLayoutF.hasDynamicOffset=0,
bufferBindingLayoutF.minBindingSize=sizeof(emscripten_align1_float);
wbbl.at(2,2)=bufferBindingLayoutF;
Input_Image_Buffer.buffer=WGPU_Buffers.at(1,1,1);
// wicb.at(2,2)=Input_Image_Buffer;
Output_Image_Buffer.buffer=WGPU_Buffers.at(0,0,0);
// wicb.at(1,1)=Output_Image_Buffer;
Mapped_Image_Buffer.buffer=WGPU_Buffers.at(2,0,2);
// wicb.at(0,0)=Mapped_Image_Buffer;
WGpuBufferDescriptor bufferDescriptorIn={u64_bfrSze.at(1,1),WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
WGpuBufferDescriptor bufferDescriptorOut={u64_bfrSze.at(0,0),WGPU_BUFFER_USAGE_STORAGE|WGPU_BUFFER_USAGE_COPY_SRC,WGPU_FALSE};
WGpuBufferDescriptor bufferDescriptorZoom={24,WGPU_BUFFER_USAGE_UNIFORM|WGPU_BUFFER_USAGE_COPY_DST,WGPU_FALSE};
wbd.at(3,3)=bufferDescriptorIn;
wbd.at(4,4)=bufferDescriptorOut;
wbd.at(8,8)=bufferDescriptorZoom;
wb.at(3,3)=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(3,3));
wb.at(4,4)=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(4,4));
wb.at(8,8)=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(8,8));
    //  vert / indice buffers
bufferDescriptor_vertex.size=sizeof(vertices);
bufferDescriptor_vertex.usage=WGPU_BUFFER_USAGE_VERTEX|WGPU_BUFFER_USAGE_COPY_DST;
bufferDescriptor_vertex.mappedAtCreation=WGPU_FALSE;
wbd.at(6,6)=bufferDescriptor_vertex;
vertAtt.offset=0;
vertAtt.shaderLocation=0;
vertAtt.format=WGPU_VERTEX_FORMAT_FLOAT32X4;
vertAtt2.offset=16;
vertAtt2.shaderLocation=1;
vertAtt2.format=WGPU_VERTEX_FORMAT_FLOAT32X2;
// WGpuVertexAttribute vertAttArray[2]={vertAtt,vertAtt2};
vertBufLayout.numAttributes=1;
vertBufLayout.attributes=&vertAtt;
vertBufLayout.arrayStride=sizeof(Vertex);
vertBufLayout.stepMode=WGPU_VERTEX_STEP_MODE_VERTEX;
wvbl.at(0,0)=vertBufLayout;
// wvbl.at(1,1)=vertBufLayoutUV;
vertex_Buffer=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(6,6));
wb.at(6,6)=vertex_Buffer;
bufferDescriptor_indice.size=36*sizeof(uint32_t);
bufferDescriptor_indice.usage=WGPU_BUFFER_USAGE_INDEX|WGPU_BUFFER_USAGE_COPY_DST;
bufferDescriptor_indice.mappedAtCreation=WGPU_FALSE;
wbd.at(7,7)=bufferDescriptor_indice;
indice_Buffer=wgpu_device_create_buffer(wd.at(0,0),&wbd.at(7,7));
wb.at(7,7)=indice_Buffer;
resizeSamplerDescriptor.addressModeU=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
resizeSamplerDescriptor.addressModeV=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
resizeSamplerDescriptor.addressModeW=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
resizeSamplerDescriptor.magFilter=WGPU_FILTER_MODE_LINEAR;
resizeSamplerDescriptor.minFilter=WGPU_FILTER_MODE_LINEAR;
resizeSamplerDescriptor.mipmapFilter=WGPU_MIPMAP_FILTER_MODE_LINEAR;
resizeSamplerDescriptor.lodMinClamp=0;
resizeSamplerDescriptor.lodMaxClamp=0;
// resizeSamplerDescriptor.compare;  // default = WGPU_COMPARE_FUNCTION_INVALID (not used)
resizeSamplerDescriptor.maxAnisotropy=16;
wsd.at(1,1)=resizeSamplerDescriptor;
resizeSampler=wgpu_device_create_sampler(wd.at(0,0),&wsd.at(1,1));
wsmp.at(3,3)=resizeSampler;
shaderModuleDescriptor.code=comp_body; // wgl_cmp_src;
shaderModuleDescriptor.numHints=0;
shaderModuleDescriptor.hints=NULL;
WGPU_ShaderModuleDescriptor.at(0,0,0)=shaderModuleDescriptor;
WGPU_ComputeModule.at(0,0,0)=wgpu_device_create_shader_module(wd.at(0,0),&WGPU_ShaderModuleDescriptor.at(0,0,0));
WGPU_BufferBindingLayout.at(0,0,1)=bufferBindingLayoutIn;
WGPU_BufferBindingLayout.at(0,0,2)=bufferBindingLayoutOut;
WGPU_BufferBindingLayout.at(0,0,3)=bufferBindingLayout3;
WGPU_BufferBindingLayout.at(0,0,4)=bufferBindingLayout4;
storageTextureBindingLayoutFloat32.access=WGPU_STORAGE_TEXTURE_ACCESS_WRITE_ONLY;
storageTextureBindingLayoutFloat32.format=wtf.at(2,2);
storageTextureBindingLayoutFloat32.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
storageTextureBindingLayoutFloat.access=WGPU_STORAGE_TEXTURE_ACCESS_WRITE_ONLY;
storageTextureBindingLayoutFloat.format=wtf.at(0,0);
storageTextureBindingLayoutFloat.viewDimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
WGPU_StorageTextureBindingLayout.at(0,0,0)=storageTextureBindingLayoutFloat;
WGPU_StorageTextureBindingLayout.at(1,1,1)=storageTextureBindingLayoutFloat32;
textureIn=wgpu_device_create_texture(wd.at(0,0),&WGPU_TextureDescriptor.at(0,0,0));
WGPU_Texture.at(0,0,0)=textureIn;
textureInV=wgpu_device_create_texture(wd.at(0,0),&WGPU_TextureDescriptor.at(0,0,3));
WGPU_Texture.at(0,0,3)=textureInV;
textureOut=wgpu_device_create_texture(wd.at(0,0),&WGPU_TextureDescriptor.at(0,0,1));
WGPU_Texture.at(0,0,1)=textureOut;
textureOut2=wgpu_device_create_texture(wd.at(0,0),&WGPU_TextureDescriptor.at(0,0,2));
WGPU_Texture.at(0,0,2)=textureOut2;
  /*
  texid.at(0,0)=77;
  extTextureDescriptor.source=texid.at(0,0);
  extTextureDescriptor.colorSpace=HTML_PREDEFINED_COLOR_SPACE_DISPLAY_P3;
  wetd.at(0,0)=extTextureDescriptor;
//  extTexture=wgpu_device_import_external_texture(wd.at(0,0),&wetd.at(0,0));
//  wet.at(0,0)=extTexture;
  videoFrm.source=wib.at(0,0);
  videoFrm.origin=oxy.at(0,0);
  videoFrm.flipY=WGPU_TRUE;
  wicei.at(0,0)=videoFrm;
  External_Image_Texture.texture=WGPU_Texture.at(0,0,3);
External_Image_Texture.mipLevel=0;
External_Image_Texture.origin=oxyz.at(0,0);
  External_Image_Texture.colorSpace=HTML_PREDEFINED_COLOR_SPACE_DISPLAY_P3;
  */
Input_Image_Texture.texture=WGPU_Texture.at(0,0,0);
Input_Image_Texture.mipLevel=0;
Input_Image_Texture.origin=oxyz.at(0,0);
Input_Image_Texture.aspect=WGPU_TEXTURE_ASPECT_ALL;
Input_Image_TextureV.texture=WGPU_Texture.at(0,0,3);
Input_Image_TextureV.mipLevel=0;
Input_Image_TextureV.origin=oxyz.at(0,0);
Input_Image_TextureV.aspect=WGPU_TEXTURE_ASPECT_ALL;
Output_Image_Texture.texture=WGPU_Texture.at(0,0,1);
Output_Image_Texture.origin=oxyz.at(0,0);
Output_Image_Texture.mipLevel=0;
Output_Image_Texture.aspect=WGPU_TEXTURE_ASPECT_ALL;
Output_Image_Texture2.texture=WGPU_Texture.at(0,0,2);
Output_Image_Texture2.mipLevel=0;
Output_Image_Texture2.origin=oxyz.at(0,0);
Output_Image_Texture2.aspect=WGPU_TEXTURE_ASPECT_ALL;
wict.at(2,2)=Input_Image_Texture;
wict.at(1,1)=Output_Image_Texture;
wict.at(3,3)=Output_Image_Texture2;
wict.at(4,4)=Input_Image_TextureV;
//  wictt.at(0,0)=External_Image_Texture;
INTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,0),&WGPU_TextureViewDescriptor.at(0,0,0));
INVTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,3),&WGPU_TextureViewDescriptor.at(0,0,3));
OUTTextureView=wgpu_texture_create_view(WGPU_Texture.at(0,0,1),&WGPU_TextureViewDescriptor.at(0,0,1));
OUTTexture2View=wgpu_texture_create_view(WGPU_Texture.at(0,0,2),&WGPU_TextureViewDescriptor.at(0,0,2));
wtv.at(3,3)=INTextureView;
wtv.at(4,4)=OUTTextureView;
wtv.at(5,5)=OUTTexture2View;
wtv.at(6,6)=INVTextureView;
videoTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
videoTextureDescriptor.format=wtf.at(2,2);
videoTextureDescriptor.usage=WGPU_TEXTURE_USAGE_TEXTURE_BINDING|WGPU_TEXTURE_USAGE_STORAGE_BINDING|WGPU_TEXTURE_USAGE_COPY_DST;
videoTextureDescriptor.width=sze.at(0,0);
videoTextureDescriptor.height=sze.at(0,0); // default = 1;
videoTextureDescriptor.depthOrArrayLayers=1;
videoTextureDescriptor.mipLevelCount=1;
videoTextureDescriptor.sampleCount=1;
videoTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
// videoViewFormats[0]={wtf.at(0,0)};
videoTextureDescriptor.numViewFormats=0; // &videoViewFormats[0];
videoTextureDescriptor.viewFormats=nullptr; // &videoViewFormats[0];
wtd.at(2,2)=videoTextureDescriptor;
videoTexture=wgpu_device_create_texture(wd.at(0,0),&wtd.at(2,2));
wt.at(2,2)=videoTexture;
videoTextureViewDescriptor.format=wtf.at(2,2);
videoTextureViewDescriptor.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
videoTextureViewDescriptor.aspect=WGPU_TEXTURE_ASPECT_ALL;
videoTextureViewDescriptor.baseMipLevel=0; // default = 0
videoTextureViewDescriptor.mipLevelCount=1;
videoTextureViewDescriptor.baseArrayLayer=0; // default = 0
videoTextureViewDescriptor.arrayLayerCount=1;
wtvd.at(2,2)=videoTextureViewDescriptor;
videoTextureView=wgpu_texture_create_view(wt.at(2,2),&wtvd.at(2,2));
wtv.at(2,2)=videoTextureView;


msaaTextureDesc.dimension=WGPU_TEXTURE_DIMENSION_2D;
msaaTextureDesc.format=wtf.at(0,0);
msaaTextureDesc.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT;
msaaTextureDesc.width=sze.at(0,0);
msaaTextureDesc.height=sze.at(0,0); // default = 1;
msaaTextureDesc.depthOrArrayLayers=1;
msaaTextureDesc.mipLevelCount=1;
msaaTextureDesc.sampleCount=4;
msaaTextureDesc.dimension=WGPU_TEXTURE_DIMENSION_2D;
// videoViewFormats[0]={wtf.at(0,0)};
msaaTextureDesc.numViewFormats=0; // &videoViewFormats[0];
msaaTextureDesc.viewFormats=nullptr; // &videoViewFormats[0];
wtd.at(3,3)=msaaTextureDesc;
msaaTexture=wgpu_device_create_texture(wd.at(0,0),&wtd.at(3,3));
wt.at(3,3)=msaaTexture;
       /*
textureViewDescriptorMSAA.format=wtf.at(2,2);
textureViewDescriptorMSAA.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
textureViewDescriptorMSAA.aspect=WGPU_TEXTURE_ASPECT_ALL;
textureViewDescriptorMSAA.baseMipLevel=0; // default = 0
textureViewDescriptorMSAA.mipLevelCount=1;
textureViewDescriptorMSAA.baseArrayLayer=0; // default = 0
textureViewDescriptorMSAA.arrayLayerCount=1;
wtvd.at(3,3)=textureViewDescriptorMSAA;
*/
msaaTextureView=wgpu_texture_create_view(wt.at(3,3),nullptr);
wtv.at(7,7)=msaaTextureView;

       
       
      // Compute Input Buffer
Compute_Bindgroup_Layout_Entries[0].binding=0;
Compute_Bindgroup_Layout_Entries[0].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[0].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Compute_Bindgroup_Layout_Entries[0].layout.buffer=WGPU_BufferBindingLayout.at(0,0,1);
        // Compute Output Buffer
Compute_Bindgroup_Layout_Entries[1].binding=1;
Compute_Bindgroup_Layout_Entries[1].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[1].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Compute_Bindgroup_Layout_Entries[1].layout.buffer=WGPU_BufferBindingLayout.at(0,0,2);
          // Compute Input Texture
Compute_Bindgroup_Layout_Entries[2].binding=2;
Compute_Bindgroup_Layout_Entries[2].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[2].type=WGPU_BIND_GROUP_LAYOUT_TYPE_TEXTURE;
Compute_Bindgroup_Layout_Entries[2].layout.texture=wtbl.at(1,1);
          // Compute Output Texture
Compute_Bindgroup_Layout_Entries[3].binding=3;
Compute_Bindgroup_Layout_Entries[3].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[3].type=WGPU_BIND_GROUP_LAYOUT_TYPE_STORAGE_TEXTURE;
Compute_Bindgroup_Layout_Entries[3].layout.storageTexture=WGPU_StorageTextureBindingLayout.at(1,1,1);
            //  Compute Sampler
Compute_Bindgroup_Layout_Entries[4].binding=4;
Compute_Bindgroup_Layout_Entries[4].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[4].type=WGPU_BIND_GROUP_LAYOUT_TYPE_SAMPLER;
Compute_Bindgroup_Layout_Entries[4].layout.sampler=wsbl.at(0,0);
            // Compute Time Uniform
Compute_Bindgroup_Layout_Entries[5].binding=5;
Compute_Bindgroup_Layout_Entries[5].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[5].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Compute_Bindgroup_Layout_Entries[5].layout.buffer=wbbl.at(2,2);
            // Compute Video Texture
Compute_Bindgroup_Layout_Entries[6].binding=6;
Compute_Bindgroup_Layout_Entries[6].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[6].type=WGPU_BIND_GROUP_LAYOUT_TYPE_STORAGE_TEXTURE;
Compute_Bindgroup_Layout_Entries[6].layout.storageTexture=WGPU_StorageTextureBindingLayout.at(1,1,1);
            // Compute Resize Texture
Compute_Bindgroup_Layout_Entries[7].binding=7;
Compute_Bindgroup_Layout_Entries[7].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[7].type=WGPU_BIND_GROUP_LAYOUT_TYPE_TEXTURE;
Compute_Bindgroup_Layout_Entries[7].layout.texture=wtbl.at(1,1);
              // Compute Video Input Texture
Compute_Bindgroup_Layout_Entries[8].binding=8;
Compute_Bindgroup_Layout_Entries[8].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[8].type=WGPU_BIND_GROUP_LAYOUT_TYPE_TEXTURE;
Compute_Bindgroup_Layout_Entries[8].layout.texture=wtbl.at(1,1);
            // Compute Color Attachment Texture
// Compute_Bindgroup_Layout_Entries[7].binding=7;
// Compute_Bindgroup_Layout_Entries[7].visibility=WGPU_SHADER_STAGE_COMPUTE;
// Compute_Bindgroup_Layout_Entries[7].type=WGPU_BIND_GROUP_LAYOUT_TYPE_STORAGE_TEXTURE;
// Compute_Bindgroup_Layout_Entries[7].layout.storageTexture=WGPU_StorageTextureBindingLayout.at(0,0,0);
              // Compute Zoom Uniform
Compute_Bindgroup_Layout_Entries[9].binding=9;
Compute_Bindgroup_Layout_Entries[9].visibility=WGPU_SHADER_STAGE_COMPUTE;
Compute_Bindgroup_Layout_Entries[9].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Compute_Bindgroup_Layout_Entries[9].layout.buffer=wbbl.at(1,1);
WGPU_Compute_Bindgroup_Layout_Entries.at(0,0,0)=Compute_Bindgroup_Layout_Entries;
WGPU_BindGroupLayout.at(0,0,0)=wgpu_device_create_bind_group_layout(wd.at(0,0),WGPU_Compute_Bindgroup_Layout_Entries.at(0,0,0),10);
WGPU_ComputePipelineLayout.at(0,0,0)=wgpu_device_create_pipeline_layout(wd.at(0,0),&WGPU_BindGroupLayout.at(0,0,0),1);
WGPU_ComputePipeline.at(0,0,0)=wgpu_device_create_compute_pipeline(wd.at(0,0),WGPU_ComputeModule.at(0,0,0),Entry,WGPU_ComputePipelineLayout.at(0,0,0),NULL,0);
      // Compute Input Buffer
Compute_Bindgroup_Entries[0]={};
Compute_Bindgroup_Entries[0].binding=0;
Compute_Bindgroup_Entries[0].resource=WGPU_Buffers.at(1,1,1);
Compute_Bindgroup_Entries[0].bufferBindOffset=0;
Compute_Bindgroup_Entries[0].bufferBindSize=InputBufferBytes;
        // Compute Output Buffer
Compute_Bindgroup_Entries[1]={};
Compute_Bindgroup_Entries[1].binding=1;
Compute_Bindgroup_Entries[1].resource=WGPU_Buffers.at(0,0,0);
Compute_Bindgroup_Entries[1].bufferBindOffset=0;
Compute_Bindgroup_Entries[1].bufferBindSize=OutputBufferBytes;
          // Compute Input Texture
Compute_Bindgroup_Entries[2]={};
Compute_Bindgroup_Entries[2].binding=2;
Compute_Bindgroup_Entries[2].resource=wtv.at(3,3);
          // Compute Output Texture
Compute_Bindgroup_Entries[3]={};
Compute_Bindgroup_Entries[3].binding=3;
Compute_Bindgroup_Entries[3].resource=wtv.at(4,4);
            //  Compute Sampler
Compute_Bindgroup_Entries[4]={};
Compute_Bindgroup_Entries[4].binding=4;
Compute_Bindgroup_Entries[4].resource=wsmp.at(3,3);
            // Compute iTime Uniform
Compute_Bindgroup_Entries[5].binding=5;
Compute_Bindgroup_Entries[5].resource=wb.at(0,0);
Compute_Bindgroup_Entries[5].bufferBindOffset=0;
Compute_Bindgroup_Entries[5].bufferBindSize=sizeof(emscripten_align1_float);
            // Compute Video Texture
Compute_Bindgroup_Entries[6]={};
Compute_Bindgroup_Entries[6].binding=6;
Compute_Bindgroup_Entries[6].resource=wtv.at(2,2); 
            // Compute Resize Texture
Compute_Bindgroup_Entries[7]={};
Compute_Bindgroup_Entries[7].binding=7;
Compute_Bindgroup_Entries[7].resource=wtv.at(5,5);
              // Compute Video In Texture
Compute_Bindgroup_Entries[8]={};
Compute_Bindgroup_Entries[8].binding=8;
Compute_Bindgroup_Entries[8].resource=wtv.at(6,6);
            // Compute Color Attachment Texture
// Compute_Bindgroup_Entries[7]={};
// Compute_Bindgroup_Entries[7].binding=7;
// Compute_Bindgroup_Entries[7].resource=wtv.at(1,1);
              // Compute Zoom Uniform
Compute_Bindgroup_Entries[9].binding=9;
Compute_Bindgroup_Entries[9].resource=wb.at(8,8);
Compute_Bindgroup_Entries[9].bufferBindOffset=0;
Compute_Bindgroup_Entries[9].bufferBindSize=sizeof(uint32_t)*3;
WGPU_BindGroupEntries.at(0,0,0)=Compute_Bindgroup_Entries;
WGPU_BindGroup.at(0,0,0)=wgpu_device_create_bind_group(wd.at(0,0),WGPU_BindGroupLayout.at(0,0,0),WGPU_BindGroupEntries.at(0,0,0),10);
WGpuComputePassTimestampWrites computePassTimestampWrites={};
computePassTimestampWrites.querySet=0;
computePassDescriptor.timestampWrites=computePassTimestampWrites;
WGPU_ComputePassDescriptor.at(0,0,0)=computePassDescriptor;
WGPU_Queue.at(0,0,0)=wgpu_device_get_queue(wd.at(0,0));
multiSamp.count=1;
multiSamp.mask=0xFFFFFFFF;
multiSamp2.count=4;
multiSamp2.mask=0xFFFFFFFF;
shaderModuleDescV.code=vert_body;
vs=wgpu_device_create_shader_module(wd.at(0,0),&shaderModuleDescV);
shaderModuleDescF.code=frag_body_main;
shaderModuleDescF2.code=frag_body_sampler;
fs=wgpu_device_create_shader_module(wd.at(0,0),&shaderModuleDescF);
fs2=wgpu_device_create_shader_module(wd.at(0,0),&shaderModuleDescF2);
free((void*)vert_body);
free((void*)frag_body_main);
free((void*)frag_body_sampler);
free((void*)comp_body);
colorTarget32.format=wtf.at(2,2); // wtf.at(0,0);
colorTarget32.writeMask=15;
colorTarget.format=wtf.at(0,0);
colorTarget.writeMask=15;
depthState2.format=wtf.at(4,4);
depthState2.depthWriteEnabled=0;
depthState2.depthCompare=WGPU_COMPARE_FUNCTION_LESS_EQUAL;
wdss.at(1,1)=depthState2;
depthState.format=wtf.at(4,4);
depthState.depthWriteEnabled=0;
depthState.depthCompare=WGPU_COMPARE_FUNCTION_LESS_EQUAL;
wdss.at(0,0)=depthState;
vertState.module=vs;
vertState.entryPoint="main";
vertState.numBuffers=1;
vertState.buffers=&wvbl.at(0,0);
vertState.numConstants=0;
vertState.constants=nullptr;
wvs.at(0,0)=vertState;
priState.topology=WGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // Defaults to WGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST ('triangle-list')
// priState.stripIndexFormat=WGPU_INDEX_FORMAT_UINT32; // Defaults to undefined, must be explicitly specified if WGPU_PRIMITIVE_TOPOLOGY_LINE_STRIP ('line-strip') or WGPU_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP ('triangle-strip') is used.
priState.frontFace=WGPU_FRONT_FACE_CW; // Defaults to WGPU_FRONT_FACE_CCW ('ccw')
priState.cullMode=WGPU_CULL_MODE_NONE; // Defaults to WGPU_CULL_MODE_NONE ('none')
priState.unclippedDepth=EM_FALSE; // defaults to EM_FALSE.
wps.at(0,0)=priState;
fragState.module=fs;
fragState.entryPoint="main";
fragState.numTargets=1;
fragState.targets=&colorTarget32;
wfs.at(0,0)=fragState;
fragState2.module=fs2;
fragState2.entryPoint="main";
fragState2.numTargets=1;
fragState2.targets=&colorTarget;
wfs.at(1,1)=fragState2;
videoSamplerDescriptor.addressModeU=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
videoSamplerDescriptor.addressModeV=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
videoSamplerDescriptor.addressModeW=WGPU_ADDRESS_MODE_CLAMP_TO_EDGE;
videoSamplerDescriptor.magFilter=WGPU_FILTER_MODE_LINEAR;
videoSamplerDescriptor.minFilter=WGPU_FILTER_MODE_LINEAR;
videoSamplerDescriptor.mipmapFilter=WGPU_MIPMAP_FILTER_MODE_LINEAR;
videoSamplerDescriptor.lodMinClamp=0;
videoSamplerDescriptor.lodMaxClamp=0;  //  default=32
// videoSamplerDescriptor.compare;  // default = WGPU_COMPARE_FUNCTION_INVALID (not used)
videoSamplerDescriptor.maxAnisotropy=16;
wsd.at(0,0)=videoSamplerDescriptor;
videoSampler=wgpu_device_create_sampler(wd.at(0,0),&wsd.at(0,0));
wsmp.at(0,0)=videoSampler;
/*
videoTextureCopy.texture=wt.at(2,2);
videoTextureCopy.mipLevel=0;
videoTextureCopy.origin=oxyz.at(0,0);
videoTextureCopy.aspect=WGPU_TEXTURE_ASPECT_ALL;
wict.at(0,0)=videoTextureCopy;
  */
samplerBindingLayout.type=WGPU_SAMPLER_BINDING_TYPE_FILTERING;
wsbl.at(1,1)=samplerBindingLayout;
  //  Render Sampler
Render_Bindgroup_Layout_Entries[0]={};
Render_Bindgroup_Layout_Entries[0].binding=0;
Render_Bindgroup_Layout_Entries[0].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries[0].type=WGPU_BIND_GROUP_LAYOUT_TYPE_SAMPLER;
Render_Bindgroup_Layout_Entries[0].layout.sampler=wsbl.at(1,1);
  //  Render iTime Buffer
Render_Bindgroup_Layout_Entries[1]={};
Render_Bindgroup_Layout_Entries[1].binding=7;
Render_Bindgroup_Layout_Entries[1].visibility=WGPU_SHADER_STAGE_FRAGMENT|WGPU_SHADER_STAGE_VERTEX;;
Render_Bindgroup_Layout_Entries[1].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries[1].layout.buffer=wbbl.at(2,2);
  //  Render TextureIN
Render_Bindgroup_Layout_Entries[2]={};
Render_Bindgroup_Layout_Entries[2].binding=2;
Render_Bindgroup_Layout_Entries[2].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries[2].type=WGPU_BIND_GROUP_LAYOUT_TYPE_TEXTURE;
Render_Bindgroup_Layout_Entries[2].layout.texture=wtbl.at(1,1);
  //  Render iResolution Buffer
Render_Bindgroup_Layout_Entries[3]={};
Render_Bindgroup_Layout_Entries[3].binding=5;
Render_Bindgroup_Layout_Entries[3].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries[3].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries[3].layout.buffer=wbbl.at(2,2);
  //  Render iFrame Buffer
Render_Bindgroup_Layout_Entries[4]={};
Render_Bindgroup_Layout_Entries[4].binding=6;
Render_Bindgroup_Layout_Entries[4].visibility=WGPU_SHADER_STAGE_FRAGMENT|WGPU_SHADER_STAGE_VERTEX;
Render_Bindgroup_Layout_Entries[4].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries[4].layout.buffer=wbbl.at(0,0);
wbgle.at(0,0)=Render_Bindgroup_Layout_Entries;
bindgroup_layout=wgpu_device_create_bind_group_layout(wd.at(0,0),wbgle.at(0,0),5);
wbgl.at(0,0)=bindgroup_layout;
  //  Render_2 Sampler
Render_Bindgroup_Layout_Entries_2[0]={};
Render_Bindgroup_Layout_Entries_2[0].binding=0;
Render_Bindgroup_Layout_Entries_2[0].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries_2[0].type=WGPU_BIND_GROUP_LAYOUT_TYPE_SAMPLER;
Render_Bindgroup_Layout_Entries_2[0].layout.sampler=wsbl.at(1,1);
  //  Render_2 iTime Buffer
Render_Bindgroup_Layout_Entries_2[1]={};
Render_Bindgroup_Layout_Entries_2[1].binding=7;
Render_Bindgroup_Layout_Entries_2[1].visibility=WGPU_SHADER_STAGE_FRAGMENT|WGPU_SHADER_STAGE_VERTEX;
Render_Bindgroup_Layout_Entries_2[1].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries_2[1].layout.buffer=wbbl.at(2,2);
  //  Render_2 TextureIN
Render_Bindgroup_Layout_Entries_2[2]={};
Render_Bindgroup_Layout_Entries_2[2].binding=2;
Render_Bindgroup_Layout_Entries_2[2].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries_2[2].type=WGPU_BIND_GROUP_LAYOUT_TYPE_TEXTURE;
Render_Bindgroup_Layout_Entries_2[2].layout.texture=wtbl.at(1,1);
  //  Render_2 iResolution Buffer
Render_Bindgroup_Layout_Entries_2[3]={};
Render_Bindgroup_Layout_Entries_2[3].binding=5;
Render_Bindgroup_Layout_Entries_2[3].visibility=WGPU_SHADER_STAGE_FRAGMENT;
Render_Bindgroup_Layout_Entries_2[3].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries_2[3].layout.buffer=wbbl.at(2,2);
  //  Render_2 iFrame Buffer
Render_Bindgroup_Layout_Entries_2[4]={};
Render_Bindgroup_Layout_Entries_2[4].binding=6;
Render_Bindgroup_Layout_Entries_2[4].visibility=WGPU_SHADER_STAGE_FRAGMENT|WGPU_SHADER_STAGE_VERTEX;
Render_Bindgroup_Layout_Entries_2[4].type=WGPU_BIND_GROUP_LAYOUT_TYPE_BUFFER;
Render_Bindgroup_Layout_Entries_2[4].layout.buffer=wbbl.at(0,0);
wbgle.at(1,1)=Render_Bindgroup_Layout_Entries_2;
bindgroup_layout_2=wgpu_device_create_bind_group_layout(wd.at(0,0),wbgle.at(1,1),5);
wbgl.at(1,1)=bindgroup_layout_2;
WGpuPipelineLayout pipeline_layout=wgpu_device_create_pipeline_layout(wd.at(0,0),&wbgl.at(0,0),1);
wrpl.at(0,0)=pipeline_layout;
// WGpuRenderPipelineDescriptor renderPipelineDesc={};
renderPipelineDesc.vertex=wvs.at(0,0);
renderPipelineDesc.vertex.entryPoint="main";
renderPipelineDesc.primitive=wps.at(0,0);
renderPipelineDesc.fragment=wfs.at(0,0);
renderPipelineDesc.depthStencil=wdss.at(0,0);
renderPipelineDesc.layout=wrpl.at(0,0);
// renderPipelineDesc.layout=WGPU_AUTO_LAYOUT_MODE_AUTO;
renderPipelineDesc.multisample=multiSamp;
wrp.at(0,0)=wgpu_device_create_render_pipeline(wd.at(0,0),&renderPipelineDesc);
WGpuPipelineLayout pipeline_layout_2=wgpu_device_create_pipeline_layout(wd.at(0,0),&wbgl.at(1,1),1);
wrpl.at(1,1)=pipeline_layout_2;
// WGpuRenderPipelineDescriptor renderPipelineDesc2={};
renderPipelineDesc2.vertex=wvs.at(0,0);
renderPipelineDesc2.vertex.entryPoint="main";
renderPipelineDesc2.primitive=wps.at(0,0);
renderPipelineDesc2.fragment=wfs.at(1,1);
renderPipelineDesc2.depthStencil=wdss.at(1,1);
renderPipelineDesc2.layout=wrpl.at(1,1);
// renderPipelineDesc.layout=WGPU_AUTO_LAYOUT_MODE_AUTO;
renderPipelineDesc2.multisample=multiSamp2;
wrp.at(1,1)=wgpu_device_create_render_pipeline(wd.at(0,0),&renderPipelineDesc2);
  //  Render Sampler
Render_Bindgroup_Entries[0]={};
Render_Bindgroup_Entries[0].binding=0;
Render_Bindgroup_Entries[0].resource=wsmp.at(0,0);
    //  Render iTime Buffer
Render_Bindgroup_Entries[1].binding=7;
Render_Bindgroup_Entries[1].resource=wb.at(0,0);
Render_Bindgroup_Entries[1].bufferBindOffset=0;
Render_Bindgroup_Entries[1].bufferBindSize=sizeof(emscripten_align1_float);
  //  Render TextureIN
Render_Bindgroup_Entries[2]={};
Render_Bindgroup_Entries[2].binding=2;
Render_Bindgroup_Entries[2].resource=wtv.at(2,2);
  //  Render iResolution Buffer
Render_Bindgroup_Entries[3]={};
Render_Bindgroup_Entries[3].binding=5;
Render_Bindgroup_Entries[3].resource=wb.at(2,2);
Render_Bindgroup_Entries[3].bufferBindOffset=0;
Render_Bindgroup_Entries[3].bufferBindSize=sizeof(emscripten_align1_float);
  //  Render iFrame Buffer
Render_Bindgroup_Entries[4]={};
Render_Bindgroup_Entries[4].binding=6;
Render_Bindgroup_Entries[4].resource=wb.at(1,1);
Render_Bindgroup_Entries[4].bufferBindOffset=0;
Render_Bindgroup_Entries[4].bufferBindSize=sizeof(uint64_t);
wbge.at(0,0)=Render_Bindgroup_Entries;
    //  Render_2 Sampler
Render_Bindgroup_Entries_2[0]={};
Render_Bindgroup_Entries_2[0].binding=0;
Render_Bindgroup_Entries_2[0].resource=wsmp.at(0,0);
    //  Render_2 iTime Buffer
Render_Bindgroup_Entries_2[1].binding=7;
Render_Bindgroup_Entries_2[1].resource=wb.at(0,0);
Render_Bindgroup_Entries_2[1].bufferBindOffset=0;
Render_Bindgroup_Entries_2[1].bufferBindSize=sizeof(emscripten_align1_float);
  //  Render_2 TextureIN
Render_Bindgroup_Entries_2[2]={};
Render_Bindgroup_Entries_2[2].binding=2;
Render_Bindgroup_Entries_2[2].resource=wtv.at(2,2);
  //  Render_2 iResolution Buffer
Render_Bindgroup_Entries_2[3]={};
Render_Bindgroup_Entries_2[3].binding=5;
Render_Bindgroup_Entries_2[3].resource=wb.at(5,5);
Render_Bindgroup_Entries_2[3].bufferBindOffset=0;
Render_Bindgroup_Entries_2[3].bufferBindSize=sizeof(emscripten_align1_float);
  //  Render_2 iFrame Buffer
Render_Bindgroup_Entries_2[4]={};
Render_Bindgroup_Entries_2[4].binding=6;
Render_Bindgroup_Entries_2[4].resource=wb.at(1,1);
Render_Bindgroup_Entries_2[4].bufferBindOffset=0;
Render_Bindgroup_Entries_2[4].bufferBindSize=sizeof(uint64_t);
wbge.at(1,1)=Render_Bindgroup_Entries_2;
colorTextureViewDescriptor.format=wtf.at(0,0);
colorTextureViewDescriptor.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
colorTextureViewDescriptor.aspect=WGPU_TEXTURE_ASPECT_ALL;
colorTextureViewDescriptor.baseMipLevel=0; // default = 0
colorTextureViewDescriptor.mipLevelCount=1;
colorTextureViewDescriptor.baseArrayLayer=0; // default = 0
colorTextureViewDescriptor.arrayLayerCount=1;
wtvd.at(1,1)=colorTextureViewDescriptor;
  /*
depthTextureViewDescriptor.format=wtf.at(5,5);
depthTextureViewDescriptor.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
depthTextureViewDescriptor.aspect=WGPU_TEXTURE_ASPECT_ALL;
depthTextureViewDescriptor.baseMipLevel=0; // default = 0
depthTextureViewDescriptor.mipLevelCount=1;
depthTextureViewDescriptor.baseArrayLayer=0; // default = 0
depthTextureViewDescriptor.arrayLayerCount=1;
wtvd.at(0,0)=depthTextureViewDescriptor;
depthTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
depthTextureDescriptor.format=wtf.at(5,5);
depthTextureDescriptor.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT;
depthTextureDescriptor.width=sze.at(0,0);
depthTextureDescriptor.height=sze.at(0,0); // default = 1;
depthTextureDescriptor.depthOrArrayLayers=1;
depthTextureDescriptor.mipLevelCount=1;
depthTextureDescriptor.sampleCount=1;
depthTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
depthViewFormats[0]={wtf.at(5,5)};
depthTextureDescriptor.viewFormats=&depthViewFormats[0];
wtd.at(0,0)=depthTextureDescriptor;
depthTexture=wgpu_device_create_texture(wd.at(0,0),&wtd.at(0,0));
wt.at(0,0)=depthTexture;
depthTextureDescriptor2.dimension=WGPU_TEXTURE_DIMENSION_2D;
depthTextureDescriptor2.format=wtf.at(5,5);
depthTextureDescriptor2.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT;
depthTextureDescriptor2.width=sze.at(1,1);
depthTextureDescriptor2.height=sze.at(1,1); // default = 1;
depthTextureDescriptor2.depthOrArrayLayers=1;
depthTextureDescriptor2.mipLevelCount=1;
depthTextureDescriptor2.sampleCount=1;
depthTextureDescriptor2.dimension=WGPU_TEXTURE_DIMENSION_2D;
depthViewFormats2[0]={wtf.at(5,5)};
depthTextureDescriptor2.viewFormats=&depthViewFormats2[0];
wtd.at(3,3)=depthTextureDescriptor2;
depthTexture2=wgpu_device_create_texture(wd.at(0,0),&wtd.at(3,3));
wt.at(5,5)=depthTexture2;
depthTextureViewDescriptor2.format=wtf.at(5,5);
depthTextureViewDescriptor2.dimension=WGPU_TEXTURE_VIEW_DIMENSION_2D;
depthTextureViewDescriptor2.aspect=WGPU_TEXTURE_ASPECT_ALL;
depthTextureViewDescriptor2.baseMipLevel=0; // default = 0
depthTextureViewDescriptor2.mipLevelCount=1;
depthTextureViewDescriptor2.baseArrayLayer=0; // default = 0
depthTextureViewDescriptor2.arrayLayerCount=1;
wtvd.at(3,3)=depthTextureViewDescriptor2;
  */
colorTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
colorTextureDescriptor.format=wtf.at(0,0);
colorTextureDescriptor.usage=WGPU_TEXTURE_USAGE_RENDER_ATTACHMENT;
colorTextureDescriptor.width=sze.at(0,0);
colorTextureDescriptor.height=sze.at(0,0); // default = 1;
colorTextureDescriptor.depthOrArrayLayers=1;
colorTextureDescriptor.mipLevelCount=1;
colorTextureDescriptor.sampleCount=1;
colorTextureDescriptor.dimension=WGPU_TEXTURE_DIMENSION_2D;
wtd.at(1,1)=colorTextureDescriptor;
wq.at(0,0)=wgpu_device_get_queue(wd.at(0,0));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(6,6),0,vertices,sizeof(vertices));
wgpu_queue_write_buffer(wq.at(0,0),wb.at(7,7),0,indices,36*sizeof(uint32_t));
// tme=get_current_time_in_milliseconds();
// wTime.iTime=get_current_time_in_milliseconds();
bindgroup=wgpu_device_create_bind_group(wd.at(0,0),wbgl.at(0,0),wbge.at(0,0),5);
wbg.at(0,0)=bindgroup;
bindgroup_2=wgpu_device_create_bind_group(wd.at(0,0),wbgl.at(1,1),wbge.at(1,1),5);
wbg.at(1,1)=bindgroup_2;
u64_uni.at(0,0)=0;
u64_uni.at(3,3)=0;
u64v.at(0,0)[0]=100;  //  zoom
u64v.at(0,0)[1]=100;  //  left/right
u64v.at(0,0)[2]=100;  //  up/down
f32_uniform.at(0,0)=0.0f;
d64_uniform.at(0,0)=0.0;
u_time.t1=boost::chrono::high_resolution_clock::now();
u_time.t2=boost::chrono::high_resolution_clock::now();
u_time.t3=boost::chrono::high_resolution_clock::now();
u_time.time_spanb=boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>(u_time.t2-u_time.t3);
u_time.time_spana=boost::chrono::duration<boost::compute::double_,boost::chrono::seconds::period>(u_time.t2-u_time.t1);
if(on.at(0,0)!=0){emscripten_cancel_main_loop();}
// emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);  //  60hz
// emscripten_set_main_loop_timing(EM_TIMING_RAF, 2);  //  30hz
emscripten_set_main_loop((void(*)())raf,0,0);
// emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);  // ??
on.at(0,0)=1;
}

static void ObtainedWebGpuAdapterStart(WGpuAdapter result, void *userData){
wa.at(0,0)=result;
deviceDesc.requiredFeatures=WGPU_FEATURE_FLOAT32_FILTERABLE; // |WGPU_FEATURE_RG11B10UFLOAT_RENDERABLE;
// WGPU_FEATURES_BITFIELD ftr=wgpu_adapter_or_device_get_features(wa.at(0,0));
// deviceDesc.requiredFeatures=ftr;
WGpuSupportedLimits lmts;
wgpu_adapter_or_device_get_limits(wa.at(0,0),&lmts);
lmts.maxBufferSize=384*1024*1024;
// lmts.maxColorAttachmentBytesPerSample=128;
// lmts.maxComputeWorkgroupSizeZ=128;
deviceDesc.requiredLimits=lmts;
wdd.at(0,0)=deviceDesc;
wgpu_adapter_request_device_async(wa.at(0,0),&wdd.at(0,0),ObtainedWebGpuDeviceStart,0);
}

EM_BOOL WGPU_Start(emscripten_align1_int vsz,emscripten_align1_int sz,emscripten_align1_int sr){
size_t num_elements = (size_t)vsz * vsz * 4;
pixel_buffer.resize(num_elements);
sze.at(1,1)=sz;
sze.at(6,6)=sz;
szeV.at(7,7)=vsz;
       compute_xyz.at(0,0)=std::max(1,(sze.at(1,1)+15)/16);
   compute_xyz.at(0,1)=std::max(1,(sze.at(1,1)+15)/16);
    compute_xyz.at(0,2)=2;
u64_uni.at(4,4)=sr;  //  texture resize amount
emscripten_log(EM_LOG_CONSOLE,"C main size: %d", sze.at(1,1));
emscripten_log(EM_LOG_CONSOLE,"C input texture size: %d", szeV.at(7,7));
emscripten_log(EM_LOG_CONSOLE,"C super res size: %d", u64_uni.at(4,4));
f32_uniform.at(2,2)=static_cast<emscripten_align1_float>(sze.at(1,1));
szef.at(1,1)=static_cast<emscripten_align1_float>(sze.at(1,1));
options.powerPreference=WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE;
options.forceFallbackAdapter=EM_FALSE;
wao.at(0,0)=options;
navigator_gpu_request_adapter_async(&wao.at(0,0),ObtainedWebGpuAdapterStart,0);
return EM_TRUE;
}

EM_BOOL WGPU_StartC(emscripten_align1_int vsz,emscripten_align1_int sz,emscripten_align1_int sr){
size_t num_elements = (size_t)vsz * vsz * 4;
pixel_buffer.resize(num_elements);
sze.at(1,1)=sz;
sze.at(6,6)=sz;
szeV.at(7,7)=vsz;
       compute_xyz.at(0,0)=std::max(1,(sze.at(1,1)+15)/16);
   compute_xyz.at(0,1)=std::max(1,(sze.at(1,1)+15)/16);
    compute_xyz.at(0,2)=2;
u64_uni.at(4,4)=sr;  //  texture resize amount
emscripten_log(EM_LOG_CONSOLE,"C input texture sizes: %d", szeV.at(7,7));
emscripten_log(EM_LOG_CONSOLE,"C main size: %d", sze.at(1,1));
emscripten_log(EM_LOG_CONSOLE,"C super res size: %d",u64_uni.at(4,4));
f32_uniform.at(2,2)=static_cast<emscripten_align1_float>(sze.at(1,1));
szef.at(1,1)=static_cast<emscripten_align1_float>(sze.at(1,1));
options.powerPreference=WGPU_POWER_PREFERENCE_HIGH_PERFORMANCE;
options.forceFallbackAdapter=EM_FALSE;
wao.at(0,0)=options;
navigator_gpu_request_adapter_async(&wao.at(0,0),ObtainedWebGpuAdapterStart,0);
return EM_TRUE;
}

extern "C" {
  void EMSCRIPTEN_KEEPALIVE reload_shaders();
}

void reload_shaders() {
if (fs) wgpu_object_destroy(fs);
if (fs2) wgpu_object_destroy(fs2);
if (wrp.at(0,0)) wgpu_object_destroy(wrp.at(0,0));
if (wrp.at(1,1)) wgpu_object_destroy(wrp.at(1,1));
const char * frag_body = rd_fl(Fnm);
const char * frag_body3 = rd_fl(FnmF2);
shaderModuleDescF.code = frag_body;
fs = wgpu_device_create_shader_module(wd.at(0,0), &shaderModuleDescF);
shaderModuleDescF2.code = frag_body3;
fs2 = wgpu_device_create_shader_module(wd.at(0,0), &shaderModuleDescF2);
fragState.module = fs;
wrp.at(0,0) = wgpu_device_create_render_pipeline(wd.at(0,0), &renderPipelineDesc);
fragState2.module = fs2;
wrp.at(1,1) = wgpu_device_create_render_pipeline(wd.at(0,0), &renderPipelineDesc2);
free((void*)frag_body);
free((void*)frag_body3);
}

#include "../../src/vanilla/webgpu_compute_js_mod.cpp"

extern"C"{

void panRight(){
PanRight();
return;
}

void panLeft(){
PanLeft();
return;
}

void panUp(){
PanUp();
return;
}

void panDown(){
PanDown();
return;
}

void zoomIn(){
ZoomIn();
return;
}

void zoomOut(){
ZoomOut();
return;
}

// void frmOn(){
// texOn();
// return;
// }

void frmsOff(){
framesOff();
return;
}

void frmsOn(){
framesOn();
return;
}

void startWebGPUi(emscripten_align1_int vsz,emscripten_align1_int sz,emscripten_align1_int sr){
WGPU_Start(vsz,sz,sr);
return;
}

void startWebGPUbi(emscripten_align1_int vsz,emscripten_align1_int sz,emscripten_align1_int sr){
WGPU_Start(vsz,sz,sr);
return;
}

void startWebGPUC(emscripten_align1_int vsz,emscripten_align1_int sz,emscripten_align1_int sr){
WGPU_StartC(vsz,sz,sr);
return;
}

}

int main(){
on.at(0,0)=0;
js_main();
return 0;
}




