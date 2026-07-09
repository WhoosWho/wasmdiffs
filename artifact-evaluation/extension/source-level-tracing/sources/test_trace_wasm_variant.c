#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static FILE *trace_fp = NULL;

static void trace_event(const char *event) {
  if (trace_fp) {
    fprintf(trace_fp, "%s\n", event);
    fflush(trace_fp);
  }
}

static void trace_event_i(const char *event, int value) {
  if (trace_fp) {
    fprintf(trace_fp, "%s %d\n", event, value);
    fflush(trace_fp);
  }
}

static void trace_read_byte(int index, unsigned char value) {
  if (trace_fp) {
    fprintf(trace_fp, "READ_BYTE index=%d value=0x%02x\n", index, value);
    fflush(trace_fp);
  }
}

int main(int argc, char *argv[]) {
  FILE *fp1 = NULL;
  FILE *fp2 = NULL;
  char buf[8];

  if (argc < 4) {
    fprintf(stderr, "usage: %s <input> <output> <trace>\n", argv[0]);
    return 2;
  }

  trace_fp = fopen(argv[3], "w");
  if (!trace_fp) {
    fprintf(stderr, "could not open trace file: %s\n", argv[3]);
    return 2;
  }

  trace_event("PROGRAM_START");
  fprintf(trace_fp, "ARG_INPUT_PATH %s\n", argv[1]);
  fprintf(trace_fp, "ARG_OUTPUT_PATH %s\n", argv[2]);
  fprintf(trace_fp, "ARG_TRACE_PATH %s\n", argv[3]);
  fflush(trace_fp);

  fp1 = fopen(argv[1], "r");
  if (fp1) {
    trace_event("OPEN_INPUT_OK");
  } else {
    trace_event("OPEN_INPUT_FAIL");
    trace_event_i("EXIT_CODE", 1);
    fclose(trace_fp);
    return 1;
  }

  fp2 = fopen(argv[2], "w");
  if (fp2) {
    trace_event("OPEN_OUTPUT_OK");
  } else {
    trace_event("OPEN_OUTPUT_FAIL");
    trace_event_i("EXIT_CODE", 1);
    fclose(fp1);
    fclose(trace_fp);
    return 1;
  }

  for (int i = 0; i < 8; ++i) {
    int r = fscanf(fp1, "%c", &buf[i]);

    if (r == EOF) {
      fprintf(trace_fp, "READ_EOF index=%d\n", i);
      fflush(trace_fp);

      trace_event("WRITE_ERROR");
      fprintf(fp2, "error!\n");

      trace_event_i("EXIT_CODE", 1);

      fclose(fp1);
      fclose(fp2);
      fclose(trace_fp);
      return 1;
    }

    fprintf(trace_fp, "FSCANF_RETURN index=%d value=%d\n", i, r);
    fflush(trace_fp);
    trace_read_byte(i, (unsigned char)buf[i]);
  }

  if (buf[0] == '0') {
    trace_event("BRANCH_ZERO_PREFIX");
    trace_event("WRITE_X86_ONE");
    fprintf(fp2, "I'm x86 one!\n");
  } else {
    trace_event("BRANCH_NONZERO_PREFIX");
    trace_event("WRITE_NONZERO");
    fprintf(fp2, "A non-zero value? How quaint!\n");
  }

  trace_event_i("EXIT_CODE", 0);

  fclose(fp1);
  fclose(fp2);
  fclose(trace_fp);
  return 0;
}
