#include <GL/freeglut.h>
#include <stdlib.h> // For abs function

// Function to round a float to the nearest integer
int round(float a) {
    return (int)(a + 0.5);
}

// Function to set a pixel
void setPixel(GLint xCoord, GLint yCoord) {
    glBegin(GL_POINTS);
    glVertex2i(xCoord, yCoord);
    glEnd();
}

// DDA Line Drawing Algorithm
void lineDDA(int x0, int y0, int xEnd, int yEnd) {
    int dx = xEnd - x0, dy = yEnd - y0, steps, k;
    float xIncrement, yIncrement, x = x0, y = y0;

    if (abs(dx) > abs(dy))
        steps = abs(dx);
    else
        steps = abs(dy);

    xIncrement = (float)dx / (float)steps;
    yIncrement = (float)dy / (float)steps;

    setPixel(round(x), round(y));
    for (k = 0; k < steps; k++) {
        x += xIncrement;
        y += yIncrement;
        setPixel(round(x), round(y));
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

    // Draw a line using the DDA algorithm
    lineDDA(20, 30, 200, 300);

    glFlush();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("DDA Line Drawing Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
