#include "svpng.inc"
#include <math.h> // fminf(), sinf(), cosf()
#include <stdlib.h> // rand(), RAND_MAX

#define TWO_PI 6.28318530718f
#define W 512
#define H 512
#define N 64
#define MAX_STEP 10
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

unsigned char img[W * H * 3];

float circleSDF(float x, float y, float cx, float cy, float r) {
    float ux = x - cx, uy = y - cy;
    return sqrtf(ux * ux + uy * uy) - r;
}

float trace(float ox, float oy, float dx, float dy) {
    float t = 0.0f;
    for (int i = 0; i < MAX_STEP && t < MAX_DISTANCE; i++) {
        float sd = circleSDF(ox + dx * t, oy + dy * t, 0.5f, 0.5f, 0.1f);
        if (sd < EPSILON)
            return 2.0f;
        t += sd;
    }
    return 0.0f;
}

float sample(float x, float y) {
    float sum = 0.0f;
    for (int i = 0; i < N; i++) {
        // float a = TWO_PI * rand() / RAND_MAX;
        // float a = TWO_PI * i / N;
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
    svpng(fopen("basic.png", "wb"), W, H, img, 0);
}
