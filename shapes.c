#include "svpng.inc"
#include <math.h> // fminf(), sinf(), cosf(), sqrt()
#include <stdlib.h> // rand(), RAND_MAX

#define TWO_PI 6.28318530718f
#define W 512
#define H 512
#define N 64
#define MAX_STEP 64
#define MAX_DISTANCE 2.0f
#define EPSILON 1e-6f

typedef struct { float sd, emissive; } Result;

unsigned char img[W * H * 3];

float circleSDF(float x, float y, float cx, float cy, float r) {
    float ux = x - cx, uy = y - cy;
    return sqrtf(ux * ux + uy * uy) - r;
}

float planeSDF(float x, float y, float px, float py, float nx, float ny) {
    return (x - px) * nx + (y - py) * ny;
}

float segmentSDF(float x, float y, float ax, float ay, float bx, float by) {
    float vx = x - ax, vy = y - ay, ux = bx - ax, uy = by - ay;
    float t = fmaxf(fminf((vx * ux + vy * uy) / (ux * ux + uy * uy), 1.0f), 0.0f);
    float dx = vx - ux * t, dy = vy - uy * t;
    return sqrtf(dx * dx + dy * dy);
}

float capsuleSDF(float x, float y, float ax, float ay, float bx, float by, float r) {
    return segmentSDF(x, y, ax, ay, bx, by) - r;
}

float boxSDF(float x, float y, float cx, float cy, float theta, float sx, float sy) {
    float costheta = cosf(theta), sintheta = sinf(theta);
    float dx = fabs((x - cx) * costheta + (y - cy) * sintheta) - sx;
    float dy = fabs((y - cy) * costheta - (x - cx) * sintheta) - sy;
    float ax = fmaxf(dx, 0.0f), ay = fmaxf(dy, 0.0f);
    return fminf(fmaxf(dx, dy), 0.0f) + sqrtf(ax * ax + ay * ay);
}

float triangleSDF(float x, float y, float ax, float ay, float bx, float by, float cx, float cy) {
    float d = fminf(fminf(
        segmentSDF(x, y, ax, ay, bx, by),
        segmentSDF(x, y, bx, by, cx, cy)),
        segmentSDF(x, y, cx, cy, ax, ay));
    return (bx - ax) * (y - ay) > (by - ay) * (x - ax) && 
           (cx - bx) * (y - by) > (cy - by) * (x - bx) && 
           (ax - cx) * (y - cy) > (ay - cy) * (x - cx) ? -d : d;
}

Result intersectOp(Result a, Result b) {
    return a.sd > b.sd ? a : b;
}

Result scene(float x, float y) {
    Result a = {   circleSDF(x, y, 0.5f, 0.5f, 0.2f), 1.0f };
    Result b = {    planeSDF(x, y, 0.0f, 0.5f, 0.0f, 1.0f), 0.8f };
    Result c = {  capsuleSDF(x, y, 0.4f, 0.4f, 0.6f, 0.6f, 0.1f), 1.0f };
    Result d = {      boxSDF(x, y, 0.5f, 0.5f, TWO_PI / 16.0f, 0.3f, 0.1f), 1.0f };
    Result e = {      boxSDF(x, y, 0.5f, 0.5f, TWO_PI / 16.0f, 0.3f, 0.1f) - 0.1f, 1.0f };
    Result f = { triangleSDF(x, y, 0.5f, 0.2f, 0.8f, 0.8f, 0.3f, 0.6f), 1.0f };
    Result g = { triangleSDF(x, y, 0.5f, 0.2f, 0.8f, 0.8f, 0.3f, 0.6f) - 0.1f, 1.0f };
    // return a;
    // return b;
    return intersectOp(a, b);
    // return c;
    // return d;
    // return e;
    // return f;
    // return g;
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
    svpng(fopen("shapes.png", "wb"), W, H, img, 0);
}
