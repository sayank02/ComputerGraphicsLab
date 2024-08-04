#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef enum { Left, Right, Bottom, Top } Boundary;
const GLint nClip = 4;

typedef struct {
    GLfloat x, y;
} wcPt2D;

// Helper functions
inline GLint roundGL(const GLfloat a) { return (GLint)(a + 0.5); }

GLint inside(wcPt2D p, Boundary b, wcPt2D wMin, wcPt2D wMax) {
    switch (b) {
        case Left: return p.x >= wMin.x;
        case Right: return p.x <= wMax.x;
        case Bottom: return p.y >= wMin.y;
        case Top: return p.y <= wMax.y;
    }
    return 1;
}

GLint cross(wcPt2D p1, wcPt2D p2, Boundary winEdge, wcPt2D wMin, wcPt2D wMax) {
    return inside(p1, winEdge, wMin, wMax) != inside(p2, winEdge, wMin, wMax);
}

wcPt2D intersect(wcPt2D p1, wcPt2D p2, Boundary winEdge, wcPt2D wMin, wcPt2D wMax) {
    wcPt2D iPt;
    GLfloat m = (p1.x != p2.x) ? (p1.y - p2.y) / (p1.x - p2.x) : 0;

    switch (winEdge) {
        case Left:
            iPt.x = wMin.x;
            iPt.y = p2.y + (wMin.x - p2.x) * m;
            break;
        case Right:
            iPt.x = wMax.x;
            iPt.y = p2.y + (wMax.x - p2.x) * m;
            break;
        case Bottom:
            iPt.y = wMin.y;
            if (p1.x != p2.x) iPt.x = p2.x + (wMin.y - p2.y) / m;
            else iPt.x = p2.x;
            break;
        case Top:
            iPt.y = wMax.y;
            if (p1.x != p2.x) iPt.x = p2.x + (wMax.y - p2.y) / m;
            else iPt.x = p2.x;
            break;
    }
    return iPt;
}

void clipPoint(wcPt2D p, Boundary winEdge, wcPt2D wMin, wcPt2D wMax, wcPt2D* pOut, int* cnt, wcPt2D* first[], wcPt2D s[]) {
    wcPt2D iPt;

    if (first[winEdge] == NULL) {
        first[winEdge] = (wcPt2D*)malloc(sizeof(wcPt2D));
        *first[winEdge] = p;
    } else {
        if (cross(p, s[winEdge], winEdge, wMin, wMax)) {
            iPt = intersect(p, s[winEdge], winEdge, wMin, wMax);
            if (winEdge < Top) {
                clipPoint(iPt, (Boundary)(winEdge + 1), wMin, wMax, pOut, cnt, first, s);
            } else {
                pOut[(*cnt)++] = iPt;
            }
        }
    }

    s[winEdge] = p;

    if (inside(p, winEdge, wMin, wMax)) {
        if (winEdge < Top) {
            clipPoint(p, (Boundary)(winEdge + 1), wMin, wMax, pOut, cnt, first, s);
        } else {
            pOut[(*cnt)++] = p;
        }
    }
}

void closeClip(wcPt2D wMin, wcPt2D wMax, wcPt2D* pOut, GLint* cnt, wcPt2D* first[], wcPt2D s[]) {
    wcPt2D iPt;
    for (Boundary winEdge = Left; winEdge <= Top; winEdge = (Boundary)(winEdge + 1)) {
        if (cross(s[winEdge], *first[winEdge], winEdge, wMin, wMax)) {
            iPt = intersect(s[winEdge], *first[winEdge], winEdge, wMin, wMax);
            if (winEdge < Top) {
                clipPoint(iPt, (Boundary)(winEdge + 1), wMin, wMax, pOut, cnt, first, s);
            } else {
                pOut[(*cnt)++] = iPt;
            }
        }
        free(first[winEdge]);
    }
}

GLint polygonClipSuthHodg(wcPt2D wMin, wcPt2D wMax, GLint n, wcPt2D* pIn, wcPt2D* pOut) {
    wcPt2D* first[nClip] = { NULL, NULL, NULL, NULL };
    wcPt2D s[nClip];
    GLint cnt = 0;

    for (GLint k = 0; k < n; k++) {
        clipPoint(pIn[k], Left, wMin, wMax, pOut, &cnt, first, s);
    }

    closeClip(wMin, wMax, pOut, &cnt, first, s);
    return cnt;
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

    wcPt2D winMin = { 50, 50 }, winMax = { 350, 350 };
    wcPt2D pIn[4] = { {20, 300}, {380, 300}, {380, 100}, {20, 100} };
    wcPt2D pOut[8];

    GLint n = 4;
    GLint cnt = polygonClipSuthHodg(winMin, winMax, n, pIn, pOut);

    // Draw the clipping window (for reference)
    glColor3f(0.0, 0.0, 0.0); // Set color to black for clipping window
    glBegin(GL_LINE_LOOP);
    glVertex2f(winMin.x, winMin.y);
    glVertex2f(winMax.x, winMin.y);
    glVertex2f(winMax.x, winMax.y);
    glVertex2f(winMin.x, winMax.y);
    glEnd();

    // Draw the original polygon (before clipping)
    glColor3f(0.0, 0.0, 1.0); // Set color to blue for original polygon
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < n; i++)
        glVertex2f(pIn[i].x, pIn[i].y);
    glEnd();

    // Draw the clipped polygon (after clipping)
    glColor3f(1.0, 0.0, 0.0); // Set color to red for clipped polygon
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < cnt; i++)
        glVertex2f(pOut[i].x, pOut[i].y);
    glEnd();

    // Draw labels (optional)
    glColor3f(0.0, 0.0, 1.0); // Set color to black for text
    glRasterPos2f(60, 370); 
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)"Blue is Original Polygon");
    glColor3f(0.0, 0.0, 0.0);
    glRasterPos2f(60, 35); // Position for "Clipped Polygon"
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)"Black is clipping window");
    glColor3f(1.0, 0.0, 0.0);
    glRasterPos2f(60, 200); // Position for "Clipped Polygon"
    glutBitmapString(GLUT_BITMAP_HELVETICA_12, (const unsigned char *)"Red is clipped polygon");

    glFlush();
}

// Main function
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(400, 400);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sutherland-Hodgman Polygon Clipping Algorithm");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}

// Dummy implementations for setPixel and lineBres
void setPixel(GLint xCoord, GLint yCoord) {
    glBegin(GL_POINTS);
    glVertex2i(xCoord, yCoord);
    glEnd();
}

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
