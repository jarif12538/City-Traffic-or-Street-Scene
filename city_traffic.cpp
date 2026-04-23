#include <GL/glut.h>
#include <cstdlib>

const int W = 1000;
const int H = 700;

void render();
void update(float dt);
extern bool isNightMode;

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
    if (h == 0) h = 1;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-500, 500, -350, 350, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void keyboard(unsigned char key, int /*x*/, int /*y*/)
{
    if (key == 27) exit(0);
    if (key == 'n' || key == 'N') {
        isNightMode = !isNightMode;
    }
}

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
