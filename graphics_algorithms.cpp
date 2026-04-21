/*
 * ============================================================
 *  GRAPHICS ALGORITHMS & UTILITIES
 *  Core drawing algorithms and transformation helpers
 * ============================================================
 */


 
#include "city_traffic.h"

// ──────────────────────────────────────────────
//  Colour helpers
// ──────────────────────────────────────────────
void setColor(float r, float g, float b)
{
    glColor3f(r, g, b);
}

// ══════════════════════════════════════════════
//  ALGORITHM 1 – DDA Line
//  Used for road lane markings
// ══════════════════════════════════════════════
void ddaLine(float x1, float y1, float x2, float y2)
{
    float dx = x2 - x1, dy = y2 - y1;
    float steps = (fabsf(dx) > fabsf(dy)) ? fabsf(dx) : fabsf(dy);
    float xInc = dx / steps, yInc = dy / steps;
    float x = x1, y = y1;

    glBegin(GL_POINTS);
    for(int i = 0; i <= (int)steps; ++i){
        glVertex2f(x, y);
        x += xInc; y += yInc;
    }
    glEnd();
}

// ══════════════════════════════════════════════
//  ALGORITHM 2 – Bresenham Line
//  Used for building outlines / window grids
// ══════════════════════════════════════════════
void bresenhamLine(int x1, int y1, int x2, int y2)
{
    int dx = abs(x2-x1), dy = abs(y2-y1);
    int sx = (x1<x2)?1:-1, sy = (y1<y2)?1:-1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while(true){
        glVertex2i(x1, y1);
        if(x1==x2 && y1==y2) break;
        int e2 = 2*err;
        if(e2 > -dy){ err -= dy; x1 += sx; }
        if(e2 <  dx){ err += dx; y1 += sy; }
    }
    glEnd();
}

// ══════════════════════════════════════════════
//  ALGORITHM 3 – Midpoint Circle
//  Used for wheels, sun, traffic-light bulbs
// ══════════════════════════════════════════════
void midpointCircle(float cx, float cy, float r, bool filled)
{
    if(filled){
        // filled via triangle fan
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for(int a=0; a<=360; a+=2)
            glVertex2f(cx + r*cosf(a*M_PI/180.0f),
                       cy + r*sinf(a*M_PI/180.0f));
        glEnd();
        return;
    }
    int x = (int)r, y = 0;
    int p = 1 - (int)r;
    glBegin(GL_POINTS);
    auto plot8 = [&](int px, int py){
        glVertex2f(cx+px, cy+py); glVertex2f(cx-px, cy+py);
        glVertex2f(cx+px, cy-py); glVertex2f(cx-px, cy-py);
        glVertex2f(cx+py, cy+px); glVertex2f(cx-py, cy+px);
        glVertex2f(cx+py, cy-px); glVertex2f(cx-py, cy-px);
    };
    plot8(x,y);
    while(x > y){
        ++y;
        if(p <= 0) p += 2*y+1;
        else { --x; p += 2*(y-x)+1; }
        plot8(x,y);
    }
    glEnd();
}

// ──────────────────────────────────────────────
//  Solid polygon helper
// ──────────────────────────────────────────────
void drawRect(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
    glVertex2f(x,   y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x,   y+h);
    glEnd();
}

// ══════════════════════════════════════════════
//  2D TRANSFORMATION UTILITIES
// ══════════════════════════════════════════════

void applyTranslation(float tx, float ty)
{
    glTranslatef(tx, ty, 0.0f);
}

void applyRotationAround(float angle, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glRotatef(angle, 0,0,1);
    glTranslatef(-cx,-cy,0);
}

void applyScalingAround(float sx, float sy, float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(sx, sy, 1);
    glTranslatef(-cx,-cy,0);
}

void applyReflectionY(float cx, float cy)
{
    glTranslatef(cx, cy, 0);
    glScalef(-1, 1, 1);
    glTranslatef(-cx,-cy,0);
}

void applyShearX(float shx)
{
    float m[16] = {
        1,0,0,0,
        shx,1,0,0,
        0,0,1,0,
        0,0,0,1
    };
    glMultMatrixf(m);
}
