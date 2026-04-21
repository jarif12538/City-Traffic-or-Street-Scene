//rafi
//---------------------------
#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>

const int W = 1000, H = 700;

//  Global animation state
float car1X      = -250.0f;
float car2X      =  250.0f;
float bus1X      = -200.0f;
float bike1X     =  300.0f;
float truck1X    = -400.0f;

float sunAngle   =  60.0f;    
float cloud1X    = -350.0f;
float cloud2X    =  100.0f;
float cloud3X    =  250.0f;

int   tlPhase    =  0;
float tlTimer    =  0.0f;
const float TL_RED_DUR    = 4.0f;
const float TL_YELLOW_DUR = 1.5f;
const float TL_GREEN_DUR  = 3.5f;

bool isNightMode = false;
float dayNightBlend = 0.0f;

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

//  ALGORITHM 1 – DDA Line

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

//  ALGORITHM 2 – Bresenham Line

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

//  ALGORITHM 3 – Midpoint Circle

void midpointCircle(float cx, float cy, float r, bool filled=false)
{
    if(filled){
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

//  Solid polygon helpers

void drawRect(float x, float y, float w, float h)
{
    glBegin(GL_QUADS);
    glVertex2f(x,   y);
    glVertex2f(x+w, y);
    glVertex2f(x+w, y+h);
    glVertex2f(x,   y+h);
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
    glColor3f(r1, g1, b1); glVertex2f(x,   y);
    glColor3f(r2, g2, b2); glVertex2f(x+w, y);
    glColor3f(r2, g2, b2); glVertex2f(x+w, y+h);
    glColor3f(r1, g1, b1); glVertex2f(x,   y+h);
    glEnd();
}

//  2D TRANSFORMATION UTILITIES

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


//  this is our shado effect

void drawShadow(float x, float y, float w, float h)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0, 0, 0, 0.15f);
    drawRect(x, y - 5, w, 4);
    glDisable(GL_BLEND);
}
 //rafi end



//shakib start----------------------------------------
// sky seen
void drawSky()
{
    if(isNightMode){
        
        glBegin(GL_QUADS);
        glColor3f(0.1f, 0.1f, 0.2f); glVertex2f(-500, -20);
        glVertex2f(500, -20);
        glColor3f(0.05f, 0.05f, 0.15f); glVertex2f(500, 350);
        glVertex2f(-500, 350);
        glEnd();
    } else {
        
        glBegin(GL_QUADS);
        glColor3f(1.0f, 0.9f, 0.7f); glVertex2f(-500, -20);   
        glColor3f(1.0f, 0.9f, 0.7f); glVertex2f(500, -20);
        glColor3f(1.0f, 0.6f, 0.3f); glVertex2f(500, 200);    
        glColor3f(0.9f, 0.5f, 0.4f); glVertex2f(-500, 200);

        glColor3f(0.9f, 0.5f, 0.4f); glVertex2f(-500, 200);
        glColor3f(0.9f, 0.5f, 0.4f); glVertex2f(500, 200);
        glColor3f(0.6f, 0.4f, 0.7f); glVertex2f(500, 350);   
        glColor3f(0.6f, 0.4f, 0.7f); glVertex2f(-500, 350);
        glEnd();
    }
}

// Sun
void drawSun()
{
    float rad = sunAngle * M_PI / 180.0f;
    float sx  = -250.0f + 300.0f * cosf(rad);
    float sy  = 0.0f + 180.0f * sinf(rad);

    if(isNightMode){
        setColor(0.7f, 0.7f, 0.3f);
        midpointCircle(sx, sy, 16, true);
    } else {
        
        setColor(1.0f, 0.8f, 0.2f);
        midpointCircle(sx, sy, 30, true);
        setColor(1.0f, 0.7f, 0.1f);
        midpointCircle(sx, sy, 25, true);
    }
}

// our clouds
void drawCloud(float cx, float cy, float scale)
{
    glPushMatrix();
    applyScalingAround(scale, scale, cx, cy);

    if(isNightMode)
        setColor(0.35f, 0.35f, 0.4f);
    else
        setColor(1.0f, 0.95f, 0.85f);  

    midpointCircle(cx,      cy,     22, true);
    midpointCircle(cx+25,   cy+5,   18, true);
    midpointCircle(cx-22,   cy+5,   16, true);
    midpointCircle(cx+10,   cy+18,  15, true);
    midpointCircle(cx-10,   cy+15,  14, true);
    glPopMatrix();
}

//  road 
void drawRoad()
{
   
    setColor(0.35f, 0.65f, 0.25f); 
    drawRect(-500, -350, 1000, 180);

    drawGradientRect(-500, -220, 1000, 170,
                     0.4f, 0.4f, 0.38f,
                     0.35f, 0.35f, 0.32f);

    setColor(0.72f, 0.72f, 0.7f);
    drawRect(-500, -225, 1000, 8);
    drawRect(-500, -50,   1000, 8);

    
    setColor(1.0f, 0.88f, 0.1f);
    glPointSize(3.5f);
    bresenhamLine(-500, -138, 500, -138);

    setColor(1.0f, 1.0f, 1.0f);
    glPointSize(2.5f);
    for(int x = -500; x < 500; x += 70){
        float offset = (x + 500) * 0.02f;
        ddaLine((float)x, -175.0f + offset,
                (float)(x+40), -175.0f + offset);
        ddaLine((float)x, -100.0f + offset,
                (float)(x+40), -100.0f + offset);
    }

    for(int x = -450; x < 500; x += 150){
        setColor(0.25f, 0.25f, 0.25f);
        drawRect(x - 2, -145, 4, 25);

        if(isNightMode)
            setColor(1.0f, 0.95f, 0.6f);
        else
            setColor(0.85f, 0.85f, 0.8f);
        drawRect(x - 6, -120, 12, 8);

        if(isNightMode){
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
    for(int i = 0; i < 12; i += 2){
        drawRect(x + i*stripeWidth, y, stripeWidth, 40);
    }
}

//  buildings
void drawBuilding(float x, float y, float w, float h,
                  float r, float g, float b,
                  int wCols, int wRows)
{
    glPushMatrix();

    // Warm-toned gradient body
    drawGradientRect(x, y, w, h,
                     r*1.1f, g*1.05f, b*0.95f,
                     r*0.8f, g*0.8f, b*0.75f);

    // Roof accent
    setColor(r*0.9f, g*0.85f, b*0.8f);
    drawRect(x-2, y+h, w+4, 6);

    // Peaked roof for modern look
    setColor(r*0.7f, g*0.65f, b*0.6f);
    drawTriangle(x, y+h, x+w/2.0f, y+h+12, x+w, y+h);

    // Windows with subtle glass effect
    float ww = w / (wCols*2.0f+1.0f);
    float wh = h / (wRows*2.5f+1.0f);

    for(int row=0; row<wRows; row++){
        for(int col=0; col<wCols; col++){
            float wx = x + ww*(1+col*2);
            float wy = y + wh*(1+row*2);

            // Glass color
            if(isNightMode)
                setColor(0.95f, 0.92f, 0.55f);
            else
                setColor(0.75f, 0.88f, 0.98f);
            drawRect(wx, wy, ww, wh);

            // Reflection
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE);
            setColorAlpha(1.0f, 1.0f, 1.0f, 0.25f);
            drawRect(wx + 2, wy + wh - 3, ww/3.0f, 2);
            glDisable(GL_BLEND);

            // Border
            setColor(0.25f, 0.25f, 0.25f);
            glPointSize(1.5f);
            bresenhamLine((int)wx, (int)wy,
                          (int)(wx+ww), (int)wy);
            bresenhamLine((int)(wx+ww), (int)wy,
                          (int)(wx+ww), (int)(wy+wh));
            bresenhamLine((int)(wx+ww), (int)(wy+wh),
                          (int)wx, (int)(wy+wh));
            bresenhamLine((int)wx, (int)(wy+wh),
                          (int)wx, (int)wy);
        }
    }

    glPopMatrix();
}

// Traffic light
void drawTrafficLight(float x, float y)
{
    setColor(0.25f, 0.25f, 0.25f);
    drawRect(x-3, y, 6, 85);

    setColor(0.12f, 0.12f, 0.12f);
    drawRect(x-14, y+82, 28, 70);

    glPointSize(2.0f);

    // Red
    float redIntensity = (tlPhase==0) ? 1.0f : 0.2f;
    setColor(redIntensity, 0.0f, 0.0f);
    midpointCircle(x, y+135, 10, true);
    if(tlPhase==0){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(1.0f, 0.2f, 0.2f, 0.3f);
        midpointCircle(x, y+135, 16, true);
        glDisable(GL_BLEND);
    }

    // Yellow
    float yellowIntensity = (tlPhase==1) ? 1.0f : 0.2f;
    setColor(yellowIntensity, yellowIntensity*0.8f, 0.0f);
    midpointCircle(x, y+115, 10, true);
    if(tlPhase==1){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(1.0f, 0.95f, 0.2f, 0.3f);
        midpointCircle(x, y+115, 16, true);
        glDisable(GL_BLEND);
    }

    // Green
    float greenIntensity = (tlPhase==2) ? 1.0f : 0.2f;
    setColor(0.0f, greenIntensity, 0.0f);
    midpointCircle(x, y+95, 10, true);
    if(tlPhase==2){
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        setColorAlpha(0.2f, 1.0f, 0.2f, 0.3f);
        midpointCircle(x, y+95, 16, true);
        glDisable(GL_BLEND);
    }
}

// Car
void drawCar(float cx, float cy, float r, float g, float b, bool facingLeft=false)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    if(facingLeft)
        applyReflectionY(0, 0);

    setColor(r, g, b);
    drawRect(-35, -12, 70, 22);

    setColor(r*0.85f, g*0.85f, b*0.85f);
    drawRect(-22, 10, 44, 18);

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

// Bus
void drawBus(float cx, float cy, float r, float g, float b, bool facingLeft=false)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    if(facingLeft)
        applyReflectionY(0, 0);

    setColor(r, g, b);
    drawRect(-50, -15, 100, 28);

    setColor(r*0.85f, g*0.85f, b*0.85f);
    drawRect(-48, 13, 25, 20);

    setColor(0.55f, 0.83f, 1.0f);
    drawRect(-46, 14, 10, 10);
    drawRect(-32, 14, 10, 10);

    for(int i = -30; i < 40; i += 20){
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

// Bike
void drawBike(float cx, float cy, float r, float g, float b, bool facingLeft=false)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    if(facingLeft)
        applyReflectionY(0, 0);

    setColor(r, g, b);
    drawRect(-18, -8, 36, 16);

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

// Truck
void drawTruck(float cx, float cy, float r, float g, float b, bool facingLeft=false)
{
    glPushMatrix();
    applyTranslation(cx, cy);

    if(facingLeft)
        applyReflectionY(0, 0);

    setColor(r, g, b);
    drawRect(-45, -12, 30, 24);

    setColor(r*0.9f, g*0.9f, b*0.9f);
    drawRect(-15, -10, 60, 20);

    setColor(0.65f, 0.88f, 1.0f);
    drawRect(-42, 0, 12, 12);

    setColor(r*0.7f, g*0.7f, b*0.7f);
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

// Tree
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

    drawShadow(x - 30*scale, y - 5, 60*scale, 4);

    glPopMatrix();
}

// Pedestrian
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
//shakib end----------------------------------------

//  RENDER – assemble full scene
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    drawSky();
    drawSun();

    // Clouds
    drawCloud(cloud1X, 230, 1.0f);
    drawCloud(cloud2X, 270, 0.9f);
    drawCloud(cloud3X, 210, 0.8f);

    // Buildings 
    drawBuilding(-420, -30, 75, 200, 0.72f, 0.55f, 0.45f, 2, 5);
    drawBuilding(-330, -40, 85, 240, 0.68f, 0.58f, 0.48f, 2, 5);
    drawBuilding(-220, -30, 70, 210, 0.75f, 0.60f, 0.50f, 2, 4);
    drawBuilding(-130, -60, 95, 280, 0.70f, 0.62f, 0.50f, 3, 6);
    drawBuilding(-15, -40, 65, 220, 0.73f, 0.58f, 0.48f, 2, 4);
    drawBuilding(70, -80, 90, 320, 0.76f, 0.62f, 0.50f, 3, 7);
    drawBuilding(180, -50, 80, 250, 0.71f, 0.60f, 0.48f, 2, 5);
    drawBuilding(280, -70, 100, 290, 0.74f, 0.61f, 0.49f, 3, 6);
    drawBuilding(400, -40, 85, 240, 0.70f, 0.58f, 0.47f, 2, 5);

    drawRoad();

    // Zebra crossings
    drawZebraCrossing(-200, -230, 80);
    drawZebraCrossing(200, -230, 80);

    // Trees
    drawTree(-380, -240, 1.0f);
    drawTree(-200, -240, 1.1f);
    drawTree(0, -240, 0.95f);
    drawTree(200, -240, 1.0f);
    drawTree(380, -240, 0.9f);

    // Pedestrians
    drawPedestrian(-280, -300, 1.0f);
    drawPedestrian(-150, -310, 0.9f);
    drawPedestrian(80, -305, 1.0f);
    drawPedestrian(250, -295, 0.95f);

    // Traffic lights
    drawTrafficLight(-150, -240);
    drawTrafficLight(150, -240);

    // Vehicles
    drawCar(car1X, -100, 0.85f, 0.25f, 0.25f, false);
    drawCar(car2X, -180, 0.25f, 0.45f, 0.80f, true);
    drawBus(bus1X, -140, 0.75f, 0.25f, 0.25f, false);
    drawBike(bike1X, -90, 0.85f, 0.55f, 0.25f, true);
    drawTruck(truck1X, -170, 0.65f, 0.40f, 0.20f, false);
}

//  UPDATE  animation state
void update(float dt)
{
    car1X += 75.0f * dt;
    if(car1X > 550.0f) car1X = -550.0f;

    car2X -= 65.0f * dt;
    if(car2X < -550.0f) car2X = 550.0f;

    // Bus stops at red light, continues on green/yellow
    if(tlPhase != 0){  // tlPhase 0 = red, 1 = yellow, 2 = green
        bus1X += 50.0f * dt;
    }
    if(bus1X > 550.0f) bus1X = -550.0f;

    bike1X -= 100.0f * dt;
    if(bike1X < -550.0f) bike1X = 550.0f;

    // Truck stops at red light, continues on green/yellow
    if(tlPhase != 0){  // tlPhase 0 = red, 1 = yellow, 2 = green
        truck1X += 55.0f * dt;
    }
    if(truck1X > 550.0f) truck1X = -550.0f;

    sunAngle += 0.5f * dt;
    if(sunAngle > 150.0f) sunAngle = 10.0f;

    cloud1X += 12.0f * dt;
    if(cloud1X > 550.0f) cloud1X = -550.0f;

    cloud2X += 8.0f * dt;
    if(cloud2X > 550.0f) cloud2X = -550.0f;

    cloud3X += 10.0f * dt;
    if(cloud3X > 550.0f) cloud3X = -550.0f;

    tlTimer += dt;
    float dur = (tlPhase==0) ? TL_RED_DUR :
                (tlPhase==1) ? TL_YELLOW_DUR : TL_GREEN_DUR;
    if(tlTimer >= dur){
        tlTimer = 0.0f;
        tlPhase = (tlPhase + 1) % 3;
    }

    if(isNightMode){
        dayNightBlend = fminf(1.0f, dayNightBlend + dt * 0.5f);
    } else {
        dayNightBlend = fmaxf(0.0f, dayNightBlend - dt * 0.5f);
    }
}

//  GLUT CALLBACKS


const int TIMER_MS = 16;
const float DT = TIMER_MS / 1000.0f;

void display()
{
    render();
    glutSwapBuffers();
}

void timer(int /*value*/)
{
    update(DT);
    glutPostRedisplay();
    glutTimerFunc(TIMER_MS, timer, 0);
}

void reshape(int w, int h)
{
    if(h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500, 500, -350, 350, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    if(key == 27) exit(0);
    if(key == 'n' || key == 'N') isNightMode = !isNightMode;
}

//  MAIN

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("City Traffic Scene");

    glPointSize(2.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500, 500, -350, 350, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}
