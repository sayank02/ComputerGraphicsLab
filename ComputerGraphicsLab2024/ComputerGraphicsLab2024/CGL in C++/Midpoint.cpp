#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>

class screenPt {
private:
    GLint x, y;
public:
    // Default Constructor: initializes coordinate position to (0, 0).
    screenPt() : x(0), y(0) {}

    void setCoords(GLint xCoordValue, GLint yCoordValue) {
        x = xCoordValue;
        y = yCoordValue;
    }

    GLint getx() const {
        return x;
    }

    GLint gety() const {
        return y;
    }

    void incrementx() {
        x++;
    }

    void decrementy() {
        y--;
    }
};

void setPixel(GLint xCoord, GLint yCoord) {
    glBegin(GL_POINTS);
    glVertex2i(xCoord, yCoord);
    glEnd();
}

void circlePlotPoints(GLint xc, GLint yc, screenPt circPt) {
    setPixel(xc + circPt.getx(), yc + circPt.gety());
    setPixel(xc - circPt.getx(), yc + circPt.gety());
    setPixel(xc + circPt.getx(), yc - circPt.gety());
    setPixel(xc - circPt.getx(), yc - circPt.gety());
    setPixel(xc + circPt.gety(), yc + circPt.getx());
    setPixel(xc - circPt.gety(), yc + circPt.getx());
    setPixel(xc + circPt.gety(), yc - circPt.getx());
    setPixel(xc - circPt.gety(), yc - circPt.getx());
}

void circleMidpoint(GLint xc, GLint yc, GLint radius) {
    screenPt circPt;
    GLint p = 1 - radius; // Initial value for midpoint parameter.
    circPt.setCoords(0, radius); // Set coordinates for top point of circle.

    // Plot the initial point in each circle quadrant.
    circlePlotPoints(xc, yc, circPt);

    // Calculate next point and plot in each octant.
    while (circPt.getx() < circPt.gety()) {
        circPt.incrementx();
        if (p < 0)
            p += 2 * circPt.getx() + 1;
        else {
            circPt.decrementy();
            p += 2 * (circPt.getx() - circPt.gety()) + 1;
        }
        circlePlotPoints(xc, yc, circPt);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Example: Draw a circle with center (150, 150) and radius 100
    circleMidpoint(150, 150, 100);

    glFlush();
}

void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 300.0, 0.0, 300.0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Midpoint Circle Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
