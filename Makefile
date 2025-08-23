WGL_BIN_NAME = w0-010
BETA_BIN_NAME = w0-040-test

LDFLAGS = -Wl,-O3,--lto-O3,-lc++,-lc++abi,-lm,-ldl
mLDFLAGS = -Wl,-O3,--lto-O3,-lc++,-lc++abi,-lc
neLDFLAGS = -Wl,-O3,--lto-O3

wLDFLAGS = -Wl,-O3,--lto-O3,-lc++,-lc++abi,-lm,-ldl
 
oldLDFLAGS = -Wl,-O3,--lto-O3,-lc++,-lpthread,-lc++abi,-lm,-lrt,-ldl
SIMD_FLAGS = -DSIMD=AVX -msimd128 -mrelaxed-simd -mavx2
SIMD_FLAGS_old = -DSIMD=AVX -msimd128 -mavx2

STDS = -std=c++26

oldSTDS = -std=gnu17 -std=c2x -std=c++11 -std=c++14 -std=c++17 -std=gnu++17 -std=c++20 -std=gnu++20 \
	 -std=c++23 -std=gnu++23 -std=c++26 -std=gnu++26

LINK_SIMD_FLAGS = --enable-simd -msimd128 -mrelaxed-simd -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mavx2
LINK_SIMD_FLAGS_old = --enable-simd -msimd128 -msse -msse2 -msse3 -mssse3 -msse4 -msse4.1 -msse4.2 -mavx -mavx2

LINK_SIMD_FLAGSb = --enable-simd -msimd128 -mcx16 -mavxifma -mbmi -mbmi2 -mlzcnt -mavxneconvert -msse -msse2 -msse3 -mssse3 \
	 -msse4 -msse4.1 -msse4.2 -mavx -mavx2 -mpclmul -msha -mfma -mbmi2 -mpopcnt -maes -enable-fma -mavxvnni

COMMON_FLAGS = -openmp -pthread -pipe -mextended-const -mbulk-memory -matomics \
	 -sWASM_WORKERS=1 -sSHARED_MEMORY=1 -fPIC -fPIE -finline-functions -funroll-loops \
	 -m32 -fmerge-all-constants -ffast-math -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -ffp-contract=off \
	 -ftree-vectorize -fstrict-vtable-pointers -funsafe-math-optimizations -fno-math-errno \
	 -ffunction-sections -fdata-sections -fno-optimize-sibling-calls -fasynchronous-unwind-tables \
	 -Rpass=loop-vectorize -Rpass-missed=loop-vectorize -Rpass-analysis=loop-vectorize \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext 

STATIC_LINK_FLAGS = -mno-tail-call -O3 -fmerge-all-constants -ffast-math -ffp-contract=off \
	 -ftree-vectorize -fstrict-vtable-pointers -funsafe-math-optimizations -fno-math-errno \
	 -ffunction-sections -fdata-sections -fno-optimize-sibling-calls -fasynchronous-unwind-tables \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext 

BOOST_FLAGS = -sUSE_BOOST_HEADERS=1 -BOOST_UBLAS_NDEBUG=1
nBOOST_FLAGS = -BOOST_UBLAS_NDEBUG=1

GL_FLAGS = -sFULL_ES3=1 -sFULL_ES2=0 -lGL -lEGL -sLEGACY_GL_EMULATION=0 -sUSE_GLFW=0 -sUSE_REGAL=0 \
	 -sGL_SUPPORT_AUTOMATIC_ENABLE_EXTENSIONS=1 -sGL_SUPPORT_SIMPLE_ENABLE_EXTENSIONS=1 -sGL_MAX_TEMP_BUFFER_SIZE=33177600 \ 
	 -sDISABLE_DEPRECATED_FIND_EVENT_TARGET_BEHAVIOR=0 -sGL_POOL_TEMP_BUFFERS=0 \ 
	 -sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sGL_TRACK_ERRORS=1

wGL_FLAGS = -sUSE_GLFW=0 -sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 
xGL_FLAGS = -sUSE_GLFW=0 -sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 -sFULL_ES2=0 -lGL -lEGL
tGL_FLAGS = -sUSE_GLFW=0 -sUSE_WEBGL2=1 -sMIN_WEBGL_VERSION=2 -sMAX_WEBGL_VERSION=2 -sFULL_ES3=1 -sFULL_ES2=0 -lGL -lEGL

LINK_FLAGS = $(LDFLAGS) -sDEFAULT_TO_CXX=1 -sALLOW_TABLE_GROWTH=1 \
	 -sEMULATE_FUNCTION_POINTER_CASTS=0 -sABORTING_MALLOC=0 -sMALLOC=emmalloc \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=0 -sIGNORE_MISSING_MAIN=0 \
	 -sASSERTIONS=1 -jsDWEBGPU_DEBUG=1 -sTEXTDECODER=2 \
	 --use-preload-plugins --closure 0 --closureFriendly \
	 -sWASM=1 -sGLOBAL_BASE=352321536 \
	 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=3221225472 --output_eol linux -mllvm -mtune=wasm32 \
	 -rtlib=compiler-rt-mt -sAUTO_ARCHIVE_INDEXES=0 -wasm-enable-eh

LINK_FLAGSb = $(LDFLAGS) -sDEFAULT_TO_CXX=1 -sALLOW_TABLE_GROWTH=1 \
	 -sEMULATE_FUNCTION_POINTER_CASTS=0 -sABORTING_MALLOC=0 -sMALLOC=emmalloc \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=0 -sIGNORE_MISSING_MAIN=0 \
	 -sASSERTIONS=1 -jsDWEBGPU_DEBUG=1 -sTEXTDECODER=2 \
	 --use-preload-plugins --closure 0 --closureFriendly \
	 -march=broadwell -sWASM=1 \
	 -sGLOBAL_BASE=352321536 -polly -polly-position=before-vectorizer \
	 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=3221225472 --output_eol linux -mllvm -mtune=wasm32 \
	 -rtlib=compiler-rt-mt -sAUTO_ARCHIVE_INDEXES=0 -wasm-enable-eh

WEBGPU_FLAGS = -sASYNCIFY=0 -sASYNCIFY_IMPORTS=['wgpu_buffer_map_sync','navigator_gpu_request_adapter_sync','wgpu_adapter_request_device_sync'] \
	 -lmath.js -lhtml5.js -lint53.js

TIMESTAMP := $(shell date +%m%y)

NAMESTAMP := v3001test-DTE.js

NAMESTAMP := $(subst DTE,$(TIMESTAMP),$(NAMESTAMP))

cmake-build:
	mkdir -p build && cd build && emcmake cmake .. && emmake make -j8

b3_wasm_loader_lto_32_test3:
	 em++ src/vanilla/wasm_loader_32.cpp $(STDS) -m64 -mtune=wasm64 -flto -flto=thin -pipe -ffp-contract=fast \
	 -fexcess-precision=fast -fno-exceptions -fforce-enable-int128 -fopenmp-simd \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -mextended-const -O3 -fno-strict-aliasing $(SIMD_FLAGS) -sMEMORY64 -c -fno-rounding-math -fcx-limited-range \
	 -fassociative-math -freciprocal-math -fno-signed-zeros --target=wasm64 -sSTRICT=1
	 em++ -O3 -sEVAL_CTORS=0 -m64 -sMALLOC=mimalloc -sWASMFS=1 \
	 -sWASM_BIGINT=1 -mextended-const -dead_strip -mbulk-memory -matomics \
	 -pipe -DQUAD -DDOUBLE -fno-exceptions -fopenmp-simd \
	 -stdlib=libc++abi-noexcept -fno-rounding-math -fassociative-math \
	 -freciprocal-math -fno-signed-zeros \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fcx-limited-range -fno-trapping-math \
	 -ffp-contract=fast -fexcess-precision=fast -sENVIRONMENT=web \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-load-32.js -sSTRICT=1 -sSTRICT_JS=0 \
	 $(LINK_SIMD_FLAGS) -sUSE_GLFW=0 -sASSERTIONS=0 -sMEMORY64 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm64 -DNDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext  \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm64 \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1024mb -ffunction-sections -fdata-sections \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=0 -sAUTO_NATIVE_LIBRARIES=0 -sDISABLE_EXCEPTION_THROWING=1 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 \
	 -sASYNCIFY=0 -sEXPORTED_FUNCTIONS='["_main"]' -sEXPORTED_RUNTIME_METHODS='["ccall"]' \
	 --pre-js js/rSlider.js --pre-js js/slideOut.js --pre-js js/pyodide.js \
	 wasm_loader_32.o --output_eol linux -rtlib=compiler-rt --closure 0 --closureFriendly \
	 -sMODULARIZE=1 -sEXPORT_ES6=0 -sEXPORT_NAME='libload' -sDISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0

b3_wasm_loader_lto_3232_test3_thread:
	 em++ src/vanilla/wasm_loader_32.cpp $(STDS) -pthread -openmp -o wasm_loader_32.o -m32 -mtune=wasm32 -flto -flto=thin -pipe -ffp-contract=fast \
	 -fexcess-precision=fast -fno-exceptions -fforce-enable-int128 \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -O3 -fno-strict-aliasing $(SIMD_FLAGS) -c -fno-rounding-math -fcx-limited-range \
	 -fassociative-math -freciprocal-math -fno-signed-zeros --target=wasm32 -sSHARED_MEMORY=1 -sWASM_WORKERS=1 -DBOOST_HAS_THREADS=1 -DBOOST_UBLAS_USE_LONG_DOUBLE=1 -DBOOST_UBLAS_NDEBUG=1
	 em++ -O3 -sEVAL_CTORS=0 -m32 -sMALLOC=mimalloc -sWASMFS=1 -pthread -openmp -sSHARED_MEMORY=1 -sWASM_WORKERS=1 \
	 -sWASM_BIGINT=1 -dead_strip -mbulk-memory -matomics \
	 -pipe -DQUAD -DDOUBLE -fno-exceptions -DBOOST_HAS_THREADS=1 -DBOOST_UBLAS_USE_LONG_DOUBLE=1 -DBOOST_UBLAS_NDEBUG=1 \
	 -stdlib=libc++abi-noexcept -fno-rounding-math -fassociative-math \
	 -freciprocal-math -fno-signed-zeros \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fcx-limited-range -fno-trapping-math \
	 -ffp-contract=fast -fexcess-precision=fast -sENVIRONMENT=web,worker \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-load-32.js -sSTRICT=1 -sSTRICT_JS=0 \
	 $(LINK_SIMD_FLAGS) -sUSE_GLFW=0 -sASSERTIONS=1 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm32 -DNDEBUG=0 -DDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext  \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm32 \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1024mb -ffunction-sections -fdata-sections \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=0 -sAUTO_NATIVE_LIBRARIES=0 -sDISABLE_EXCEPTION_THROWING=1 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 \
	 -sASYNCIFY=0 -sEXPORTED_FUNCTIONS='["_main"]' -sEXPORTED_RUNTIME_METHODS='["ccall"]' \
	 --pre-js js/rSlider.js --pre-js js/slideOut.js --pre-js js/pyodide.js \
	 wasm_loader_32.o --output_eol linux -rtlib=compiler-rt-mt --closure 0 --closureFriendly \
	 -sMODULARIZE=1 -sEXPORT_ES6=0 -sEXPORT_NAME='libload' -sDISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0

b3_wasm_loader_lto_32_test3_thread:
	 em++ src/vanilla/wasm_loader_32.cpp $(STDS) -Wl,-O3,--lto-O3,-lc++,-lpthread,-lc++abi,-lm,-lrt,-ldl -pthread -openmp -o wasm_loader_32.o -m64 -mtune=wasm64 -flto -flto=thin -pipe -ffp-contract=fast \
	 -fexcess-precision=fast -fforce-enable-int128 -openmp \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext -sNO_DISABLE_EXCEPTION_CATCHING=1 \
	 -O3 -fno-strict-aliasing $(SIMD_FLAGS_old) -sMEMORY64 -c -fno-rounding-math -fcx-limited-range -sPTHREAD_POOL_SIZE=16 \
	 -fassociative-math -freciprocal-math -fno-signed-zeros --target=wasm64 -sSHARED_MEMORY=1 -sWASM_WORKERS=1 -DBOOST_HAS_THREADS=1 -DBOOST_UBLAS_USE_LONG_DOUBLE=1 -DBOOST_UBLAS_NDEBUG=1
	 em++ -O3 -sEVAL_CTORS=0 -Wl,-O3,--lto-O3,-lc++,-lpthread,-lc++abi,-lm,-lrt,-ldl -m64 -sMALLOC=mimalloc -sWASMFS=1 -pthread -openmp -sSHARED_MEMORY=1 -sWASM_WORKERS=1 \
	 -sWASM_BIGINT=1 -dead_strip -mbulk-memory -matomics -sEXIT_RUNTIME=1 \
	 -pipe -DQUAD -DDOUBLE -DBOOST_HAS_THREADS=1 -DBOOST_UBLAS_USE_LONG_DOUBLE=1 -DBOOST_UBLAS_NDEBUG=1 \
	 -stdlib=libc++abi -fno-rounding-math -fassociative-math \
	 -freciprocal-math -fno-signed-zeros -openmp \
	 --use-preload-plugins --closureFriendly \
	 -fno-strict-aliasing \
	 -ffast-math -ffinite-math-only -funsafe-math-optimizations -fcx-limited-range -fno-trapping-math \
	 -ffp-contract=fast -fexcess-precision=fast -sENVIRONMENT=web,worker \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-load-32.js -sSTRICT=1 -sSTRICT_JS=0 \
	 $(LINK_SIMD_FLAGS) -sUSE_GLFW=0 -sASSERTIONS=1 -sMEMORY64 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm64 -DNDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext  \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm64 \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1024mb -ffunction-sections -fdata-sections \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=0 -sAUTO_NATIVE_LIBRARIES=0 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 \
	 -sASYNCIFY=1 -sEXPORTED_FUNCTIONS='["_main"]' -sEXPORTED_RUNTIME_METHODS='["ccall"]' \
	 --pre-js js/rSlider.js --pre-js js/slideOut.js --pre-js js/pyodide.js \
	 wasm_loader_32.o --output_eol linux -rtlib=compiler-rt-mt --closure 0 --closureFriendly \
	 -sMODULARIZE=1 -sEXPORT_ES6=0 -sEXPORT_NAME='libload' -sNO_DISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0

b3_compute_mod_test5:
	 em++ src/vanilla/webgpu_mod.cpp $(STDS) -pipe -lembind -ffast-math -ffp-contract=fast \
	 -fexcess-precision=fast -fexperimental-library -fopenmp-simd \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -mextended-const -O3 -m32 -mtune=wasm32 $(STDS) $(xGL_FLAGS) -fno-strict-aliasing \
	 -I/content/RAMDRIVE2/b3/include/vanilla/ -I/content/RAMDRIVE2/b3/highway/ \
	 -I/content/RAMDRIVE2/b3/boost_1_88_0/ -c $(nBOOST_FLAGS) $(SIMD_FLAGS) -sSTRICT=1
	 em++ $(mLDFLAGS) -O3 --bind -lembind -sWASM_BIGINT=1 -sEVAL_CTORS=0 \
	 -ffast-math -mextended-const -mbulk-memory -sEXIT_RUNTIME=0 -fopenmp-simd \
	 -matomics -pipe -DQUAD -DDOUBLE -fexperimental-library \
	 -sUSE_GLFW=0 -sOFFSCREEN_FRAMEBUFFER=1 -sOFFSCREENCANVAS_SUPPORT=1 -stdlib=libc++ \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -ffp-contract=fast \
	 -fexcess-precision=fast -sENVIRONMENT=web \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-mod.js -sSTRICT_JS=0 -sSTRICT=1 -ffunction-sections -fdata-sections \
	 $(nBOOST_FLAGS) $(LINK_SIMD_FLAGS) $(xGL_FLAGS) -sASSERTIONS=0 -mtune=wasm32 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm32 -DNDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=1024mb -lmath.js -lhtml5.js -lint53.js \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=1 -sTEXTDECODER=1 -sEMBIND_STD_STRING_IS_UTF8=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=1 -sAUTO_NATIVE_LIBRARIES=1 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 \
	 -sEXPORTED_FUNCTIONS='["_main","_startWebGPUi","_startWebGPUbi","_startWebGPUC","_frmsOff","_frmsOn","_zoomIn","_zoomOut","_panRight","_panLeft","_panUp","_panDown"]' \
	 -sASYNCIFY=1 -sASYNCIFY_STACK_SIZE=81920 -sASYNCIFY_IMPORTS='["wgpu_buffer_map_sync"]' -sEXPORTED_RUNTIME_METHODS='["ccall","FS"]' -sMALLOC='emmalloc' \
	 --js-library lib/lib_webgpu.js --js-library lib/lib_demo.js -sWASMFS=1 \
	 --js-library lib/library_miniprintf.js --closure-args=--externs=lib/webgpu-closure-externs.js \
	 webgpu_mod.o --output_eol linux -rtlib=compiler-rt --closure 0 \
	 -sMODULARIZE=1 -sEXPORT_NAME='lib1ink' -sWASM_LEGACY_EXCEPTIONS=0

b3_compute_mod_test5_thread:
	 em++ src/vanilla/webgpu_thread.cpp $(STDS) -pipe -lembind -pthread -openmp -o webgpu_thread.o -ffast-math -ffp-contract=fast \
	 -fexcess-precision=fast -flto -flto=thin -fexperimental-library \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -O3 -m32 -mtune=wasm32 $(STDS) $(xGL_FLAGS) -fno-strict-aliasing \
	 -I/content/RAMDRIVE2/b3/include/vanilla/ -I/content/RAMDRIVE2/b3/highway/ -sNO_DISABLE_EXCEPTION_CATCHING=1 \
	 -I/content/RAMDRIVE2/b3/boost_1_88_0/ -c $(SIMD_FLAGS_old) -sSHARED_MEMORY=1 -sWASM_WORKERS=1
	 em++ $(mLDFLAGS) -O3 -pthread -openmp --bind -lembind -flto -flto=thin -sWASM_BIGINT=1 -sEVAL_CTORS=0 \
	 -ffast-math -mbulk-memory \
	 -matomics -pipe -DQUAD -DDOUBLE -fexperimental-library -sOFFSCREENCANVASES_TO_PTHREAD="#scanvas" -sSHARED_MEMORY=1 -sWASM_WORKERS=1 \
	 -sUSE_GLFW=0 -sOFFSCREENCANVAS_SUPPORT=1 -sOFFSCREEN_FRAMEBUFFER=0 -sPROXY_TO_PTHREAD=0 -stdlib=libc++ \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -ffp-contract=fast \
	 -fexcess-precision=fast -sENVIRONMENT=web,worker \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-mod.js -sSTRICT_JS=0 -sSTRICT=1 -ffunction-sections -fdata-sections \
	 $(LINK_SIMD_FLAGS) $(xGL_FLAGS) -sASSERTIONS=0 -mtune=wasm32 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm32 -DNDEBUG=0 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=2048mb -lmath.js -lhtml5.js -lint53.js \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=0 -sTEXTDECODER=1 -sEMBIND_STD_STRING_IS_UTF8=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=1 -sAUTO_NATIVE_LIBRARIES=1 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=1 -sIGNORE_MISSING_MAIN=0 \
	 -sEXPORTED_FUNCTIONS='["_main","_startWebGPUi","_startWebGPUbi","_startWebGPUC","_frmsOff","_frmsOn","_zoomIn","_zoomOut","_panRight","_panLeft","_panUp","_panDown"]' \
	 -sASYNCIFY=0 -sASYNCIFY_STACK_SIZE=81920 -sASYNCIFY_IMPORTS='["wgpu_buffer_map_sync"]' -sEXPORTED_RUNTIME_METHODS='["ccall","FS"]' -sMALLOC='mimalloc' \
	 --js-library lib/lib_webgpu.js --js-library lib/lib_demo.js -sWASMFS=1 \
	 --js-library lib/library_miniprintf.js --closure-args=--externs=lib/webgpu-closure-externs.js \
	 webgpu_thread.o --output_eol linux -rtlib=compiler-rt-mt --closure 0 \
	 -sMODULARIZE=1 -sEXPORT_NAME='lib1ink' -sNO_DISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0

b3_compute_mod_test64:
	 em++ src/vanilla/webgpu_mod.cpp $(STDS) -pipe -ffast-math -ffp-contract=fast \
	 -fexcess-precision=fast -m64 -mtune=wasm64 -sMEMORY64 -fexperimental-library --target=wasm64 -lembind \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -mextended-const -O3 $(STDS) $(xGL_FLAGS) -fno-strict-aliasing \
	 -I/content/RAMDRIVE2/b3/include/vanilla/ -I/content/RAMDRIVE2/b3/highway/ \
	 -I/content/RAMDRIVE2/b3/boost_1_88_0/ -c $(nBOOST_FLAGS) $(SIMD_FLAGS_old)
	 em++ $(mLDFLAGS) -O3 -m64 --bind -lembind -fexperimental-library -sMEMORY64 -sLEGALIZE_JS_FFI=1 -sEVAL_CTORS=2 \
	 -sEXIT_RUNTIME=0 -ffast-math -mextended-const -mbulk-memory \
	 -matomics -pipe -DQUAD -DDOUBLE \
	 -sDEFAULT_TO_CXX=0 -sUSE_GLFW=0 -sOFFSCREENCANVAS_SUPPORT=1 -stdlib=libc++ \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -ffp-contract=fast \
	 -fexcess-precision=fast -sENVIRONMENT=web \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-mod.js -sSTRICT_JS=0 -sSTRICT=1 -ffunction-sections -fdata-sections \
	 $(nBOOST_FLAGS) $(LINK_SIMD_FLAGS) $(xGL_FLAGS) -sASSERTIONS=0 \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm64 -DNDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext  \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm64 \
	 -sALLOW_MEMORY_GROWTH=0 -sINITIAL_MEMORY=2048mb -lmath.js -lhtml5.js -lint53.js \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=1 -sTEXTDECODER=2 -sEMBIND_STD_STRING_IS_UTF8=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=1 -sAUTO_NATIVE_LIBRARIES=1 -sDISABLE_EXCEPTION_THROWING=0 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=0 -sIGNORE_MISSING_MAIN=0 \
	 -sEXPORTED_FUNCTIONS='["_main","_startWebGPUi","_startWebGPUbi","_startWebGPUC","_frmsOff","_frmsOn","_zoomIn","_zoomOut","_panRight","_panLeft","_panUp","_panDown"]' \
	 -sEXPORTED_RUNTIME_METHODS='["ccall","FS"]' -sMALLOC='mimalloc' \
	 --js-library lib/lib_webgpu.js --js-library lib/lib_demo.js \
	 --js-library lib/library_miniprintf.js --closure-args=--externs=lib/webgpu-closure-externs.js \
	 webgpu_mod.o --output_eol linux -rtlib=compiler-rt --closure 0 \
	 -sMODULARIZE -sEXPORT_NAME='lib1ink' -sDISABLE_EXCEPTION_CATCHING=1 -sWASM_LEGACY_EXCEPTIONS=0

b3_compute_mod_test6:
	 em++ src/vanilla/webgpu_mod_b3.cpp $(STDS) -pipe -ffast-math -ffp-contract=fast \
	 -fexcess-precision=fast \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -fno-math-errno \
	 -mmutable-globals -mbulk-memory -matomics -mnontrapping-fptoint -msign-ext  \
	 -mextended-const -O3 $(STDS) $(xGL_FLAGS) -fno-strict-aliasing \
	 -I/content/RAMDRIVE2/b3/include/vanilla/ -I/content/RAMDRIVE2/b3/highway/ \
	 -I/content/RAMDRIVE2/b3/boost_1_88_0/ -c $(nBOOST_FLAGS) $(SIMD_FLAGS_old)
	 em++ $(mLDFLAGS) -O3 --bind -sLEGALIZE_JS_FFI=1 -sEVAL_CTORS=2 \
	 -sEXIT_RUNTIME=0 -ffast-math -mextended-const -mbulk-memory \
	 -matomics -pipe -DQUAD -DDOUBLE \
	 -sDEFAULT_TO_CXX=0 -sUSE_GLFW=0 -sOFFSCREENCANVAS_SUPPORT=1 -stdlib=libc++ \
	 --use-preload-plugins --closureFriendly --typed-function-references --enable-reference-types \
	 -fno-strict-aliasing \
	 -ffinite-math-only -funsafe-math-optimizations -fno-trapping-math -ffp-contract=fast \
	 -fexcess-precision=fast -sENVIRONMENT=web \
	 -DCOMPUTE -o $(BETA_BIN_NAME)-b3.js -sSTRICT_JS=0 -sSTRICT=1 -ffunction-sections -fdata-sections \
	 $(nBOOST_FLAGS) $(LINK_SIMD_FLAGS) $(xGL_FLAGS) -sASSERTIONS=0 -march=broadwell \
	 -ftree-vectorize -fstrict-vtable-pointers -fno-math-errno --target=wasm32 -DNDEBUG=1 \
	 -mmutable-globals -mnontrapping-fptoint -msign-ext  \
	 -fwhole-program-vtables -polly -polly-position=before-vectorizer -mtune=wasm32 \
	 -sALLOW_MEMORY_GROWTH=1 -sINITIAL_MEMORY=1984mb -lmath.js -lhtml5.js -lint53.js \
	 -sABORT_ON_WASM_EXCEPTIONS=0 -sEMULATE_FUNCTION_POINTER_CASTS=1 -sTEXTDECODER=2 -sEMBIND_STD_STRING_IS_UTF8=0 \
	 -sUSE_SDL=0 -sFORCE_FILESYSTEM=1 -sAUTO_JS_LIBRARIES=1 -sAUTO_NATIVE_LIBRARIES=1 -sDISABLE_EXCEPTION_THROWING=0 \
	 -sTRUSTED_TYPES=1 -sALLOW_UNIMPLEMENTED_SYSCALLS=0 -sIGNORE_MISSING_MAIN=0 \
	 -sASYNCIFY=0 -sASYNCIFY_IMPORTS='["wgpu_buffer_map_sync"]' \
	 -sEXPORTED_FUNCTIONS='["_main","_startWebGPUi","_startWebGPUbi","_startWebGPUC","_frmsOff","_frmsOn","_zoomIn","_zoomOut","_panRight","_panLeft","_panUp","_panDown"]' \
	 -sEXPORTED_RUNTIME_METHODS='["ccall","FS"]' --extern-pre-js js/gpujsx.js \
	 --js-library lib/lib_webgpu.js --js-library lib/lib_demo.js \
	 --js-library lib/library_miniprintf.js --closure-args=--externs=lib/webgpu-closure-externs.js \
	 webgpu_mod_b3.o --output_eol linux -rtlib=compiler-rt --closure 0 \
	 -sMODULARIZE -sEXPORT_NAME='lib1ink' -sDISABLE_EXCEPTION_CATCHING=1

all: b3_compute_mod_test5
	echo 'Built 1ink.us Shaders.'


