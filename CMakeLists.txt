cmake_minimum_required(VERSION 3.15)
project(b3_wasm_loader_lto_32_test3)

# set(CMAKE_C_COMPILER "emcc" CACHE STRING "" FORCE)
# set(CMAKE_CXX_COMPILER "em++" CACHE STRING "" FORCE)

# Set C++ standard to match your Makefile
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the Emscripten toolchain if not already set
set(CMAKE_TOOLCHAIN_FILE "/content/RAMDRIVE2/emsdk/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake")

# Custom flags from your Makefile
set(SIMD_FLAGS "-DSIMD=AVX -msimd128 -mrelaxed-simd -mavx2")
set(STDS "-std=c++23")
set(COMPILE_FLAGS "-m64 -mtune=wasm64 -flto -flto=thin -pipe -ffp-contract=fast -fexcess-precision=fast -fno-exceptions -fforce-enable-int128 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext -mextended-const -O3 -fno-strict-aliasing ${SIMD_FLAGS} -sMEMORY64 -fno-rounding-math -fcx-limited-range -fassociative-math -freciprocal-math -fno-signed-zeros --target=wasm64 -sSTRICT=1")
set(LINK_SIMD_FLAGS "--enable-simd -msimd128 -mrelaxed-simd -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mavx2")

set(LINK_FLAGS "-O3 -sEVAL_CTORS=0 -m64 -sMALLOC=mimalloc -sWASMFS=1 -sWASM_BIGINT=1 -mextended-const -dead_strip -mbulk-memory -matomics -pipe -DQUAD -DDOUBLE -fno-exceptions -stdlib=libc++abi-noexcept -fno-rounding-math -fassociative-math -freciprocal-math -fno-signed-zeros --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types -fno-strict-aliasing -ffast-math -ffinite-math-only -funsafe-math-optimizations -fcx-limited-range -fno-trapping-math -ffp-contract=fast -fexcess-precision=fast -sENVIRONMENT=web -DCOMPUTE -sSTRICT=1 -sSTRICT_JS=0 ${LINK_SIMD_FLAGS} -sUSE_GLFW=0 -sASSERTIONS=0 -sMEMORY64 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm64 -DNDEBUG=1 -mmutable-globals -mnontrapping-fptoint -msign-ext -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm64 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1024mb -ffunction-sections -fdata-sections -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=0 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=0 -sAUTO_NATIVE_LIBRARIES=0 -sDISABLE_EXCEPTION_THROWING=1 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 -sASYNCIFY=0 -sEXPORTED_FUNCTIONS='[\"_main\"]' -sEXPORTED_RUNTIME_METHODS='[\"ccall\"]' --pre-js ${CMAKE_SOURCE_DIR}/js/rSlider.js --pre-js ${CMAKE_SOURCE_DIR}/js/slideOut.js --pre-js ${CMAKE_SOURCE_DIR}/js/pyodide.js --output_eol linux -rtlib=compiler-rt --closure 0 --closureFriendly -sMODULARIZE=1 -sEXPORT_ES6=0 -sEXPORT_NAME='libload' -sDISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0")

# Build object file
add_library(wasm_loader_32_obj OBJECT src/vanilla/wasm_loader_32.cpp)
set_target_properties(wasm_loader_32_obj PROPERTIES
    COMPILE_FLAGS "${COMPILE_FLAGS}"
)

# Actual output: a custom command to mimic the em++ link line with all flags
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/b3_load_32.js
    DEPENDS wasm_loader_32_obj
    COMMAND em++ $<TARGET_OBJECTS:wasm_loader_32_obj> ${LINK_FLAGS} -o ${CMAKE_BINARY_DIR}/b3_load_32.js
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Building b3_wasm_loader_lto_32_test3"
)

add_custom_target(b3_wasm_loader_lto_32_test3 ALL
    DEPENDS ${CMAKE_BINARY_DIR}/b3_load_32.js
)
