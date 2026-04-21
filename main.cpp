/*
 * ============================================================
 *  CITY TRAFFIC SCENE – Main Entry Point
 *  Computer Graphics Project
 * ============================================================
 *  Features implemented
 *  ─────────────────────────────────────────────────────────
 *  Graphics Primitives  : points, lines, polygons, circles
 *  Algorithms           : DDA Line, Bresenham Line,
 *                         Midpoint Circle
 *  2D Transformations   : translation, rotation, scaling,
 *                         reflection, shear
 *  Animated Objects     : moving cars, cycling traffic light,
 *                         moving sun, drifting clouds
 *  Theme                : Day-time city street scene
 * ============================================================
 *  Build (Linux / macOS)
 *    g++ main.cpp scene_components.cpp graphics_algorithms.cpp -o city_traffic -lGL -lGLU -lglut -lm
 *  Build (Windows – MinGW)
 *    g++ main.cpp scene_components.cpp graphics_algorithms.cpp -o city_traffic -lopengl32 -lglu32 -lfreeglut -lm
 * ============================================================
 */

#include "city_traffic.h"

// ──────────────────────────────────────────────
//  Window dimensions
// ──────────────────────────────────────────────
const int W = 900, H = 600;

// ──────────────────────────────────────────────
//  Global animation state
// ──────────────────────────────────────────────
float carX1      = -250.0f;   // car going right (lane 1)
float carX2      =  250.0f;   // car going left  (lane 2)
float sunAngle   =  30.0f;    // degrees from horizon
float cloud1X    = -350.0f;
float cloud2X    =  100.0f;
int   tlPhase    =  0;        // 0=red 1=yellow 2=green
float tlTimer    =  0.0f;
const float TL_RED_DUR    = 3.0f;
const float TL_YELLOW_DUR = 1.0f;
const float TL_GREEN_DUR  = 3.0f;

// ══════════════════════════════════════════════
//  RENDER – assemble full scene
// ══════════════════════════════════════════════
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // ── Sky & atmosphere ──
    drawSky();
    drawSun();

    // ── Clouds ────────────
    drawCloud(cloud1X,  180, 1.0f);
    drawCloud(cloud2X,  220, 0.85f);

    // ── Buildings (background) ────────────────
    // Sheared building (demonstrates shear)
    drawBuilding(-340, -50, 80, 230, 0.55f,0.60f,0.70f, 2,5, true);
    drawBuilding(-240, -50, 70, 200, 0.65f,0.55f,0.60f, 2,4);
    drawBuilding(-150, -50, 90, 260, 0.50f,0.55f,0.65f, 3,6);
    drawBuilding( -40, -50, 60, 180, 0.60f,0.62f,0.58f, 2,4);
    drawBuilding(  50, -50, 85, 240, 0.58f,0.60f,0.68f, 3,5);
    drawBuilding( 170, -50, 75, 210, 0.62f,0.58f,0.55f, 2,5);
    drawBuilding( 270, -50, 95, 280, 0.53f,0.58f,0.67f, 3,6);
    drawBuilding( 385, -50, 65, 195, 0.60f,0.65f,0.60f, 2,4);

    // ── Road ──────────────
    drawRoad();

    // ── Trees on pavement ─
    drawTree(-380, -220, 0.9f);
    drawTree(-200, -220, 1.0f);
    drawTree(  20, -220, 0.95f);
    drawTree( 200, -220, 1.0f);
    drawTree( 380, -220, 0.9f);
    drawTree( 400, -220, 0.9f);

    // ── Traffic lights ────
    drawTrafficLight(-100, -220);
    drawTrafficLight( 100, -220);

    // ── Rotating sign ─────
    // Angle slowly oscillates – using static so it persists
    static float signAngle = 0.0f;
    signAngle += 0.3f;
    drawRotatedSign(-370, -45, signAngle);
    drawRotatedSign( 370, -45, -signAngle);

    // ── Cars ──────────────
    // Car 1 – moving right, upper lane
    drawCar(carX1, -100, 0.85f, 0.15f, 0.15f, false);
    // Car 2 – moving left,  lower lane  (reflected)
    drawCar(carX2, -170, 0.15f, 0.35f, 0.75f, true);
    // Parked car (scaled down – scaling transform)
    glPushMatrix();
    applyScalingAround(0.8f, 0.8f, -250, -185);
    drawCar(-250, -185, 0.2f, 0.65f, 0.2f, false);
    glPopMatrix();
}

// ══════════════════════════════════════════════
//  UPDATE – advance animation state
// ══════════════════════════════════════════════
void update(float dt)
{
    // Cars
    carX1 += 80.0f * dt;
    if(carX1 > 500.0f) carX1 = -500.0f;

    carX2 -= 60.0f * dt;
    if(carX2 < -500.0f) carX2 = 500.0f;

    // Sun arc
    sunAngle += 2.0f * dt;
    if(sunAngle > 150.0f) sunAngle = 10.0f;

    // Clouds drift
    cloud1X += 15.0f * dt;
    if(cloud1X > 500.0f) cloud1X = -500.0f;
    cloud2X += 10.0f * dt;
    if(cloud2X > 500.0f) cloud2X = -500.0f;

    // Traffic light cycle
    tlTimer += dt;
    float dur = (tlPhase==0) ? TL_RED_DUR :
                (tlPhase==1) ? TL_YELLOW_DUR : TL_GREEN_DUR;
    if(tlTimer >= dur){
        tlTimer = 0.0f;
        tlPhase = (tlPhase + 1) % 3;
    }
}

// ══════════════════════════════════════════════
//  GLUT CALLBACKS
// ══════════════════════════════════════════════

// Fixed timestep: ~60 fps  (16 ms per tick)
const int TIMER_MS = 16;
const float DT     = TIMER_MS / 1000.0f;

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
    glOrtho(-450, 450, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    if(key == 27) exit(0);   // ESC to quit
}

// ══════════════════════════════════════════════
//  MAIN
// ══════════════════════════════════════════════
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(W, H);
    glutInitWindowPosition(100, 80);
    glutCreateWindow("City Traffic or Street Scene");

    // OpenGL state
    glPointSize(2.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Set projection once
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-450, 450, -300, 300, -1, 1);
    glMatrixMode(GL_MODELVIEW);

    // Register callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(TIMER_MS, timer, 0);

    glutMainLoop();
    return 0;
}
