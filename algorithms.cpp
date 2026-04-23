#include <GL/glut.h>
#include <cmath>
#include <cstdlib>

void setColor(float r, float g, float b)
{
    glColor3f(r, g, b);
}

void setColorAlpha(float r, float g, float b, float a)
{
    glColor4f(r, g, b, a);
}

void blendColor(float r1, float g1, float b1,
    float r2, float g2, float b2, float t)
{
    float r = r1 * (1 - t) + r2 * t;
    float g = g1 * (1 - t) + g2 * t;
    float b = b1 * (1 - t) + b2 * t;
    glColor3f(r, g, b);
}

void ddaLine(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1, dy = y2 - y1;
    float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);
    float xInc = dx / steps, yInc = dy / steps;
    float x = x1, y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; ++i) {
        glVertex2f(x, y);
        x += xInc;
        y += yInc;
    }
    glEnd();
}

void bresenhamLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
    glEnd();
}

void midpointCircle(float cx, float cy, float r, bool filled = false)
{
    if (filled) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int a = 0; a <= 360; a += 2) {
            glVertex2f(cx + r * cosf(a * M_PI / 180.0f),
                cy + r * sinf(a * M_PI / 180.0f));
        }
        glEnd();
        return;
    }

    int x = (int)r, y = 0;
    int p = 1 - (int)r;
    glBegin(GL_POINTS);
    auto plot8 = [&](int px, int py) {
        glVertex2f(cx + px, cy + py); glVertex2f(cx - px, cy + py);
        glVertex2f(cx + px, cy - py); glVertex2f(cx - px, cy - py);
        glVertex2f(cx + py, cy + px); glVertex2f(cx - py, cy + px);
        glVertex2f(cx + py, cy - px); glVertex2f(cx - py, cy - px);
    };
    plot8(x, y);
    while (x > y) {
        ++y;
        if (p <= 0) p += 2 * y + 1;
        else { --x; p += 2 * (y - x) + 1; }
        plot8(x, y);
    }
    glEnd();
}

void drawRect(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3)
{
    glBegin(GL_TRIANGLES);
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void drawGradientRect(float x, float y, float w, float h,
    float r1, float g1, float b1,
    float r2, float g2, float b2)
{
    glBegin(GL_QUADS);
    glColor3f(r1, g1, b1); glVertex2f(x, y);
    glColor3f(r2, g2, b2); glVertex2f(x + w, y);
    glColor3f(r2, g2, b2); glVertex2f(x + w, y + h);
    glColor3f(r1, g1, b1); glVertex2f(x, y + h);
    glEnd();
}

void drawExtrudedRect(float x, float y, float w, float h,
    float depthX, float depthY,
    float frontR, float frontG, float frontB,
    float sideR, float sideG, float sideB,
    float topR, float topG, float topB)
{
    glBegin(GL_QUADS);
    glColor3f(sideR, sideG, sideB);
    glVertex2f(x + w, y);
    glVertex2f(x + w + depthX, y + depthY);
    glVertex2f(x + w + depthX, y + h + depthY);
    glVertex2f(x + w, y + h);

    glColor3f(topR, topG, topB);
    glVertex2f(x, y + h);
    glVertex2f(x + w, y + h);
    glVertex2f(x + w + depthX, y + h + depthY);
    glVertex2f(x + depthX, y + h + depthY);

    glColor3f(frontR, frontG, frontB);
    glVertex2f(x, y);
    glVertex2f(x + w, y);
    glVertex2f(x + w, y + h);
    glVertex2f(x, y + h);
    glEnd();
}

void applyTranslation(float tx, float ty)
{
    glTranslatef(tx, ty, 0.0f);
}

void applyRotationAround(float angle, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glRotatef(angle, 0, 0, 1);
    glTranslatef(-cx, -cy, 0);
}

void applyScalingAround(float sx, float sy, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(sx, sy, 1);
    glTranslatef(-cx, -cy, 0);
}

void applyReflectionY(float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(-1, 1, 1);
    glTranslatef(-cx, -cy, 0);
}

float depthScale(float y)
{
    float t = (y + 350.0f) / 700.0f;
    if (t < 0.0f) t = 0.0f;
    if (t > 1.0f) t = 1.0f;
    return 1.18f - t * 0.55f;
}

void drawShadow(float x, float y, float w, float h)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.15f);
    drawRect(x, y - h * 0.6f, w, h);
    glDisable(GL_BLEND);
}
