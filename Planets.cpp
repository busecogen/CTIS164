/*********
STUDENT NAME : Ýclal Buse Çöðen
HOMEWORK : Homework #3

AIM: implement a program that simulates lighting effects on a circular object

MIN REQUIREMENT: One black planet and four light sources (red, green and blue,white) rotating in different orbits around the planet
                 Random values assigned for the radius, rotation speed and rotation direction of the planets
                 Lighting effect of all active light sources on the planet is calculated
                 A bottom bar that shows the status of light sources
                 F1, F2, F3 and F4 keys are used to switch their states
                 F5 key should pause/start the animation
                 F6 key should restart the animation by assigning new random values to the global variables
----------
PROBLEMS:
----------
ADDITIONAL FEATURES :
*4th planet with white color is added as an additional light source
*Up/Down keys can be used to increase/decrease the radii of the orbits of the planets
*Vector application is used to add shining effect to planets which seems like beam of lights around the planets
*Vector application is used to draw colorful circle wires for backgroud. Distance impact to the color intensity of wires is used.
*Mouse hover is used to enlarge the radius of black planet which also impact the intensity of light sources

* ********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include "vec.h"
#include "time.h"

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD   16 // Period for the timer.
#define TIMER_ON        1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

bool timerActivator = true;

//radius of orbitals
int crad= 100, r1=150, r2=200, r3=250, r4=300;
int rads[] = {r1,r2,r3,r4};

//angles of each planet
double ang1=30, ang2=40, ang3=60, ang4=90;
double angles[] = { ang1,ang2,ang3,ang4 };


typedef struct {
    float x, y;
} point_t;

//show x-y coord of mouse
point_t mouse;

typedef struct {
    float r, g, b;
} color_t;

typedef struct {
    int visibility;
    polar_t   pos;
    color_t color;
    double   w;
} planet_t;

typedef struct {
    vec_t pos;
    vec_t N;
} vertex_t;

#define NUM 4

//light sources
planet_t planet[4] =
{
   { {1}, {r1,ang1}, { 1, 0, 0 }, {0.60} },
   { {1}, {r2,ang2}, { 0, 1, 0 }, {0.75} },
   { {1}, {r3,ang3}, { 0, 0, 1 }, {1.00} },
   { {1}, {r4,ang4}, { 1, 1, 1 }, {-0.50} }
};

//calculate distance of mouse position to the black planet
bool distanceToCircle(int x, int y) {
    int d = sqrt((x * x) + (y * y));
    return (d < 100);
}

color_t mulColor(float k, color_t c) {
    color_t tmp = { k * c.r, k * c.g, k * c.b };
    return tmp;
}

color_t addColor(color_t c1, color_t c2) {
    color_t tmp = { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
    return tmp;
}

// To add distance into calculation
// when distance is 0  => its impact is 1.0
// when distance is 350 => impact is 0.0
// Linear impact of distance on light calculation.
double distanceImpact(double d) {
    return (-1.0 / 350.0) * d + 1.0;
}

color_t calculateColor(planet_t source, vertex_t v) {
    vec_t L = subV(pol2rec(source.pos), v.pos);
    vec_t uL = unitV(L);
    float factor = dotP(uL, v.N) * distanceImpact(magV(L));
    return mulColor(factor, source.color);
}


//
// to draw circle, center at (x,y)
// radius r
//
void circle(int x, int y, int r)
{
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}


void circle_wire(int x, int y, int r)
{
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++)
    {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}


//flash effect for planets
void circle_shading(planet_t l, int rad) {
#define PI 3.1415
    for (double j = 0; j < 1; j += 0.01)
    {
        float angle;

        glBegin(GL_POLYGON);

        for (int i = 0; i < 100; i++)
        {
            angle = 2 * PI * i / 100;
            vec_t g = pol2rec(l.pos);
            glColor3f(l.color.r*j, l.color.g*j, l.color.b*j);
            glVertex2f(g.x + (rad*(0.70-j)) * cos(angle), g.y + (rad * (0.70 - j)) * sin(angle));

        }
        glColor3f(0, 0, 0);
        glEnd();
    }
}

//add to the background colorful wires
void circle_wire_shading(planet_t l, int rad) {
    #define PI 3.1415
    vec_t g = pol2rec(l.pos);
    for (double j = 0; j < 1; j += 0.01) {
        float angle;
        glColor3f(l.color.r * j, l.color.g * j, l.color.b * j);

        glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++)
        {
            angle = 2 * PI * i / 100;
            glVertex2f( rad * cos(angle) * j*5, rad * sin(angle) * j*5);
        }
        glEnd();
    }
}

void print(int x, int y, const char* string, void* font)
{
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, string[i]);
    }
}

// display text with variables.
void vprint(int x, int y, void* font, const char* string, ...)
{
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutBitmapCharacter(font, str[i]);
    }
}

void vprint2(int x, int y, float size, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);
    glPushMatrix();
    glTranslatef(x, y, 0);
    glScalef(size, size, 1);

    int len, i;
    len = (int)strlen(str);
    for (i = 0; i < len; i++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

void message() {
    glColor3f(1, 1, 20 / 255.);
    print(450, 360, "Hover mouse to enlarge 'Black Planet'", GLUT_BITMAP_HELVETICA_12);
    print(-660, 360, "HOMEWORK #3", GLUT_BITMAP_HELVETICA_12);
    print(-660, 340, "Iclal Buse Cogen", GLUT_BITMAP_HELVETICA_12);
    print(-660, 320, "22103300", GLUT_BITMAP_HELVETICA_12);
    print(-650, -350, "F1(Red): ", GLUT_BITMAP_HELVETICA_12);
    print(-550, -350, "F2(Green): ", GLUT_BITMAP_HELVETICA_12);
    print(-450, -350, "F3(Blue): ", GLUT_BITMAP_HELVETICA_12);
    print(-350, -350, "F4(White): ", GLUT_BITMAP_HELVETICA_12);

    if (planet[0].visibility == 1) { print(-600, -350, "ON", GLUT_BITMAP_HELVETICA_12); }
    else { print(-600, -350, "OFF", GLUT_BITMAP_HELVETICA_12); }

    if (planet[1].visibility == 1) { print(-490, -350, "ON", GLUT_BITMAP_HELVETICA_12); }
    else { print(-490, -350, "OFF", GLUT_BITMAP_HELVETICA_12); }

    if (planet[2].visibility == 1) { print(-400, -350, "ON", GLUT_BITMAP_HELVETICA_12); }
    else { print(-400, -350, "OFF", GLUT_BITMAP_HELVETICA_12); }

    if (planet[3].visibility == 1) { print(-290, -350, "ON", GLUT_BITMAP_HELVETICA_12); }
    else { print(-290, -350, "OFF", GLUT_BITMAP_HELVETICA_12); }

    print(-240, -350, "F5(ANIMATION): ", GLUT_BITMAP_HELVETICA_12);
    if (timerActivator){ print(-140, -350, "ON ", GLUT_BITMAP_HELVETICA_12); }
    else{ print(-140, -350, "OFF", GLUT_BITMAP_HELVETICA_12); }

    print(-80, -350, "F6:Restart ", GLUT_BITMAP_HELVETICA_12);
    
    print(10, -350, "Scale Orbit: UP/DOWN KEY", GLUT_BITMAP_HELVETICA_12);

}

void orbitDisplay() {
    //orbitals of each planet with its own color
    glColor3f(1, 0, 0);
    circle_wire(0, 0, planet[0].pos.magnitude);
    glColor3f(0, 1, 0);
    circle_wire(0, 0, planet[1].pos.magnitude);
    glColor3f(0, 0, 1);
    circle_wire(0, 0, planet[2].pos.magnitude);
    glColor3f(1, 1, 1);
    circle_wire(0, 0, planet[3].pos.magnitude);
}

//
// To display onto window using OpenGL commands
//
void display() {

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    //drawing colorful orbitals for background
    for (int i = 0; i < NUM; i++) { circle_wire_shading(planet[i], rads[i]); }
    
    //planets with a shading effect
    for (int i = 0; i < NUM; i++) {
        if (planet[i].visibility == 1)
            circle_shading(planet[i], 50);

    }

    orbitDisplay();

    // calculate the color of each point on the surface
    for (double x = 0; x <= 360; x+=0.1) {
        //converting mag and ang to polar
        polar_t te;
        if (distanceToCircle(mouse.x, mouse.y)) {
            te.magnitude = crad + 30;
            te.angle = x;
        }else {
            te.magnitude = crad;
            te.angle = x;
        }
        polar_t tn = { 1, x };

        //creating vector
        vec_t e = pol2rec(te);
        vec_t n = pol2rec(tn);
        vertex_t P = { e , n };

        color_t res = { 0, 0, 0 };
        for (int i = 0; i < NUM; i++) {
            if (planet[i].visibility == 1) {res = addColor(res, calculateColor(planet[i], P));}
        }

        //line segments will be used to paint the area in depth
        glBegin(GL_LINES);
        //from the surface
        glColor3f(res.r, res.g, res.b);
        glVertex2f(e.x,e.y);
        //to the bottom
        glColor3f(0, 0, 0);
        glVertex2f(0, 0);
        glEnd();
    }

    message();

    glutSwapBuffers();

}

//
// key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
//
void onKeyDown(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);
    
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y)
{
    // exit when ESC is pressed.
    if (key == 27)
        exit(0);

    // to refresh the window it calls display() function
    glutPostRedisplay();
}
//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; for (int i = 0; i < NUM; i++) { if (abs((int)(planet[i].pos.magnitude+1 - rads[i])) < 100) { planet[i].pos.magnitude++; } }break;
    case GLUT_KEY_DOWN: down = true; for (int i = 0; i < NUM; i++) { if (abs((int)(planet[i].pos.magnitude-1 - rads[i])) < 225) { planet[i].pos.magnitude--; } }  break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
    case GLUT_KEY_F1: 
        if (planet[0].visibility == 0) { planet[0].visibility = 1;  }
        else { planet[0].visibility = 0; }
        break;
    case GLUT_KEY_F2:
        if (planet[1].visibility == 0) { planet[1].visibility = 1; }
        else { planet[1].visibility = 0; }
        break;
    case GLUT_KEY_F3:
        if (planet[2].visibility == 0) { planet[2].visibility = 1; }
        else { planet[2].visibility = 0; }
        break;

    case GLUT_KEY_F4:
        if (planet[3].visibility == 0) { planet[3].visibility = 1; }
        else { planet[3].visibility = 0;}
        break;

    case GLUT_KEY_F5:
        if (timerActivator) {timerActivator = !timerActivator;}
        else { timerActivator = true; }
        break;
    case GLUT_KEY_F6: 
        srand(time(NULL)); 
        for (int i = 0; i < NUM; i++) {
            planet[i].pos.angle = (rand() % 350) + 10;
            int r = (rand() % 101 - 50);
            if (r > 0) {r += 50;}
            else { r -= 50; }
            planet[i].w =  r / 200.;
        }
        break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyUp(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = false; break;
    case GLUT_KEY_DOWN: down = false; break;
    case GLUT_KEY_LEFT: left = false; break;
    case GLUT_KEY_RIGHT: right = false; break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// When a click occurs in the window,
// It provides which button
// buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
// states  : GLUT_UP , GLUT_DOWN
// x, y is the coordinate of the point that mouse clicked.
//
void onClick(int button, int stat, int x, int y)
{
    // Write your codes here.



    // to refresh the window it calls display() function
    glutPostRedisplay();
}

//
// This function is called when the window size changes.
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels.
//
void onResize(int w, int h)
{
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); // refresh window.
}

void onMoveDown(int x, int y) {
    // Write your codes here.



    // to refresh the window it calls display() function   
    glutPostRedisplay();
}

void onMove(int x, int y) {
    // Write your codes here.
    mouse.x = x - winWidth / 2;
    mouse.y = winHeight / 2 - y;
    


    // to refresh the window it calls display() function
    glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
    if (timerActivator) {
        for (int i = 0; i < NUM; i++) {
            planet[i].pos.angle += planet[i].w;
            if (planet[i].pos.angle > 360) { planet[i].pos.angle = 0; }
        }
    }
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

}

void main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("Planet Trip");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //
    // keyboard registration
    //
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);
  
    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //
    // mouse registration
    //
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if  TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    Init();

    glutMainLoop();
}