#!/usr/bin/env bash

DOC="/work/artifact-evaluation/technical-documentation.md"
TEST="/work/artifact-evaluation/manual-tests/test-01-starts-with-zero"

rm -f "$DOC"

{
  echo "# Technical Documentation: Reproduction of the WasmDiff Artifact"
  echo
  echo "## 1. Objective"
  echo
  echo "The goal of this reproduction is to set up and execute the WasmDiff artifact in a controlled Docker environment before implementing any project extension."
  echo
  echo "The initial reproduction focuses on the basic Native-to-Wasm comparison workflow:"
  echo
  echo "1. Execute the native Linux binary with a fixed input."
  echo "2. Execute the corresponding WebAssembly binary with the same input."
  echo "3. Compare exit codes and output files."
  echo "4. Document whether both executions behave identically or produce a discrepancy."
  echo
  echo "At this stage, the goal is not yet to extend WasmDiff, but to verify that the provided artifact can be built, executed, and evaluated in a reproducible way."
  echo
  echo "## 2. First Manual Reproduction Test"
  echo
  echo "### Stored Summary"
  echo
  echo "The first manual reproduction test was stored under:"
  echo
  echo "$TEST"
  echo
  echo "The stored summary is:"
  echo
  cat "$TEST/summary.txt"
} > "$DOC"

echo "Created documentation:"
echo "$DOC"
