#include <errno.h>
#include <inttypes.h>
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))

static void die(const char *s) {
  fprintf(stderr, "%s\n", s);
  exit(1);
}

int N = 801;
int **grid;

// you can also use this function to debug print your sandpile
static void render(char *filename) {
  int chan = 3;
  unsigned char *pixels = (unsigned char *)malloc(N * N * chan * sizeof(char));
  int stride = N * chan;
  if (!pixels)
    die("malloc failed for PNG pixel buffer");
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      unsigned char r = 0, g = 0, b = 0;
      switch (grid[i][j]) {
      case 0: // obsidian
        r = 15;
        g = 15;
        b = 25;
        break;
      case 1: // electric cyan
        r = 0;
        g = 210;
        b = 255;
        break;
      case 2: // vibrant magenta
        r = 230;
        g = 30;
        b = 130;
        break;
      case 3: // bright gold
        r = 255;
        g = 200;
        b = 0;
        break;
      default: // white
        r = 0;
        g = 0;
        b = 0;
      }

      int pid = (i * N + j) * chan;
      pixels[pid] = r;
      pixels[pid + 1] = g;
      pixels[pid + 2] = b;
    }
  }
  if (stbi_write_png(filename, N, N, chan, pixels, stride)) {
    printf("Successfully saved sandpile identity to: %s\n", filename);
  } else {
    die("Error: stb_image_write failed to save png file\n");
  }
  free(pixels);
}

static void stabilize() {
  const int width = N;
  int *lo = malloc(N * sizeof(int));
  int *hi = malloc(N * sizeof(int));
  size_t stride = (size_t)N + 2;
  int (*q)[stride] = calloc(stride, sizeof *q);
  int N2 = (N / 2) * (N / 2);
  for (int i = 0; i < N; ++i) {
    lo[i] = N;
    hi[i] = -1; // empty interval initially
    int iN2 = (i - N / 2) * (i - N / 2);
    for (int j = 0; j <= MAX(i, N - 1 - i); ++j) {
      if (iN2 + (j - N / 2) * (j - N / 2) <= N2) {
        lo[i] = MIN(lo[i], j);
        hi[i] = j;
      }
    }
  }
  while (1) {
    int changed = 0;
#pragma omp parallel for schedule(static) reduction(| : changed)
    for (int i = 0; i < width; ++i) {
      for (int j = lo[i]; j <= hi[i]; ++j) {
        int topples = grid[i][j] / 4;
        q[i + 1][j + 1] = topples;
        if (topples) {
          changed = 1;
        }
      }
    }
    if (!changed)
      break;

#pragma omp parallel for schedule(static)
    for (int i = 0; i < width; ++i) {
      for (int j = 0; j < width; ++j) {
        grid[i][j] = grid[i][j] - 4 * q[i + 1][j + 1] + q[i][j + 1] +
                     q[i + 2][j + 1] + q[i + 1][j] + q[i + 1][j + 2];
      }
    }
  }
  free(q);
  free(lo);
  free(hi);
}

void find_identity() {
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      grid[i][j] = 6;
    }
  }
  stabilize();
  for (int i = 0; i < N; ++i) {
    for (int j = 0; j < N; ++j) {
      grid[i][j] = 6 - grid[i][j];
    }
  }
  stabilize();
}

int main(int argc, char **argv) {
  if (argc < 2 || argc > 2) {
    fprintf(stderr, "Usage: %s N\n", argv[0]);
    return EXIT_FAILURE;
  }
  char *end;
  uintmax_t parsed = strtoumax(argv[1], &end, 10);
  if (errno || argv[1][0] < '0' || argv[1][0] > '9' || *end || !parsed ||
      parsed > INT32_MAX)
    die("grid size must be a positive int");
  N = (int)parsed;

  grid = (int **)malloc(N * sizeof(int *));
  for (int i = 0; i < N; ++i) {
    grid[i] = (int *)malloc(N * sizeof(int));
  }

  find_identity();

  render("identity.png");
  for (int i = 0; i < N; ++i) {
    free(grid[i]);
  }
  free(grid);
  return EXIT_SUCCESS;
}
