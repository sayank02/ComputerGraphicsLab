#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>

// 2D Point class
class wcPt2D {
public:
    GLfloat x, y;
};

// Helper functions
inline GLint roundGL(const GLfloat a) { return GLint(a + 0.5); }

// Bit codes for the regions outside the clipping window
const GLint winLeftBitCode = 0x1;
const GLint winRightBitCode = 0x2;
const GLint winBottomBitCode = 0x4;
const GLint winTopBitCode = 0x8;

inline GLint inside(GLint code) { return GLint(!code); }
inline GLint reject(GLint code1, GLint code2) { return GLint(code1 & code2); }
inline GLint accept(GLint code1, GLint code2) { return GLint(!(code1 | code2)); }

// Encode a point's position relative to the clipping window
GLubyte encode(wcPt2D pt, wcPt2D winMin, wcPt2D winMax) {
    GLubyte code = 0x00;
    if (pt.x < winMin.x)
        code = code | winLeftBitCode;
    if (pt.x > winMax.x)
        code = code | winRightBitCode;
    if (pt.y < winMin.y)
        code = code | winBottomBitCode;
    if (pt.y > winMax.y)
        code = code | winTopBitCode;
    return code;
}

// Swap points
void swapPts(wcPt2D *p1, wcPt2D *p2) {
    wcPt2D tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;
}

// Swap codes
void swapCodes(GLubyte *c1, GLubyte *c2) {
    GLubyte tmp = *c1;
    *c1 = *c2;
    *c2 = tmp;
}

// Set a pixel
void setPixel(GLint xCoord, GLint yCoord) {
    glBegin(GL_POINTS);
    glVertex2i(xCoord, yCoord);
    glEnd();
}

// Bresenham's line drawing algorithm
void lineBres(int x0, int y0, int xEnd, int yEnd) {
    int dx = fabs(xEnd - x0), dy = fabs(yEnd - y0);
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
        if (p < 0)
            p += twoDy;
        else {
            y++;
            p += twoDyMinusDx;
        }
        setPixel(x, y);
    }
}

// Cohen-Sutherland line clipping algorithm
void lineClipCohSuth(wcPt2D winMin, wcPt2D winMax, wcPt2D p1, wcPt2D p2) {
    GLubyte code1, code2;
    GLint done = false, plotLine = false;
    GLfloat m;

    while (!done) {
        code1 = encode(p1, winMin, winMax);
        code2 = encode(p2, winMin, winMax);

        if (accept(code1, code2)) {
            done = true;
            plotLine = true;
        } else if (reject(code1, code2)) {
            done = true;
        } else {
            if (inside(code1)) {
                swapPts(&p1, &p2);
                swapCodes(&code1, &code2);
            }

            if (p2.x != p1.x)
                m = (p2.y - p1.y) / (p2.x - p1.x);

            if (code1 & winLeftBitCode) {
                p1.y += (winMin.x - p1.x) * m;
                p1.x = winMin.x;
            } else if (code1 & winRightBitCode) {
                p1.y += (winMax.x - p1.x) * m;
                p1.x = winMax.x;
            } else if (code1 & winBottomBitCode) {
                if (p2.x != p1.x)
                    p1.x += (winMin.y - p1.y) / m;
                p1.y = winMin.y;
            } else if (code1 & winTopBitCode) {
                if (p2.x != p1.x)
                    p1.x += (winMax.y - p1.y) / m;
                p1.y = winMax.y;
            }
        }
    }

    if (plotLine)
        lineBres(roundGL(p1.x), roundGL(p1.y), roundGL(p2.x), roundGL(p2.y));
}

// Render text at specified position
void renderText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, *c);
    }
}

// OpenGL initialization
void init() {
    glClearColor(1.0, 1.0, 1.0, 0.0);
    glColor3f(0.0, 0.0, 0.0);
    glMatrixMode(GL_PROJECTION);
    gluOrtho2D(0.0, 400.0, 0.0, 400.0);
}

// Display function
void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    wcPt2D winMin = {50, 50}, winMax = {350, 350};
    wcPt2D p1 = {20, 200}, p2 = {380, 100};

    // Draw clipping window
    glColor3f(0.0, 0.0, 0.0); // Set color to black for window border
    glBegin(GL_LINE_LOOP);
    glVertex2f(winMin.x, winMin.y);
    glVertex2f(winMax.x, winMin.y);
    glVertex2f(winMax.x, winMax.y);
    glVertex2f(winMin.x, winMax.y);
    glEnd();

    glColor3f(0.0, 0.0, 0.0); // Set color to red for text
    renderText(150, 35, "Clipping Window");

    // Draw the original line (before clipping)
    glColor3f(0.0, 0.0, 1.0); // Set color to blue for original line
    lineBres(roundGL(p1.x), roundGL(p1.y), roundGL(p2.x), roundGL(p2.y));

    // Label the original line
    glColor3f(0.0, 0.0, 1.0); // Set color to blue for text
    renderText(60, 250, "Blue is Original Line");

    // Draw the clipped line (after clipping)
    glColor3f(1.0, 0.0, 0.0); // Set color to red for clipped line
    lineClipCohSuth(winMin, winMax, p1, p2);

    // Label the clipped line
    
    glColor3f(1.0, 0.0, 0.0); // Set color to red for text
    renderText(220, 220, "Red is clipped line");


    glFlush();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Cohen-Sutherland Line Clipping Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
