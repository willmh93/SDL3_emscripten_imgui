<pre>git clone --recursive https://github.com/willmh93/SDL3_emscripten_imgui.git
cd SDL3_emscripten_imgui
emcmake cmake -B build -G Ninja
cmake --build build
emrun build/my_app.html --port 8000
</pre>
