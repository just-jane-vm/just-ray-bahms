// main.cpp
//
// Minimal FreeType + Cairo example (Cairo user-font) that renders text to a
// PNG.
// - Loads a .ttf/.otf with FreeType
// - Provides glyph outlines to Cairo via cairo_user_font_face
// - Draws colored text at a chosen font size
//
// Build (example):
//   g++ -std=c++17 main.cpp -o freetype_cairo_text $(pkg-config --cflags --libs
//   cairo freetype2)
//
// Run:
//   ./freetype_cairo_text /path/to/font.ttf out.png "Hello, FreeType + Cairo!"
//   72 0.1 0.2 0.8 1.0
//
// Notes:
// - This demo maps UTF-8 bytes directly -> glyphs (i.e., effectively ASCII /
// single-byte).
//   For real Unicode shaping, use HarfBuzz + FreeType and feed glyph
//   ids/positions to Cairo.

#include <cairo/cairo.h>

#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_OUTLINE_H

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

static cairo_user_data_key_t g_ft_cairo_font_key;

// Holds FreeType library + face for the lifetime of the Cairo font face usage.
struct FtCairoFont {
  FT_Library ftLib = nullptr;
  FT_Face ftFace = nullptr;

  explicit FtCairoFont(const char *fontPath) {
    if (FT_Init_FreeType(&ftLib) != 0) {
      throw std::runtime_error("FT_Init_FreeType failed");
    }
    if (FT_New_Face(ftLib, fontPath, 0, &ftFace) != 0) {
      throw std::runtime_error(
          "FT_New_Face failed (font not found or invalid)");
    }
    // Prefer Unicode charmap
    FT_Select_Charmap(ftFace, FT_ENCODING_UNICODE);
  }

  ~FtCairoFont() {
    if (ftFace)
      FT_Done_Face(ftFace);
    if (ftLib)
      FT_Done_FreeType(ftLib);
  }

  FtCairoFont(const FtCairoFont &) = delete;
  FtCairoFont &operator=(const FtCairoFont &) = delete;
};

// --- FreeType outline -> Cairo path callbacks ---
// FreeType coordinates are in 26.6 fixed point, y-up.
// We'll convert to doubles and flip Y to match Cairo's y-down user space.
static int move_to_cb(const FT_Vector *to, void *user) {
  cairo_t *cr = static_cast<cairo_t *>(user);
  cairo_move_to(cr, to->x / 64.0, -to->y / 64.0);
  return 0;
}

static int line_to_cb(const FT_Vector *to, void *user) {
  cairo_t *cr = static_cast<cairo_t *>(user);
  cairo_line_to(cr, to->x / 64.0, -to->y / 64.0);
  return 0;
}

// Quadratic -> cubic conversion
static int conic_to_cb(const FT_Vector *control, const FT_Vector *to,
                       void *user) {
  cairo_t *cr = static_cast<cairo_t *>(user);

  double x0, y0;
  cairo_get_current_point(cr, &x0, &y0);

  double x1 = control->x / 64.0, y1 = -control->y / 64.0;
  double x2 = to->x / 64.0, y2 = -to->y / 64.0;

  // quadratic (P0,P1,P2) -> cubic (P0, C1, C2, P2)
  double c1x = x0 + (2.0 / 3.0) * (x1 - x0);
  double c1y = y0 + (2.0 / 3.0) * (y1 - y0);
  double c2x = x2 + (2.0 / 3.0) * (x1 - x2);
  double c2y = y2 + (2.0 / 3.0) * (y1 - y2);

  cairo_curve_to(cr, c1x, c1y, c2x, c2y, x2, y2);
  return 0;
}

static int cubic_to_cb(const FT_Vector *c1, const FT_Vector *c2,
                       const FT_Vector *to, void *user) {
  cairo_t *cr = static_cast<cairo_t *>(user);
  cairo_curve_to(cr, c1->x / 64.0, -c1->y / 64.0, c2->x / 64.0, -c2->y / 64.0,
                 to->x / 64.0, -to->y / 64.0);
  return 0;
}

// Optional init callback; can provide font extents.
// We'll leave it minimal.
static cairo_status_t user_font_init(cairo_scaled_font_t * /*scaled_font*/,
                                     cairo_t * /*cr*/,
                                     cairo_font_extents_t * /*extents*/) {
  return CAIRO_STATUS_SUCCESS;
}

// Cairo asks us to "render" a glyph by defining its shape on the cairo_t.
// Cairo will fill/stroke it with current source.
static cairo_status_t user_font_render_glyph(cairo_scaled_font_t *scaled_font,
                                             unsigned long glyph_index,
                                             cairo_t *cr,
                                             cairo_text_extents_t *extents) {
  cairo_font_face_t *ff = cairo_scaled_font_get_font_face(scaled_font);
  auto *ctx = static_cast<FtCairoFont *>(
      cairo_font_face_get_user_data(ff, &g_ft_cairo_font_key));
  if (!ctx)
    return CAIRO_STATUS_USER_FONT_ERROR;

  FT_Face face = ctx->ftFace;

  if (FT_Load_Glyph(face, (FT_UInt)glyph_index, FT_LOAD_NO_BITMAP) != 0)
    return CAIRO_STATUS_USER_FONT_ERROR;

  if (face->glyph->format != FT_GLYPH_FORMAT_OUTLINE)
    return CAIRO_STATUS_USER_FONT_ERROR;

  FT_Outline_Funcs funcs{};
  funcs.move_to = move_to_cb;
  funcs.line_to = line_to_cb;
  funcs.conic_to = conic_to_cb;
  funcs.cubic_to = cubic_to_cb;
  funcs.shift = 0;
  funcs.delta = 0;

  cairo_new_path(cr);
  if (FT_Outline_Decompose(&face->glyph->outline, &funcs, cr) != 0)
    return CAIRO_STATUS_USER_FONT_ERROR;

  // Helpful extents (advisory)
  if (extents) {
    extents->x_bearing = face->glyph->metrics.horiBearingX / 64.0;
    extents->y_bearing = -face->glyph->metrics.horiBearingY / 64.0;
    extents->width = face->glyph->metrics.width / 64.0;
    extents->height = face->glyph->metrics.height / 64.0;
    extents->x_advance = face->glyph->metrics.horiAdvance / 64.0;
    extents->y_advance = 0.0;
  }

  return CAIRO_STATUS_SUCCESS;
}

// Maps text -> glyph indices + positions.
// Minimal: treat each UTF-8 byte as a "character" (OK for ASCII).
static cairo_status_t
user_font_text_to_glyphs(cairo_scaled_font_t *scaled_font, const char *utf8,
                         int utf8_len, cairo_glyph_t **glyphs, int *num_glyphs,
                         cairo_text_cluster_t ** /*clusters*/,
                         int * /*num_clusters*/,
                         cairo_text_cluster_flags_t * /*cluster_flags*/) {
  cairo_font_face_t *ff = cairo_scaled_font_get_font_face(scaled_font);
  auto *ctx = static_cast<FtCairoFont *>(
      cairo_font_face_get_user_data(ff, &g_ft_cairo_font_key));
  if (!ctx)
    return CAIRO_STATUS_USER_FONT_ERROR;

  FT_Face face = ctx->ftFace;

  std::string s;
  if (utf8_len >= 0)
    s.assign(utf8, (size_t)utf8_len);
  else
    s.assign(utf8);

  *num_glyphs = (int)s.size();
  *glyphs = cairo_glyph_allocate(*num_glyphs);
  if (!*glyphs)
    return CAIRO_STATUS_NO_MEMORY;

  double x = 0.0;
  for (int i = 0; i < *num_glyphs; i++) {
    unsigned char ch = (unsigned char)s[i];
    FT_UInt gindex = FT_Get_Char_Index(face, (FT_ULong)ch);

    // Load for advance
    if (FT_Load_Glyph(face, gindex, FT_LOAD_DEFAULT) != 0) {
      (*glyphs)[i].index = 0;
      (*glyphs)[i].x = x;
      (*glyphs)[i].y = 0.0;
      continue;
    }

    (*glyphs)[i].index = gindex;
    (*glyphs)[i].x = x;
    (*glyphs)[i].y = 0.0;

    x += face->glyph->advance.x / 64.0;
  }

  return CAIRO_STATUS_SUCCESS;
}

static cairo_font_face_t *create_user_font_face(FtCairoFont *ctx) {
  cairo_font_face_t *ff = cairo_user_font_face_create();
  cairo_user_font_face_set_init_func(ff, user_font_init);
  cairo_user_font_face_set_render_glyph_func(ff, user_font_render_glyph);
  cairo_user_font_face_set_text_to_glyphs_func(ff, user_font_text_to_glyphs);

  // Store pointer for callbacks
  cairo_font_face_set_user_data(ff, &g_ft_cairo_font_key, ctx, nullptr);
  return ff;
}

static double parse_double_or(const char *s, double fallback) {
  if (!s)
    return fallback;
  char *end = nullptr;
  double v = std::strtod(s, &end);
  if (!end || end == s)
    return fallback;
  return v;
}

int main(int argc, char **argv) {
  if (argc < 4) {
    std::cerr
        << "Usage:\n  " << argv[0]
        << " /path/to/font.ttf out.png \"Text\" [fontSizePx] [r g b a]\n\n"
        << "Example:\n  " << argv[0]
        << " ./DejaVuSans.ttf out.png \"Hello!\" 72 0.1 0.2 0.8 1.0\n";
    return 1;
  }

  const char *fontPath = argv[1];
  const char *outPath = argv[2];
  std::string text = argv[3];

  double fontSizePx = (argc >= 5) ? parse_double_or(argv[4], 72.0) : 72.0;
  double r = (argc >= 6) ? parse_double_or(argv[5], 0.1) : 0.1;
  double g = (argc >= 7) ? parse_double_or(argv[6], 0.2) : 0.2;
  double b = (argc >= 8) ? parse_double_or(argv[7], 0.8) : 0.8;
  double a = (argc >= 9) ? parse_double_or(argv[8], 1.0) : 1.0;

  // Canvas size (simple fixed size for a minimal demo)
  const int W = 800;
  const int H = 300;

  cairo_surface_t *surface =
      cairo_image_surface_create(CAIRO_FORMAT_ARGB32, W, H);
  cairo_t *cr = cairo_create(surface);

  // Background (white)
  cairo_set_source_rgba(cr, 1, 1, 1, 1);
  cairo_paint(cr);

  // Load font and create Cairo user font face
  FtCairoFont ft(fontPath);
  cairo_font_face_t *userFace = create_user_font_face(&ft);

  // Set font face + size
  cairo_set_font_face(cr, userFace);
  cairo_set_font_size(cr, fontSizePx);

  // Color
  cairo_set_source_rgba(cr, r, g, b, a);

  // Position baseline
  cairo_move_to(cr, 50, 160);

  // Draw text
  cairo_show_text(cr, text.c_str());

  // Write to PNG
  cairo_status_t st = cairo_surface_write_to_png(surface, outPath);
  if (st != CAIRO_STATUS_SUCCESS) {
    std::cerr << "cairo_surface_write_to_png failed: "
              << cairo_status_to_string(st) << "\n";
  } else {
    std::cout << "Wrote: " << outPath << "\n";
  }

  // Cleanup
  cairo_font_face_destroy(userFace);
  cairo_destroy(cr);
  cairo_surface_destroy(surface);

  return (st == CAIRO_STATUS_SUCCESS) ? 0 : 2;
}
