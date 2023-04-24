cd src
emcc web.cpp main.cpp graphics.cpp -o ../webbuild/app_wasm.js -s TOTAL_MEMORY=32mb -s WASM=1 --std=c++17
cd ../