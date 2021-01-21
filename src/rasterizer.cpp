#include "rasterizer.h"

using namespace std;

namespace CGL {

RasterizerImp::RasterizerImp(PixelSampleMethod psm, LevelSampleMethod lsm,
                                       size_t width, size_t height,
                                       unsigned int sample_rate) {
  this->psm = psm;
  this->lsm = lsm;
  this->width = width;
  this->height = height;
  this->sample_rate = sample_rate;

  supersample_buffer.resize(width * height * sample_rate, Color::White);
}

// Used by rasterize_point and rasterize_line
void RasterizerImp::fill_pixel(size_t x, size_t y, Color c) {
  // TODO: Task 2: You need to change this function to fix points and lines (such as the black rectangle border in test4.svg)
  // NOTE: You are not required to implement proper supersampling for points and lines
  // It is sufficient to use the same color for all supersamples of a pixel for points and lines (not triangles)
  // NOTE: You should not use this function to fill the framebuffer pixel! In task 2, you should change this function so that
  // it renders to the internal buffer instead of the framebuffer!

  rgb_framebuffer_target[3 * (y * width + x)    ] = (unsigned char)(c.r * 255);
  rgb_framebuffer_target[3 * (y * width + x) + 1] = (unsigned char)(c.g * 255);
  rgb_framebuffer_target[3 * (y * width + x) + 2] = (unsigned char)(c.b * 255);
}

// Optional helper function to add a sample to the supersample_buffer
void RasterizerImp::fill_supersample(size_t x, size_t y, size_t s, Color c) {
  // TODO: Task 2: You may want to implement this function. Hint: our solution uses one line

};

// Rasterize a point: simple example to help you start familiarizing
// yourself with the starter code.
void RasterizerImp::rasterize_point(float x, float y, Color color) {
  // fill in the nearest pixel
  int sx = (int)floor(x);
  int sy = (int)floor(y);

  // check bounds
  if (sx < 0 || sx >= width) return;
  if (sy < 0 || sy >= height) return;

  fill_pixel(sx, sy, color);
  return;
}

// Rasterize a line.
void RasterizerImp::rasterize_line(float x0, float y0,
  float x1, float y1,
  Color color) {
  if (x0 > x1) {
    swap(x0, x1); swap(y0, y1);
  }

  float pt[] = { x0,y0 };
  float m = (y1 - y0) / (x1 - x0);
  float dpt[] = { 1,m };
  int steep = abs(m) > 1;
  if (steep) {
    dpt[0] = x1 == x0 ? 0 : 1 / abs(m);
    dpt[1] = x1 == x0 ? (y1 - y0) / abs(y1 - y0) : m / abs(m);
  }

  while (floor(pt[0]) <= floor(x1) && abs(pt[1] - y0) <= abs(y1 - y0)) {
    rasterize_point(pt[0], pt[1], color);
    pt[0] += dpt[0]; pt[1] += dpt[1];
  }
}

int RasterizerImp::line_test(Vector2D P, Vector2D P1, Vector2D P2) {
  double result = (P2.y - P1.y) * (P.x - P1.x) - (P2.x - P1.x) * (P.y - P1.y);
  if (result > 0) {
    return 1;
  }
  else if (result < 0) {
    return -1;
  }
  else {
    return 0;
  }
}

// Rasterize a triangle.
void RasterizerImp::rasterize_triangle(float x0, float y0,
                                       float x1, float y1,
                                       float x2, float y2,
                                       Color color) {
  // TODO: Task 1: Implement basic triangle rasterization here, no supersampling
  
  // TODO: Task 2: Update to implement super-sampled rasterization
  
  Vector2D P0 = { x0, y0 }, P1 = { x1, y1 }, P2 = { x2, y2 };

  int test0 = line_test(P0, P1, P2);
  int test1 = line_test(P1, P2, P0);
  int test2 = line_test(P2, P0, P1);

  for (int y = 0; y < height; y++) {
    for (int x = 0; x < width; x++) {
      Vector2D P = {x + 0.5, y + 0.5};
      if (test0 * line_test(P, P1, P2) > 0 && test1 * line_test(P, P2, P0) > 0 && test2 * line_test(P, P0, P1) > 0) {
        rasterize_point(x, y, color);
      }
    }
  }
}


void RasterizerImp::rasterize_interpolated_color_triangle(float x0, float y0, Color c0,
                                                          float x1, float y1, Color c1,
                                                          float x2, float y2, Color c2)
{
  // TODO: Task 4: Rasterize the triangle, calculating barycentric coordinates and using them to interpolate vertex colors across the triangle
  // Hint: You can reuse code from rasterize_triangle
  // Hint: Can you render a normal single colored triangle using this function?


}


void RasterizerImp::rasterize_textured_triangle(float x0, float y0, float u0, float v0,
                                                     float x1, float y1, float u1, float v1,
                                                     float x2, float y2, float u2, float v2,
                                                     Texture &tex)
{
  // TODO: Task 5: Fill in the SampleParams struct and pass it to the tex.sample function.
  // TODO: Task 6: Set the correct barycentric differentials in the SampleParams struct.
  // Hint: You can reuse code from rasterize_triangle/rasterize_interpolated_color_triangle



}

void RasterizerImp::set_sample_rate(unsigned int rate) {
  // TODO: Task 2: You may want to update this function for supersampling support
  // HINT: Different sampleing rate means you need different amount of space to store the samples

  this->sample_rate = rate;


}


void RasterizerImp::set_framebuffer_target( unsigned char* rgb_framebuffer,
                                                size_t width, size_t height )
{
  // TODO: Task 2: You may want to update this function for supersampling support

  this->width = width;
  this->height = height;
  this->rgb_framebuffer_target = rgb_framebuffer;
  


}


void RasterizerImp::clear_buffers() {
  // TODO: Task 2: You may want to update this function for supersampling support
  // Hint: With supersampling, you have an additional buffer to take care of

  std::fill(rgb_framebuffer_target, rgb_framebuffer_target + 3 * width * height, 255);



}


// This function is called at the end of rasterizing all elements of the
// SVG file.  If you use a supersample buffer to rasterize SVG elements
// for antialising, you could use this call to fill the target framebuffer
// pixels from the supersample buffer data.
void RasterizerImp::resolve_to_framebuffer() {
  // TODO: Task 2: You will likely want to update this function for supersampling support
  // NOTE: Check the rendering pipeline description in task 2 specs.


  
}

Rasterizer::~Rasterizer() { }


}// CGL
