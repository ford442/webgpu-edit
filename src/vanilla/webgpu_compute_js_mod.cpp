EM_JS(void,js_main,(),{

FS.mkdir('/shader');
FS.mkdir('/video');
let blank=new Float32Array();
FS.writeFile('/video/frame.gl',blank);
FS.writeFile('/video/frameBFR.gl',blank);
let running=0;

// WebGPU globals
let device = null;
let webgpu_sampler = null;
let renderTargetTexture = null;
let readbackBuffer = null;
let renderPipeline = null;
let vvicGpuTexture = null; // Texture for the content of 'vvic'
let vvic_cached_width = 0;
let vvic_cached_height = 0;

let textureBindGroup = null; // Bind group for vvicGpuTexture and sampler
let transformUniformBuffer = null;
let targetUniformBuffer = null;
let uniformBindGroup = null; // Bind group for uniform buffers

// Shader parameters (will be derived from your existing logic)
let currentKeepSize, currentDrawX, currentDrawY, currentW$, currentH$;

// Original variables (ensure these are accessible or passed appropriately)
// let vvic, srsiz, vsiz, SiZ; // Make sure these are initialized
// let pause = 'ready'; // Assuming 'pause' is managed elsewhere or globally
// let running = 0; // Assuming 'running' is managed elsewhere or globally
// const fileStream = FS.open('/video/frame.gl','w'); // Ensure this is initialized

let frameBufferViewF32 = []; // The view into C++ memory

async function drawFrameAsync() {
    if (!device) {
        console.warn("WebGPU device not initialized. Skipping frame.");
        return;
    }

    const vvic_elem = document.querySelector('#mvi'); // Get the source element

    // ---- 1. Determine current dimensions of vvic_elem ----
    let current_vvic_w, current_vvic_h;
    let vvic_content_ready = false;

    if (vvic_elem.tagName === 'CANVAS') {
        current_vvic_w = vvic_elem.width;
        current_vvic_h = vvic_elem.height;
        if (current_vvic_w > 0 && current_vvic_h > 0) vvic_content_ready = true;
    } else if (vvic_elem.tagName === 'IMG') {
        current_vvic_w = vvic_elem.naturalWidth;
        current_vvic_h = vvic_elem.naturalHeight;
        if (vvic_elem.complete && current_vvic_w > 0 && current_vvic_h > 0) vvic_content_ready = true;
    } else if (vvic_elem.tagName === 'VIDEO') {
        current_vvic_w = vvic_elem.videoWidth;
        current_vvic_h = vvic_elem.videoHeight;
        if (vvic_elem.readyState >= 3) {vvic_content_ready = true;} // HAVE_ENOUGH_DATA
    }

    const commandEncoder = device.createCommandEncoder();

    // ---- 2. Update vvicGpuTexture and Render (if pause is 'ready') ----
    if (window.pause === 'ready' && vvic_content_ready && current_vvic_w > 0 && current_vvic_h > 0) {
        // Update currentW$ and currentH$ which are used by the shader via uniforms
        // These should reflect the actual dimensions of the content being drawn.
        // The original w$, h$ were calculated once. If vvic can change size (e.g. video), these need to update.
        // For simplicity, using the initially calculated currentW$, currentH$ which map to w_orig, h_orig.
        // If vvic's content size (current_vvic_w, current_vvic_h) should be used instead of w_orig, h_orig
        // then currentW$ and currentH$ should be updated here with current_vvic_w, current_vvic_h.
        // And drawX/drawY might need recalculation if you want to keep it centered based on new dimensions.
        // Let's stick to the original w_orig, h_orig for drawing dimensions for now.
        const imageDrawWidth = currentW$;
        const imageDrawHeight = currentH$;

        // Recreate source texture if vvic dimensions for drawing changed or not initialized
        // (Assuming imageDrawWidth/Height are stable unless explicitly changed)
        if (!vvicGpuTexture || vvic_cached_width !== current_vvic_w || vvic_cached_height !== current_vvic_h) {
            if (vvicGpuTexture) {
                vvicGpuTexture.destroy();
            }
            vvic_cached_width = current_vvic_w;
            vvic_cached_height = current_vvic_h;

            vvicGpuTexture = device.createTexture({
                size: [vvic_cached_width, vvic_cached_height], // Use actual current dimensions of media
                format: 'rgba8unorm',
                usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST | GPUTextureUsage.RENDER_ATTACHMENT,
            });

            textureBindGroup = device.createBindGroup({ // Bind Group 0
                layout: renderPipeline.getBindGroupLayout(0),
                entries: [
                    { binding: 0, resource: webgpu_sampler },
                    { binding: 1, resource: vvicGpuTexture.createView() },
                ],
            });
        }

        // Copy vvic content to vvicGpuTexture
        // This uses the full vvic_elem (current_vvic_w x current_vvic_h) as source
        device.queue.copyExternalImageToTexture(
            { source: vvic_elem, flipY: false }, // flipY might be needed based on source and coordinate system expectations
            { texture: vvicGpuTexture, premultipliedAlpha: true },
            [vvic_cached_width, vvic_cached_height]
        );

        // Update transform uniform buffer (drawX, drawY, imageDrawWidth, imageDrawHeight)
        // currentDrawX, currentDrawY are offsets in keepSize texture
        // imageDrawWidth, imageDrawHeight are dimensions of the image being drawn
        device.queue.writeBuffer(transformUniformBuffer, 0, new Float32Array([currentDrawX, currentDrawY, imageDrawWidth, imageDrawHeight]));

        // Render vvicGpuTexture to renderTargetTexture
        const renderPassDescriptor = {
            colorAttachments: [{
                view: renderTargetTexture.createView(),
                clearValue: { r: 0.0, g: 0.0, b: 0.0, a: 0.0 }, // Clear to transparent black
                loadOp: 'clear',
                storeOp: 'store',
            }],
        };
        const passEncoder = commandEncoder.beginRenderPass(renderPassDescriptor);
        passEncoder.setPipeline(renderPipeline);
        passEncoder.setBindGroup(0, textureBindGroup);    // Sampler and vvicGpuTexture
        passEncoder.setBindGroup(1, uniformBindGroup); // Transformation uniforms
        passEncoder.draw(6); // Draw 2 triangles (a quad)
        passEncoder.end();

    } else {
        // If not ready or paused, just clear the renderTargetTexture
        // This ensures the getImageData equivalent gets a black/transparent frame
        const passEncoder = commandEncoder.beginRenderPass({
            colorAttachments: [{
                view: renderTargetTexture.createView(),
                clearValue: { r: 0.0, g: 0.0, b: 0.0, a: 0.0 },
                loadOp: 'clear',
                storeOp: 'store',
            }],
        });
        passEncoder.end();
    }

    // ---- 3. Copy from renderTargetTexture to readbackBuffer ----
    commandEncoder.copyTextureToBuffer(
        { texture: renderTargetTexture, mipLevel: 0 },
        { buffer: readbackBuffer, bytesPerRow: Math.ceil((currentKeepSize * 4) / 256) * 256, rowsPerImage: currentKeepSize },
        { width: currentKeepSize, height: currentKeepSize, depthOrArrayLayers: 1 }
    );

    // ---- 4. Submit commands and Read Data Back ----
    device.queue.submit([commandEncoder.finish()]);

    await readbackBuffer.mapAsync(GPUMapMode.READ);
    const arrayBuffer = readbackBuffer.getMappedRange();
    // Create a Uint8Array. Important: make a copy (.slice(0)) because the ArrayBuffer will be invalidated on unmap.
    const pixelDataUint8 = new Uint8Array(arrayBuffer.slice(0));
    readbackBuffer.unmap();

    // ---- 5. Write to Emscripten FS ----
    // Your original code converted to Float32Array: const pixelData = new Float32Array(imageData.data);
    // This created a Float32 for each Uint8 component, which is unusual for image data.
    // The pixelDataUint8 (RGBA8 format) is likely what your C++ side would expect if it's processing standard image data.
    // If your C++ side truly expects Float32 for each component (R as float, G as float, etc.),
    // you'd need to manually convert pixelDataUint8 to a Float32Array here.
    // Example: const pixelDataFloat32 = new Float32Array(pixelDataUint8.length);
    //          for(let i=0; i < pixelDataUint8.length; i++) pixelDataFloat32[i] = pixelDataUint8[i];
    // But this is generally inefficient and uncommon. We'll use Uint8Array.

    if (window.fileStream) { // Check if fileStream is valid
        FS.write(window.fileStream, pixelDataUint8, 0, pixelDataUint8.byteLength, 0, true /* canOwn: true, if FS can take ownership */);
        // The FS.rename logic from your original code:
        FS.rename('/video/frameBFR.gl', '/video/frameB.gl');
        FS.rename('/video/frame.gl', '/video/frameBFR.gl'); // This renames the fileStream's path if it was current
        FS.rename('/video/frameB.gl', '/video/frame.gl');

        // If frame.gl was renamed, fileStream might be invalid or point to the old name.
        // You might need to re-open or manage file handles carefully if renaming the active stream's path.
        // A safer approach for file rotation:
        // 1. Write to a temporary name: e.g., '/video/frame_new.gl'
        // 2. If '/video/frame.gl' exists, rename it to '/video/frame_old.gl'
        // 3. Rename '/video/frame_new.gl' to '/video/frame.gl'
        // 4. If '/video/frame_old.gl' exists, delete it (or keep as backup)
        // This example keeps your existing rename logic.
    }

    Module.cnvOn(); // Signal C++ side
        setTimeout(drawFrameAsync, 16.6); 
}


async function canvasStartSize2() {
    // ---- 1. Initial Setup and Parameter Calculation (from your original code) ----
    const vvic_elem = document.querySelector('#mvi'); // Renamed to avoid conflict with global vvic
    const srsiz_val = document.querySelector('#srsiz').innerHTML;
    const vsiz_val = document.querySelector('#vsiz').innerHTML;
    const SiZ_val = window.innerHeight;
    let w_orig, h_orig; // w$, h$ from your original code
    if (vvic_elem.tagName == 'CANVAS') {
        // If vvic_elem is a canvas, ensure its dimensions are set before reading
        // vvic_elem.width = vsiz_val; // Example, adjust as per your logic
        // vvic_elem.height = vsiz_val; // Example
        w_orig = vvic_elem.width;
        h_orig = vvic_elem.height;
    } else if (vvic_elem.tagName == 'IMG') {
        w_orig = vvic_elem.naturalWidth;
        h_orig = vvic_elem.naturalHeight;
        // Original code set vvic_elem.width/height here, which can distort.
        // WebGPU copyExternalImageToTexture will use natural dimensions.
    } else if (vvic_elem.tagName == 'VIDEO') {
        w_orig = vvic_elem.videoWidth;
        h_orig = vvic_elem.videoHeight;
    } else {
        console.error("Unsupported #mvi element type:", vvic_elem.tagName);
        return;
    }
    const keepSize_val = Math.min(Math.max(h_orig, w_orig), parseFloat(vsiz_val));
    const drawX_val = (keepSize_val - w_orig) / 2;
    const drawY_val = (keepSize_val - h_orig) / 2;
    console.log("Target canvas size for WebGPU: ", keepSize_val, ", ", keepSize_val);
    console.log("Image original dims: ", w_orig, "x", h_orig);
    console.log("Drawing at: ", drawX_val, ",", drawY_val);
    // Store these for the draw loop
    currentKeepSize = keepSize_val;
    currentDrawX = drawX_val;
    currentDrawY = drawY_val;
    currentW$ = w_orig; // This is the w$ that should be used for drawing dimension
    currentH$ = h_orig; // This is the h$ that should be used for drawing dimension
    // Update canvas elements for display (if any, this part is from your original)
    // const scnv = document.querySelector('#scanvas');
    // const bcnv = document.querySelector('#bcanvas');
    // if (scnv) { scnv.height = SiZ_val; scnv.width = SiZ_val; }
    // if (bcnv) {
    //     bcnv.height = keepSize_val; bcnv.style.height = keepSize_val + 'px';
    //     bcnv.width = keepSize_val; bcnv.style.width = keepSize_val + 'px';
    // }
    // OffscreenCanvas is not directly used for WebGPU context in this approach
    // ---- 2. WebGPU Initialization ----
    if (!navigator.gpu) {
        console.error("WebGPU not supported on this browser.");
        alert("WebGPU not supported on this browser.");
        return;
    }
    const adapter = await navigator.gpu.requestAdapter();
    if (!adapter) {
        console.error("Failed to get GPU adapter.");
        alert("Failed to get GPU adapter.");
        return;
    }
    device = await adapter.requestDevice();
    if (!device) {
        console.error("Failed to get GPU device.");
        alert("Failed to get GPU device.");
        return;
    }
    // ---- 3. Create WebGPU Resources ----
    webgpu_sampler = device.createSampler({
        magFilter: 'linear',
        minFilter: 'linear',
    });
    renderTargetTexture = device.createTexture({
        size: [currentKeepSize, currentKeepSize],
        format: 'rgba8unorm', // Common format, good for image data
        usage: GPUTextureUsage.RENDER_ATTACHMENT | GPUTextureUsage.COPY_SRC | GPUTextureUsage.COPY_DST,
    });

const bytesPerPixel = 4; // For rgba8unorm format
const unpaddedBytesPerRow = currentKeepSize * bytesPerPixel;
const paddedBytesPerRow = Math.ceil(unpaddedBytesPerRow / 256) * 256;

 const readbackBufferSize = paddedBytesPerRow * currentKeepSize; 
readbackBuffer = device.createBuffer({
        size: readbackBufferSize,
        usage: GPUBufferUsage.COPY_DST | GPUBufferUsage.MAP_READ,
    });

    // Uniform Buffers for transformation
    transformUniformBuffer = device.createBuffer({
        size: 4 * 4, // vec4<f32> for (drawX, drawY, w_img, h_img)
        usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
    });
    targetUniformBuffer = device.createBuffer({
        size: 2 * 4, // vec2<f32> for (target_width, target_height)
        usage: GPUBufferUsage.UNIFORM | GPUBufferUsage.COPY_DST,
    });
    // Write initial target size to uniform buffer (assuming keepSize is fixed for the session)
    device.queue.writeBuffer(targetUniformBuffer, 0, new Float32Array([currentKeepSize, currentKeepSize]));
    // ---- 4. Define Shaders (WGSL) ----
    const shaderModule = device.createShaderModule({
        code: `
            struct VertexOutput {
                @builtin(position) position: vec4<f32>,
                @location(0) texCoord: vec2<f32>,
            };
            @group(1) @binding(0) var<uniform> transform_p: vec4<f32>; // x: drawX, y: drawY, z: w_img, w: h_img
            @group(1) @binding(1) var<uniform> target_p: vec2<f32>;  // x: target_width, y: target_height
            // Unit quad vertices (0,0), (1,0), (0,1), (1,1), (1,0), (1,1)
            // Covers area from (0,0) to (1,1)
            const POSITIONS = array<vec2<f32>, 6>(
                vec2<f32>(0.0, 0.0), vec2<f32>(1.0, 0.0), vec2<f32>(0.0, 1.0),
                vec2<f32>(0.0, 1.0), vec2<f32>(1.0, 0.0), vec2<f32>(1.0, 1.0)
            );
            // Standard texture coordinates
            const TEXCOORDS = array<vec2<f32>, 6>(
                vec2<f32>(0.0, 0.0), vec2<f32>(1.0, 0.0), vec2<f32>(0.0, 1.0),
                vec2<f32>(0.0, 1.0), vec2<f32>(1.0, 0.0), vec2<f32>(1.0, 1.0)
            );
            @vertex
            fn vs_main(@builtin(vertex_index) vertexIndex: u32) -> VertexOutput {
                var out: VertexOutput;
                let pos_unit = POSITIONS[vertexIndex]; // (0..1, 0..1)
                let draw_x_px = transform_p.x;
                let draw_y_px = transform_p.y;
                let img_w_px = transform_p.z;
                let img_h_px = transform_p.w;
                let target_w_px = target_p.x;
                let target_h_px = target_p.y;
                let final_x_px = draw_x_px + pos_unit.x * img_w_px;
                let final_y_px = draw_y_px + pos_unit.y * img_h_px;
                // Convert to Normalized Device Coordinates (-1 to 1 for X and Y)
                // WebGPU NDC Y is upwards. To map 2D canvas top-left (0,0) to top-left of screen:
                // X_ndc = (pixel_x / width_px) * 2.0 - 1.0
                // Y_ndc = (1.0 - (pixel_y / height_px)) * 2.0 - 1.0  <- This maps Y=0 to +1 (top), Y=height to -1 (bottom)
                // Y_ndc = 1.0 - (final_y_px / target_h_px) * 2.0;  <-- This flips Y
                out.position = vec4<f32>(
                    (final_x_px / target_w_px) * 2.0 - 1.0,
                    (1.0 - (final_y_px / target_h_px) * 2.0), // Flip Y to match 2D canvas
                    0.0, // Z
                    1.0  // W
                );
                out.texCoord = TEXCOORDS[vertexIndex];
                return out;
            }
            @group(0) @binding(0) var smplr: sampler;
            @group(0) @binding(1) var txtr: texture_2d<f32>;
            @fragment
            fn fs_main(in: VertexOutput) -> @location(0) vec4<f32> {
                return textureSample(txtr, smplr, in.texCoord);
            }
        `,
    });
    // ---- 5. Create Render Pipeline ----
    const pipelineLayout = device.createPipelineLayout({
        bindGroupLayouts: [
            // Group 0: Sampler and Texture
            device.createBindGroupLayout({
                entries: [
                    { binding: 0, visibility: GPUShaderStage.FRAGMENT, sampler: {} },
                    { binding: 1, visibility: GPUShaderStage.FRAGMENT, texture: {} },
                ],
            }),
            // Group 1: Uniforms
            device.createBindGroupLayout({
                entries: [
                    { binding: 0, visibility: GPUShaderStage.VERTEX, buffer: { type: 'uniform'} },
                    { binding: 1, visibility: GPUShaderStage.VERTEX, buffer: { type: 'uniform'} },
                ]
            })
        ],
    });
    renderPipeline = device.createRenderPipeline({
        layout: pipelineLayout,
        vertex: {
            module: shaderModule,
            entryPoint: 'vs_main',
        },
        fragment: {
            module: shaderModule,
            entryPoint: 'fs_main',
            targets: [{ format: renderTargetTexture.format }], // Output format matches target texture
        },
        primitive: {
            topology: 'triangle-list',
        },
    });
    // Create the uniform bind group (Group 1)
    uniformBindGroup = device.createBindGroup({
        layout: renderPipeline.getBindGroupLayout(1),
        entries: [
            { binding: 0, resource: { buffer: transformUniformBuffer } },
            { binding: 1, resource: { buffer: targetUniformBuffer } },
        ],
    });
    // ---- 6. Event Listeners and Starting the Loop (from your original code) ----
    document.querySelector('#moveFwdb').addEventListener('click', function() {
        Module.ccall('frmsOff');
        console.log('stopping frames for move');
        window.pause = 'loading'; // Assuming pause is a global or window property
        setTimeout(function() {
            window.pause = 'ready';
            Module.ccall('frmsOn');
        }, 1900);
    });
    // File stream (ensure 'FS' is Emscripten's FS object)
    // Make sure fileStream is initialized appropriately before this point
    if (!window.fileStream) { // Example of lazy initialization if not done earlier
        window.fileStream = FS.open('/video/frame.gl', 'w');
    }
    if (window.running == 0) { // Assuming 'running' is global or window property
        setTimeout(() => {
            console.log('Sending to WebGPU C++ (if applicable): ', currentKeepSize, vsiz_val, srsiz_val);
            // Adjust this call if your C++ side expects different parameters or no call at this stage
            Module.ccall("startWebGPUC", null, ["Number", "Number", "Number"], [currentKeepSize, parseFloat(vsiz_val), parseFloat(srsiz_val)]);
            window.running = 1;
          //  setInterval(drawFrameAsync, 16.6); // ~60 FPS
            drawFrameAsync();
        }, 250);
    } else {
            drawFrameAsync();
        // setInterval(drawFrameAsync, 16.6);
    }
}

function flipImageData(imageData){
const width=imageData.width;
const height=imageData.height;
const data=imageData.data;
for(let y=0;y<height/2;y++){
for(let x=0;x<width;x++){
const topIndex=(y*width+x)*4;
const bottomIndex=((height-1-y)*width+x)*4;
for(let c=0;c<4;c++){
[data[topIndex+c],data[bottomIndex+c]]=[data[bottomIndex+c],data[topIndex+c]];
}}}
return imageData;
}

function nearestPowerOf2(n){
if(n&(n-1)){
return Math.pow(2,Math.ceil(Math.log2(n)));
}else{
return n;
}}

let pause='ready';
    
function canvasStart2(){
let vvic=document.querySelector('#mvi');
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
var SiZ=window.innerHeight;
var w$=parseInt(vsiz,10);
vvic.width=SiZ;
var h$=parseInt(vsiz,10);
vvic.height=SiZ;
console.log("canvas size: ",h$,", ",w$);
const cnvb=new OffscreenCanvas(h$,w$); 
// document.querySelector('#contain2').appendChild(cnvb);
const cnv=document.querySelector('#scanvas');
const cnvc=document.querySelector('#bcanvas');
cnv.height=SiZ;
cnvb.height=vsiz;
cnvc.height=vsiz;
cnvc.style.height=vsiz+'px';
cnv.width=SiZ;
cnvb.width=vsiz;
cnvc.width=vsiz;
cnvc.style.width=vsiz+'px';
const gl3=cnvb.getContext('2d',{
// colorType:'float64',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
});
gl3.imageSmoothingEnabled=false;
let fileStream=FS.open('/video/frame.gl','w');
  function drawFrame() {
    if (pause === 'ready') {
      gl3.clearRect(0, 0, w$, h$);
      gl3.drawImage(vvic, 0, 0, SiZ, SiZ, 0, 0, w$, h$);
    }else{
        console.log('frames stopped');
        }
    const image = gl3.getImageData(0, 0, w$, h$);
    const imageData = image.data;
    const pixelData = new Float64Array(imageData);
    FS.write(fileStream, pixelData, 0, pixelData.length, 0);
    Module.frmOn();
  }
  if (running == 0) {
    setTimeout(() => {
      Module.ccall("startWebGPUC", null,"Number",[vsiz,srsiz]);
      running = 1;
      setInterval(drawFrame, 16.6); 
    }, 250);
  } else {
    setInterval(drawFrame, 16.6);
  }
}

function canvasStartSize(){
const media_mode = document.querySelector('#media').value;
if(media_mode==='img'){
const vvic=document.querySelector('#ivi');
}
if(media_mode==='vid'){
const vvic=document.querySelector('#mvi');
}
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
// vvic.width=vsiz;
// vvic.height=vsiz;
let w$; //=vsiz;
let h$; //=vsiz;
if(vvic.tagName=='CANVAS'){
vvic.width=vsiz;
vvic.height=vsiz;
w$=vsiz;
h$=vsiz;
}
if(vvic.tagName=='IMG'){
w$=vvic.naturalWidth;
h$=vvic.naturalHeight;
vvic.width=vvic.naturalWidth;
vvic.height=vvic.naturalHeight;
}
if(vvic.tagName=='VIDEO'){
w$=vvic.videoWidth;
h$=vvic.videoHeight;
vvic.width=vvic.videoWidth;
vvic.height=vvic.videoHeight;
}
const keepSizea = Math.max(h$, w$);
const keepSize = parseInt(Math.min(keepSizea, vsiz));
const drawX = parseInt((keepSize - w$) / 2);
const drawY = parseInt((keepSize - h$) / 2);
console.log("canvas size: ",keepSize,", ",keepSize);
const OffscCnv=new OffscreenCanvas(keepSize,keepSize); 
// document.querySelector('#contain2').appendChild(OffscCnv);
const scnv=document.querySelector('#scanvas');
const bcnv=document.querySelector('#bcanvas');
scnv.height=SiZ;
OffscCnv.height=keepSize;
bcnv.height=keepSize;
bcnv.style.height=keepSize+'px';
scnv.width=SiZ;
OffscCnv.width=keepSize;
bcnv.width=keepSize;
bcnv.style.width=keepSize+'px';
const gl3=OffscCnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:false,
preserveDrawingBuffer:false
});
document.querySelector('#moveFwdb').addEventListener('click',function(){
Module.ccall('frmsOff');
console.log('stopping frames for move');
pause = 'loading';
setTimeout(function(){
pause = 'ready';
Module.ccall('frmsOn');
// console.log('restarting frames for move');
}, 1900);
});
// gl3.imageSmoothingEnabled=false;
// const fileStream=FS.open('/video/frameBFR.gl','w+');
const fileStream=FS.open('/video/frame.gl','w');
function drawFrame() {
if (pause == 'ready') {
gl3.clearRect(0, 0, keepSize, keepSize);
gl3.drawImage(vvic, 0, 0, w$, h$, drawX, drawY, w$, h$); 
}
const image = gl3.getImageData(0, 0, keepSize, keepSize);
const imageData = image.data;
const pixelData = new Float32Array(imageData);
FS.write(fileStream, pixelData, 0, pixelData.length, 0);
FS.rename('/video/frameBFR.gl', '/video/frameB.gl');
FS.rename('/video/frame.gl', '/video/frameBFR.gl');
FS.rename('/video/frameB.gl', '/video/frame.gl');
Module.cnvOn();
setTimeout(drawFrame, 16);
}
if (running == 0) {
setTimeout(() => {
console.log('sending: ',keepSize,vsiz,srsiz);
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[keepSize,vsiz,srsiz]);
running = 1;
// setInterval(drawFrame, 16.6); 
drawFrame(); 
}, 250);
} else {
// setInterval(drawFrame, 16.6);
drawFrame(); 
}
}

function canvasStartEmbind(){
const vvic=document.querySelector('#mvi');
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
// vvic.width=vsiz;
// vvic.height=vsiz;
let w$; //=vsiz;
let h$; //=vsiz;
if(vvic.tagName=='CANVAS'){
vvic.width=vsiz;
vvic.height=vsiz;
w$=vsiz;
h$=vsiz;
}
if(vvic.tagName=='IMG'){
w$=vvic.width; // naturalWidth;
h$=vvic.height; // naturalHeight;
// vvic.width=vvic.naturalWidth;
// vvic.height=vvic.naturalHeight;
}
if(vvic.tagName=='VIDEO'){
w$=vvic.width; // videoWidth;
h$=vvic.height; // videoHeight;
// vvic.width=vvic.videoWidth;
// vvic.height=vvic.videoHeight;
}
const keepSizea = Math.max(h$, w$);
const keepSize = Math.min(keepSizea, vsiz);
const drawX = (keepSize - w$) / 2;
const drawY = (keepSize - h$) / 2;
console.log("canvas size: ",keepSize,", ",keepSize);
const OffscCnv=new OffscreenCanvas(keepSize,keepSize); 
// document.querySelector('#contain2').appendChild(OffscCnv);
const scnv=document.querySelector('#scanvas');
const bcnv=document.querySelector('#bcanvas');
scnv.height=SiZ;
OffscCnv.height=keepSize;
bcnv.height=keepSize;
bcnv.style.height=keepSize+'px';
scnv.width=SiZ;
OffscCnv.width=keepSize;
bcnv.width=keepSize;
bcnv.style.width=keepSize+'px';
const gl3=OffscCnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:true,
depth:true,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:false,
preserveDrawingBuffer:false
});
document.querySelector('#moveFwdb').addEventListener('click',function(){
Module.ccall('frmsOff');
console.log('stopping frames for move');
pause = 'loading';
setTimeout(function(){
pause = 'ready';
Module.ccall('frmsOn');
// console.log('restarting frames for move');
}, 1900);
});
if (running == 0) {
setTimeout(() => {
console.log('sending: ',keepSize,vsiz,srsiz);
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
running = 1;
frameBufferViewF32 = Module.getPixelBufferView();
console.log(`Obtained C++ buffer view with length: ${frameBufferViewF32.length}`);
}, 250);
} else {
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
frameBufferViewF32 = Module.getPixelBufferView();
console.log(`Obtained C++ buffer view with length: ${frameBufferViewF32.length}`);
}
setInterval(function() {
if (pause === 'ready') {
gl3.clearRect(0, 0, keepSize, keepSize);
gl3.drawImage(vvic, 0, 0, w$, h$, drawX, drawY, w$, h$); 
}
const image = gl3.getImageData(0, 0, keepSize, keepSize);
const imageData = image.data;
Module.processFrameAndConvert(imageData);
Module.frmOn();
},16);
}

function canvasStartSizeBFR(){
const vvic=document.querySelector('#mvi');
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
// vvic.width=vsiz;
// vvic.height=vsiz;
let w$; //=vsiz;
let h$; //=vsiz;
if(vvic.tagName=='CANVAS'){
vvic.width=vsiz;
vvic.height=vsiz;
w$=vsiz;
h$=vsiz;
}
if(vvic.tagName=='IMG'){
w$=vvic.naturalWidth;
h$=vvic.naturalHeight;
vvic.width=vvic.naturalWidth;
vvic.height=vvic.naturalHeight;
}
if(vvic.tagName=='VIDEO'){
w$=vvic.videoWidth;
h$=vvic.videoHeight;
vvic.width=vvic.videoWidth;
vvic.height=vvic.videoHeight;
}
const keepSizea = Math.max(h$, w$);
const keepSize = Math.min(keepSizea, vsiz);
const drawX = (keepSize - w$) / 2;
const drawY = (keepSize - h$) / 2;
console.log("canvas size: ",keepSize,", ",keepSize);
const OffscCnv=new OffscreenCanvas(keepSize,keepSize); 
// document.querySelector('#contain2').appendChild(OffscCnv);
const scnv=document.querySelector('#scanvas');
const bcnv=document.querySelector('#bcanvas');
scnv.height=SiZ;
OffscCnv.height=keepSize;
bcnv.height=keepSize;
bcnv.style.height=keepSize+'px';
scnv.width=SiZ;
OffscCnv.width=keepSize;
bcnv.width=keepSize;
bcnv.style.width=keepSize+'px';
const gl3=OffscCnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
});
document.querySelector('#moveFwdb').addEventListener('click',function(){
Module.ccall('frmsOff');
console.log('stopping frames for move');
pause = 'loading';
setTimeout(function(){
pause = 'ready';
Module.ccall('frmsOn');
// console.log('restarting frames for move');
}, 1900);
});

if (running == 0) {
setTimeout(() => {
console.log('sending: ',keepSize,vsiz,srsiz);
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
running = 1;
frameBufferViewF32 = Module.getPixelBufferView();
console.log(`Obtained C++ buffer view with length: ${frameBufferViewF32.length}`);
}, 250);
} else {
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);

frameBufferViewF32 = Module.getPixelBufferView();
console.log(`Obtained C++ buffer view with length: ${frameBufferViewF32.length}`);
}

// gl3.imageSmoothingEnabled=false;
// const fileStream=FS.open('/video/frame.gl','w');
setInterval(function() {
if (pause === 'ready') {
gl3.clearRect(0, 0, keepSize, keepSize);
gl3.drawImage(vvic, 0, 0, w$, h$, drawX, drawY, w$, h$); 
}
const image = gl3.getImageData(0, 0, keepSize, keepSize);
const imageData = image.data;
  // val array
const pixelCount = keepSize * keepSize * 4; // RGBA
for (let i = 0; i < pixelCount; ++i) {
// Normalize uint8 (0-255) to float (0.0-1.0)
const normalizedValue = imageData[i] / 255.0;
frameBufferViewF32[i] = normalizedValue;
}
// const pixelData = new Float32Array(imageData);
// const pixelData = new Uint8Array(imageData);
// FS.write(fileStream, pixelData, 0, pixelData.length, 0);
Module.frmOn();
},16);

}


function canvasStart(){
const vvic=document.querySelector('#mvi');
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
const w$=parseInt(vsiz,10);
// vvic.width=SiZ;
const h$=parseInt(vsiz,10);
// vvic.height=SiZ;
console.log("canvas size: ",h$,", ",w$);
const cnvb=new OffscreenCanvas(h$,w$); 
// document.querySelector('#contain2').appendChild(cnvb);
const cnv=document.querySelector('#scanvas');
const cnvc=document.querySelector('#bcanvas');
cnv.height=SiZ;
// cnvb.height=vsiz;
cnvc.height=vsiz;
cnvc.style.height=vsiz+'px';
cnv.width=SiZ;
// cnvb.width=vsiz;
cnvc.width=vsiz;
cnvc.style.width=vsiz+'px';
const gl3=cnvb.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
});
// gl3.imageSmoothingEnabled=false;
const fileStream=FS.open('/video/frame.gl','w+');
function drawFrame() {
if (pause === 'ready') {
gl3.clearRect(0, 0, w$, h$);
gl3.drawImage(vvic, 0, 0, SiZ, SiZ, 0, 0, w$, h$);
const image = gl3.getImageData(0, 0, w$, h$);
const imageData = image.data;
const pixelData = new Float32Array(imageData);
FS.write(fileStream, pixelData, 0, pixelData.length, 0);
Module.frmOn();
}
}
if (running == 0) {
setTimeout(() => {
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vvic.height,vsiz,srsiz]);
running = 1;
setInterval(drawFrame, 16.6); 
}, 250);
} else {
setInterval(drawFrame, 16.6);
}
}


function khz(){
let timeStart=performance.now();
let cycle;
let dot;
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
console.log("canvas size: ",SiZ,", ",SiZ);
const OffscCnv=new OffscreenCanvas(vsiz,vsiz); 
const scnv=document.querySelector('#scanvas');
const bcnv=document.querySelector('#bcanvas');
scnv.height=SiZ;
OffscCnv.height=vsiz;
bcnv.height=vsiz;
bcnv.style.height=vsiz+'px';
scnv.width=SiZ;
OffscCnv.width=vsiz;
bcnv.width=vsiz;
bcnv.style.width=vsiz+'px';
const gl3=OffscCnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
});
// gl3.imageSmoothingEnabled=false;
const fileStream=FS.open('/video/frame.gl','w+');
let matrix=    gl3.createImageData(vsiz,vsiz);
for (let i = 0; i < matrix.data.length; i += 4){
matrix.data[i+0] = 0;
matrix.data[i+1] = 0;
matrix.data[i+2] = 0;
matrix.data[i+3] = 255;
}
function drawFrame() {
if (pause === 'ready') {
gl3.clearRect(0, 0, vsiz, vsiz);
gl3.fillStyle = 'black';
gl3.fillRect(0, 0, vsiz, vsiz);
cycle=performance.now()-timeStart;
cycle=cycle%1.0;
dot=330000*cycle;
dot=(vsiz*vsiz)*dot*4;
for (let i = 0; i < matrix.data.length; i += 4){
matrix.data[i+0] = 0;
matrix.data[i+1] = 0;
matrix.data[i+2] = 0;
matrix.data[i+3] = 255;
}
matrix.data[dot] = 255;
matrix.data[dot+1] = 255;
matrix.data[dot+2] = 255;
matrix.data[dot+3] = 255;
matrix.data[dot+4] = 255;
matrix.data[dot+5] = 255;
matrix.data[dot+6] = 255;
matrix.data[dot+7] = 255;
matrix.data[dot+8] = 255;
matrix.data[dot+9] = 255;
matrix.data[dot+10] = 255;
matrix.data[dot+11] = 255;
gl3.putImageData(matrix,vsiz,vsiz);
}
const image = gl3.getImageData(0, 0, vsiz, vsiz);
const imageData = image.data;
const pixelData = new Float32Array(imageData);
FS.write(fileStream, pixelData, 0, pixelData.length, 0);
Module.frmOn();
}
if (running == 0) {
setTimeout(() => {
console.log('sending: ',vsiz,vsiz,srsiz);
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
running = 1;
setInterval(drawFrame, 16.6); 
}, 250);
} else {
setInterval(drawFrame, 16.6);
}
}

function createRGBAFrame(audioChunk, chunkIndex) {
const width = 1024;
const height = 1024;
const frameSize = width * height * 4; // RGBA frame size
const frameData = new Uint8ClampedArray(frameSize);
const vvic=document.querySelector('#mvi');
const srsiz=document.querySelector('#srsiz').innerHTML;
const vsiz=document.querySelector('#vsiz').innerHTML;
const SiZ=window.innerHeight;
const w$=parseInt(vsiz,10);
// vvic.width=SiZ;
const h$=parseInt(vsiz,10);
// vvic.height=SiZ;
console.log("canvas size: ",h$,", ",w$);
const cnvb=new OffscreenCanvas(h$,w$); 
// document.querySelector('#contain2').appendChild(cnvb);
const cnv=document.querySelector('#scanvas');
const cnvc=document.querySelector('#bcanvas');
cnv.height=SiZ;
// cnvb.height=vsiz;
cnvc.height=vsiz;
cnvc.style.height=vsiz+'px';
cnv.width=SiZ;
// cnvb.width=vsiz;
cnvc.width=vsiz;
cnvc.style.width=vsiz+'px';
const gl3=cnvb.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:true,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
});
// gl3.imageSmoothingEnabled=false;
const fileStream=FS.open('/video/frame.gl','w+');
for (let i = 0; i < audioChunk.length; i++) {
const sampleValue = audioChunk[i];
const rgbaValue = Math.floor((sampleValue + 1) * 127.5); // Normalize to 0-255
const x = i % width;
const y = Math.floor(i / width);
const index = (y * width + x) * 4;
frameData[index] = rgbaValue; // R
frameData[index + 1] = rgbaValue; // G
frameData[index + 2] = rgbaValue; // B
frameData[index + 3] = 255; // A
}
const image = new ImageData(frameData, width, height);
const imageData = image.data;
const pixelData = new Float32Array(imageData);
FS.write(fileStream, pixelData, 0, pixelData.length, 0);
Module.frmOn();
}

function splitAudioIntoChunks(audioData) {
const chunkSize = 1024 * 1024; // 1024x1024 samples
const numberOfChunks = Math.ceil(audioData.length / chunkSize);
for (let i = 0; i < numberOfChunks; i++) {
const start = i * chunkSize;
const end = Math.min(start + chunkSize, audioData.length);
const chunk = audioData.slice(start, end);
setTimeout(function(){
createRGBAFrame(chunk, i);
},16.6);
  }
}

function processAudioBuffer(audioBuffer) {
  const channelData = audioBuffer.getChannelData(0); // Get the first channel data
  const sampleRate = audioBuffer.sampleRate;
  const duration = audioBuffer.duration;
  const numberOfChannels = audioBuffer.numberOfChannels;
  console.log('Sample Rate:', sampleRate);
  console.log('Duration:', duration);
  console.log('Number of Channels:', numberOfChannels);
  splitAudioIntoChunks(channelData);
}

function birdsongStart(){
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
const pth=document.querySelector('#birdsongPath').innerHTML;
const audioContext = new (window.AudioContext || window.webkitAudioContext)();
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
document.querySelector('#stat').innerHTML='Downloading Song';
document.querySelector('#stat').style.backgroundColor='yellow';
ff.addEventListener("load", function() {
  let sarrayBuffer = ff.response;
  if (sarrayBuffer) {
    audioContext.decodeAudioData(sarrayBuffer).then(audioBuffer => {
      processAudioBuffer(audioBuffer);
    }).catch(err => {
      console.error('Error decoding audio data:', err);
    });
  }
});
ff.send();
if (running == 0) {
Module.ccall("startWebGPUC", null,["Number","Number","Number"],[1024,vsiz,srsiz]);
running = 1;
}
}

function videoStartA(){
let vvi,h$,w$,SiZ;
const media_mode = document.querySelector('#media').value;
let cropSize; // The side length of the square to cut from the source
let sx = 0;   // Source X for cropping
let sy = 0;   // Source Y for cropping
if (media_mode=='vid'){
vvi=document.querySelector('#mvi');
w$=parseInt(document.querySelector("#mvi").width);
h$=parseInt(document.querySelector("#mvi").height);
SiZ=window.innerHeight;
if (w$ > h$) {
            cropSize = h$;
            sx = (w$ - h$) / 2;
} else {
            cropSize = w$;
            sy = (h$ - w$) / 2;
}
// vvi.height=SiZ;
// vvi.width=Math.min(w$,(SiZ/h$)*w$);
// w$=parseInt(document.querySelector("#mvi").width);
// h$=parseInt(document.querySelector("#mvi").height);
}
if (media_mode=='img'){
vvi=document.querySelector('#ivi');
w$=parseInt(document.querySelector("#ivi").width);
h$=parseInt(document.querySelector("#ivi").height);
SiZ=window.innerHeight;
if (w$ > h$) {
            cropSize = h$;
            sx = (w$ - h$) / 2;
} else {
            cropSize = w$;
            sy = (h$ - w$) / 2;
}
// vvi.height=SiZ;
// vvi.width=Math.min(w$,(SiZ/h$)*w$);
// w$=parseInt(document.querySelector("#ivi").width);
// h$=parseInt(document.querySelector("#ivi").height);
}
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
if(running==0){
setTimeout(function(){
Module.ccall("startWebGPUi",null,["Number","Number","Number"],[vvi.height,vsiz,srsiz]);
console.log('Starting..');
frameBufferViewF32 = Module.getPixelBufferView();
running=1;
},250);
}else{
setTimeout(function(){
Module.ccall("startWebGPUbi",null,["Number","Number","Number"],[vvi.height,vsiz,srsiz]);
console.log('Starting..');
frameBufferViewF32 = Module.getPixelBufferView();
},250);
}
     //    console.log(`Obtained C++ buffer view with length: ${frameBufferViewF32.length}`);
// const bufferPtr = Module.get_buffer_ptr();
const bufferSizeFloats = w$*h$*4;
// const frameView = new Float32Array(Module.HEAPF32.buffer, bufferPtr, bufferSizeFloats);
// console.log(`JS: Created manual view at ${bufferPtr}, size ${bufferSizeFloats}`);
console.log("vid size: ",h$,", ",w$);
const cnvb=new OffscreenCanvas(h$,w$);
// cnvb.id='ccanvas';
// cnvb.hidden=true;
// document.querySelector('#contain2').appendChild(cnvb);
const cnv=document.querySelector('#scanvas');
const cnvc=document.querySelector('#bcanvas');
cnv.height=SiZ;
cnvb.height=vsiz;
cnvc.height=vsiz;
cnvc.style.height=vsiz+'px';
cnv.width=SiZ;
cnvb.width=vsiz;
cnvc.width=vsiz;
cnvc.style.width=vsiz+'px';
const gl3=cnvb.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:false,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:false,
preserveDrawingBuffer:false
});
gl3.drawImage(vvi, sx, sy, cropSize, cropSize, 0, 0, vsiz, vsiz); 
// var image=flipImageData(gl3.getImageData(0,0,w$,h$));
var image=gl3.getImageData(0,0,cropSize,cropSize);
var imageData=image.data;
// var pixelData=new Float32Array(imageData);
const pixelCount = cropSize * cropSize * 4; // RGBA
for (let i = 0; i < pixelCount; ++i) {
// Normalize uint8 (0-255) to float (0.0-1.0)
const normalizedValue = imageData[i] / 255.0;
frameBufferViewF32[i] = normalizedValue;
}
/*
// let pixelData=new Uint8ClampedArray(imageData);
var pixelData=new Float32Array(imageData);
// var pixelData=new Float32Array(imageData,0,la);
let fileStream=FS.open('/video/frameBFR.gl','w');
FS.write(fileStream,pixelData,0,pixelData.length,0);
*/
/*
// Module.processCopiedDataVal(pixelData);
const floatArray = new Float32Array(imageData.length);
for(let i = 0; i < imageData.length; i++) {
floatArray[i] = imageData[i] / 255.0;
}
*/
// Module.processCopiedDataVal(imageData);
Module.frmOn();
setInterval(function(){
gl3.clearRect(0,0,cropSize,cropSize);  
gl3.drawImage(vvi, sx, sy, cropSize, cropSize, 0, 0, vsiz, vsiz);
// image=flipImageData(gl3.getImageData(0,0,w$,h$));
image=gl3.getImageData(0,0,cropSize,cropSize);
imageData=image.data;
for (let i = 0; i < pixelCount; ++i) {
// Normalize uint8 (0-255) to float (0.0-1.0)
const normalizedValue = imageData[i] / 255.0;
frameBufferViewF32[i] = normalizedValue;
}
/*
// pixelData=new Uint8ClampedArray(imageData);
pixelData=new Float32Array(imageData);
 // pixelData=new Float32Array(imageData);
 //  const externalTexture = gpuDevice.createTexture({size: [imageWidth, imageHeight, 1],format: 'rgba8unorm',usage: GPUTextureUsage.TEXTURE_BINDING | GPUTextureUsage.COPY_DST });
// gpuQueue.writeTexture({ texture }, pixelData, { bytesPerRow }, { width: w$, height: h$ } );
// pixelData=new Float32Array(imageData,0,la);  // causes sub-array data array-reforming (slower)
FS.write(fileStream,pixelData,0,pixelData.length,0);
*/
// pixelData=new Float32Array(imageData);
// Module.processCopiedDataVal(pixelData);
// console.log(`Frame data sample: [${frameBufferViewF32[0].toFixed(2)}, ${frameBufferViewF32[1].toFixed(2)}, ${frameBufferViewF32[2].toFixed(2)}, ${frameBufferViewF32[3].toFixed(2)}]`);
/*
for(let i = 0; i < imageData.length; i++) {
floatArray[i] = imageData[i] / 255.0;
}
*/
// Module.processCopiedDataVal(imageData);
Module.frmOn();
},16.666);
}                 //  have gemini help crop to square



// It's good practice to store the interval ID in a higher scope
// so it can be cleared properly if videoStart is called again.
let animationIntervalId = null;


      //  video called by startBtn  //
      ////////////////////////////////

function videoStart() {
    // 1. Interval Management: Clear any existing animation interval
    if (animationIntervalId) {
        clearInterval(animationIntervalId);
        animationIntervalId = null;
    }
    let vvi, h$, w$, SiZ;
    const media_mode = document.querySelector('#media').value;
    let cropSize; // The side length of the square to cut from the source
    let sx = 0;   // Source X for cropping
    let sy = 0;   // Source Y for cropping
    if (media_mode == 'vid') {
        vvi = document.querySelector('#mvi');
        // Ensure video metadata is loaded to get correct dimensions
        // This might require waiting for 'loadedmetadata' event if dimensions are 0 initially
        w$ = parseInt(vvi.videoWidth || vvi.width);
        h$ = parseInt(vvi.videoHeight || vvi.height);
        SiZ = window.innerHeight;
        if (w$ > h$) {
            cropSize = h$;
            sx = (w$ - h$) / 2;
        } else {
            cropSize = w$;
            sy = (h$ - w$) / 2;
        }
    } else if (media_mode == 'img') {
        vvi = document.querySelector('#ivi');
        // Ensure image is loaded to get correct dimensions
        // This might require waiting for 'load' event if dimensions are 0 initially
        w$ = parseInt(vvi.naturalWidth || vvi.width);
        h$ = parseInt(vvi.naturalHeight || vvi.height);
        SiZ = window.innerHeight;
        if (w$ > h$) {
            cropSize = h$;
            sx = (w$ - h$) / 2;
        } else {
            cropSize = w$;
            sy = (h$ - w$) / 2;
        }
    } else {
        console.error("Unknown media mode:", media_mode);
        return; // Exit if media_mode is not recognized
    }
    // Ensure w$ and h$ (and thus cropSize) are valid before proceeding
    if (!w$ || !h$ || !cropSize) {
        console.warn("Media dimensions are not available yet or are invalid. Retrying in 100ms.");
        // Potentially retry or ensure media is loaded before calling videoStart
        setTimeout(videoStart, 100);
        return;
    }
    // Read target processing size (vsiz) and source size info (srsiz) from HTML
    // These are assumed to be set correctly in your HTML
    const srsiz = parseInt(document.querySelector('#srsiz').innerHTML);
    const vsiz = parseInt(document.querySelector('#vsiz').innerHTML);
    if (isNaN(vsiz) || vsiz <= 0) {
        console.error("#vsiz HTML element has invalid content. It should be a positive number.");
        return;
    }
    if (isNaN(srsiz)) {
        console.error("#srsiz HTML element has invalid content.");
        // Decide if srsiz is critical and return if necessary
    }
    // Initialize or re-initialize WebAssembly Module
    // The parameters to ccall (vvi.height, vsiz, srsiz) are kept as in your original code.
    // Ensure these are the correct parameters your Wasm module expects.
    // Specifically, vvi.height might be h$ (cropped height before scaling if that's relevant).
    if (running == 0) {
        setTimeout(function() {
            Module.ccall("startWebGPUi", null, ["number", "number", "number"], [vsiz, vsiz, srsiz]);
            console.log('Starting WebGPU (initial)...');
            frameBufferViewF32 = Module.getPixelBufferView(); // Get buffer for pixel data
            running = 1;
            // Start processing loop only after Wasm is initialized and buffer is ready
            if (frameBufferViewF32) {
                startProcessingLoop();
            } else {
                console.error("Failed to get pixel buffer view from Wasm module.");
            }
        }, 250);
    } else {
        setTimeout(function() {
            Module.ccall("startWebGPUbi", null, ["number", "number", "number"], [vsiz, vsiz, srsiz]);
            console.log('Starting WebGPU (re-init)...');
            frameBufferViewF32 = Module.getPixelBufferView(); // Re-get buffer if necessary
             // Start processing loop only after Wasm is initialized and buffer is ready
            if (frameBufferViewF32) {
                startProcessingLoop();
            } else {
                console.error("Failed to get pixel buffer view from Wasm module (re-init).");
            }
        }, 250);
    }
    // This function will contain the canvas operations and the setInterval
    function startProcessingLoop() {
        console.log("Source media dimensions (w,h): ", w$, ",", h$);
        console.log("Crop settings (sx,sy,cropSize): ", sx, ",", sy, ",", cropSize);
        console.log("Target processing size (cropSize): ", cropSize);
        // 2. OffscreenCanvas Creation:
        // Create the OffscreenCanvas with the target processing dimensions 'vsiz x vsiz'.
        const cnvb = new OffscreenCanvas(cropSize, cropSize);
        // Setup for main display canvas (scanvas) and an intermediate canvas (bcanvas)
        const cnv = document.querySelector('#scanvas');  // Final display canvas
        const cnvc = document.querySelector('#bcanvas'); // Intermediate display canvas (shows what's on OffscreenCanvas)
     //   cnv.height = SiZ;
     //   cnv.width = SiZ;
        // Set bcanvas (presumably for debugging/previewing the OffscreenCanvas content)
        // to the same dimensions as the OffscreenCanvas.
        cnvc.height = cropSize;
        cnvc.width = cropSize;
        cnvc.style.height = cropSize + 'px';
        cnvc.style.width = cropSize + 'px';
        const gl3 = cnvb.getContext('2d', {
            // Note: 'colorType' is not a standard 2D context option.
            // For standard 2D context, color space is managed differently.
            // If you intended WebGL, options are different. Assuming 2D for now.
            alpha: true,
            willReadFrequently: true, // Set to true as you are using getImageData frequently
            // desynchronized: true, // Consider for lower latency if supported and applicable
            // powerPreference: "high-performance", // Good choice
        });
        if (!gl3) {
            console.error("Failed to get 2D context from OffscreenCanvas.");
            return;
        }
        // Initial draw and data extraction (if needed immediately before interval)
        // This part is largely similar to what's in the interval, so you might only need the interval.
        // However, if the first frame is critical to be processed fast, keep it.
        gl3.drawImage(vvi, sx, sy, cropSize, cropSize, 0, 0, cropSize, cropSize);
        let image = gl3.getImageData(0, 0, cropSize, cropSize); // 3. Corrected: Use vsiz
        let imageData = image.data;
        const pixelCount = cropSize * cropSize * 4; // 4. Corrected: Use vsiz for pixel count
        // Check if frameBufferViewF32 is valid and has enough space
        if (!frameBufferViewF32 || frameBufferViewF32.length < pixelCount) {
             console.error(`frameBufferViewF32 is not correctly initialized or is too small. Expected: ${pixelCount}, Got: ${frameBufferViewF32 ? frameBufferViewF32.length : 'null'}`);
             return; // Stop if buffer is not ready
        }
        for (let i = 0; i < pixelCount; ++i) {
            frameBufferViewF32[i] = imageData[i] / 255.0; // Normalize uint8 (0-255) to float (0.0-1.0)
        }
        Module.frmOn(); // Send the first frame to Wasm
        // --- Animation Loop using setInterval ---
        animationIntervalId = setInterval(function() {
            // Clear the OffscreenCanvas for the new frame
            gl3.clearRect(0, 0, cropSize, cropSize);
            // Draw the current state of the video/image (cropped and scaled) onto the OffscreenCanvas
            // sx, sy, cropSize are from the source media (vvi)
            // 0, 0, vsiz, vsiz are for the destination (cnvb)
            gl3.drawImage(vvi, sx, sy, cropSize, cropSize, 0, 0, cropSize, cropSize);
            // Get the pixel data from the OffscreenCanvas
            image = gl3.getImageData(0, 0, cropSize, cropSize); // 3. Corrected: Use vsiz
            imageData = image.data;
            // pixelCount is already defined correctly based on vsiz
            // Normalize and copy pixel data to the WebAssembly module's buffer
            for (let i = 0; i < pixelCount; ++i) {
                const normalizedValue = imageData[i] / 255.0;
                frameBufferViewF32[i] = normalizedValue;
            }
            // Notify the WebAssembly module that a new frame is ready
            Module.frmOn();
/*
            // Optional: If you want to display the content of the OffscreenCanvas on 'bcanvas'
            const cnvcCtx = cnvc.getContext('2d');
            if (cnvcCtx) {
                cnvcCtx.clearRect(0,0,cropSize,cropSize);
                cnvcCtx.drawImage(cnvb, 0, 0, cropSize, cropSize);
            }
*/
        }, 16.666); // Aim for roughly 60 FPS
    }
}


  //  image called by startBtn //
  ///////////////////////////////

function imageStart(){
var vvi=document.querySelector('#ivi');
let SiZ=window.innerHeight;
let w$=parseInt(document.querySelector("#ivi").width);
let h$=parseInt(document.querySelector("#ivi").height);
if(running==0){
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUi",null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
console.log('Starting..');
running=1;
},250);
}else{
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUbi",null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
console.log('Starting..');
},250);
}
console.log("vid size: ",h$,", ",w$);
let cnv=document.querySelector('#bcanvas');
let cnvb=document.querySelector('#scanvas');
var offsetX=Math.floor((w$-h$)/2);
var offsetY=Math.floor((h$-w$)/2);
cnvb.height=SiZ;
cnv.height=h$-offsetY;
cnvb.width=SiZ;
cnv.width=w$-offsetX;
let la=nearestPowerOf2(((w$*h$*4)/4)*4);
// const gl3=cnvb.getContext('2d',{colorType:'float32',alpha:true}); // 
const gl3=cnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:false,
stencil:false,
depth:false,
colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:false,
preserveDrawingBuffer:false
});
gl3.drawImage(vvi,0,0,w$-offsetX,h$-offsetY,0,0,w$-offsetX,h$-offsetY);
var image=gl3.getImageData(0,0,w$-offsetX,h$-offsetY);
var imageData=image.data;
let pixelData=new Uint8ClampedArray(imageData);
var fileStream=FS.open('/video/frame.gl','w+');
FS.write(fileStream,pixelData,0,pixelData.length,0);
Module.cnvOn();
setInterval(function(){
gl3.clearRect(0,0,w$,h$);  
gl3.drawImage(vvi,0,0,w$-offsetX,h$-offsetY,0,0,w$-offsetX,h$-offsetY);
var image2=gl3.getImageData(0,0,w$-offsetX,h$-offsetY);
var imageData=image2.data;
var pixelData=new Float32Array(imageData);
FS.write(fileStream,pixelData,0,pixelData.length,0);
Module.cnvOn();
},16.666);
}

function imageStartSR(){
let vvi=document.querySelector('#ivi');
let SiZ=window.innerHeight;
let w$=parseInt(document.querySelector("#ivi").width);
let h$=parseInt(document.querySelector("#ivi").height);
if(running==0){
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUi",null,["Number","Number","Number"],[vvi.height,vsiz,srsiz]);
console.log('Starting..');
running=1;
},250);
}else{
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUbi",null,["Number","Number","Number"],[vvi.height,vsiz,srsiz]);
console.log('Starting..');
},250);
}
console.log("vid size: ",h$,", ",w$);
let cnv=document.querySelector('#bcanvas');
let cnvb=document.querySelector('#scanvas');
cnv.height=SiZ;
cnvb.height=h$;
cnv.width=SiZ;
cnvb.width=w$;
let offS=Math.floor((w$-h$)/2);
let la=nearestPowerOf2(((w$*h$*4)/4)*4);
// const gl3=cnvb.getContext('2d',{colorType:'float32',alpha:true}); // 
const gl3=cnvb.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:false,
stencil:false,
depth:false,
// colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
}); // 
 const gl4=cnv.getContext('2d',{
// colorType:'float32',
alpha:true,
willReadFrequently:false,
stencil:false,
depth:false,
// colorSpace:"display-p3",
desynchronized:false,
antialias:true,
powerPreference:"high-performance",
premultipliedAlpha:true,
preserveDrawingBuffer:false
}); // 
gl3.drawImage(vvi,0,0,w$,h$,0,0,w$,h$);
// let image=flipImageData(gl3.getImageData(0,0,w$,h$));
let image=gl3.getImageData(0,0,w$,h$);
let imageData=image.data;
// let pixelData=new Uint8ClampedArray(imageData);
let pixelData=new Float32Array(imageData);
// var pixelData=new Float32Array(imageData,0,la);
FS.writeFile('/video/frame.gl',pixelData);
Module.frmOn();
setInterval(function(){
image=gl4.getImageData(0,0,SiZ,SiZ);
gl3.drawImage(image,0,offS,h$,h$,0,0,h$,h$);
let image2=gl3.getImageData(0,0,w$,h$);
imageData=image2.data;
pixelData=new Float32Array(imageData);
FS.writeFile('/video/frame.gl',pixelData);
Module.frmOn();
},16.666);
}

function regularStart(){
let SiZ=window.innerHeight;
let cnvb=document.querySelector('#scanvas');
const vvic=document.querySelector('#mvi');
let shdName=document.querySelector('#sh1').value;
document.querySelector('#path').innerHTML=shdName;
getShader(shdName,'frag.wgsl');
cnvb.height=SiZ;
cnvb.width=SiZ;
if(running==0){
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUi",null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
console.log('Starting..');
running=1;
},50);
}else{
setTimeout(function(){
let srsiz=document.querySelector('#srsiz').innerHTML;
let vsiz=document.querySelector('#vsiz').innerHTML;
Module.ccall("startWebGPUbi",null,["Number","Number","Number"],[vsiz,vsiz,srsiz]);
console.log('Starting..');
},50);
}
}
  
function getShader(pth,fname){
const ff=new XMLHttpRequest();
ff.open('GET',pth,true);
ff.responseType='arraybuffer';
document.querySelector('#stat').innerHTML='Downloading Shader';
document.querySelector('#stat').style.backgroundColor='yellow';
ff.addEventListener("load",function(){
let sarrayBuffer=ff.response;
if(sarrayBuffer){
let sfil=new Uint8ClampedArray(sarrayBuffer);
FS.unlink('/shader/'+fname);
FS.writeFile('/shader/'+fname,sfil, 'w+');
document.querySelector('#stat').innerHTML='Downloaded Shader';
document.querySelector('#stat').style.backgroundColor='blue';
}
});
ff.send(null);
}
  
let codeMessage=new BroadcastChannel('codeMessage');
let codeMessageV=new BroadcastChannel('codeMessageV');

codeMessage.addEventListener('message',event=>{
var pth2=document.querySelector('#computePathNovid').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
document.querySelector('#status').style.backgroundColor="blue";
let flDat=event.data.data;
var buffer = new ArrayBuffer(flDat.length*2);
var bufferView = new Uint16Array(buffer);
for (var i = 0; i < flDat.length; i++) {
bufferView[i] = flDat.charCodeAt(i);
}
// console.log(bufferView);
FS.unlink('/shader/shader.wgsl');
FS.writeFile('/shader/shader.wgsl',bufferView);
// document.querySelector('#startBtn').click();
setTimeout(function(){
document.querySelector('#di').click();
document.querySelector('#status').style.backgroundColor="green";
regularStart();
},50);
});

codeMessageV.addEventListener('message',event=>{
var pth2=document.querySelector('#computePath').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
document.querySelector('#status').style.backgroundColor="blue";
let flDat=event.data.data;
var buffer = new ArrayBuffer(flDat.length*2);
var bufferView = new Uint16Array(buffer);
for (var i = 0; i < flDat.length; i++) {
bufferView[i] = flDat.charCodeAt(i);
}
// console.log(bufferView);

FS.unlink('/shader/shader.wgsl');
FS.writeFile('/shader/shader.wgsl',bufferView);
// document.querySelector('#startBtn').click();

  Module.ccall('reload_shaders', null, [], []);


setTimeout(function(){
document.querySelector('#di').click();
document.querySelector('#status').style.backgroundColor="green";
regularStart();
// imageStart();
// videoStart();
},50);
});

const vsiz=document.querySelector('#vsiz');
let menuSz=parseInt(window.innerWidth*.5,10);


   //  video / image  // 
   /////////////////////

document.querySelector('#startBtn').addEventListener('click',function(){
let vsiz=document.querySelector('#vsiz').innerHTML;
var $h,$pt,slt,$ll,r$,$w,$r,$lt,$hg,$ls,lo,mv,he,wi;

const $iwid=document.querySelector('#iwid');
var mV=document.querySelector('#mvi');
var loadV=document.querySelector('#ldv');
var iV=document.querySelector('#ivi');
var loadiV=document.querySelector('#lvi');
var $vids=[];

function vids(xml){
const vparser=new DOMParser();
const htmlDocv=vparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocv.getElementsByTagName('pre')[0].getElementsByTagName('a');
$vids[0]=preList.length;
for(var i=1;i<preList.length;i++){
var txxt=preList[i].href;
let pathName = window.location.pathname;
let lastSlashIndex = pathName.lastIndexOf('/');
let basePath = pathName.substring(0, lastSlashIndex + 1);
txxt=txxt.replace('https://noahcohn.com/','');
$vids[i]=basePath+'video/'+txxt;
$vids[i]='https://noahcohn.com/video/'+txxt;
}}

function scanVideos(){
const fxhttp=new XMLHttpRequest();
fxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
vids(this);
}};
fxhttp.open('GET','video/',true);
fxhttp.send();
}

function imgs(xml){
const vparser=new DOMParser();
const htmlDocv=vparser.parseFromString(xml.responseText,'text/html');
const preList=htmlDocv.getElementsByTagName('pre')[0].getElementsByTagName('a');
$vids[0]=preList.length;
for(var i=1;i<preList.length;i++){
var txxt=preList[i].href;
let pathName = window.location.pathname;
let lastSlashIndex = pathName.lastIndexOf('/');
let basePath = pathName.substring(0, lastSlashIndex + 1);

txxt=txxt.replace('https://noahcohn.com/','');
txxt=txxt.replace('https://www.noahcohn.com/','');

$vids[i]='./pics/'+txxt;
// $vids[i]=basePath+'./pics/'+txxt;
// $vids[i]='https://noahcohn.com/pics/'+txxt;
}}

function scanImages(){
const fxhttp=new XMLHttpRequest();
fxhttp.onreadystatechange=function(){
if(this.readyState==4&&this.status==200){
imgs(this);
}};
fxhttp.open('GET','pics/',true);
fxhttp.send();
}

const media_mode = document.querySelector('#media').value;

document.querySelector('#pmhig').innerHTML=parseInt(window.innerHeight,10);
document.querySelector('#ihig').innerHTML=parseInt(window.innerHeight,10);

if(media_mode=='vid'){
document.querySelector('#mvi').load();
document.querySelector('#ldv').load();
}

const tem=document.querySelector('#tim');
const ban=document.querySelector('#menuBtn');
const sfr=document.querySelector('#slideframe');

if(media_mode=='vid'){
var adr='./intro.mp4';
wi=1280;
he=720;
}
if(media_mode=='img'){
var adr='./bezel.jpg';
wi=1920;
he=1080;
}
var hii=window.innerHeight;
document.querySelector('#ihid').innerHTML=hii;
r$=hii/he;
$w=wi*r$;
const $ihigB=document.querySelector('#ihid');
const $ihig=document.querySelector('#ihig');
$hg=hii+'px';
$ihig.innerHTML=parseInt(window.innerHeight,10);
$iwid.innerHTML=parseInt($w,10);
document.querySelector('#wrap').style.lineheight=$hg;
document.querySelector('#wrap').style.pointerEvents='auto';
document.querySelector('#isrc').innerHTML=adr;
if(media_mode=='vid'){
mV.play();
var vv=document.querySelector('#mvi');
}
if(media_mode=='img'){
var vv=document.querySelector('#ivi');
}

let lockVid;

function spKey(e){
if(e.code=='Space'){
e.preventDefault();
if(lockVid==0){lockVid=1;};
if(lockVid==1){lockVid=0;};
};
if(e.code=='KeyZ'){lockVid=1;};
if(e.code=='KeyX'){lockVid=0;};
}

const pnnl=document.body;
pnnl.addEventListener('keydown',spKey);

function loada(){
if(lockVid!=1){
// document.querySelector('#ldv').height=window.innerHeight;
// document.querySelector('#lvi').height=window.innerHeight;
// document.querySelector('#ivi').height=window.innerHeight;
// document.querySelector('#mvi').height=window.innerHeight;
if(media_mode=='vid'){
mV.addEventListener('canplay',function(){
mV.width=this.videoWidth;
mV.height=this.videoHeight;
});
loadV.addEventListener('canplay',function(){
loadV.width=this.videoWidth;
loadV.height=window.innerHeight;
document.querySelector('#wid').innerHTML=this.width; // videoWidth;
document.querySelector('#hig').innerHTML=this.height; // videoHeight;
document.querySelector('#blnnk').innerHTML=Math.max((this.width-this.height)/2.0,0);
var $sc=this.duration;
var mic=Math.round($sc*1000000);
$pt=Math.random()*mic;
$pt=$pt*1000000;
$pt=$pt/1000000;
$pt=Math.round($pt);
$pt=$pt/1000000;
document.querySelector('#idur').innerHTML=mic/1000000;
document.querySelector('#itim').innerHTML=$pt;
});
}
if(media_mode=='img'){
iV.addEventListener('load',function(){
this.width=this.naturalWidth;
this.height=this.naturalHeight;
});
loadiV.addEventListener('load',function(){
this.width=this.naturalWidth;
this.height=this.naturalHeight;
document.querySelector('#wid').innerHTML=this.width; // naturalWidth;
document.querySelector('#hig').innerHTML=this.height; // naturalHeight;
document.querySelector('#blnnk').innerHTML=Math.max((this.width-this.height)/2.0,0);
});
}
if(media_mode=='vid'){
var vide=document.querySelectorAll('video');
}
if(media_mode=='img'){
var vide=document.querySelector('#images').querySelectorAll('img');
}
document.querySelector('#pmhig').innerHTML=parseInt(window.innerHeight,10);
hii=window.innerHeight;
document.querySelector('#ihid').innerHTML=hii;
$lt=Math.round(tem.innerHTML);
var $ldt=document.querySelector('#tim').innerHTML;
$ls=$lt/1000;
$ls=$ls*1000;
$ls=Math.round($ls);
$ls=$ls/1000;
var rnum=Math.floor((Math.random()*($vids[0]-5))+5);
document.querySelector('#isrc').innerHTML=$vids[rnum];
$h=window.innerHeight;
he=document.querySelector('#hig').innerHTML;
wi=document.querySelector('#wid').innerHTML;
r$=he/$h;
$w=wi/r$;
hii=$ihigB.innerHTML;
var $hi=$h-hii;
if($hi>1){$h=$ihigB.innerHTML;$ihig.innerHTML=$h;$r=$h/he;$r=$r*1000;$r=$r/1000;$w=wi*$r;};
$hg=$h+'px';
window.scroll(0,0);
mv=vide[0].id;
lo=vide[1].id;
vide[0].id=lo;
vide[1].id=mv;
if(media_mode=='vid'){
document.querySelector('#mvi').play();
document.querySelector('#ldv').src=document.querySelector('#isrc').innerHTML;
}
$iwid.innerHTML=parseInt($w,10);
$ihig.innerHTML=parseInt(window.innerHeight,10);  
document.querySelector('#pmhig').innerHTML=parseInt(window.innerHeight,10);
document.querySelector('#circle').height=parseInt(window.innerHeight,10);
document.querySelector('#circle').width=parseInt(window.innerWidth,10);
if(media_mode=='img'){
document.querySelector('#lvi').src=document.querySelector('#isrc').innerHTML;
// document.querySelector('#ivi').height=window.innerHeight;
// document.querySelector('#lvi').height=window.innerHeight;
}
if(media_mode=='vid'){
document.querySelector('#ldv').load();
document.querySelector('#ldv').currentTime=document.querySelector('#itim').innerHTML;
// document.querySelector('#ldv').height=he;
// document.querySelector('#ldv').width=wi;
}
if(media_mode=='img'){
// document.querySelector('#lvi').height=he;
// document.querySelector('#lvi').width=wi;
}
if(he>1){
Module.resizeInputTexture(he);
Module.sizeBuffer(he);
}
document.querySelector('#di').click();
// videoStart();
// let srsiz=document.querySelector('#srsiz').innerHTML;
// let vsiz=document.querySelector('#vsiz').innerHTML;
// Module.ccall("startWebGPUbi",null,["Number","Number","Number"],[document.querySelector('#ivi').height,vsiz,srsiz]);

// imageStart();
}
setTimeout(function(){
loada();
},$ldt);
}

var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePath').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');

if(media_mode=='vid'){
scanVideos();
setTimeout(function(){
loada()},3200);
setTimeout(function(){
videoStart()},4200);
}
if(media_mode=='img'){
scanImages();
setTimeout(function(){
imageStart()},3200);
setTimeout(function(){
loada()},4200);
}
});

document.querySelector('#startBtn2').addEventListener('click',function(){
var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePathNovid').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
regularStart();
});

document.querySelector('#startBtnC').addEventListener('click',function(){
var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePath').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
setTimeout(function(){
canvasStartSize();
},2000);
});

document.querySelector('#startBtnB').addEventListener('click',function(){
var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePathBird').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
setTimeout(function(){
birdsongStart();
},3000);
});


document.querySelector('#startBtnH').addEventListener('click',function(){
var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePathBird').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
setTimeout(function(){
khz();
},3000);
});

document.querySelector('#startBtnI').addEventListener('click',function(){
var pth=document.querySelector('#path').innerHTML;
getShader(pth,'shader.wgsl');
var pth2=document.querySelector('#computePath').innerHTML;
var pth3=document.querySelector('#fragPath').innerHTML;
var pth4=document.querySelector('#vertPath').innerHTML;
getShader(pth2,'compute.wgsl');
getShader(pth3,'frag2.wgsl');
getShader(pth4,'vert.wgsl');
setTimeout(function(){
imageStart();
},1000);
});

document.querySelector('#sizeUp').addEventListener('click',function(){
Module.ccall("zoomIn");
});

document.querySelector('#sizeDown').addEventListener('click',function(){
Module.ccall("zoomOut");
});

document.querySelector('#moveDown').addEventListener('click',function(){
Module.ccall("panDown");
});

document.querySelector('#moveUp').addEventListener('click',function(){
Module.ccall("panUp");
});

document.querySelector('#moveRight').addEventListener('click',function(){
Module.ccall("panRight");
});

document.querySelector('#moveLeft').addEventListener('click',function(){
Module.ccall("panLeft");
});

setTimeout(function(){
document.querySelector('#di').click();
},250);
});












