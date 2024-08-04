#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>

void setPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void lineBres(int x0, int y0, int xEnd, int yEnd) {
    int dx = abs(xEnd - x0);
    int dy = abs(yEnd - y0);
    int p = 2 * dy - dx;
    int twoDy = 2 * dy, twoDyMinusDx = 2 * (dy - dx);
    int x, y;

    if (x0 > xEnd) {
        x = xEnd;
        y = yEnd;
        xEnd = x0;
    } else {
        x = x0;
        y = y0;
    }

    setPixel(x, y);

    while (x < xEnd) {
        x++;
        if (p < 0) {
            p += twoDy;
        } else {
            y += (y0 < yEnd) ? 1 : -1;
            p += twoDyMinusDx;
        }
        setPixel(x, y);
    }
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Example line from (50, 50) to (200, 200)
    lineBres(0, 0, 200, 200);

    glFlush();
}

void init(void) {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 300.0, 0.0, 300.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 300);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bresenham's Line Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
