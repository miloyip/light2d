#include "svpng.inc"
#include <math.h> // fminf(), sinf(), cosf(), sqrt()
#include <stdlib.h> // rand(), RAND_MAX

#define TWO_PI 6.28318530718f
#define W 1024
#define H 1024
#define N 256
#define MAX_STEP 64
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

typedef struct { float sd, emissive; } Result;

unsigned char img[W * H * 3];

float capsuleSDF(float x, float y, float ax, float ay, float bx, float by, float r) {
    float vx = x - ax, vy = y - ay, ux = bx - ax, uy = by - ay;
    float t = fmaxf(fminf((vx * ux + vy * uy) / (ux * ux + uy * uy), 1.0f), 0.0f);
    float dx = vx - ux * t, dy = vy - uy * t;
    return sqrtf(dx * dx + dy * dy) - r;
}

Result scene(float x, float y) {
    x = fabsf(x - 0.5f) + 0.5f;
    Result a = { capsuleSDF(x, y, 0.75f, 0.25f, 0.75f, 0.75f, 0.05f), 1.0f };
    Result b = { capsuleSDF(x, y, 0.75f, 0.25f, 0.50f, 0.75f, 0.05f), 1.0f };
    return a.sd < b.sd ? a : b;
}

float trace(float ox, float oy, float dx, float dy) {
    float t = 0.0f;
    for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        Result r = scene(ox + dx * t, oy + dy * t);
        if (r.sd < EPSILON)
            return r.emissive;
        t += r.sd;
    }
    return 0.0f;
}

float sample(float x, float y) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        float a = TWO_PI * (i + (float)rand() / RAND_MAX) / N;
        sum += trace(x, y, cosf(a), sinf(a));
    }
    return sum / N;
}

int main() {
    unsigned char* p = img;
    for (int y = 0; y < H; y++)
        for (int x = 0; x < W; x++, p += 3)
            p[0] = p[1] = p[2] = (int)(fminf(sample((float)x / W, (float)y / H) * 255.0f, 255.0f));
    svpng(fopen("m.png", "wb"), W, H, img, 0);
}
