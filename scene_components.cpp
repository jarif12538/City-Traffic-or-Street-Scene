/*
 * ============================================================
 *  SCENE COMPONENTS
 *  All drawing functions for scene elements
 * ============================================================
 */

#include "city_traffic.h"

// ── Sky gradient (two quads blended) ──────────
void drawSky()
{
    glBegin(GL_QUADS);
      glColor3f(0.53f, 0.81f, 0.98f); glVertex2f(-450,-20);
      glColor3f(0.53f, 0.81f, 0.98f); glVertex2f( 450,-20);
      glColor3f(0.18f, 0.55f, 0.90f); glVertex2f( 450, 300);
      glColor3f(0.18f, 0.55f, 0.90f); glVertex2f(-450, 300);
    glEnd();
}

// ── Sun (Midpoint circle + rays via DDA) ──────
void drawSun()
{
    // Sun position derived from angle
    float rad = sunAngle * M_PI / 180.0f;
    float sx  = -300.0f + 320.0f * cosf(rad);
    float sy  =  -10.0f + 200.0f * sinf(rad);

    // Glow
    setColor(1.0f, 0.95f, 0.5f);
    midpointCircle(sx, sy, 28, true);
    // Core
    setColor(1.0f, 0.88f, 0.2f);
    midpointCircle(sx, sy, 20, true);
    // Outline
    setColor(1.0f, 0.7f, 0.0f);
    glPointSize(2.0f);
    midpointCircle(sx, sy, 20, false);

    // Rays using DDA lines
    setColor(1.0f, 0.9f, 0.3f);
    glPointSize(1.5f);
    for(int a=0; a<360; a+=45){
        float ar = a*M_PI/180.0f;
        ddaLine(sx + 22*cosf(ar), sy + 22*sinf(ar),
                sx + 36*cosf(ar), sy + 36*sinf(ar));
    }
}

// ── Cloud (group of circles) ──────────────────
void drawCloud(float cx, float cy, float scale)
{
    glPushMatrix();
    applyScalingAround(scale, scale, cx, cy);
    setColor(1,1,1);
    midpointCircle(cx,      cy,     22, true);
    midpointCircle(cx+25,   cy+5,   18, true);
    midpointCircle(cx-22,   cy+5,   16, true);
    midpointCircle(cx+10,   cy+18,  15, true);
    midpointCircle(cx-10,   cy+15,  14, true);
    glPopMatrix();
}

// ── Ground / road ─────────────────────────────
void drawRoad()
{
    // Grass / ground
    setColor(0.3f, 0.6f, 0.2f);
    drawRect(-450, -300, 900, 200);   // bottom strip

    // Road surface
    setColor(0.22f, 0.22f, 0.22f);
    drawRect(-450, -220, 900, 170);

    // Pavement kerbs
    setColor(0.75f, 0.75f, 0.75f);
    drawRect(-450, -225, 900, 8);
    drawRect(-450, -55,  900, 8);

    // Centre divider (yellow solid line – Bresenham)
    setColor(1.0f, 0.85f, 0.0f);
    glPointSize(3.0f);
    bresenhamLine(-450, -138, 450, -138);

    // Dashed white lane markings using DDA
    setColor(1,1,1);
    glPointSize(2.5f);
    for(int x = -440; x < 450; x += 60){
        ddaLine((float)x,       -175.0f,
                (float)(x+35),  -175.0f);
        ddaLine((float)x,       -100.0f,
                (float)(x+35),  -100.0f);
    }
}

// ── Building (with Bresenham window grid) ─────
void drawBuilding(float x, float y, float w, float h,
                  float r, float g, float b,
                  int wCols, int wRows, bool sheared)
{
    glPushMatrix();

    // Optional shear transform (demonstrates shear)
    if(sheared){
        glTranslatef(x, y, 0);
        applyShearX(0.08f);
        glTranslatef(-x, -y, 0);
    }

    // Building body
    setColor(r,g,b);
    drawRect(x, y, w, h);

    // Roof edge
    setColor(r*0.7f, g*0.7f, b*0.7f);
    drawRect(x-3, y+h, w+6, 6);

    // Windows – Bresenham outlines
    float ww = w / (wCols*2.0f+1.0f);
    float wh = h / (wRows*2.5f+1.0f);
    for(int row=0; row<wRows; row++){
        for(int col=0; col<wCols; col++){
            float wx = x + ww*(1+col*2);
            float wy = y + wh*(1+row*2);
            // Lit window fill
            setColor(1.0f, 0.95f, 0.7f);
            drawRect(wx, wy, ww, wh);
            // Bresenham border
            setColor(0.3f,0.3f,0.3f);
            glPointSize(1.5f);
            bresenhamLine((int)wx,      (int)wy,
                          (int)(wx+ww), (int)wy);
            bresenhamLine((int)(wx+ww), (int)wy,
                          (int)(wx+ww), (int)(wy+wh));
            bresenhamLine((int)(wx+ww), (int)(wy+wh),
                          (int)wx,      (int)(wy+wh));
            bresenhamLine((int)wx,      (int)(wy+wh),
                          (int)wx,      (int)wy);
        }
    }

    glPopMatrix();
}

// ── Traffic light ─────────────────────────────
void drawTrafficLight(float x, float y)
{
    // Pole
    setColor(0.3f,0.3f,0.3f);
    drawRect(x-3, y, 6, 80);

    // Housing box
    setColor(0.15f,0.15f,0.15f);
    drawRect(x-12, y+80, 24, 65);

    // Bulbs – Midpoint circles
    glPointSize(2.0f);
    // Red
    setColor(tlPhase==0 ? 1.0f:0.3f, 0.0f, 0.0f);
    midpointCircle(x, y+130, 8, true);
    // Yellow
    setColor(tlPhase==1 ? 1.0f:0.3f,
             tlPhase==1 ? 0.8f:0.3f, 0.0f);
    midpointCircle(x, y+112, 8, true);
    // Green
    setColor(0.0f, tlPhase==2 ? 1.0f:0.3f, 0.0f);
    midpointCircle(x, y+94,  8, true);
}

// ── Car body (demonstrates translation transform) ─
void drawCar(float cx, float cy, float r, float g, float b, bool facingLeft)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    // Flip if facing left (reflection)
    if(facingLeft)
        applyReflectionY(0, 0);

    // Body
    setColor(r, g, b);
    drawRect(-35, -12, 70, 22);

    // Cabin (with slight rotation for 2D perspective effect)
    glPushMatrix();
    setColor(r*0.85f, g*0.85f, b*0.85f);
    drawRect(-22, 10, 44, 18);
    glPopMatrix();

    // Windshields (Bresenham)
    setColor(0.6f, 0.85f, 1.0f);
    drawRect(-21,11, 18,15);
    drawRect(  3,11, 18,15);

    // Wheels – Midpoint circle
    setColor(0.1f,0.1f,0.1f);
    midpointCircle(-20,-12, 10, true);
    midpointCircle( 20,-12, 10, true);
    setColor(0.5f,0.5f,0.5f);
    midpointCircle(-20,-12,  5, true);
    midpointCircle( 20,-12,  5, true);

    // Head/tail lights
    setColor(1.0f, 1.0f, 0.6f);
    drawRect( 33, -6, 4, 8);   // headlight
    setColor(0.9f, 0.1f, 0.1f);
    drawRect(-37, -6, 4, 8);   // tail light

    glPopMatrix();
}

// ── Pavement trees ────────────────────────────
void drawTree(float x, float y, float scale)
{
    glPushMatrix();
    applyTranslation(x, y);
    applyScalingAround(scale, scale, 0, 0);

    // Trunk
    setColor(0.45f, 0.30f, 0.10f);
    drawRect(-5, 0, 10, 40);

    // Foliage – three filled circles
    setColor(0.13f, 0.55f, 0.13f);
    midpointCircle(0,  60, 28, true);
    midpointCircle(-18,48, 22, true);
    midpointCircle( 18,48, 22, true);

    glPopMatrix();
}

// ── Rotated decorative star / signage ─────────
void drawRotatedSign(float x, float y, float angle)
{
    glPushMatrix();
    applyTranslation(x, y);
    // Rotation transform
    glRotatef(angle, 0, 0, 1);

    // Sign board
    setColor(0.9f, 0.2f, 0.1f);
    drawRect(-18, -8, 36, 16);
    setColor(1,1,1);
    // Simple cross on sign via DDA
    glPointSize(2.0f);
    ddaLine(-12, 0, 12, 0);
    ddaLine(0, -6, 0,  6);

    glPopMatrix();
}
