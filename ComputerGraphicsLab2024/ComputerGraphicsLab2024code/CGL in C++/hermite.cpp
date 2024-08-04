#include <GL/freeglut.h>
#include <math.h>

// 3D Point structure
typedef struct {
    GLfloat x, y, z;
} wcPt3D;

void init(void) {
    // Set color of display window to white
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

void plotPoint(wcPt3D point) {
    glBegin(GL_POINTS);
    glVertex2f(point.x, point.y);
    glEnd();
}

void hermiteCurve(wcPt3D p0, wcPt3D p1, wcPt3D r0, wcPt3D r1, GLint nPoints) {
    GLfloat t, h1, h2, h3, h4;
    wcPt3D point;

    for (GLint i = 0; i <= nPoints; ++i) {
        t = (GLfloat)i / (GLfloat)nPoints;

        // Hermite basis functions
        h1 = 2 * t * t * t - 3 * t * t + 1;
        h2 = -2 * t * t * t + 3 * t * t;
        h3 = t * t * t - 2 * t * t + t;
        h4 = t * t * t - t * t;

        point.x = h1 * p0.x + h2 * p1.x + h3 * r0.x + h4 * r1.x;
        point.y = h1 * p0.y + h2 * p1.y + h3 * r0.y + h4 * r1.y;
        point.z = h1 * p0.z + h2 * p1.z + h3 * r0.z + h4 * r1.z;

        plotPoint(point);
    }
}

void displayFcn(void) {
    GLint nPoints = 1000;

    // Define the endpoints and tangents for the Hermite curve
    wcPt3D p0 = { -40.0, -40.0, 0.0 };
    wcPt3D p1 = { 40.0, 10.0, 0.0 };
    wcPt3D r0 = { 100.0, 200.0, 0.0 };
    wcPt3D r1 = { 100.0, -200.0, 0.0 };

    glClear(GL_COLOR_BUFFER_BIT); // Clear display window

    glPointSize(4);
    glColor3f(1.0, 0.0, 0.0); // Set point color to red

    hermiteCurve(p0, p1, r0, r1, nPoints);

    glFlush();
}

// Define the viewing window
GLfloat xwcMin = -50.0, xwcMax = 50.0;
GLfloat ywcMin = -50.0, ywcMax = 50.0;
GLint winWidth = 600, winHeight = 600;

void winReshapeFcn(GLint newWidth, GLint newHeight) {
    /* Maintain an aspect ratio of 1.0. */
    glViewport(0, 0, newHeight, newHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(xwcMin, xwcMax, ywcMin, ywcMax);
    glClear(GL_COLOR_BUFFER_BIT);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowPosition(50, 50);
    glutInitWindowSize(winWidth, winHeight);
    glutCreateWindow("Hermite Curve");
    init();
    glutDisplayFunc(displayFcn);
    glutReshapeFunc(winReshapeFcn);
    glutMainLoop();
    return 0;
}
