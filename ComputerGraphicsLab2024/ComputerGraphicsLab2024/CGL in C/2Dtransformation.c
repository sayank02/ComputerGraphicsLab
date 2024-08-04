#include <GL/freeglut.h>
#include <stdlib.h>
#include <math.h>
#include <string.h> // For memcpy

/* Set initial display-window size. */
GLsizei winWidth = 600, winHeight = 600;
/* Set range for world coordinates. */
GLfloat xwcMin = 0.0, xwcMax = 600.0;
GLfloat ywcMin = 0.0, ywcMax = 600.0;

/* Define a structure to represent 2D points. */
typedef struct {
    GLfloat x, y;
} wcPt2D;

typedef GLfloat Matrix3x3[3][3];
Matrix3x3 matComposite;
const GLdouble PI = 3.14159265358979323846; // Define PI

void init(void) {
    /* Set color of display window to white. */
    glClearColor(1.0, 1.0, 1.0, 0.0);
}

/* Construct the 3 x 3 identity matrix. */
void matrix3x3SetIdentity(Matrix3x3 matIdent3x3) {
    GLint row, col;
    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
            matIdent3x3[row][col] = (row == col);
}

/* Premultiply matrix m1 times matrix m2, store result in m2. */
void matrix3x3PreMultiply(Matrix3x3 m1, Matrix3x3 m2) {
    GLint row, col;
    Matrix3x3 matTemp;
    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
            matTemp[row][col] = m1[row][0] * m2[0][col] + m1[row][1] * m2[1][col] + m1[row][2] * m2[2][col];
    for (row = 0; row < 3; row++)
        for (col = 0; col < 3; col++)
            m2[row][col] = matTemp[row][col];
}

void translate2D(GLfloat tx, GLfloat ty) {
    Matrix3x3 matTransl;
    /* Initialize translation matrix to identity. */
    matrix3x3SetIdentity(matTransl);
    matTransl[0][2] = tx;
    matTransl[1][2] = ty;
    /* Concatenate matTransl with the composite matrix. */
    matrix3x3PreMultiply(matTransl, matComposite);
}

void rotate2D(wcPt2D pivotPt, GLfloat theta) {
    Matrix3x3 matRot;
    /* Initialize rotation matrix to identity. */
    matrix3x3SetIdentity(matRot);
    matRot[0][0] = cos(theta);
    matRot[0][1] = -sin(theta);
    matRot[0][2] = pivotPt.x * (1 - cos(theta)) + pivotPt.y * sin(theta);
    matRot[1][0] = sin(theta);
    matRot[1][1] = cos(theta);
    matRot[1][2] = pivotPt.y * (1 - cos(theta)) - pivotPt.x * sin(theta);
    /* Concatenate matRot with the composite matrix. */
    matrix3x3PreMultiply(matRot, matComposite);
}

void scale2D(GLfloat sx, GLfloat sy, wcPt2D fixedPt) {
    Matrix3x3 matScale;
    /* Initialize scaling matrix to identity. */
    matrix3x3SetIdentity(matScale);
    matScale[0][0] = sx;
    matScale[1][1] = sy;
    matScale[0][2] = (1 - sx) * fixedPt.x;
    matScale[1][2] = (1 - sy) * fixedPt.y;
    /* Concatenate matScale with the composite matrix. */
    matrix3x3PreMultiply(matScale, matComposite);
}

/* Using the composite matrix, calculate transformed coordinates. */
void transformVerts2D(GLint nVerts, wcPt2D* verts) {
    GLint k;
    GLfloat temp;
    for (k = 0; k < nVerts; k++) {
        temp = matComposite[0][0] * verts[k].x + matComposite[0][1] * verts[k].y + matComposite[0][2];
        verts[k].y = matComposite[1][0] * verts[k].x + matComposite[1][1] * verts[k].y + matComposite[1][2];
        verts[k].x = temp;
    }
}

void triangle(wcPt2D* verts) {
    GLint k;
    glBegin(GL_TRIANGLES);
    for (k = 0; k < 3; k++)
        glVertex2f(verts[k].x, verts[k].y);
    glEnd();
}

void displayFcn(void) {
    /* Define initial position for triangle. */
    GLint nVerts = 3;
    wcPt2D verts[3] = { {150.0, 100.0}, {400.0, 100.0}, {300.0, 400.0} };

    /* Calculate position of triangle centroid. */
    wcPt2D centroidPt;
    GLfloat xSum = 0, ySum = 0;
    for (GLint k = 0; k < nVerts; k++) {
        xSum += verts[k].x;
        ySum += verts[k].y;
    }
    centroidPt.x = xSum / nVerts;
    centroidPt.y = ySum / nVerts;

    /* Clear the display window. */
    glClear(GL_COLOR_BUFFER_BIT);

    /* Set color for original triangle. */
    glColor3f(0.0, 1.0, 1.0); // Cyan
    glViewport(0, 0, winWidth / 2, winHeight / 2);
    triangle(verts); // Display original triangle

    /* Label for original triangle */
    glColor3f(0.0, 0.0, 0.0); // Black
    glRasterPos2f(150.0, 50.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Original Triangle");

    /* Initialize composite matrix to identity. */
    matrix3x3SetIdentity(matComposite);

    /* Apply and display the scaled triangle. */
    wcPt2D vertsScaled[3];
    memcpy(vertsScaled, verts, sizeof(verts));
    scale2D(0.5, 0.5, centroidPt);
    transformVerts2D(nVerts, vertsScaled);
    glColor3f(0.0, 1.0, 0.0); // Green
    glViewport(0, winHeight / 2, winWidth / 2, winHeight / 2);
    triangle(vertsScaled); // Display scaled triangle

    /* Label for scaled triangle */
    glColor3f(0.0, 0.0, 0.0); // Black
    glRasterPos2f(150.0, 100.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Scaled Triangle");

    /* Apply and display the rotated triangle. */
    matrix3x3SetIdentity(matComposite);
    wcPt2D vertsRotated[3];
    memcpy(vertsRotated, verts, sizeof(verts));
    rotate2D(centroidPt, PI / 2.0);
    transformVerts2D(nVerts, vertsRotated);
    glColor3f(1.0, 0.0, 0.0); // Red
    glViewport(winWidth / 2, 0, winWidth / 2, winHeight / 2);
    triangle(vertsRotated); // Display rotated triangle

    /* Label for rotated triangle */
    glColor3f(0.0, 0.0, 0.0); // Black
    glRasterPos2f(100.0, 50.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Rotated Triangle");

    /* Apply and display the translated triangle. */
    matrix3x3SetIdentity(matComposite);
    wcPt2D vertsTranslated[3];
    memcpy(vertsTranslated, verts, sizeof(verts));
    translate2D(100.0, 100.0);
    transformVerts2D(nVerts, vertsTranslated);
    glColor3f(0.0, 0.0, 1.0); // Blue
    glViewport(winWidth / 2, winHeight / 2, winWidth / 2, winHeight / 2);
    triangle(vertsTranslated); // Display translated triangle

    /* Label for translated triangle */
    glColor3f(0.0, 0.0, 0.0); // Black
    glRasterPos2f(200.0, 150.0);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)"Translated Triangle");

    glFlush();
}


void winReshapeFcn(GLint newWidth, GLint newHeight) {
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
    glutCreateWindow("Geometric Transformation Sequence");
    init();
    glutDisplayFunc(displayFcn);
    glutReshapeFunc(winReshapeFcn);
    glutMainLoop();
    return 0;
}
