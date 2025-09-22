#!/usr/bin/env bash
# Simple wrapper to generate Doxygen documentation for libftpp
set -euo pipefail
ROOT="$(cd "$(dirname "$0")/.." && pwd)"
DOXYFILE="$ROOT/Doxyfile"
OUT="$ROOT/docs/doxygen"
mkdir -p "$OUT"
if ! command -v doxygen >/dev/null 2>&1; then
  echo "doxygen is not installed. Install it (brew install doxygen) and retry."
  exit 2
fi
pushd "$ROOT" >/dev/null
# force output directory to docs/doxygen
DOXYGEN_OUTPUT_DIR="$OUT"
doxygen "$DOXYFILE"
popd >/dev/null
echo "Documentation generated in $OUT"
