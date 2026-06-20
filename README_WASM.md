# Wörterzerleger WebAssembly Build

This keeps the German word-breaking engine in C++ and exposes three tiny C functions to JavaScript:

- `wb_init()` loads `dictionary.txt`
- `wb_analyze(word)` returns JSON as a C string
- `wb_free(ptr)` frees that returned string

## Install Emscripten once

```bash
git clone https://github.com/emscripten-core/emsdk.git
cd emsdk
./emsdk install latest
./emsdk activate latest
source ./emsdk_env.sh
```

## Build

From this folder:

```bash
./build_wasm.sh
```

## Run locally

```bash
cd web
python3 -m http.server 8000
```

Open `http://localhost:8000`.

## Make it iPhone-friendly

Once hosted somewhere, open it in Safari on iPhone and use Share → Add to Home Screen.
