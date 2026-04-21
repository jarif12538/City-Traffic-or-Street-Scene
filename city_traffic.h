/*
 * ============================================================
 *  CITY TRAFFIC SCENE – Header File
 *  Shared declarations and constants
 * ============================================================
 */

#ifndef CITY_TRAFFIC_H
#define CITY_TRAFFIC_H

#include <GL/glut.h>
#include <cmath>

// ──────────────────────────────────────────────
//  Window dimensions
// ──────────────────────────────────────────────
extern const int W, H;

// ──────────────────────────────────────────────
//  Global animation state
// ──────────────────────────────────────────────
extern float carX1, carX2;
extern float sunAngle;
extern float cloud1X, cloud2X;
extern int tlPhase;
extern float tlTimer;
extern const float TL_RED_DUR, TL_YELLOW_DUR, TL_GREEN_DUR;

// ──────────────────────────────────────────────
//  Color & primitive helpers
// ──────────────────────────────────────────────
void setColor(float r, float g, float b);
void drawRect(float x, float y, float w, float h);

// ──────────────────────────────────────────────
//  Graphics Algorithms
// ──────────────────────────────────────────────
void ddaLine(float x1, float y1, float x2, float y2);
void bresenhamLine(int x1, int y1, int x2, int y2);
void midpointCircle(float cx, float cy, float r, bool filled=false);

// ──────────────────────────────────────────────
//  2D Transformation Utilities
// ──────────────────────────────────────────────
void applyTranslation(float tx, float ty);
void applyRotationAround(float angle, float cx, float cy);
void applyScalingAround(float sx, float sy, float cx, float cy);
void applyReflectionY(float cx, float cy);
void applyShearX(float shx);

// ──────────────────────────────────────────────
//  Scene Components
// ──────────────────────────────────────────────
void drawSky();
void drawSun();
void drawCloud(float cx, float cy, float scale);
void drawRoad();
void drawBuilding(float x, float y, float w, float h,
                  float r, float g, float b,
                  int wCols, int wRows, bool sheared=false);
void drawTrafficLight(float x, float y);
void drawCar(float cx, float cy, float r, float g, float b, bool facingLeft=false);
void drawTree(float x, float y, float scale);
void drawRotatedSign(float x, float y, float angle);

// ──────────────────────────────────────────────
//  Main rendering & update
// ──────────────────────────────────────────────
void render();
void update(float dt);

#endif // CITY_TRAFFIC_H
