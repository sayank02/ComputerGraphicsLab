#include <GL/freeglut.h>
#include <stdlib.h> // For abs function

// Function to set a pixel
void setPixel(GLint xCoord, GLint yCoord) {
    glBegin(GL_POINTS);
    glVertex2i(xCoord, yCoord);
    glEnd();
}

// Helper function to draw all octants of the circle
void drawCircle(int xc, int yc, int x, int y) {
    setPixel(xc + x, yc + y);
    setPixel(xc - x, yc + y);
    setPixel(xc + x, yc - y);
    setPixel(xc - x, yc - y);
    setPixel(xc + y, yc + x);
    setPixel(xc - y, yc + x);
    setPixel(xc + y, yc - x);
    setPixel(xc - y, yc - x);
}

// Bresenham's Circle Drawing Algorithm
void circleBres(int xc, int yc, int radius) {
    int x = 0, y = radius;
    int d = 3 - 2 * radius;

    drawCircle(xc, yc, x, y);
    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        drawCircle(xc, yc, x, y);
    }
}

// OpenGL initialization
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0); // Set white background color
    glColor3f(0.0, 0.0, 0.0); // Set drawing color to black
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw a circle using Bresenham's circle drawing algorithm
    circleBres(200, 200, 100);

    glFlush();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bresenham's Circle Drawing Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
