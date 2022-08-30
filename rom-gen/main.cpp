#include <array>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "macro.hpp"

using color_t = byte; // gray
constexpr static const int elem_x {6};
constexpr static const int elem_y {7};

inline color_t* get_pixel(stbi_uc* data, int x, int y, int X, int comp) {
  assert(comp == STBI_grey);
  auto ptr {rcast(color_t*, data)};
  return &ptr[y * X + x];
}


void insert_bit(vector_t<color_t>& dst, int pos_x, int pos_y, int X) {
  static const std::array<color_t, elem_x * elem_y> table {
    0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0xFF,0x00,0xFF,0x00,
    0x00,0x00,0xFF,0xFF,0x00,0x00,
    0x00,0x00,0xFF,0x00,0x00,0x00,
    0x00,0xFF,0x00,0xFF,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00
  };
  FOR (y, elem_y)
  FOR (x, elem_x)
    dst[(pos_y + y) * X + (pos_x + x)] = table[y * elem_x + x];
}

void draw_grid(vector_t<color_t>& dst, int X, int Y) {
  FOR (y, Y)
  FOR (x, X)
    if ((x+elem_x-1) % elem_x == 0)
      dst[y * X + x] = 0xFF;
  FOR (y, Y)
  FOR (x, X)
    if ((y+elem_y-1) % elem_y == 0)
      dst[y * X + x] ^= 0xFF;
}

int main(int argc, char *argv[]) {
  using namespace std;
// парс аргументов
  if (argc < 3) {
    cerr << "usage: ./rom-gen \"input image\" \"output\"\n";
    return EXIT_FAILURE;
  }
  Str input(argv[1]);
  Str output(argv[2]);
// прочитать картинку
  int X, Y, comp;
  auto decoded {
    stbi_load(input.c_str(), &X, &Y, &comp, STBI_grey)
  };
  if ( !decoded) {
    cerr << "image data is not decoded\n";
    return EXIT_FAILURE;
  }
  cout << "input: " << X << "x" << Y << "\n";
  auto OX {X * elem_x+3};
  auto OY {Y * elem_y+3};
  cout << "output: " << OX << "x" << OY << "\n";

  auto encoded {vector_t<color_t>(OX * OY * sizeof(color_t))};
  FOR (y, Y)
  FOR (x, X) {
    auto src {get_pixel(decoded, x, y, X, STBI_grey)};
    if (*src > 0)
      insert_bit(encoded, x*elem_x+2, y*elem_y+2, OX);
  }

  draw_grid(encoded, OX, OY);

  stbi_write_png(output.c_str(), OX, OY, STBI_grey,
    rcast(stbi_uc*, encoded.data()), 0);
  stbi_image_free(decoded);
}
