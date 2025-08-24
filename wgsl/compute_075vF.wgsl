@group(0)@binding(1)var <storage,read_write>fragBuffer:vec4<f32>;
@group(0)@binding(2)var textureIN:texture_2d<f32>;
@group(0)@binding(3)var textureOUT:texture_storage_2d<rgba32float,write>;
@group(0)@binding(5)var <uniform>iTime:f32;
@group(0)@binding(6)var screenOUT:texture_storage_2d<rgba32float,write>;
@group(0)@binding(7)var resizeIN:texture_2d<f32>;
@group(0)@binding(8)var videoIN:texture_2d<f32>; // Unused in original, kept for consistency
@group(0)@binding(9)var <uniform>iZoom:vec3<i32>;
@group(0)@binding(10)var <uniform>inSize:u32;


// --- Constants ---
const DITHER_STRENGTH: f32 = 0.5 / 255.0; // Half an 8-bit step for final output dithering

// --- Color Space Conversion (Unchanged, they are correct) ---
const LINEAR_TO_SRGB_CUTOFF: f32 = 0.0031308;
const LINEAR_TO_SRGB_DIVISOR: f32 = 12.92;
const LINEAR_TO_SRGB_OFFSET: f32 = 0.055;
const LINEAR_TO_SRGB_MULTIPLIER: f32 = 1.055;
const LINEAR_TO_SRGB_INV_GAMMA: f32 = 1.0 / 2.4;

fn linearToSrgb(linear: vec4f) -> vec4f {
    let lower = linear.rgb * LINEAR_TO_SRGB_DIVISOR;
    let higher = LINEAR_TO_SRGB_MULTIPLIER * pow(linear.rgb, vec3f(LINEAR_TO_SRGB_INV_GAMMA)) - LINEAR_TO_SRGB_OFFSET;
    let srgb_rgb = select(higher, lower, linear.rgb <= vec3f(LINEAR_TO_SRGB_CUTOFF));
    return vec4f(srgb_rgb, linear.a);
}

const SRGB_TO_LINEAR_CUTOFF = 0.04045;
const SRGB_TO_LINEAR_DIVISOR_LOW: f32 = 12.92;
const SRGB_TO_LINEAR_OFFSET: f32 = 0.055;
const SRGB_TO_LINEAR_DIVISOR_HIGH: f32 = 1.055;
const SRGB_TO_LINEAR_GAMMA: f32 = 2.4;

fn srgbToLinear(srgb: vec4f) -> vec4f {
    let lower = srgb.rgb / SRGB_TO_LINEAR_DIVISOR_LOW;
    let higher = pow((srgb.rgb + SRGB_TO_LINEAR_OFFSET) / SRGB_TO_LINEAR_DIVISOR_HIGH, vec3f(SRGB_TO_LINEAR_GAMMA));
    let linear_rgb = select(higher, lower, srgb.rgb <= vec3f(SRGB_TO_LINEAR_CUTOFF));
    return vec4f(linear_rgb, srgb.a);
}

// --- IMPROVEMENT: Placeholder for true Gamut Conversion ---
// To convert to Display P3, you would apply a matrix transformation
// to the linear sRGB values. The exact matrix depends on the white point.
// This is a common one for D65 white point.
const SRGB_TO_P3_MATRIX = mat3x3f(
    vec3f(0.8224621, 0.177538, 0.0),
    vec3f(0.0331941, 0.9668058, 0.0),
    vec3f(0.0170827, 0.0723974, 0.9105199)
);
fn srgbLinearToP3Linear(c: vec3f) -> vec3f {
    // return SRGB_TO_P3_MATRIX * c;
    return c; // Keep as identity until you are ready to use it
}


// --- Interpolation & Sampling (Unchanged core logic) ---
fn cubicInterpolation(v0:vec4f, v1:vec4f, v2:vec4f, v3:vec4f, t:f32) -> vec4f {
    let a = (-0.5*v0) + (1.5*v1) - (1.5*v2) + (0.5*v3);
    let b = v0 - (2.5*v1) + (2.0*v2) - (0.5*v3);
    let c = (-0.5*v0) + (0.5*v2);
    let d = v1;
    return d + t * (c + t * (b + t * a));
}

fn hash(p: vec2f) -> f32 {
    let p3 = fract(vec3f(p.xyx) * 0.1031);
    let p3_swizzled = p3.xyz + dot(p3, p3.yzx + 19.19);
    return fract((p3_swizzled.x + p3_swizzled.y) * p3_swizzled.z);
}

// --- Post-Processing (Unchanged) ---
fn adjustContrast(color: vec4f, factor: f32) -> vec4f {
    let rgb = (color.rgb - 0.5) * factor + 0.5;
    return vec4f(rgb, color.a);
}

fn adjustBlack(color: vec4f) -> vec4f {
    var rgb = color.rgb;
    if (all(rgb < vec3f(0.04))) {
        rgb = vec3f(0.0);
    }
    return vec4f(rgb, color.a);
}


// --- IMPROVEMENT: Refactored bicubic sampling logic into a single function ---
// This reduces code duplication and makes the main loop cleaner.
// It takes a texture, coordinates, and a flag to indicate if sRGB->Linear conversion is needed.
fn resampleBicubic(tex: texture_2d<f32>, in_dims: vec2f, in_coords_f: vec2f, convert_from_srgb: bool) -> vec4f {
    let in_coords_i = vec2i(floor(in_coords_f));
    let fractions = fract(in_coords_f);

    // Unrolled 4x4 sample collection for clarity and potential performance
    var v: array<vec4f, 4>; // To hold the 4 vertically interpolated samples

    for (var j = 0; j < 4; j++) {
        let y = j - 1;
        var p: array<vec4f, 4>; // To hold the 4 horizontal samples for a row
        for (var i = 0; i < 4; i++) {
            let x = i - 1;
            let sample_coord = clamp(in_coords_i + vec2i(x, y), vec2i(0), vec2i(in_dims) - 1);
            let raw_color = textureLoad(tex, sample_coord, 0);

            if (convert_from_srgb) {
                p[i] = srgbToLinear(raw_color);
            } else {
                p[i] = raw_color;
            }
        }
        v[j] = cubicInterpolation(p[0], p[1], p[2], p[3], fractions.x);
    }

    let final_color = cubicInterpolation(v[0], v[1], v[2], v[3], fractions.y);

    // Clamp to avoid negative colors from interpolation overshoot
    return vec4f(max(final_color.rgb, vec3f(0.0)), final_color.a);
}


@compute @workgroup_size(16, 16, 1)
fn main_image(@builtin(global_invocation_id) global_id: vec3<u32>) {

    // Pass 0: Pan/Zoom `textureIN` (sRGB) to `textureOUT` (Linear Float)
    if (global_id.z == 0) {
        let out_dims = vec2f(textureDimensions(textureOUT));
        if (any(vec2f(global_id.xy) >= out_dims)) { return; }

        let out_coords = vec2f(global_id.xy);

        // --- Pan & Zoom Setup (Identical logic) ---
        let zoomFactor: f32 = max(0.01, f32(iZoom.x) / 100.0);
        let panX: f32 = (f32(iZoom.y) / 100.0) - 1.0;
        let panY: f32 = (f32(iZoom.z) / 100.0) - 1.0;

        let in_dims = vec2f(textureDimensions(textureIN));
        let center_offset = (out_coords - out_dims * 0.5);
        let zoomed_offset = center_offset / zoomFactor;
        let panned_offset = zoomed_offset + (out_dims * 0.5) + vec2f(panX, -panY) * in_dims * 0.5;
        let in_coords_f = panned_offset * (in_dims / out_dims);

        // --- IMPROVEMENT: Call the refactored helper function ---
        // Pass `true` to convert the source sRGB texture to linear space.
        let final_color = resampleBicubic(textureIN, in_dims, in_coords_f, true);

        // The result is already linear and clamped. Store it directly.
        textureStore(textureOUT, global_id.xy, final_color);

    }
    // Pass 1: Resize `resizeIN` (Linear Float) to `screenOUT` (sRGB)
    else if (global_id.z == 1) {
        let out_dims = vec2f(textureDimensions(screenOUT));
        if (any(vec2f(global_id.xy) >= out_dims)) { return; }

        let out_coords = vec2f(global_id.xy);
        let in_dims = vec2f(textureDimensions(resizeIN));
        let in_coords_f = out_coords * (in_dims / out_dims);

        // --- IMPROVEMENT: Call the refactored helper function ---
        // Pass `false` because the source `resizeIN` is already linear.
        var final_color = resampleBicubic(resizeIN, in_dims, in_coords_f, false);

        // --- Post Processing on Linear Data ---
        // This is the correct place to do adjustments
        final_color = adjustContrast(final_color, 1.01);
        final_color = adjustBlack(final_color);

        // --- Optional Gamut Conversion ---
        //final_color.rgb = srgbLinearToP3Linear(final_color.rgb);

        // --- IMPROVEMENT: Dithering applied at the end ---
        // Add dither noise just before final conversion to sRGB to break up banding.
        let dither_noise = (hash(out_coords) - 0.5) * DITHER_STRENGTH;
        final_color = vec4f(max(final_color.rgb + dither_noise, vec3f(0.0)),1.0f);

        // Convert to sRGB for display and set alpha
        final_color = linearToSrgb(final_color);
        final_color.a = 1.0;

        textureStore(screenOUT, global_id.xy, final_color);
    }
}
