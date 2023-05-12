cd webbuild
emcc ../src/web.cpp ../src/main.cpp ../src/graphics.cpp ../src/audio.cpp ^
-I ../extern/stb/ ^
-o %1.js ^
-s TOTAL_MEMORY=32mb ^
-s WASM=1 ^
--std=c++17 ^
-D WEBAL ^
-lopenal ^
-D %1 ^
--preload-file assets ^
-sNO_DISABLE_EXCEPTION_CATCHING
cd ../