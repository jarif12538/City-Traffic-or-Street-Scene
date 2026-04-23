#include <GL/glut.h>
#include <cmath>

void setColor(float r, float g, float b);
void setColorAlpha(float r, float g, float b, float a);
void blendColor(float r1, float g1, float b1,
    float r2, float g2, float b2, float t);
void ddaLine(float x1, float y1, float x2, float y2);
void bresenhamLine(int x1, int y1, int x2, int y2);
void midpointCircle(float cx, float cy, float r, bool filled = false);
void drawRect(float x, float y, float w, float h);
void drawTriangle(float x1, float y1, float x2, float y2, float x3, float y3);
void drawGradientRect(float x, float y, float w, float h,
    float r1, float g1, float b1,
    float r2, float g2, float b2);
void drawExtrudedRect(float x, float y, float w, float h,
    float depthX, float depthY,
    float frontR, float frontG, float frontB,
    float sideR, float sideG, float sideB,
    float topR, float topG, float topB);
void applyTranslation(float tx, float ty);
void applyRotationAround(float angle, float cx, float cy);
void applyScalingAround(float sx, float sy, float cx, float cy);
void applyReflectionY(float cx, float cy);
float depthScale(float y);
void drawShadow(float x, float y, float w, float h);

float car1X = -250.0f;
float car2X = 250.0f;
float bus1X = -200.0f;
float bike1X = 300.0f;
float truck1X = -400.0f;

float sunAngle = 60.0f;
float cloud1X = -350.0f;
float cloud2X = 100.0f;
float cloud3X = 250.0f;

int tlPhase = 0;
float tlTimer = 0.0f;
const float TL_RED_DUR = 4.0f;
const float TL_YELLOW_DUR = 1.5f;
const float TL_GREEN_DUR = 3.5f;

bool isNightMode = false;
float dayNightBlend = 0.0f;

void drawSky()
{
    if (isNightMode) {
        glBegin(GL_QUADS);
        glColor3f(0.05f, 0.07f, 0.14f); glVertex2f(-500, -20);
        glVertex2f(500, -20);
        glColor3f(0.02f, 0.03f, 0.07f); glVertex2f(500, 350);
        glVertex2f(-500, 350);
        glEnd();
    }
    else {
        glBegin(GL_QUADS);
        glColor3f(0.95f, 0.84f, 0.66f); glVertex2f(-500, -20);
        glColor3f(0.95f, 0.84f, 0.66f); glVertex2f(500, -20);
        glColor3f(0.75f, 0.47f, 0.38f); glVertex2f(500, 170);
        glColor3f(0.70f, 0.42f, 0.42f); glVertex2f(-500, 170);
        glColor3f(0.70f, 0.42f, 0.42f); glVertex2f(-500, 170);
        glColor3f(0.64f, 0.40f, 0.56f); glVertex2f(500, 170);
        glColor3f(0.42f, 0.50f, 0.76f); glVertex2f(500, 350);
        glColor3f(0.36f, 0.44f, 0.68f); glVertex2f(-500, 350);
        glEnd();
    }
}

void drawSun()
{
    float rad = sunAngle * M_PI / 180.0f;
    float sx = -250.0f + 300.0f * cosf(rad);
    float sy = 0.0f + 180.0f * sinf(rad);

    if (isNightMode) {
        setColor(0.7f, 0.7f, 0.3f);
        midpointCircle(sx, sy, 16, true);
    }
    else {
        setColor(1.0f, 0.8f, 0.2f);
        midpointCircle(sx, sy, 30, true);
        setColor(1.0f, 0.7f, 0.1f);
        midpointCircle(sx, sy, 25, true);
    }
}

void drawCloud(float cx, float cy, float scale)
{
    glPushMatrix();
    applyScalingAround(scale, scale, cx, cy);

    if (isNightMode)
        setColor(0.35f, 0.35f, 0.4f);
    else
        setColor(1.0f, 0.95f, 0.85f);

    midpointCircle(cx, cy, 22, true);
    midpointCircle(cx + 25, cy + 5, 18, true);
    midpointCircle(cx - 22, cy + 5, 16, true);
    midpointCircle(cx + 10, cy + 18, 15, true);
    midpointCircle(cx - 10, cy + 15, 14, true);
    glPopMatrix();
}

void drawRoad()
{
    drawGradientRect(-500, -350, 1000, 180,
        0.23f, 0.45f, 0.20f,
        0.32f, 0.58f, 0.28f);

    setColor(0.18f, 0.20f, 0.24f);
    drawRect(-500, -76, 1000, 6);

    glBegin(GL_QUADS);
    glColor3f(0.13f, 0.13f, 0.14f); glVertex2f(-500, -220);
    glColor3f(0.13f, 0.13f, 0.14f); glVertex2f(500, -220);
    glColor3f(0.26f, 0.26f, 0.28f); glVertex2f(320, -72);
    glColor3f(0.26f, 0.26f, 0.28f); glVertex2f(-320, -72);
    glEnd();

    glBegin(GL_QUADS);
    glColor3f(0.24f, 0.25f, 0.26f); glVertex2f(-500, -220);
    glColor3f(0.24f, 0.25f, 0.26f); glVertex2f(500, -220);
    glColor3f(0.18f, 0.19f, 0.21f); glVertex2f(340, -58);
    glColor3f(0.18f, 0.19f, 0.21f); glVertex2f(-340, -58);
    glEnd();

    setColor(0.55f, 0.56f, 0.58f);
    drawRect(-500, -225, 1000, 5);
    drawRect(-340, -58, 680, 4);

    setColor(1.0f, 0.86f, 0.18f);
    for (int i = 0; i < 9; ++i) {
        float t = i / 8.0f;
        float dashWidth = 20.0f + t * 28.0f;
        float dashY = -195.0f + t * 145.0f;
        drawRect(-dashWidth * 0.5f, dashY, dashWidth, 6);
    }

    setColor(0.96f, 0.96f, 0.96f);
    for (int i = 0; i < 8; ++i) {
        float t = i / 7.0f;
        float width = 18.0f + t * 26.0f;
        float xOffset = 105.0f + t * 55.0f;
        float y = -186.0f + t * 138.0f;
        drawRect(-xOffset - width * 0.5f, y, width, 5);
        drawRect(xOffset - width * 0.5f, y, width, 5);
    }

    for (int x = -450; x < 500; x += 150) {
        drawExtrudedRect(x - 2, -145, 4, 25, 3, 2,
            0.20f, 0.20f, 0.22f,
            0.12f, 0.12f, 0.14f,
            0.28f, 0.28f, 0.30f);

        if (isNightMode)
            setColor(1.0f, 0.92f, 0.60f);
        else
            setColor(0.90f, 0.90f, 0.86f);
        drawExtrudedRect(x - 8, -120, 16, 8, 4, 2,
            0.88f, 0.88f, 0.82f,
            0.55f, 0.55f, 0.50f,
            0.98f, 0.98f, 0.90f);

        if (isNightMode) {
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            setColorAlpha(1.0f, 0.95f, 0.5f, 0.2f);
            midpointCircle(x, -116, 28, true);
            glDisable(GL_BLEND);
        }
    }
}

void drawZebraCrossing(float x, float y, float width)
{
    float stripeWidth = width / 12.0f;
    setColor(1.0f, 1.0f, 1.0f);
    for (int i = 0; i < 12; i += 2) {
        drawRect(x + i * stripeWidth, y, stripeWidth, 40);
    }
}

void drawBuilding(float x, float y, float w, float h,
    float r, float g, float b,
    int wCols, int wRows,
    float scale = 1.0f)
{
    glPushMatrix();
    applyTranslation(x, y);
    glScalef(scale, scale, 1.0f);

    float depthX = 16.0f + w * 0.10f;
    float depthY = 10.0f;
    float facadeR = r * 0.98f;
    float facadeG = g * 0.95f;
    float facadeB = b * 0.92f;
    float sideR = r * 0.66f;
    float sideG = g * 0.62f;
    float sideB = b * 0.58f;
    float topR = r * 1.06f;
    float topG = g * 1.02f;
    float topB = b * 0.98f;

    drawExtrudedRect(0, 0, w, h, depthX, depthY,
        facadeR, facadeG, facadeB,
        sideR, sideG, sideB,
        topR, topG, topB);

    setColor(r * 0.55f, g * 0.52f, b * 0.48f);
    drawRect(w - 1, 0, 2, h);
    drawShadow(-4, -6, w + depthX + 12, 10);

    setColor(r * 0.90f, g * 0.85f, b * 0.80f);
    drawRect(-2, h, w + depthX + 6, 6);

    setColor(r * 0.76f, g * 0.71f, b * 0.66f);
    drawRect(3, h + 6, w + depthX - 2, 4);

    setColor(r * 0.72f, g * 0.66f, b * 0.60f);
    drawTriangle(0, h, w / 2.0f + depthX * 0.25f, h + 12, w + depthX, h + depthY);

    setColor(r * 0.85f, g * 0.82f, b * 0.78f);
    for (int stripe = 1; stripe < 4; ++stripe) {
        float sx = w * stripe / 4.0f;
        ddaLine(sx, 0, sx + depthX * 0.18f, h + depthY * 0.16f);
    }

    float ww = w / (wCols * 2.0f + 1.0f);
    float wh = h / (wRows * 2.5f + 1.0f);

    for (int row = 0; row < wRows; row++) {
        for (int col = 0; col < wCols; col++) {
            float wx = ww * (1 + col * 2);
            float wy = wh * (1 + row * 2);
            float windowPattern = fmodf((float)(row * 3 + col + (int)(sunAngle * 2.0f)), 5.0f);
            bool litWindow = isNightMode && windowPattern < 2.5f;

            if (litWindow)
                setColor(1.0f, 0.92f, 0.58f);
            else if (isNightMode)
                setColor(0.16f, 0.18f, 0.22f);
            else
                setColor(0.72f, 0.87f, 0.98f);
            drawRect(wx, wy, ww, wh);

            if (!isNightMode || litWindow) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                setColorAlpha(1.0f, 1.0f, 1.0f, 0.20f);
                drawRect(wx + 2, wy + wh - 3, ww / 3.0f, 2);
                glDisable(GL_BLEND);
            }

            if (litWindow) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE);
                setColorAlpha(1.0f, 0.82f, 0.35f, 0.12f);
                midpointCircle(wx + ww * 0.5f, wy + wh * 0.5f, fmaxf(ww, wh) * 0.75f, true);
                glDisable(GL_BLEND);
            }

            setColor(0.25f, 0.25f, 0.25f);
            glPointSize(1.5f);
            bresenhamLine((int)wx, (int)wy,
                (int)(wx + ww), (int)wy);
            bresenhamLine((int)(wx + ww), (int)wy,
                (int)(wx + ww), (int)(wy + wh));
            bresenhamLine((int)(wx + ww), (int)(wy + wh),
                (int)wx, (int)(wy + wh));
            bresenhamLine((int)wx, (int)(wy + wh),
                (int)wx, (int)wy);
        }
    }

    setColor(0.22f, 0.22f, 0.24f);
    drawRect(w * 0.15f, h + 10, 6, 16);
    drawRect(w * 0.62f, h + 10, 6, 12);
    ddaLine(w * 0.15f + 3, h + 26, w * 0.15f - 4, h + 34);
    ddaLine(w * 0.62f + 3, h + 22, w * 0.62f + 9, h + 32);

    glPopMatrix();
}

void drawTrafficLight(float x, float y)
{
    drawExtrudedRect(x - 3, y, 6, 85, 2, 2,
        0.24f, 0.24f, 0.26f,
        0.12f, 0.12f, 0.14f,
        0.30f, 0.30f, 0.32f);

    drawExtrudedRect(x - 14, y + 82, 28, 70, 5, 3,
        0.12f, 0.12f, 0.13f,
        0.06f, 0.06f, 0.07f,
        0.18f, 0.18f, 0.19f);

    glPointSize(2.0f);

    float redIntensity = (tlPhase == 0) ? 1.0f : 0.2f;
    setColor(redIntensity, 0.0f, 0.0f);
    midpointCircle(x, y + 135, 10, true);
    if (tlPhase == 0) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(1.0f, 0.2f, 0.2f, 0.3f);
        midpointCircle(x, y + 135, 16, true);
        glDisable(GL_BLEND);
    }

    float yellowIntensity = (tlPhase == 1) ? 1.0f : 0.2f;
    setColor(yellowIntensity, yellowIntensity * 0.8f, 0.0f);
    midpointCircle(x, y + 115, 10, true);
    if (tlPhase == 1) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(1.0f, 0.95f, 0.2f, 0.3f);
        midpointCircle(x, y + 115, 16, true);
        glDisable(GL_BLEND);
    }

    float greenIntensity = (tlPhase == 2) ? 1.0f : 0.2f;
    setColor(0.0f, greenIntensity, 0.0f);
    midpointCircle(x, y + 95, 10, true);
    if (tlPhase == 2) {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(0.2f, 1.0f, 0.2f, 0.3f);
        midpointCircle(x, y + 95, 16, true);
        glDisable(GL_BLEND);
    }
}

void drawCar(float cx, float cy, float r, float g, float b, bool facingLeft = false, float scale = 1.0f)
{
    glPushMatrix();
    applyTranslation(cx, cy);
    glScalef(scale, scale, 1.0f);

    if (facingLeft)
        applyReflectionY(0, 0);

    drawExtrudedRect(-35, -12, 70, 22, 8, 4,
        r * 1.02f, g * 1.02f, b * 1.02f,
        r * 0.70f, g * 0.70f, b * 0.72f,
        r * 1.10f, g * 1.08f, b * 1.08f);

    drawExtrudedRect(-22, 10, 44, 18, 6, 3,
        r * 0.88f, g * 0.88f, b * 0.90f,
        r * 0.60f, g * 0.60f, b * 0.62f,
        r * 1.02f, g * 1.02f, b * 1.02f);

    setColor(0.65f, 0.88f, 1.0f);
    drawRect(-21, 11, 18, 15);
    drawRect(3, 11, 18, 15);

    setColor(0.1f, 0.1f, 0.1f);
    midpointCircle(-20, -12, 10, true);
    midpointCircle(20, -12, 10, true);
    setColor(0.5f, 0.5f, 0.5f);
    midpointCircle(-20, -12, 5, true);
    midpointCircle(20, -12, 5, true);

    setColor(1.0f, 1.0f, 0.65f);
    drawRect(33, -6, 4, 8);
    setColor(0.9f, 0.1f, 0.1f);
    drawRect(-37, -6, 4, 8);

    drawShadow(cx - 35, cy - 20, 70, 4);

    glPopMatrix();
}

void drawBus(float cx, float cy, float r, float g, float b, bool facingLeft = false, float scale = 1.0f)
{
    glPushMatrix();
    applyTranslation(cx, cy);
    glScalef(scale, scale, 1.0f);

    if (facingLeft)
        applyReflectionY(0, 0);

    drawExtrudedRect(-50, -15, 100, 28, 10, 5,
        r * 1.00f, g * 1.00f, b * 1.00f,
        r * 0.68f, g * 0.68f, b * 0.70f,
        r * 1.08f, g * 1.08f, b * 1.10f);

    drawExtrudedRect(-48, 13, 25, 20, 5, 3,
        r * 0.88f, g * 0.88f, b * 0.90f,
        r * 0.60f, g * 0.60f, b * 0.62f,
        r * 1.02f, g * 1.02f, b * 1.02f);

    setColor(0.55f, 0.83f, 1.0f);
    drawRect(-46, 14, 10, 10);
    drawRect(-32, 14, 10, 10);

    for (int i = -30; i < 40; i += 20) {
        drawRect(i, 13, 15, 12);
    }

    setColor(0.1f, 0.1f, 0.1f);
    midpointCircle(-30, -15, 11, true);
    midpointCircle(30, -15, 11, true);
    setColor(0.5f, 0.5f, 0.5f);
    midpointCircle(-30, -15, 6, true);
    midpointCircle(30, -15, 6, true);

    setColor(1.0f, 1.0f, 0.7f);
    drawRect(48, -8, 5, 10);

    drawShadow(cx - 50, cy - 20, 100, 5);

    glPopMatrix();
}

void drawBike(float cx, float cy, float r, float g, float b, bool facingLeft = false, float scale = 1.0f)
{
    glPushMatrix();
    applyTranslation(cx, cy);
    glScalef(scale, scale, 1.0f);

    if (facingLeft)
        applyReflectionY(0, 0);

    drawExtrudedRect(-18, -8, 36, 16, 5, 3,
        r * 1.02f, g * 1.02f, b * 1.02f,
        r * 0.62f, g * 0.62f, b * 0.65f,
        r * 1.08f, g * 1.08f, b * 1.08f);

    setColor(0.35f, 0.25f, 0.15f);
    midpointCircle(-2, 12, 6, true);
    drawRect(-5, 8, 6, 4);

    setColor(0.05f, 0.05f, 0.05f);
    midpointCircle(-12, -8, 8, true);
    midpointCircle(12, -8, 8, true);
    setColor(0.45f, 0.45f, 0.45f);
    midpointCircle(-12, -8, 4, true);
    midpointCircle(12, -8, 4, true);

    setColor(1.0f, 1.0f, 0.55f);
    drawRect(16, -3, 3, 6);

    drawShadow(cx - 18, cy - 15, 36, 3);

    glPopMatrix();
}

void drawTruck(float cx, float cy, float r, float g, float b, bool facingLeft = false, float scale = 1.0f)
{
    glPushMatrix();
    applyTranslation(cx, cy);
    glScalef(scale, scale, 1.0f);

    if (facingLeft)
        applyReflectionY(0, 0);

    drawExtrudedRect(-45, -12, 30, 24, 6, 3,
        r * 1.02f, g * 1.02f, b * 1.02f,
        r * 0.62f, g * 0.62f, b * 0.64f,
        r * 1.08f, g * 1.08f, b * 1.08f);

    drawExtrudedRect(-15, -10, 60, 20, 8, 4,
        r * 0.92f, g * 0.92f, b * 0.92f,
        r * 0.62f, g * 0.62f, b * 0.64f,
        r * 1.04f, g * 1.04f, b * 1.04f);

    setColor(0.65f, 0.88f, 1.0f);
    drawRect(-42, 0, 12, 12);

    setColor(r * 0.7f, g * 0.7f, b * 0.7f);
    glPointSize(2.0f);
    ddaLine(-13, 0, 43, 0);
    ddaLine(-13, 5, 43, 5);

    setColor(0.1f, 0.1f, 0.1f);
    midpointCircle(-25, -12, 11, true);
    midpointCircle(15, -12, 11, true);
    midpointCircle(35, -12, 11, true);
    setColor(0.5f, 0.5f, 0.5f);
    midpointCircle(-25, -12, 6, true);
    midpointCircle(15, -12, 6, true);
    midpointCircle(35, -12, 6, true);

    setColor(1.0f, 1.0f, 0.7f);
    drawRect(43, -8, 5, 10);
    setColor(0.9f, 0.1f, 0.1f);
    drawRect(-48, -8, 5, 10);

    drawShadow(cx - 45, cy - 20, 105, 5);

    glPopMatrix();
}

void drawTree(float x, float y, float scale)
{
    glPushMatrix();
    applyTranslation(x, y);
    applyScalingAround(scale, scale, 0, 0);

    setColor(0.42f, 0.28f, 0.08f);
    drawRect(-6, 0, 12, 45);

    setColor(0.25f, 0.58f, 0.2f);
    midpointCircle(0, 70, 35, true);

    setColor(0.3f, 0.65f, 0.25f);
    midpointCircle(-20, 55, 28, true);
    midpointCircle(20, 55, 28, true);

    setColor(0.35f, 0.7f, 0.3f);
    midpointCircle(-28, 40, 20, true);
    midpointCircle(28, 40, 20, true);

    drawShadow(x - 30 * scale, y - 5, 60 * scale, 4);

    glPopMatrix();
}

void drawPedestrian(float x, float y, float scale)
{
    glPushMatrix();
    applyTranslation(x, y);
    applyScalingAround(scale, scale, 0, 0);

    setColor(0.85f, 0.65f, 0.45f);
    midpointCircle(0, 25, 5, true);

    setColor(0.35f, 0.4f, 0.55f);
    drawRect(-4, 12, 8, 12);

    setColor(0.25f, 0.25f, 0.25f);
    drawRect(-3, 0, 3, 12);
    drawRect(0, 0, 3, 12);

    drawRect(-6, 10, 3, 8);
    drawRect(3, 10, 3, 8);

    glPopMatrix();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();
    drawSun();

    drawCloud(cloud1X, 230, 1.0f);
    drawCloud(cloud2X, 270, 0.9f);
    drawCloud(cloud3X, 210, 0.8f);

    drawBuilding(-470, -5, 55, 150, 0.34f, 0.34f, 0.40f, 2, 4, 0.72f);
    drawBuilding(-395, -12, 45, 135, 0.31f, 0.33f, 0.38f, 2, 4, 0.68f);
    drawBuilding(-300, 0, 60, 165, 0.36f, 0.36f, 0.42f, 2, 5, 0.74f);
    drawBuilding(295, -8, 52, 155, 0.33f, 0.34f, 0.40f, 2, 4, 0.70f);
    drawBuilding(395, -16, 64, 175, 0.35f, 0.36f, 0.43f, 2, 5, 0.76f);

    drawBuilding(-420, -30, 75, 200, 0.72f, 0.55f, 0.45f, 2, 5, depthScale(-30));
    drawBuilding(-330, -40, 85, 240, 0.68f, 0.58f, 0.48f, 2, 5, depthScale(-40));
    drawBuilding(-220, -30, 70, 210, 0.75f, 0.60f, 0.50f, 2, 4, depthScale(-30));
    drawBuilding(-130, -60, 95, 280, 0.70f, 0.62f, 0.50f, 3, 6, depthScale(-60));
    drawBuilding(-15, -40, 65, 220, 0.73f, 0.58f, 0.48f, 2, 4, depthScale(-40));
    drawBuilding(70, -80, 90, 320, 0.76f, 0.62f, 0.50f, 3, 7, depthScale(-80));
    drawBuilding(180, -50, 80, 250, 0.71f, 0.60f, 0.48f, 2, 5, depthScale(-50));
    drawBuilding(280, -70, 100, 290, 0.74f, 0.61f, 0.49f, 3, 6, depthScale(-70));
    drawBuilding(400, -40, 85, 240, 0.70f, 0.58f, 0.47f, 2, 5, depthScale(-40));
    drawBuilding(445, -120, 78, 280, 0.60f, 0.54f, 0.46f, 2, 6, 1.05f);

    drawRoad();

    drawZebraCrossing(-200, -230, 80);
    drawZebraCrossing(200, -230, 80);

    drawPedestrian(-280, -300, 1.0f * depthScale(-300));
    drawPedestrian(-150, -310, 0.9f * depthScale(-310));
    drawPedestrian(80, -305, 1.0f * depthScale(-305));
    drawPedestrian(250, -295, 0.95f * depthScale(-295));

    drawCar(car1X, -100, 0.85f, 0.25f, 0.25f, false, depthScale(-100));
    drawCar(car2X, -180, 0.25f, 0.45f, 0.80f, true, depthScale(-180));
    drawBus(bus1X, -140, 0.75f, 0.25f, 0.25f, false, depthScale(-140));
    drawBike(bike1X, -90, 0.85f, 0.55f, 0.25f, true, depthScale(-90));
    drawTruck(truck1X, -170, 0.65f, 0.40f, 0.20f, false, depthScale(-170));

    drawTree(-380, -240, 1.0f * depthScale(-240));
    drawTree(-200, -240, 1.1f * depthScale(-240));
    drawTree(0, -240, 0.95f * depthScale(-240));
    drawTree(200, -240, 1.0f * depthScale(-240));
    drawTree(380, -240, 0.9f * depthScale(-240));

    drawTrafficLight(-150, -240);
    drawTrafficLight(150, -240);
}

void update(float dt)
{
    car1X += 75.0f * dt;
    if (car1X > 550.0f) car1X = -550.0f;

    car2X -= 65.0f * dt;
    if (car2X < -550.0f) car2X = 550.0f;

    if (tlPhase != 0) {
        bus1X += 50.0f * dt;
    }
    if (bus1X > 550.0f) bus1X = -550.0f;

    bike1X -= 100.0f * dt;
    if (bike1X < -550.0f) bike1X = 550.0f;

    if (tlPhase != 0) {
        truck1X += 55.0f * dt;
    }
    if (truck1X > 550.0f) truck1X = -550.0f;

    sunAngle += 0.5f * dt;
    if (sunAngle > 150.0f) sunAngle = 10.0f;

    cloud1X += 12.0f * dt;
    if (cloud1X > 550.0f) cloud1X = -550.0f;

    cloud2X += 8.0f * dt;
    if (cloud2X > 550.0f) cloud2X = -550.0f;

    cloud3X += 10.0f * dt;
    if (cloud3X > 550.0f) cloud3X = -550.0f;

    tlTimer += dt;
    float dur = (tlPhase == 0) ? TL_RED_DUR :
        (tlPhase == 1) ? TL_YELLOW_DUR : TL_GREEN_DUR;
    if (tlTimer >= dur) {
        tlTimer = 0.0f;
        tlPhase = (tlPhase + 1) % 3;
    }

    if (isNightMode) {
        dayNightBlend = fminf(1.0f, dayNightBlend + dt * 0.5f);
    }
    else {
        dayNightBlend = fmaxf(0.0f, dayNightBlend - dt * 0.5f);
    }
}
