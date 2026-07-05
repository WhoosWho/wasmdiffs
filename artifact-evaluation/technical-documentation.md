# Technical Documentation: Reproduction of the WasmDiff Artifact

## 1. Objective

The goal of this reproduction is to set up and execute the WasmDiff artifact in a controlled Docker environment before implementing any project extension.

The initial reproduction focuses on the basic Native-to-Wasm comparison workflow:

1. Execute the native Linux binary with a fixed input.
2. Execute the corresponding WebAssembly binary with the same input.
3. Compare exit codes and output files.
4. Document whether both executions behave identically or produce a discrepancy.

At this stage, the goal is not yet to extend WasmDiff, but to verify that the provided artifact can be built, executed, and evaluated in a reproducible way.

## 2. First Manual Reproduction Test

Test ID: test-01-starts-with-zero

Input:

01234567

Native exit code: 0

Wasm exit code: 0

Native output:

I'm wasm one!

Wasm output:

I'm wasm one!

Diff result:

The diff was empty.

Interpretation:

No Native-to-Wasm discrepancy was observed for this input. Both the native binary and the WebAssembly binary produced the same output and the same exit code.

## 3. Stored Test Artifacts

The files for this test were stored under:

artifact-evaluation/manual-tests/test-01-starts-with-zero/

This directory contains the input file, native output, Wasm output, exit-code files, diff result, and summary file.

## 4. Second Manual Reproduction Test

Test ID: test-02-starts-with-one

Input:

11234567

Native exit code: 0

Wasm exit code: 0

Native output:

A non-zero value? How quaint!

Wasm output:

A non-zero value? How quaint!

Diff result:

The diff was empty.

Interpretation:

No Native-to-Wasm discrepancy was observed for this input. Both the native binary and the WebAssembly binary produced the same output and the same exit code.

The purpose of this test was to execute the alternative program branch where the first input byte is not the character `0`.

## 5. Third Manual Reproduction Test

Test ID: test-03-short-input

Input:

123

Native exit code: 1

Wasm exit code: 1

Native output:

error!

Wasm output:

error!

Diff result:

The diff was empty.

Interpretation:

No Native-to-Wasm discrepancy was observed for this input. Both the native binary and the WebAssembly binary followed the same error path, produced the same error output, and exited with the same non-zero exit code.

The purpose of this test was to verify that both executions behave identically when the input file is shorter than the eight bytes expected by the program.

## 6. Fourth Manual Reproduction Test

Test ID: test-04-special-characters

Input:

!@#$%^&*

Native exit code: 0

Wasm exit code: 0

Native output:

A non-zero value? How quaint!

Wasm output:

A non-zero value? How quaint!

Diff result:

The diff was empty.

Interpretation:

No Native-to-Wasm discrepancy was observed for this input. Both the native binary and the WebAssembly binary produced the same output and the same exit code.

The purpose of this test was to verify that both executions behave identically for an eight-byte input containing special characters.

During this test, Bash history expansion initially interpreted the exclamation mark in the input string. This was resolved by disabling history expansion with `set +H` and writing the summary using `printf`.

## 7. Fifth Manual Reproduction Test

Test ID: test-05-binary-input

Input hex:

00 01 02 03 04 05 06 07

Native exit code: 0

Wasm exit code: 0

Native output:

A non-zero value? How quaint!

Wasm output:

A non-zero value? How quaint!

Diff result:

The diff was empty.

Interpretation:

No Native-to-Wasm discrepancy was observed for this input. Both the native binary and the WebAssembly binary produced the same output and the same exit code for an input containing a null byte and other non-printable bytes.

The purpose of this test was to verify that both executions behave identically for a binary input, rather than only for printable text input.

## 8. Manual Reproduction Summary

The following table summarizes the manually reproduced test cases.

| Test ID | Input type | Native exit | Wasm exit | Output comparison | Result |
|---|---|---:|---:|---|---|
| test-01-starts-with-zero | Text input starting with `0` | 0 | 0 | Equal | No discrepancy |
| test-02-starts-with-one | Text input starting with `1` | 0 | 0 | Equal | No discrepancy |
| test-03-short-input | Input shorter than 8 bytes | 1 | 1 | Equal | No discrepancy |
| test-04-special-characters | Special-character input | 0 | 0 | Equal | No discrepancy |
| test-05-binary-input | Binary input with null byte | 0 | 0 | Equal | No discrepancy |

The basic Native-to-Wasm comparison workflow was successfully reproduced manually. For all tested inputs, the native Linux binary and the WebAssembly binary produced identical output files and identical exit codes.

No Native-to-Wasm discrepancy was observed in these manual tests.

## 9. Controlled Native-to-Wasm Discrepancy

After the initial non-discrepant manual tests, a controlled discrepancy was reproduced using the provided native variant `test-0` and the WebAssembly binary `test.wasm`.

Test ID: test-06-controlled-discrepancy

Input:

01234567

Native binary:

test-0

WebAssembly binary:

test.wasm

Native exit code: 0

Wasm exit code: 0

Native output:

I'm x86 one!

Wasm output:

I'm wasm one!

Diff result:

--- native-discrepant.out
+++ wasm-discrepant.out
@@ -1 +1 @@
-I'm x86 one!
+I'm wasm one!

Interpretation:

A controlled Native-to-Wasm discrepancy was observed. The native variant `test-0` and the WebAssembly binary `test.wasm` produced different output files for the same input, while both executions exited successfully with exit code 0.

This confirms that the comparison setup can expose output-level behavioral differences between a native executable and a WebAssembly executable.

## 10. Attempted Automated AFL++/WasmDiff Workflow

After the manual comparison tests, I attempted to reproduce the automated AFL++/WasmDiff workflow described in the repository documentation.

The fuzzing input directory was initialized with one seed file:

in/seed = 01234567

The WebAssembly runtime was configured using:

AFL_WASM_RUNTIME=/root/.wasmtime/bin/wasmtime

The automated run was started with:

AFL_NO_UI=1 AFL_SKIP_CPUFREQ=1 timeout 20s /usr/local/bin/afl-fuzz -i in -o out -J 'test.file' -- ./test @@ test.file

The fuzzer successfully started, loaded the seed input, created the output directory, and started the main forkserver and the first differential forkserver.

However, the second differential forkserver failed during initialization with a forkserver handshake error:

PROGRAM ABORT : Fork server handshake failed
Location : afl_fsrv_start_diff(), src/diff-afl-forkserver.c:847

The run also showed that the WasmDiff-specific runtime variable was internally recognized:

Successfully get the Webassembly runtime:/root/.wasmtime/bin/wasmtime

Interpretation:

The automated WasmDiff/AFL++ workflow was partially reproduced. AFL++ started correctly and initialized part of the differential execution setup. However, the second differential forkserver failed when using Wasmtime as the WebAssembly runtime.

The repository documentation expects an instrumented WasmEdge runtime. In this reproduction, Wasmtime was used instead. Therefore, the failure is likely related to the WebAssembly runtime integration and forkserver instrumentation, rather than to the manual Native-to-Wasm comparison logic itself.

As a result, the automated fuzzing workflow has not yet been fully reproduced.

## 11. Problems Encountered

During the reproduction process, several technical issues occurred:

1. Docker Desktop became unresponsive and required a host restart.
2. The original Docker build initially failed due to package download timeouts.
3. The Dockerfile required `libssl-dev` because `openssl/md5.h` was missing.
4. The AFL++ QEMU build failed, so the Dockerfile was adjusted to avoid the optional QEMU build path.
5. Wasmtime was not part of the original Docker image and had to be installed manually inside the container.
6. Because the container was started with `--rm`, manually installed tools such as Wasmtime were not persisted across container restarts.
7. Bash history expansion interpreted the exclamation mark in the special-character input. This was resolved using `set +H`.
8. The automated AFL++/WasmDiff workflow failed at the second differential forkserver when using Wasmtime instead of the instrumented WasmEdge runtime expected by the repository documentation.

## 12. Current Reproduction Status

The basic Native-to-Wasm comparison workflow was successfully reproduced manually.

The reproduction confirmed that:

- the Docker image can be built and started,
- the native Linux binaries can be executed inside the container,
- the WebAssembly binary can be executed using Wasmtime,
- both executions can be run with identical inputs,
- exit codes and output files can be compared,
- non-discrepant inputs can be documented,
- and a controlled output-level discrepancy can be reproduced using `test-0` and `test.wasm`.

The full automated AFL++/WasmDiff fuzzing workflow has not yet been fully reproduced. The run started successfully, but failed during initialization of the second differential forkserver.

## 13. Next Steps

The next technical step is to reproduce the automated workflow using the runtime expected by the repository documentation.

This likely requires building or installing the instrumented WasmEdge runtime and setting:

AFL_WASM_RUNTIME=<path-to-instrumented-wasmedge>

After that, the AFL++/WasmDiff command should be retried:

AFL_NO_UI=1 AFL_SKIP_CPUFREQ=1 timeout 20s /usr/local/bin/afl-fuzz -i in -o out -J 'test.file' -- ./test @@ test.file

If the automated workflow succeeds, the next goal is to verify whether discrepancies are stored in the expected output directory and whether the controlled discrepancy can be detected automatically.

Only after this reproduction step should project extensions be implemented.
