@group(0)@binding(0)var videoSampler: sampler;
// @group(0)@binding(1)var textureIN: texture_storage_2d <rgba32float,write>;
@group(0)@binding(2)var videoOUT: texture_2d <f32>;
@group(0)@binding(5)var<uniform> iResolution : f32;
@group(0)@binding(6)var<uniform> iFrame : u32;
@group(0)@binding(7)var<uniform> iTime : f32;
var<private> fragColor_1 : vec4<f32>;
var<private> gl_FragCoord : vec4<f32>;
var<private> iMouse : vec4<f32>;
var<private> iPosition : vec4<f32>;
fn mainImage_vf4_vf2_(fragColor: ptr<function, vec4<f32>>,fragCoord: ptr<function, vec2<f32>>) {
var col : vec3<f32>;
col = vec3<f32>(0.40000000596046447754f, 0.0f, 0.5f);
let x_24 : vec3<f32> = col;
*(fragColor) = vec4<f32>(x_24.x, x_24.y, x_24.z, 1.0f);
//  let b3_col : vec4<f32> = *(fragColor);
// textureStore(textureIN,vec2<u32>(gl_FragCoord.xy),vec4<f32>(b3_col.rgb,1.0f));
return;}
fn main_1() {
var param : vec4<f32>;
var param_1 : vec2<f32>;
let x_36 : vec4<f32> = gl_FragCoord;
param_1 = vec2<f32>(x_36.x, x_36.y);
mainImage_vf4_vf2_(&(param), &(param_1));
let x_39 : vec4<f32> = param;
// let ress:u32=u32(textureDimensions(videoOUT).x);
// fragColor_1=vec4<f32>(textureSampleBaseClampToEdge(videoOUT,videoSampler,gl_FragCoord.xy/vec2<f32>(vec2<u32>(ress,ress))));
fragColor_1=vec4<f32>(textureSampleBaseClampToEdge(videoOUT,videoSampler,gl_FragCoord.xy/vec2<f32>(vec2<f32>(iResolution,iResolution))));
// fragColor_1=vec4<f32>(textureSample(videoOUT,videoSampler,gl_FragCoord.xy/vec2<f32>(vec2<f32>(iResolution,iResolution))));
return;
}
struct main_out {
@location(0)
fragColor_1_1 : vec4<f32>,
@location(1)
iPosition_1 : vec4<f32>,
}
@fragment
fn main(@builtin(position) gl_FragCoord_param : vec4<f32>) -> main_out {
gl_FragCoord = gl_FragCoord_param;
main_1();
return main_out(fragColor_1, iPosition);
}
