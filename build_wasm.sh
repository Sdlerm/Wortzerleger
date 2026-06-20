#!/usr/bin/env bash
set -euo pipefail

mkdir -p web

emcc WordBreaker.cpp wasm_wrapper.cpp \
  -std=c++17 -O2 \
  -s WASM=1 \
  -s EXPORTED_FUNCTIONS='["_wb_init","_wb_analyze","_wb_free","_malloc","_free"]' \
  -s EXPORTED_RUNTIME_METHODS='["ccall","UTF8ToString"]' \
  --preload-file dictionary.txt \
  -o web/wordbreaker.js

cat <<MSG
Built:
  web/wordbreaker.js
  web/wordbreaker.wasm
  web/wordbreaker.data

Serve it with:
  cd web
  python3 -m http.server 8000

Then open:
  http://localhost:8000
MSG
