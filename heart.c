#include "svpng.inc"
#include <math.h> // fabsf(), fminf(), fmaxf(), sinf(), cosf(), sqrt()
#include <stdlib.h> // rand(), RAND_MAX

#define TWO_PI 6.28318530718f
#define W 1024
#define H 1024
#define N 256
#define MAX_STEP 64
#define MAX_DISTANCE 5.0f
#define EPSILON 1e-6f
#define BIAS 1e-4f
#define MAX_DEPTH 3
#define BLACK { 0.0f, 0.0f, 0.0f }

typedef struct { float r, g, b; } Color;
typedef struct { float sd, reflectivity, eta; Color emissive, absorption; } Result;

unsigned char img[W * H * 3];

Color colorAdd(Color a, Color b) {
    Color c = { a.r + b.r, a.g + b.g, a.b + b.b };
    return c;
}

Color colorMultiply(Color a, Color b) {
    Color c = { a.r * b.r, a.g * b.g, a.b * b.b };
    return c;
}

Color colorScale(Color a, float s) {
    Color c = { a.r * s, a.g * s, a.b * s };
    return c;
}

float circleSDF(float x, float y, float cx, float cy, float r) {
    float ux = x - cx, uy = y - cy;
    return sqrtf(ux * ux + uy * uy) - r;
}

float planeSDF(float x, float y, float px, float py, float nx, float ny) {
    return (x - px) * nx + (y - py) * ny;
}

float ngonSDF(float x, float y, float cx, float cy, float r, float n) {
    float ux = x - cx, uy = y - cy, a = TWO_PI / n;
    float t = fmodf(atan2f(uy, ux) + TWO_PI, a), s = sqrtf(ux * ux + uy * uy);
    return planeSDF(s * cosf(t), s * sinf(t), r, 0.0f, cosf(a * 0.5f), sinf(a * 0.5f));
}

Result unionOp(Result a, Result b) {
    return a.sd < b.sd ? a : b;
}

Result intersectOp(Result a, Result b) {
    return a.sd > b.sd ? a : b;
}

Result scene(float x, float y) {
    float u = x - 0.5f, v = y - 0.5f, t = fmodf(atan2f(v, u) + TWO_PI, TWO_PI / 16), s = sqrtf(u * u + v * v);
    x = fabsf(x - 0.5f) + 0.5f;
    Color m = { 0.0f, 3.0f, 3.0f };
    Result a = { ngonSDF(x, y, 0.7f, 0.35f, 0.2f, 16), 0.0f, 1.77f, BLACK, m };
    Result b = { ngonSDF(x, y, 0.35f, 0.35f, 0.55f, 32), 0.0f, 1.77f, BLACK, m };
    Result c = {  planeSDF(x, y, 0.5f, 0.35f, 0.0f, -1.0f), 0.0f, 1.77f, BLACK, m };
    // y = fabsf(y - 0.5f) + 0.5f;
    // Result d = { circleSDF(x, y, 1.05f, 1.05f, 0.05f), 0.0f, 0.0f, { 5.0f, 5.0f, 5.0f }, BLACK };
    // Result d = { -circleSDF(x, y, 0.5f, 0.5f, 3.0f), 0.0f, 0.0f, { 0.5f, 0.5f, 0.5f }, BLACK };
    Result d = { circleSDF(s * cosf(t), s * sinf(t), 0.6f * cosf(TWO_PI / 32), 0.5f * sinf(TWO_PI / 32), 0.05f), 0.0f, 0.0f, { 2.0f, 2.0f, 2.0f }, BLACK };
    return unionOp(unionOp(a, intersectOp(b, c)), d);
}

void gradient(float x, float y, float* nx, float* ny) {
    *nx = (scene(x + EPSILON, y).sd - scene(x - EPSILON, y).sd) * (0.5f / EPSILON);
    *ny = (scene(x, y + EPSILON).sd - scene(x, y - EPSILON).sd) * (0.5f / EPSILON);
}

void reflect(float ix, float iy, float nx, float ny, float* rx, float* ry) {
    float idotn2 = (ix * nx + iy * ny) * 2.0f;
    *rx = ix - idotn2 * nx;
    *ry = iy - idotn2 * ny;
}

int refract(float ix, float iy, float nx, float ny, float eta, float* rx, float* ry) {
    float idotn = ix * nx + iy * ny;
    float k = 1.0f - eta * eta * (1.0f - idotn * idotn);
    if (k < 0.0f)
        return 0; // Total internal reflection
    float a = eta * idotn + sqrtf(k);
    *rx = eta * ix - a * nx;
    *ry = eta * iy - a * ny;
    return 1;
}

float fresnel(float cosi, float cost, float etai, float etat) {
    float rs = (etat * cosi - etai * cost) / (etat * cosi + etai * cost);
    float rp = (etai * cosi - etat * cost) / (etai * cosi + etat * cost);
    return (rs * rs + rp * rp) * 0.5f;
}

Color beerLambert(Color a, float d) {
    Color c = { expf(-a.r * d), expf(-a.g * d), expf(-a.b * d) };
    return c;
}

Color trace(float ox, float oy, float dx, float dy, int depth) {
    float t = 1e-3f;
    float sign = scene(ox, oy).sd > 0.0f ? 1.0f : -1.0f;
    for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        float x = ox + dx * t, y = oy + dy * t;
        Result r = scene(x, y);
        if (r.sd * sign < EPSILON) {
            Color sum = r.emissive;
            if (depth < MAX_DEPTH && r.eta > 0.0f) {
                float nx, ny, rx, ry, refl = r.reflectivity;
                gradient(x, y, &nx, &ny);
                float s = 1.0f / (nx * nx + ny * ny);
                nx *= sign * s;
                ny *= sign * s;
                if (r.eta > 0.0f) {
                    if (refract(dx, dy, nx, ny, sign < 0.0f ? r.eta : 1.0f / r.eta, &rx, &ry)) {
                        float cosi = -(dx * nx + dy * ny);
                        float cost = -(rx * nx + ry * ny);
                        refl = sign < 0.0f ? fresnel(cosi, cost, r.eta, 1.0f) : fresnel(cosi, cost, 1.0f, r.eta);
                        refl = fmaxf(fminf(refl, 1.0f), 0.0f);
                        sum = colorAdd(sum, colorScale(trace(x - nx * BIAS, y - ny * BIAS, rx, ry, depth + 1), 1.0f - refl));
                    }
                    else
                        refl = 1.0f; // Total internal reflection
                }
                if (refl > 0.0f) {
                    reflect(dx, dy, nx, ny, &rx, &ry);
                    sum = colorAdd(sum, colorScale(trace(x + nx * BIAS, y + ny * BIAS, rx, ry, depth + 1), refl));
                }
            }
            Color c = colorMultiply(sum, beerLambert(r.absorption, t));
            return c;
        }
        t += r.sd * sign;
    }
    Color black = BLACK;
    return black;
}

Color sample(float x, float y) {
    Color sum = BLACK;
    for (int i = 0; i < N; i++) {
        float a = TWO_PI * (i + (float)rand() / RAND_MAX) / N;
        sum = colorAdd(sum, trace(x, y, cosf(a), sinf(a), 0));
    }
    return colorScale(sum, 1.0f / N);
}

int main() {
    unsigned char* p = img;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++, p += 3) {
            Color c = sample((float)x / W, (float)y / H);
            p[0] = (int)(fminf(c.r * 255.0f, 255.0f));
            p[1] = (int)(fminf(c.g * 255.0f, 255.0f));
            p[2] = (int)(fminf(c.b * 255.0f, 255.0f));
        }
    svpng(fopen("heart.png", "wb"), W, H, img, 0);
}
