/*********
STUDENT NAME : Ýclal Buse Çöðen
HOMEWORK: Homework #2

AIM: User try to remove complex shaped ufos from the screen using rockets leaving the tank weapon 
on Space bacground. If the user presses F1 key, game starts and user can start shooting the target. After the game
start, user can pause/continue the game with F1 key. At the beggining, 5 ufos created automatically and
come from upper side of the screen. A new ufo is created for the removed one. Game continue for 20 seconds. 
The user has 3 energy. If 3 ufos land on the ground, aliens invade the space and the game ends. Otherwise, 
the game continue until the time run out.
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
    * 100 random star is created for the backround of the game
        X-cord, y-cord and size of the starts are created randomly
    * 5 ufos that have different speeds and directions added
        If any of the ufos removed from the screen or land on the ground, a new ufos is created
    * Complex shaped aliens that have different speeds and directions
        If user cannot shoot a ufo until it lands on the ground, an alien is created at the position of 
            the landed ufo
        If the number of aliens reach 3, the game ends
    * In addition to up, down,left and right special keys, tank weapon can move using 'a' and'd' keys
    * In addition to F1, 'r' key also restart the game even after the game starts
    * Remaining time is shown on the top right corner of the window in the form of (seconds.milliseconds)
        An additional function is added to show the remaining milliseconds
    * Before starting the game and after pausing the game, new rockets cannot be launched
    * Ammo is limited by 10 bullets. After the bullets finish, ammo is refilled
    * Collected points are counted until the game is over. Each succesful shot gain 10 points
    * A proper beggining and ending messages are displayed
    * Program terminates when ESC key is pressed
*********/

#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  800
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD   20 //period for the timer
#define TIMER_ON        1 //0:disable timer, 1:enable timer
#define GROUPING 1

#define D2R 0.0174532



/* Global Variables for Template File */

bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; //current Window width and height

bool timerActivator = false;

bool active = false;
int counter = 0; //for time period

//flag for game status
bool  gameover = false; 




/*****|objects global|******/

//collecting points
char s[10] = "";
int point = 0;

//to control ufos' movement
float ufo[5][4] = { {1,-300,450, (rand() % 5 + 4) / 50.},
                    {1,-150,450, (rand() % 5 + 2) / 50.},
                    {1,  0, 450, (rand() % 6 + 1) / 50.},
                    {1, 150,450, (rand() % 6 + 3) / 50.},
                    {1, 300,450, (rand() % 5 + 1) / 50.}}; //visibiliy //x-cord //y-cord //speed

//to control tank's movement
float xt=-100, yt=-220, vt=9;

//to control alien movement
float alien[3][4] = { {0, 25, -175, (rand() % 5 + 2) / 5.}, 
                      {0, 25, -175, (rand() % 5 + 2) / 5.}, 
                      {0, 25, -175, (rand() % 5 + 2) / 5.} }; //visibiliy //x-cord //y-cord //speed
int currentAlien = 0;

//to control fire's movement
int fire[10][3] = { {0},{0},{0},{0},{0},{0},{0},{0},{0},{0} }; //moving action //x-cord //y-cord 
int currentFire = 0;
float vf = 7;
int deadFire = 0; //number of fires served out 




/*****|shapes|******/

void circle(int x, int y, int r) {
#define PI 3.1415
    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 100; i++) {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void circle_wire(int x, int y, int r) {
#define PI 3.1415
    float angle;

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 100; i++) {
        angle = 2 * PI * i / 100;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void ellipse(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = 2 * 3.1415926 / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = 1;//we start at angle = 0 
    float y = 0;

    glBegin(GL_POLYGON);
    for (int ii = 0; ii < num_segments; ii++)
    {
        //apply radius and offset
        glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void ellipse_wire(float cx, float cy, float rx, float ry, int num_segments)
{
    float theta = 2 * 3.1415926 / float(num_segments);
    float c = cosf(theta);//precalculate the sine and cosine
    float s = sinf(theta);
    float t;

    float x = 1;//we start at angle = 0 
    float y = 0;

    glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < num_segments; ii++)
    {
        //apply radius and offset
        glVertex2f(x * rx + cx, y * ry + cy);//output vertex 

        //apply the rotation matrix
        t = x;
        x = c * x - s * y;
        y = s * t + c * y;
    }
    glEnd();
}

void rectangle_wire(float x,float y, float x2, float y2) {
    glColor3f(0, 0, 0); //black
    glLineWidth(2);
    glRectf(x, y, x2, y2);
}

void triangle_wire(float x, float y, float x2, float y2, float x3, float y3) {
    glColor3f(0, 0, 0); //black
    glLineWidth(2);
    glBegin(GL_TRIANGLES);
    glVertex2f(x, y);
    glVertex2f(x2, y2);
    glVertex2f(x3, y3);
    glEnd();
}

void halfCircle(float x, float y)  // the empty one
{
    float radius = 10;
    float twoPI = 2 * PI;
    

    glBegin(GL_POINTS);

    for (float i = 0.0; i <= twoPI / 2; i += 0.001)
        glVertex2f(x + (-cos(i) * radius), y + (-sin(i) * radius));

    glEnd();
    glFlush();
}




/*****|print|******/

void print(int x, int y, const char* string, void* font) {
    int len, i;

    glRasterPos2f(x, y);
    len = (int)strlen(string);
    for (i = 0; i < len; i++) {
        glutBitmapCharacter(font, string[i]);
    }
}

/*****
   to display text with variables
vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
*****/
void vprint(int x, int y, void* font, const char* string, ...) {
    va_list ap;
    va_start(ap, string);
    char str[1024];
    vsprintf_s(str, string, ap);
    va_end(ap);

    int len, i;
    glRasterPos2f(x, y);
    len = (int)strlen(str);
    for (i = 0; i < len; i++) {
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
    for (i = 0; i < len; i++) {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
    }
    glPopMatrix();
}

//showing remining msec
int msec(int x) {
    if (x == 100) { return 00; }
    else { return x; }
}



/*****|background|******/

void drawGradient(int x1, int y1, int w, int h, float r, float g, float b) {
    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x1, y1);
    glVertex2f(x1 + w, y1);
    glColor3f(r + 0.4, g + 0.4, b + 0.4);
    glVertex2f(x1 + w, y1 - h);
    glVertex2f(x1, y1 - h);
    glEnd();
    glColor3f(0.1, 0.1, 0.1);
    glBegin(GL_LINE_LOOP);
    glVertex2f(x1, y1);
    glVertex2f(x1 + w, y1);
    glVertex2f(x1 + w, y1 - h);
    glVertex2f(x1, y1 - h);
    glEnd();
}

void drawStars(int num) {
    srand(4);
    glColor3f(1, 1, 1);
    int x = 0, y = 0, r = 0;
    for (int i = 0; i < num; i++) {
        x = rand() % 801 - 400;
        y = rand() % 601 - 220;
        r = rand() % 3 + 1;
        circle(x, y, r);
    }
}

void drawLights(int x, int y, int r) {
    glColor3f(0.8, 0.9, 1); //light
    circle(x, y, r);
    glColor3f(0, 0, 0); //black
    circle_wire(x, y, r+1);
}

void drawHArrow(int x1, int x2, int y, int num, int r, int g, int b) {
    glColor3f(r, g, b); //black
    glLineWidth(2);

    glBegin(GL_LINES);

    glVertex2f(x1, y);
    glVertex2f(x1 + x2 , y-10);
    
    glEnd();

}


/*****|reseting|******/

void resetTank() {
    xt = 0; 
    yt = -220;
    counter = 0; //reseting time period
    point = 0; //reseting point
}

void resetUfo() {
    for (int i = 0; i < 5; i++) {
        ufo[i][0] = 1;
        ufo[i][2] = 450;
        ufo[i][3] = (rand() % 6 + 3) / 50.;
    }
}

void resetFire() {
    currentFire = 0;
    deadFire = 0;
    for (int i = 0; i < 10; i++) {
        fire[i][0] == 0;
        fire[i][1] = xt;
        fire[i][2] = yt;
    }
}

void resetAlien() {
    currentAlien = 0;
    for (int i = 0; i < 3; i++) {
        alien[i][0] = 0;
    }
}


/*****|objects|******/

void drawUfo(int x, int y) {
    glColor3f(0, 0, 0); //black
    circle_wire(0 + x, 0 + y, 31);
    glColor3f(0.5, 0.5, 0.7); //gray
    circle(0 + x, 0 + y, 30);

    glColor3f(0, 0, 0); //black
    ellipse_wire(0 + x, -10 + y, 51, 21, 200);
    glColor3f(0.2, 0.4, 1); //blue
    ellipse(0 + x, -10 + y, 50, 20, 200);

    drawLights(0 + x - 30, -3 + y,5);
    drawLights(0 + x - 10, y,5);
    drawLights(0 + x + 10, y,5);
    drawLights(0 + x + 30, -4 + y,5);

    glColor3f(0, 0, 0); //black
    ellipse_wire(0 + x, -25 + y, 36, 16, 200);
    glColor3f(0.5, 0.5, 0.7); //gray
    ellipse(0 + x, -25 + y, 35, 15, 200);

    glColor3f(0, 0, 0); //black
    ellipse_wire(0 + x, -30 + y, 21, 9, 200);
    glColor3f(0.8, 0.9, 1); //light
    ellipse(0 + x, -30 + y, 20, 8, 200);
    
    //light rays
    glColor3f(1, 1, 0);
    drawHArrow(0 + x - 10, -5, -45 + y, 1, 0, 0, 0);
    drawHArrow(0 + x , 0, -45 + y, 1, 0, 0, 0);
    drawHArrow(0 + x + 10, +5, -45 + y, 1, 0, 0, 0);
}

void drawUfos() {
    for (int i = 0; i < 5; i++) {
        if (ufo[i][0] == 1) { drawUfo(ufo[i][1], ufo[i][2]);}
        else { 
            ufo[i][0] = 1;
            ufo[i][2] = 350;
        }
    }
}

void drawFire(int xf, int yf) {
    rectangle_wire(xf-1, yf-1, xf + 16, yf + 26);
    rectangle_wire(xf + 4, yf + 24, xf + 11, yf + 31);
    triangle_wire(xf - 2, yf + 29, xf + 17, yf + 29, xf + 7, yf + 47);
    
    glColor3f(0.627, 0.573, 0.573);
    glRectf(xf, yf, xf + 15, yf + 25); //bottom
    glRectf(xf+5, yf+25, xf + 10, yf + 30); //middle

    //missle
    glBegin(GL_TRIANGLES);
    glVertex2f(xf, yf + 30);
    glVertex2f(xf + 15, yf + 30);
    glVertex2f(xf + 7, yf + 45);
    glEnd();

    //fire
    drawHArrow(xf+2, 0, yf - 3, 1, 1, 0.2, 0);
    drawHArrow(xf + 8, 0, yf - 3, 1, 1, 0.2, 0);
    drawHArrow(xf + 14, 0, yf - 3, 1, 1, 0.2, 0);

}

void drawFires() {
    for (int i = 0; i < 10; i++) {
        if (fire[i][0] == 0) {
            fire[i][1] = xt;
            fire[i][2] = yt;
        }
        drawFire(fire[i][1], fire[i][2]);
    }

}

void drawTank() {
    glColor3f(0, 0, 0);//black
    ellipse_wire(xt, yt + 40, 21, 41, 200);
    glColor3f(0, 0.2, 0); //black-green
    ellipse(xt, yt + 40, 20, 40, 200);
    
    glColor3f(0, 0, 0);//black
    circle_wire(xt, yt + 20, 41);
    glColor3f(0.2, 0.6, 0); //light green
    circle(xt, yt+20, 40);

    glColor3f(0, 0, 0);//black
    ellipse_wire(xt, yt, 71, 21, 200);
    glColor3f(0, 0.4, 0); //green
    ellipse(xt, yt, 70, 20, 200);

    drawLights(xt - 30, yt,12);
    drawLights(xt, yt, 12);
    drawLights(xt + 30, yt, 12);

}

void drawAlien(int x, int y) {
    drawHArrow(x-15, 10, y+35, 1, 0, 0, 0); //left antenna
    drawHArrow(x+15, -10, y + 35, 1, 0, 0, 0); //right antenna

    glColor3f(1, 1, 1);
    
    rectangle_wire(x-26, -221, x + 26, y + 26); //body
    rectangle_wire(x-16, -241, x - 4, y - 46);  //left leg
    rectangle_wire(x+4, -241, x + 16, y - 46); //right leg
    rectangle_wire(x-36, -201, x - 24, y+1); //left arm
    rectangle_wire(x+24, -201, x + 36, y+1); //right arm

    glColor3f(0, 1, 0);
    glRectf(x - 25, y-45, x + 25, y + 25); //body
    glRectf(x - 15, y - 65, x - 5, y - 45); //left leg
    glRectf(x + 5, y - 65, x + 15, y-45); //right leg
    glRectf(x - 35, y - 25, x - 25, y); //left arm
    glRectf(x + 25, y - 25, x + 35, y); //right arm

    glColor3f(1, 1, 1);
    circle(x, y+5, 12); //white eye
    glColor3f(0, 0, 0);
    circle(x, y+5, 4); //black eye
    halfCircle(x, y-15); //smiley
}

void drawAliens() {
    for (int i = 0; i < 3; i++) {
        if (alien[i][0] == 1) {
            drawAlien(alien[i][1], alien[i][2]);
        }
    }
}




/*****|displaying onto GL window|******/

void drawBackground() {
    drawGradient(-400, 300, 800, 600, 0, 37./255, 51. / 255); //environment
    drawGradient(-400, -240, 800, 50, 0, 0, 0); //base line
    drawStars(100);
    

    //title
    glColor3f(1, 1, 1);
    print(-90, 275, "*** Space Invaders ***", GLUT_BITMAP_TIMES_ROMAN_24);
    

    //bottom info panel
    glColor3f(0, 0, 0);
    glRectf(-400, -280, 400, -300);

    //info message
    glColor3f(1, 1, 0); //yellow
    print(300, -295, "Points: ", GLUT_BITMAP_HELVETICA_12);
    if(gameover)
        print(-100, -295, "Press <F1> to Start a New Game", GLUT_BITMAP_HELVETICA_12);
    else
        print(-100, -295, "<Spacebar> Fire,  <F1> Pause/Restart", GLUT_BITMAP_HELVETICA_12);

}

//Remaining time and Collected Points
void displayStat() {
    glColor3f(1, 0, 0);
    //showing the points gained
    
    sprintf_s(s, "%d", point);
    print(340, -295, s, GLUT_BITMAP_HELVETICA_12);

    //showing remaining time
    print(200, 270, "Remaining Time: ", GLUT_BITMAP_HELVETICA_18);
    char t[10] = "";
    sprintf_s(t, "%d.%2d", (int)(20 - counter / 50.), msec(100 - (counter % 50) * 2));
    print(340, 270, t, GLUT_BITMAP_HELVETICA_18);
}

void displayBegginingMessage(){
    drawGradient(-140, 65, 300, 120, 0, 0, 204 / 255.);
    glColor3f(0, 1, 204/255.);
    print(-100, 40, "F1: Start / Pause", GLUT_BITMAP_HELVETICA_18);
    print(-100, 20, "A/Left Button: Move left", GLUT_BITMAP_HELVETICA_18);
    print(-100, 0, "D/Right Button: Move right", GLUT_BITMAP_HELVETICA_18);
    print(-100, -20, "Space: Launch missle", GLUT_BITMAP_HELVETICA_18);
    print(-100, -40, "R: Reset game", GLUT_BITMAP_HELVETICA_18);


}
void displayEndingMessage() {
    drawGradient(-130, 60, 300, 100, 0, 0, 204 / 255.);

    glColor3f(1, 1, 0);
    print(-80, 0, " ====SCORE==== ", GLUT_BITMAP_TIMES_ROMAN_24);
    print(0, -20, s, GLUT_BITMAP_TIMES_ROMAN_24);
    if (currentAlien == 3) {
        print(-100, 20, "! ALIEN INVASION !", GLUT_BITMAP_TIMES_ROMAN_24);
    }
    else {
        print(-50, 20, "! TIME IS UP !", GLUT_BITMAP_TIMES_ROMAN_24);
    }

}

void display() {
    //clear window to black
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground();
    drawUfos();
    drawFires();
    drawAliens();
    drawTank();
    displayStat();
    if (gameover) {
        displayEndingMessage();
    }
    if(!timerActivator && !gameover)
        displayBegginingMessage();

    glutSwapBuffers();
}



/*****|on functions|******/

/*****
   key function for ASCII charachters like ESC, a,b,c..,A,B,..Z
*****/
void onKeyDown(unsigned char key, int x, int y) {
    //exit when ESC is pressed.

    switch (key) {

    case 27:
        exit(0);
        break;

    case 'r':
        gameover = false;
        resetTank();
        resetFire();
        resetUfo();
        resetAlien();
        if (timerActivator) {
            timerActivator = !timerActivator;
        }
        break;

    case ' ':
        if (timerActivator) {
            if (currentFire < 10) {
                fire[currentFire][0] = 1; //fire moving 
                currentFire++;
            }
        }
        break;

    case 'a':
        left = true;
        if (xt > -320 && !gameover) {
            xt -= vt;
        }
        break;

    case 'd':
        right = true;
        if (xt < 320 && !gameover) {
            xt += vt;
        }
        break;

    }
    //to refresh the window it calls display() function
    glutPostRedisplay();
}

void onKeyUp(unsigned char key, int x, int y) {
    //exit when ESC is pressed.
    if (key == 27)
        exit(0);

    //to refresh the window it calls display() function
    glutPostRedisplay();
}

/*****
   special Key like GLUT_KEY_F1, F2, F3,...
   arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
*****/
void onSpecialKeyDown(int key, int x, int y) {
    //write your codes here.

    switch (key) {

    case GLUT_KEY_UP:
        up = true;
        break;

    case GLUT_KEY_DOWN:
        down = true;
        break;

    case GLUT_KEY_LEFT:
        left = true;
        if (xt > -320 && !gameover) {
            xt -= vt;
        }
        break;

    case GLUT_KEY_RIGHT:
        right = true;
        if (xt < 320 && !gameover) {
            xt += vt;
        }
        break;

    case GLUT_KEY_F1:
        if (!gameover) {
            timerActivator = !timerActivator;
        }
        else {
            gameover = false;
            resetTank();
            resetFire();
            resetUfo();
            resetAlien();
            if (timerActivator) {
                timerActivator = !timerActivator;
            }
            break;
        }
    }

    //to refresh the window it calls display() function
    glutPostRedisplay();
}

/*****
   special Key like GLUT_KEY_F1, F2, F3,...
   arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
*****/
void onSpecialKeyUp(int key, int x, int y) {
    //write your codes here

    switch (key) {

    case GLUT_KEY_UP:
        up = false;
        break;

    case GLUT_KEY_DOWN:
        down = false;
        break;

    case GLUT_KEY_LEFT:
        left = false;
        break;

    case GLUT_KEY_RIGHT:
        right = false;
        break;
    }

    //to refresh the window it calls display() function
    glutPostRedisplay();
}

/*****
   When a click occurs in the window,
   It provides which button
   buttons : GLUT_LEFT_BUTTON , GLUT_RIGHT_BUTTON
   states  : GLUT_UP , GLUT_DOWN
   x, y is the coordinate of the point that mouse clicked
*****/
void onClick(int button, int stat, int x, int y) {
    //write your codes here.
    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN) {
       

    }
    
    //to refresh the window it calls display() function
    glutPostRedisplay();
}

/*****
   This function is called when the window size changes.
   w : is the new width of the window in pixels
   h : is the new height of the window in pixels
*****/
void onResize(int w, int h) {
    winWidth = w;
    winHeight = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-w / 2, w / 2, -h / 2, h / 2, -1, 1);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    display(); //refresh window
}

void onMoveDown(int x, int y) {
    //write your codes here.



    //to refresh the window it calls display() function   
    glutPostRedisplay();
}

/*****
   GLUT to OpenGL coordinate conversion
   x2 = x1 - winWidth / 2
   y2 = winHeight / 2 - y1
*****/
void onMove(int x, int y) {
    //write your codes here.



    //to refresh the window it calls display() function
    glutPostRedisplay();
}




/*****|timer|******/

void onTimer(int v) {
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
   
    if (!gameover) { 
        if (timerActivator) {
            counter++; //measuring runnign time
            if (counter == 20 * 50) {gameover = true;} //time is 20 sec
            //moving fire
            for (int i = 0; i < 10; i++) {
                if (fire[i][0] == 1) fire[i][2] += vf;

                if (fire[i][2] > 300) {fire[i][0] = 0; deadFire++;}

                for (int j = 0; j < 5; j++) {
                    if (ufo[j][0] == 1){
                        if (ufo[j][2] > -180) {
                            ufo[j][2] -= ufo[j][3]; //changing position of ufos
                            if ((fire[i][0] == 1) && (fire[i][1] > ufo[j][1] - 50) && (fire[i][1] < ufo[j][1] + 43) &&
                                (fire[i][2] > ufo[j][2] - 34) && (fire[i][2] < ufo[j][2] + 30)) {
                                fire[i][0] = 0;
                                ufo[j][0] = 0;
                                point += 10;
                                if (++deadFire == 10) resetFire(); //ammo is refilled
                            }
                        }
                        else {
                            ufo[j][0] = 0;
                            alien[currentAlien][0] = 1;
                            alien[currentAlien][1] = ufo[j][1];
                            currentAlien++;
                            if (currentAlien == 3) { gameover = true; }
                        }
                    }
                }
            }
            for (int i = 0; i < 3; i++) {
                if (alien[i][0] == 1) {
                    alien[i][1] += alien[i][3];
                    if (alien[i][1] < -350 || alien[i][1] > 350) alien[i][3] *= -1; //alien invasion results in gameover
                }
            }
            if (deadFire == 10) resetFire();
        }
    }

    //to refresh the window it calls display() function
    glutPostRedisplay(); //display()

}


void init() {
    //smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void main(int argc, char* argv[]) {

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    glutCreateWindow("SPACE INVADERS - Ýclal Buse Çöðen");

    glutDisplayFunc(display);
    glutReshapeFunc(onResize);

    //keyboard registration
    glutKeyboardFunc(onKeyDown);
    glutSpecialFunc(onSpecialKeyDown);

    glutKeyboardUpFunc(onKeyUp);
    glutSpecialUpFunc(onSpecialKeyUp);

    //mouse registration
    glutMouseFunc(onClick);
    glutMotionFunc(onMoveDown);
    glutPassiveMotionFunc(onMove);

#if TIMER_ON == 1
    // timer event
    glutTimerFunc(TIMER_PERIOD, onTimer, 0);
#endif

    init();

    glutMainLoop();

}