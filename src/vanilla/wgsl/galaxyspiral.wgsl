var<private> iTime : f32;

var<private> iResolution : vec3<f32>;

var<private> fragColor_1 : vec4<f32>;

var<private> gl_FragCoord : vec4<f32>;

fn rot_vf2_f1_(p_4 : ptr<function, vec2<f32>>, a : ptr<function, f32>) {
  var c_2 : f32;
  var s_1 : f32;
  let x_183 : f32 = *(a);
  c_2 = cos(x_183);
  let x_186 : f32 = *(a);
  s_1 = sin(x_186);
  let x_188 : f32 = c_2;
  let x_190 : f32 = (*(p_4)).x;
  let x_192 : f32 = s_1;
  let x_194 : f32 = (*(p_4)).y;
  let x_197 : f32 = s_1;
  let x_200 : f32 = (*(p_4)).x;
  let x_202 : f32 = c_2;
  let x_204 : f32 = (*(p_4)).y;
  *(p_4) = vec2<f32>(((x_188 * x_190) + (x_192 * x_194)), ((-(x_197) * x_200) + (x_202 * x_204)));
  return;
}

fn toPolar_vf2_(p : ptr<function, vec2<f32>>) -> vec2<f32> {
  let x_89 : vec2<f32> = *(p);
  let x_94 : f32 = (*(p)).y;
  let x_97 : f32 = (*(p)).x;
  return vec2<f32>(length(x_89), atan2(x_94, x_97));
}

fn toRect_vf2_(p_1 : ptr<function, vec2<f32>>) -> vec2<f32> {
  let x_103 : f32 = (*(p_1)).x;
  let x_105 : f32 = (*(p_1)).y;
  let x_108 : f32 = (*(p_1)).y;
  return (vec2<f32>(cos(x_105), sin(x_108)) * x_103);
}

fn twirl_vf2_f1_f1_(p_5 : ptr<function, vec2<f32>>, a_1 : ptr<function, f32>, z : ptr<function, f32>) -> vec2<f32> {
  var pp : vec2<f32>;
  var param : vec2<f32>;
  var param_1 : vec2<f32>;
  let x_210 : vec2<f32> = *(p_5);
  param = x_210;
  let x_211 : vec2<f32> = toPolar_vf2_(&(param));
  pp = x_211;
  let x_213 : f32 = pp.x;
  let x_216 : f32 = *(a_1);
  let x_219 : f32 = pp.y;
  pp.y = (x_219 + ((x_213 * 2.5f) + x_216));
  let x_223 : vec2<f32> = pp;
  param_1 = x_223;
  let x_224 : vec2<f32> = toRect_vf2_(&(param_1));
  *(p_5) = x_224;
  let x_225 : f32 = *(z);
  let x_226 : vec2<f32> = *(p_5);
  *(p_5) = (x_226 * x_225);
  let x_228 : vec2<f32> = *(p_5);
  return x_228;
}

fn noise1_vf2_(p_3 : ptr<function, vec2<f32>>) -> f32 {
  var s : f32;
  var tm : f32;
  var a_3 : f32;
  var b : f32;
  var c_1 : f32;
  var d_1 : f32;
  s = 1.0f;
  let x_138 : vec2<f32> = *(p_3);
  let x_142 : vec2<f32> = *(p_3);
  *(p_3) = (x_142 * tanh((0.10000000149011611938f * length(x_138))));
  let x_147 : f32 = iTime;
  tm = (x_147 * 0.10000000149011611938f);
  let x_151 : f32 = (*(p_3)).x;
  a_3 = cos(x_151);
  let x_155 : f32 = (*(p_3)).y;
  b = cos(x_155);
  let x_159 : f32 = (*(p_3)).x;
  let x_162 : f32 = tm;
  c_1 = cos(((x_159 * 1.87082874774932861328f) + x_162));
  let x_167 : f32 = (*(p_3)).y;
  let x_170 : f32 = tm;
  d_1 = cos(((x_167 * 1.22474491596221923828f) + x_170));
  let x_173 : f32 = a_3;
  let x_174 : f32 = b;
  let x_176 : f32 = c_1;
  let x_178 : f32 = d_1;
  return (((x_173 * x_174) * x_176) * x_178);
}

fn galaxy_vf2_f1_f1_(p_6 : ptr<function, vec2<f32>>, a_2 : ptr<function, f32>, z_1 : ptr<function, f32>) -> f32 {
  var param_2 : vec2<f32>;
  var param_3 : f32;
  var param_4 : f32;
  var param_5 : vec2<f32>;
  let x_232 : vec2<f32> = *(p_6);
  param_2 = x_232;
  let x_234 : f32 = *(a_2);
  param_3 = x_234;
  let x_236 : f32 = *(z_1);
  param_4 = x_236;
  let x_237 : vec2<f32> = twirl_vf2_f1_f1_(&(param_2), &(param_3), &(param_4));
  *(p_6) = x_237;
  let x_239 : vec2<f32> = *(p_6);
  param_5 = x_239;
  let x_240 : f32 = noise1_vf2_(&(param_5));
  return x_240;
}

fn height_vf2_(p_8 : ptr<function, vec2<f32>>) -> f32 {
  var ang : f32;
  var l_1 : f32;
  var sp : f32;
  var s_4 : f32;
  var a_4 : f32;
  var f : f32;
  var d_2 : f32;
  var i_1 : i32;
  var g : f32;
  var param_13 : vec2<f32>;
  var param_14 : f32;
  var param_15 : f32;
  let x_402 : f32 = (*(p_8)).y;
  let x_404 : f32 = (*(p_8)).x;
  ang = atan2(x_402, x_404);
  let x_407 : vec2<f32> = *(p_8);
  l_1 = length(x_407);
  let x_411 : f32 = ang;
  let x_412 : f32 = l_1;
  let x_422 : f32 = l_1;
  sp = mix(1.0f, pow((0.75f + (0.25f * sin((2.0f * (x_411 + (x_412 * 2.5f)))))), 3.0f), tanh((6.0f * x_422)));
  s_4 = 0.0f;
  a_4 = 1.0f;
  f = 15.0f;
  d_2 = 0.0f;
  i_1 = 0i;
  loop {
    let x_437 : i32 = i_1;
    if ((x_437 < 11i)) {
    } else {
      break;
    }
    let x_441 : f32 = a_4;
    let x_442 : f32 = iTime;
    let x_445 : i32 = i_1;
    let x_450 : vec2<f32> = *(p_8);
    param_13 = x_450;
    param_14 = ((x_442 * 0.10000000149011611938f) * (0.02500000037252902985f * f32(x_445)));
    let x_453 : f32 = f;
    param_15 = x_453;
    let x_454 : f32 = galaxy_vf2_f1_f1_(&(param_13), &(param_14), &(param_15));
    g = (x_441 * x_454);
    let x_456 : f32 = g;
    let x_457 : f32 = s_4;
    s_4 = (x_457 + x_456);
    let x_460 : f32 = a_4;
    a_4 = (x_460 * 0.67082041501998901367f);
    let x_463 : f32 = f;
    f = (x_463 * 1.41421353816986083984f);
    let x_465 : f32 = a_4;
    let x_466 : f32 = d_2;
    d_2 = (x_466 + x_465);

    continuing {
      let x_468 : i32 = i_1;
      i_1 = (x_468 + 1i);
    }
  }
  let x_470 : f32 = sp;
  let x_471 : f32 = s_4;
  s_4 = (x_471 * x_470);
  let x_474 : f32 = s_4;
  let x_475 : f32 = d_2;
  let x_479 : f32 = s_4;
  let x_480 : f32 = d_2;
  let x_485 : f32 = s_4;
  let x_486 : f32 = d_2;
  let x_490 : f32 = s_4;
  let x_491 : f32 = d_2;
  let x_499 : f32 = l_1;
  let x_501 : f32 = l_1;
  return (mix((((1.0f * (-0.25f + (x_474 / x_475))) * (-0.25f + (x_479 / x_480))) + 0.25f), abs((-0.25f + (x_485 / x_486))), step(0.0f, (abs((-0.25f + (x_490 / x_491))) - 0.5f))) * exp(((-5.5f * x_499) * x_501)));
}

fn mod2_vf2_vf2_(p_2 : ptr<function, vec2<f32>>, size : ptr<function, vec2<f32>>) -> vec2<f32> {
  var c : vec2<f32>;
  let x_115 : vec2<f32> = *(p_2);
  let x_116 : vec2<f32> = *(size);
  let x_120 : vec2<f32> = *(size);
  c = floor(((x_115 + (x_116 * 0.5f)) / x_120));
  let x_123 : vec2<f32> = *(p_2);
  let x_124 : vec2<f32> = *(size);
  let x_127 : vec2<f32> = *(size);
  let x_129 : vec2<f32> = *(size);
  *(p_2) = (((x_123 + (x_124 * 0.5f)) - (x_127 * floor(((x_123 + (x_124 * 0.5f)) / x_127)))) - (x_129 * 0.5f));
  let x_132 : vec2<f32> = c;
  return x_132;
}

fn rand_vf2_(co : ptr<function, vec2<f32>>) -> f32 {
  let x_243 : vec2<f32> = *(co);
  return fract((sin(dot(x_243, vec2<f32>(12.98980045318603515625f, 78.233001708984375f))) * 43758.546875f));
}

fn stars_vf2_(p_7 : ptr<function, vec2<f32>>) -> vec3<f32> {
  var l : f32;
  var pp_1 : vec2<f32>;
  var param_6 : vec2<f32>;
  var param_7 : vec2<f32>;
  var sz : f32;
  var s_3 : vec3<f32>;
  var i : i32;
  var param_8 : vec2<f32>;
  var param_9 : f32;
  var ip : vec2<f32>;
  var n : vec2<f32>;
  var param_10 : vec2<f32>;
  var param_11 : vec2<f32>;
  var r : f32;
  var param_12 : vec2<f32>;
  var o : vec2<f32>;
  let x_313 : vec2<f32> = *(p_7);
  l = length(x_313);
  let x_317 : vec2<f32> = *(p_7);
  param_6 = x_317;
  let x_318 : vec2<f32> = toPolar_vf2_(&(param_6));
  pp_1 = x_318;
  let x_320 : f32 = pp_1.x;
  let x_325 : f32 = pp_1.x;
  pp_1.x = (x_325 / ((1.0f + length(x_320)) * 0.5f));
  let x_329 : vec2<f32> = pp_1;
  param_7 = x_329;
  let x_330 : vec2<f32> = toRect_vf2_(&(param_7));
  *(p_7) = x_330;
  sz = 0.00749999983236193657f;
  s_3 = vec3<f32>(10000.0f, 10000.0f, 10000.0f);
  i = 0i;
  loop {
    let x_345 : i32 = i;
    if ((x_345 < 3i)) {
    } else {
      break;
    }
    let x_349 : vec2<f32> = *(p_7);
    param_8 = x_349;
    param_9 = 0.5f;
    rot_vf2_f1_(&(param_8), &(param_9));
    let x_352 : vec2<f32> = param_8;
    *(p_7) = x_352;
    let x_354 : vec2<f32> = *(p_7);
    ip = x_354;
    let x_356 : f32 = sz;
    let x_359 : vec2<f32> = ip;
    param_10 = x_359;
    param_11 = vec2<f32>(x_356, x_356);
    let x_361 : vec2<f32> = mod2_vf2_vf2_(&(param_10), &(param_11));
    let x_362 : vec2<f32> = param_10;
    ip = x_362;
    n = x_361;
    let x_365 : vec2<f32> = n;
    param_12 = x_365;
    let x_366 : f32 = rand_vf2_(&(param_12));
    r = x_366;
    let x_369 : f32 = r;
    let x_370 : f32 = r;
    o = (vec2<f32>(-1.0f, -1.0f) + (vec2<f32>(x_369, fract((x_370 * 1000.0f))) * 2.0f));
    let x_379 : f32 = s_3.x;
    let x_380 : vec2<f32> = ip;
    let x_382 : f32 = sz;
    let x_384 : vec2<f32> = o;
    s_3.x = min(x_379, length((x_380 - (x_384 * (0.25f * x_382)))));
    let x_390 : vec2<f32> = n;
    let x_391 : vec2<f32> = (x_390 * 0.10000000149011611938f);
    let x_392 : vec3<f32> = s_3;
    s_3 = vec3<f32>(x_392.x, x_391.x, x_391.y);

    continuing {
      let x_394 : i32 = i;
      i = (x_394 + 1i);
    }
  }
  let x_397 : vec3<f32> = s_3;
  return x_397;
}

fn normal_vf2_(p_9 : ptr<function, vec2<f32>>) -> vec3<f32> {
  var eps : vec2<f32>;
  var n_1 : vec3<f32>;
  var param_16 : vec2<f32>;
  var param_17 : vec2<f32>;
  var param_18 : vec2<f32>;
  var param_19 : vec2<f32>;
  eps = vec2<f32>(0.00012500000593718141f, 0.0f);
  let x_511 : vec2<f32> = *(p_9);
  let x_512 : vec2<f32> = eps;
  param_16 = (x_511 - x_512);
  let x_515 : f32 = height_vf2_(&(param_16));
  let x_516 : vec2<f32> = *(p_9);
  let x_517 : vec2<f32> = eps;
  param_17 = (x_516 + x_517);
  let x_520 : f32 = height_vf2_(&(param_17));
  n_1.x = (x_515 - x_520);
  let x_524 : f32 = eps.x;
  n_1.y = (2.0f * x_524);
  let x_527 : vec2<f32> = *(p_9);
  let x_528 : vec2<f32> = eps;
  param_18 = (x_527 - vec2<f32>(x_528.y, x_528.x));
  let x_532 : f32 = height_vf2_(&(param_18));
  let x_533 : vec2<f32> = *(p_9);
  let x_534 : vec2<f32> = eps;
  param_19 = (x_533 + vec2<f32>(x_534.y, x_534.x));
  let x_538 : f32 = height_vf2_(&(param_19));
  n_1.z = (x_532 - x_538);
  let x_542 : vec3<f32> = n_1;
  return normalize(x_542);
}

const x_611 = vec3<f32>(0.0f, 0.0f, 0.0f);

const x_624 = vec3<f32>(0.5f, 0.5f, 0.5f);

fn galaxy_vf2_vf3_vf3_f1_(p_10 : ptr<function, vec2<f32>>, ro_1 : ptr<function, vec3<f32>>, rd_1 : ptr<function, vec3<f32>>, d : ptr<function, f32>) -> vec3<f32> {
  var param_20 : vec2<f32>;
  var param_21 : f32;
  var h : f32;
  var param_22 : vec2<f32>;
  var s_5 : vec3<f32>;
  var param_23 : vec2<f32>;
  var th : f32;
  var n_2 : vec3<f32>;
  var param_24 : vec2<f32>;
  var p3 : vec3<f32>;
  var lh : f32;
  var lp1 : vec3<f32>;
  var ld1 : vec3<f32>;
  var lp2 : vec3<f32>;
  var ld2 : vec3<f32>;
  var l_2 : f32;
  var tl : f32;
  var diff1 : f32;
  var diff2 : f32;
  var col_1 : vec3<f32>;
  var sr : f32;
  var param_25 : vec2<f32>;
  var si : f32;
  var scol : vec3<f32>;
  var ddust : f32;
  var t : f32;
  let x_546 : f32 = iTime;
  let x_550 : vec2<f32> = *(p_10);
  param_20 = x_550;
  param_21 = (0.5f * (x_546 * 0.10000000149011611938f));
  rot_vf2_f1_(&(param_20), &(param_21));
  let x_553 : vec2<f32> = param_20;
  *(p_10) = x_553;
  let x_556 : vec2<f32> = *(p_10);
  param_22 = x_556;
  let x_557 : f32 = height_vf2_(&(param_22));
  h = x_557;
  let x_560 : vec2<f32> = *(p_10);
  param_23 = x_560;
  let x_561 : vec3<f32> = stars_vf2_(&(param_23));
  s_5 = x_561;
  let x_563 : f32 = h;
  th = tanh(x_563);
  let x_567 : vec2<f32> = *(p_10);
  param_24 = x_567;
  let x_568 : vec3<f32> = normal_vf2_(&(param_24));
  n_2 = x_568;
  let x_571 : f32 = (*(p_10)).x;
  let x_572 : f32 = th;
  let x_574 : f32 = (*(p_10)).y;
  p3 = vec3<f32>(x_571, x_572, x_574);
  lh = 0.5f;
  let x_579 : f32 = lh;
  lp1 = vec3<f32>(-0.0f, x_579, 0.0f);
  let x_582 : vec3<f32> = lp1;
  let x_583 : vec3<f32> = p3;
  ld1 = normalize((x_582 - x_583));
  let x_587 : f32 = lh;
  lp2 = vec3<f32>(0.0f, x_587, 0.0f);
  let x_590 : vec3<f32> = lp2;
  let x_591 : vec3<f32> = p3;
  ld2 = normalize((x_590 - x_591));
  let x_595 : vec2<f32> = *(p_10);
  l_2 = length(x_595);
  let x_598 : f32 = l_2;
  tl = tanh(x_598);
  let x_601 : vec3<f32> = ld1;
  let x_602 : vec3<f32> = n_2;
  diff1 = max(dot(x_601, x_602), 0.0f);
  let x_606 : vec3<f32> = ld2;
  let x_607 : vec3<f32> = n_2;
  diff2 = max(dot(x_606, x_607), 0.0f);
  col_1 = x_611;
  let x_613 : f32 = h;
  let x_615 : vec3<f32> = col_1;
  col_1 = (x_615 + (vec3<f32>(0.5f, 0.5f, 0.75f) * x_613));
  let x_617 : f32 = diff2;
  let x_620 : f32 = (0.25f * pow(x_617, 4.0f));
  let x_621 : vec3<f32> = col_1;
  col_1 = (x_621 + vec3<f32>(x_620, x_620, x_620));
  let x_625 : f32 = h;
  let x_628 : f32 = n_2.y;
  let x_635 : f32 = tl;
  let x_636 : f32 = (1.25f * x_635);
  let x_641 : vec3<f32> = col_1;
  col_1 = (x_641 + pow((x_624 * x_625), (mix(vec3<f32>(0.5f, 1.0f, 1.5f), vec3<f32>(1.5f, 1.0f, 0.5f), vec3<f32>(x_636, x_636, x_636)) * (x_628 * 1.75f))));
  let x_645 : vec3<f32> = s_5;
  param_25 = vec2<f32>(x_645.y, x_645.z);
  let x_647 : f32 = rand_vf2_(&(param_25));
  sr = x_647;
  let x_649 : f32 = th;
  let x_650 : f32 = sr;
  si = (pow((x_649 * x_650), 0.25f) * 0.00100000004749745131f);
  let x_656 : f32 = sr;
  let x_660 : f32 = l_2;
  let x_662 : f32 = l_2;
  let x_666 : f32 = si;
  let x_668 : f32 = s_5.x;
  let x_675 : f32 = sr;
  let x_677 : f32 = (x_675 * 0.60000002384185791016f);
  scol = (mix(vec3<f32>(0.5f, 0.75f, 1.0f), vec3<f32>(1.0f, 0.75f, 0.5f), vec3<f32>(x_677, x_677, x_677)) * (((x_656 * 5.0f) * exp(((-2.5f * x_660) * x_662))) * tanh(pow((x_666 / x_668), 2.5f))));
  let x_681 : vec3<f32> = scol;
  scol = clamp(x_681, vec3<f32>(0.0f, 0.0f, 0.0f), vec3<f32>(1.0f, 1.0f, 1.0f));
  let x_685 : vec3<f32> = scol;
  let x_688 : f32 = n_2.y;
  let x_692 : vec3<f32> = col_1;
  col_1 = (x_692 + (x_685 * smoothstep(0.0f, 0.34999999403953552246f, (1.0f - x_688))));
  let x_695 : f32 = h;
  let x_697 : f32 = (*(ro_1)).y;
  let x_700 : f32 = (*(rd_1)).y;
  ddust = ((x_695 - x_697) / x_700);
  let x_702 : f32 = ddust;
  let x_703 : f32 = *(d);
  if ((x_702 < x_703)) {
    let x_708 : f32 = *(d);
    let x_709 : f32 = ddust;
    t = (x_708 - x_709);
    let x_716 : f32 = t;
    let x_721 : vec3<f32> = col_1;
    col_1 = (x_721 + (vec3<f32>(0.69999998807907104492f, 0.62999999523162841797f, 0.52499997615814208984f) * (1.0f - exp((-2.0f * x_716)))));
  }
  let x_723 : vec3<f32> = col_1;
  return x_723;
}

const x_280 = vec2<f32>(-1.0f, -1.0f);

fn raySphere_vf3_vf3_vf3_f1_(ro : ptr<function, vec3<f32>>, rd : ptr<function, vec3<f32>>, center : ptr<function, vec3<f32>>, radius : ptr<function, f32>) -> vec2<f32> {
  var m : vec3<f32>;
  var b_1 : f32;
  var c_3 : f32;
  var discr : f32;
  var normalMultiplier : f32;
  var s_2 : f32;
  var t0 : f32;
  var t1 : f32;
  let x_255 : vec3<f32> = *(ro);
  let x_256 : vec3<f32> = *(center);
  m = (x_255 - x_256);
  let x_259 : vec3<f32> = m;
  let x_260 : vec3<f32> = *(rd);
  b_1 = dot(x_259, x_260);
  let x_263 : vec3<f32> = m;
  let x_264 : vec3<f32> = m;
  let x_266 : f32 = *(radius);
  let x_267 : f32 = *(radius);
  c_3 = (dot(x_263, x_264) - (x_266 * x_267));
  let x_270 : f32 = c_3;
  let x_274 : f32 = b_1;
  if (((x_270 > 0.0f) & (x_274 > 0.0f))) {
    return x_280;
  }
  let x_283 : f32 = b_1;
  let x_284 : f32 = b_1;
  let x_286 : f32 = c_3;
  discr = ((x_283 * x_284) - x_286);
  let x_288 : f32 = discr;
  if ((x_288 < 0.0f)) {
    return x_280;
  }
  normalMultiplier = 1.0f;
  let x_295 : f32 = discr;
  s_2 = sqrt(x_295);
  let x_298 : f32 = b_1;
  let x_300 : f32 = s_2;
  t0 = (-(x_298) - x_300);
  let x_303 : f32 = b_1;
  let x_305 : f32 = s_2;
  t1 = (-(x_303) + x_305);
  let x_307 : f32 = t0;
  let x_308 : f32 = t1;
  return vec2<f32>(x_307, x_308);
}

fn render_vf3_vf3_(ro_2 : ptr<function, vec3<f32>>, rd_2 : ptr<function, vec3<f32>>) -> vec3<f32> {
  var dgalaxy : f32;
  var col_2 : vec3<f32>;
  var p_11 : vec3<f32>;
  var param_26 : vec2<f32>;
  var param_27 : vec3<f32>;
  var param_28 : vec3<f32>;
  var param_29 : f32;
  var cgalaxy : vec2<f32>;
  var param_30 : vec3<f32>;
  var param_31 : vec3<f32>;
  var param_32 : vec3<f32>;
  var param_33 : f32;
  var t0_1 : f32;
  var t1_1 : f32;
  var t_1 : f32;
  var x_769 : bool;
  var x_770 : bool;
  let x_728 : f32 = (*(ro_2)).y;
  let x_731 : f32 = (*(rd_2)).y;
  dgalaxy = ((0.0f - x_728) / x_731);
  col_2 = x_611;
  let x_734 : f32 = dgalaxy;
  if ((x_734 > 0.0f)) {
    col_2 = x_624;
    let x_739 : vec3<f32> = *(ro_2);
    let x_740 : f32 = dgalaxy;
    let x_741 : vec3<f32> = *(rd_2);
    p_11 = (x_739 + (x_741 * x_740));
    let x_745 : vec3<f32> = p_11;
    param_26 = vec2<f32>(x_745.x, x_745.z);
    let x_748 : vec3<f32> = *(ro_2);
    param_27 = x_748;
    let x_750 : vec3<f32> = *(rd_2);
    param_28 = x_750;
    let x_752 : f32 = dgalaxy;
    param_29 = x_752;
    let x_753 : vec3<f32> = galaxy_vf2_vf3_vf3_f1_(&(param_26), &(param_27), &(param_28), &(param_29));
    col_2 = x_753;
  }
  let x_757 : vec3<f32> = *(ro_2);
  param_30 = x_757;
  let x_759 : vec3<f32> = *(rd_2);
  param_31 = x_759;
  param_32 = x_611;
  param_33 = 0.125f;
  let x_762 : vec2<f32> = raySphere_vf3_vf3_vf3_f1_(&(param_30), &(param_31), &(param_32), &(param_33));
  cgalaxy = x_762;
  let x_763 : f32 = dgalaxy;
  let x_764 : bool = (x_763 > 0.0f);
  x_770 = x_764;
  if (x_764) {
    let x_768 : f32 = cgalaxy.x;
    x_769 = (x_768 > 0.0f);
    x_770 = x_769;
  }
  if (x_770) {
    let x_774 : f32 = dgalaxy;
    let x_776 : f32 = cgalaxy.x;
    t0_1 = max((x_774 - x_776), 0.0f);
    let x_781 : f32 = cgalaxy.y;
    let x_783 : f32 = cgalaxy.x;
    t1_1 = (x_781 - x_783);
    let x_786 : f32 = t0_1;
    let x_787 : f32 = t1_1;
    t_1 = min(x_786, x_787);
  } else {
    let x_791 : f32 = cgalaxy.x;
    let x_793 : f32 = cgalaxy.y;
    if ((x_791 < x_793)) {
      let x_798 : f32 = cgalaxy.y;
      let x_800 : f32 = cgalaxy.x;
      t_1 = (x_798 - x_800);
    }
  }
  let x_806 : f32 = t_1;
  let x_811 : vec3<f32> = col_2;
  col_2 = (x_811 + (vec3<f32>(1.70000004768371582031f, 1.52999997138977050781f, 1.27499997615814208984f) * (1.0f - exp((-1.0f * x_806)))));
  let x_813 : vec3<f32> = col_2;
  return x_813;
}

fn postProcess_vf3_vf2_(col : ptr<function, vec3<f32>>, q : ptr<function, vec2<f32>>) -> vec3<f32> {
  let x_816 : vec3<f32> = *(col);
  *(col) = pow(clamp(x_816, vec3<f32>(0.0f, 0.0f, 0.0f), vec3<f32>(1.0f, 1.0f, 1.0f)), vec3<f32>(0.75f, 0.75f, 0.75f));
  let x_822 : vec3<f32> = *(col);
  let x_825 : vec3<f32> = *(col);
  let x_827 : vec3<f32> = *(col);
  let x_829 : vec3<f32> = *(col);
  *(col) = ((x_822 * 0.60000002384185791016f) + (((x_825 * 0.40000000596046447754f) * x_827) * (vec3<f32>(3.0f, 3.0f, 3.0f) - (x_829 * 2.0f))));
  let x_835 : vec3<f32> = *(col);
  let x_836 : vec3<f32> = *(col);
  let x_839 : f32 = dot(x_836, vec3<f32>(0.33000001311302185059f, 0.33000001311302185059f, 0.33000001311302185059f));
  *(col) = mix(x_835, vec3<f32>(x_839, x_839, x_839), vec3<f32>(-0.40000000596046447754f, -0.40000000596046447754f, -0.40000000596046447754f));
  let x_846 : f32 = (*(q)).x;
  let x_849 : f32 = (*(q)).y;
  let x_852 : f32 = (*(q)).x;
  let x_856 : f32 = (*(q)).y;
  let x_862 : vec3<f32> = *(col);
  *(col) = (x_862 * (0.5f + (0.5f * pow(((((19.0f * x_846) * x_849) * (1.0f - x_852)) * (1.0f - x_856)), 0.69999998807907104492f))));
  let x_864 : vec3<f32> = *(col);
  return x_864;
}

fn mainImage_vf4_vf2_(fragColor : ptr<function, vec4<f32>>, fragCoord : ptr<function, vec2<f32>>) {
  var q_1 : vec2<f32>;
  var p_12 : vec2<f32>;
  var ro_3 : vec3<f32>;
  var la : vec3<f32>;
  var up : vec3<f32>;
  var ww : vec3<f32>;
  var uu : vec3<f32>;
  var vv : vec3<f32>;
  var rd_3 : vec3<f32>;
  var col_3 : vec3<f32>;
  var param_34 : vec3<f32>;
  var param_35 : vec3<f32>;
  var param_36 : vec3<f32>;
  var param_37 : vec2<f32>;
  let x_868 : vec2<f32> = *(fragCoord);
  let x_871 : vec3<f32> = iResolution;
  q_1 = (x_868 / vec2<f32>(x_871.x, x_871.y));
  let x_875 : vec2<f32> = q_1;
  p_12 = (vec2<f32>(-1.0f, -1.0f) + (x_875 * 2.0f));
  let x_880 : f32 = iResolution.x;
  let x_882 : f32 = iResolution.y;
  let x_885 : f32 = p_12.x;
  p_12.x = (x_885 * (x_880 / x_882));
  ro_3 = vec3<f32>(0.0f, 0.52499997615814208984f, 1.5f);
  la = x_611;
  up = vec3<f32>(-0.5f, 1.0f, 0.0f);
  let x_895 : vec3<f32> = la;
  let x_896 : vec3<f32> = ro_3;
  ww = normalize((x_895 - x_896));
  let x_900 : vec3<f32> = up;
  let x_901 : vec3<f32> = ww;
  uu = normalize(cross(x_900, x_901));
  let x_905 : vec3<f32> = ww;
  let x_906 : vec3<f32> = uu;
  vv = normalize(cross(x_905, x_906));
  let x_911 : f32 = p_12.x;
  let x_912 : vec3<f32> = uu;
  let x_915 : f32 = p_12.y;
  let x_916 : vec3<f32> = vv;
  let x_919 : vec3<f32> = ww;
  rd_3 = normalize((((x_912 * x_911) + (x_916 * x_915)) + (x_919 * 2.5f)));
  let x_925 : vec3<f32> = ro_3;
  param_34 = x_925;
  let x_927 : vec3<f32> = rd_3;
  param_35 = x_927;
  let x_928 : vec3<f32> = render_vf3_vf3_(&(param_34), &(param_35));
  col_3 = x_928;
  let x_930 : vec3<f32> = col_3;
  param_36 = x_930;
  let x_932 : vec2<f32> = q_1;
  param_37 = x_932;
  let x_933 : vec3<f32> = postProcess_vf3_vf2_(&(param_36), &(param_37));
  col_3 = x_933;
  let x_934 : vec3<f32> = col_3;
  *(fragColor) = vec4<f32>(x_934.x, x_934.y, x_934.z, 1.0f);
  return;
}

fn main_1() {
  var param_38 : vec4<f32>;
  var param_39 : vec2<f32>;
  let x_945 : vec4<f32> = gl_FragCoord;
  param_39 = vec2<f32>(x_945.x, x_945.y);
  mainImage_vf4_vf2_(&(param_38), &(param_39));
  let x_948 : vec4<f32> = param_38;
  fragColor_1 = x_948;
  return;
}

struct main_out {
  @location(0)
  fragColor_1_1 : vec4<f32>,
}

@fragment
fn main(@builtin(position) gl_FragCoord_param : vec4<f32>) -> main_out {
  gl_FragCoord = gl_FragCoord_param;
  main_1();
  return main_out(fragColor_1);
}
