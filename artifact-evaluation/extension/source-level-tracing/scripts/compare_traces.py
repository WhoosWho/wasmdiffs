#!/usr/bin/env python3
import sys
from pathlib import Path

IGNORE_PREFIXES = (
    "ARG_INPUT_PATH",
    "ARG_OUTPUT_PATH",
    "ARG_TRACE_PATH",
)

def normalize(lines):
    result = []
    for line in lines:
        line = line.strip()
        if not line:
            continue
        if line.startswith(IGNORE_PREFIXES):
            continue
        result.append(line)
    return result

def main():
    if len(sys.argv) != 4:
        print("usage: compare_traces.py <native.trace> <wasm.trace> <output.txt>", file=sys.stderr)
        return 2

    native_path = Path(sys.argv[1])
    wasm_path = Path(sys.argv[2])
    output_path = Path(sys.argv[3])

    native = normalize(native_path.read_text(errors="replace").splitlines())
    wasm = normalize(wasm_path.read_text(errors="replace").splitlines())

    max_len = max(len(native), len(wasm))
    divergence = None

    for i in range(max_len):
        n = native[i] if i < len(native) else "<missing>"
        w = wasm[i] if i < len(wasm) else "<missing>"

        if n != w:
            divergence = (i + 1, n, w)
            break

    with output_path.open("w") as f:
        f.write("Trace comparison result\n")
        f.write("=======================\n\n")
        f.write(f"Native trace: {native_path}\n")
        f.write(f"Wasm trace:   {wasm_path}\n\n")
        f.write(f"Ignored prefixes: {', '.join(IGNORE_PREFIXES)}\n\n")

        if divergence is None:
            f.write("Result: no semantic divergence found\n")
            f.write("Explanation: After ignoring path metadata, both traces contain the same semantic events.\n")
        else:
            idx, n, w = divergence
            f.write("Result: semantic divergence found\n")
            f.write(f"First divergent semantic event: {idx}\n\n")
            f.write("Native event:\n")
            f.write(f"  {n}\n\n")
            f.write("Wasm event:\n")
            f.write(f"  {w}\n")

    if divergence is None:
        print("no semantic divergence found")
        return 0

    print(f"semantic divergence found at event {divergence[0]}")
    return 1

if __name__ == "__main__":
    raise SystemExit(main())
