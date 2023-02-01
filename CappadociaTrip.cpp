/*********
STUDENT NAME : Ýclal Buse Çöðen
HOMEWORK: Homework #1
AIM: User create at most 10 complex shaped hot-air balloons at the click position on Cappadocia 
     view background when clicking on screen. If the user presses F1 key, all the objects on 
	 the window start to animate by moving up. After firing, user cannot be able to create more 
	 objects until all the objects disappear on screen.
----------
PROBLEMS:
----------
ADDITIONAL FEATURES:
	* Three clouds that have different speeds and directions added
		If any cloud reaches the end of window, it changes its direction to opposite side
		A cloud is in the background of all balloons, one in front of balloons 1 to 5 and 
		in the background from 6 to 10, and one in front of all balloons.
	* A sun, complex shaped roads and fairy chimneys have been added 
	* Color of hot air ballons can be changed manually if you press 'c' or 'C'
	* All ballons have random speeds
	* User cannot stop the baloons when they are in motion
	* After firing, new ballons cannot be added
	* After all the ballons exit the screen, new objects can be added.
	* In the beginning, an important message has been given as "Click to Create Ballons (Max 10)"
		If new ballons are added, this message disappears
		After all the ballons exit the screen, the message reappears
	* Program terminates when ESC key is pressed
*********/

#ifdef _MSCVER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#define WINDOW_WIDTH  1000
#define WINDOW_HEIGHT 600

#define TIMER_PERIOD  16 // Period for the timer. 60fps 1000/16
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
int radius; //current radius of baloon

bool timerActivator = false;

int r = 180, g = 172, b = 50; // initial color

//cloud initial positions and speeds 
double cloudX=0, cloudX2=-500, cloudX3=200, cloudSpeed=-1, cloudSpeed2=1.3, cloudSpeed3=1.15;

//baloon visibility, x-cor, y-cor, speed, rgb colors features are in forms of an array 
int baloons[10][7] = {0};

//baloon index
int nextBaloon = 0;

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

// background drawing functions
void drawGround() {
	glColor3f(206/255., 105 /255., 3/255.);
	glRectf(-1*(winWidth /2), -1 *(winHeight / 2), winWidth /2, -1*winHeight/5);
}

void drawSun() {
	glColor3f(255/255., 213/255., 0/255.);
	circle(-1*winWidth / 2, winHeight/1.95, winWidth/10);
}

void drawCloud(double xOffset, double yOffset) {
	glColor3f(255 / 255., 255 / 255., 255 / 255.);
	for (int i = 0; i < 4; i++) {
		circle(winWidth / 4.5 + i * 17 + xOffset, winHeight / 3 + yOffset, winWidth / 80);
		circle(winWidth / 4.6 + i * 20 + xOffset, winHeight / 3.37 + yOffset, winWidth / 50);
		circle(winWidth / 4.5 + i * 17 + xOffset, winHeight / 3.7 + yOffset, winWidth / 80);
	}
	for (int i = 1; i < 3; i++) {
		circle(winWidth / 4.5 + i * 17 + xOffset, winHeight / 3 + yOffset, winWidth / 50);
		circle(winWidth / 4.5 + i * 17 + xOffset, winHeight / 3.7 + yOffset, winWidth / 50);
	}
}

void drawChimneys() {
	//counting from left to right...

	//2nd chimney
	glColor3f(247 / 255., 206 / 255., 136 / 255.);
	glBegin(GL_TRIANGLES);
	glVertex2f(-1 * winWidth / 3.3, winHeight / winHeight); //hill
	glVertex2f(-1 * winWidth / 5, -1 * winHeight / 2.4); //right corner
	glVertex2f(-1 * winWidth / 2.5, -1 * winHeight / 2.4); //left corner

	// 1st chimney
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	glVertex2f(-1*winWidth/2.6, -1 * winHeight / 12); //hill
	glVertex2f(-1*winWidth/3.3, -1 * winHeight / 2.4); //right corner
	glVertex2f(-1*winWidth/2.2, -1 * winHeight / 2.4); //left corner
	
	// 3rd chimney
	glColor3f(250 / 255., 200 / 255., 116 / 255.);
	glVertex2f(-1 * winWidth / 4.4, -1 * winHeight / 12); //hill
	glVertex2f(-1 * winWidth / 6.6, -1 * winHeight / 2.4); //right corner
	glVertex2f(-1 * winWidth / 3.3, -1 * winHeight / 2.4); //left corner

	// 5th chimney
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	glVertex2f(winWidth / 6.6, winHeight / 12); //hill
	glVertex2f(winWidth / 5, -1 * winHeight / 5); //right corner
	glVertex2f(winWidth / 10, -1 * winHeight / 5); //left corner

	// 4th chimney
	glColor3f(247 / 255., 206 / 255., 136 / 255.);
	glVertex2f(winWidth / 13.3, -1 * winHeight / winHeight); //hill
	glVertex2f(winWidth / 6.6, -1 * winHeight / 4); //right corner
	glVertex2f(winWidth / winWidth, -1 * winHeight / 4); //left corner
	glEnd();

	//road going from back
	glColor3f(93 / 255., 104 / 255., 111 / 255.);
	glRectf(-1 * winWidth / 10, -1 * winHeight / 3.15, winWidth, -1 * winHeight / 3.75);

	// 6th chimney
	glBegin(GL_TRIANGLES);
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	glVertex2f(winWidth / 2.15, winHeight / 12); //hill
	glVertex2f(winWidth / 1.85, -1 * winHeight / 3); //right corner
	glVertex2f(winWidth / 2.56, -1 * winHeight / 3); //left corner

	glEnd();

	//2nd chimney
	glColor3f(247 / 255., 206 / 255., 136 / 255.);
	circle(-1 * winWidth / 3.3, winHeight / winHeight, winWidth / 50);

	// 1st chimney
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	circle(-1 * winWidth / 2.6, -1 * winHeight / 12, winWidth / 50);
	
	// 3th chimney
	glColor3f(250 / 255., 200 / 255., 116 / 255.);
	circle(-1 * winWidth / 4.4, -1 * winHeight / 12, winWidth / 50);

	// 5th chimney
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	circle(winWidth / 6.6, winHeight / 12, winWidth / 50);

	// 4th chimney
	glColor3f(247 / 255., 206 / 255., 136 / 255.);
	circle(winWidth / 13.3, -1 * winHeight / winHeight, winWidth / 50);

	// 6th chimney
	glColor3f(245 / 255., 196 / 255., 112 / 255.);
	circle(winWidth / 2.15, winHeight / 12, winWidth / 50);
}

void drawRoad() {
	//road segments
	glColor3f(93 / 255., 104 / 255., 111 / 255.);
	glRectf(-1 * winWidth, -1 * winHeight / 2.04, -1 * winWidth / 10, -1 * winHeight / 2.3);
	glRectf(-1 * winWidth / 10, -1 * winHeight / 2.04, -1 * winWidth / 15.38, -1 * winHeight / 3.75);

	//crosswalks
	glColor3f(255 / 255., 255 / 255., 255 / 255.);
	for (int i = 0; i < 8; i++) {
		float x_init = -1 * winWidth / 2;
		float crossWalkLength = winWidth / 25;
		float offsetStep = winWidth / 100;
		float offset = i * (crossWalkLength + offsetStep);
		glRectf(x_init + offset, -1 * winHeight / 2.12, 
			x_init + crossWalkLength + offset, -1 * winHeight / 2.2);
	}
	for (int i = 0; i < 3; i++) {
		float y_init = -1 * winHeight / 2.04 + winHeight/60;
		float crossWalkLength = winHeight / 20;
		float offsetStep = winHeight / 60;
		float offset = i * (crossWalkLength + offsetStep);
		glRectf(-1 * winWidth / 11.1, y_init+ offset,
			-1 * winWidth / 12.75, y_init+ crossWalkLength+ offset);
	}
	for (int i = 0; i < 9; i++) {
		float x_init = -1 * winWidth / 10 + winWidth/25;
		float crossWalkLength = winWidth / 25;
		float offsetStep = winWidth / 100;
		float offset = i * (crossWalkLength + offsetStep);
		glRectf(x_init + offset, -1 * winHeight / 3.3,
			x_init + crossWalkLength + offset, -1 * winHeight / 3.52);
	}
}

// function to draw a ballon
void drawHotAirBaloon(int baloonX, int baloonY, int bColorR, int bColorG, int bColorB) {
	baloonX += winWidth / 20;
	baloonY += 0;
	radius = winWidth / 12;

	//Part 1 : Big Box
	glColor3f(128 / 255., 0 / 255., 0 / 255.);
	glRectf(-1 * winWidth / 10 + baloonX, -1 * winHeight / 5 + baloonY, winWidth/ winWidth + baloonX, -1 * winHeight / 8 +baloonY);

	//Part 2: Small Box
	glColor3f(87 / 255., 0 / 255., 0 / 255.);
	glRectf(-1 * winWidth / 9.09 +baloonX, -1 * winHeight / 8+baloonY, winWidth / 90.9+baloonX, -1 * winHeight / 9.23+baloonY);

	// Part 4 : string/rope 
	glLineWidth(2);
	glColor3f(0, 0, 0); // black
	glBegin(GL_LINES);
	//left string
	glVertex2f(-1 * winWidth / 12 + baloonX, -1 * winHeight / 9.23 + baloonY);
	glVertex2f(-1 * winWidth / 12 + baloonX - radius/3, -1 * winHeight / 20 + baloonY + radius);
	//right string
	glVertex2f(-1 * winWidth / 50 + baloonX, -1 * winHeight / 9.23 + baloonY);
	glVertex2f(-1 * winWidth / 50 + baloonX + radius/3, -1 * winHeight / 20 + baloonY + radius);
	glEnd();
	glLineWidth(1);

	//Part 5 : Body
	glColor3ub(0, 0, 0);
	circle_wire(-1 * winWidth / 20 + baloonX, winHeight/6 + baloonY, radius+1);
	glColor3ub(bColorR, bColorG, bColorB);
	circle(-1 * winWidth / 20 + baloonX, winHeight/6 + baloonY, radius);

	//Part 6: FireBox
	glColor3f(26 / 255., 0 / 255., 0 / 255.);
	glRectf(-1 * winWidth / 14 + baloonX, -1 * winHeight / 9 + baloonY, 
		-1 * winWidth / 35 + baloonX, -1 * winHeight / 14 + baloonY);

	//Part 7:Fire
	
	glBegin(GL_TRIANGLES);

	glColor3f(255 / 255., 83 / 255., 26 / 255.); // orange
	glVertex2f(-1 * winWidth / 15 + baloonX, -1 * winHeight / 14 + baloonY); //left
	glVertex2f(-1 * winWidth / 28 + baloonX, -1 * winHeight / 14 + baloonY); //right
	glVertex2f(-1 * winWidth / 19 + baloonX, -1 * winHeight / 30 + baloonY); //middle

	glColor3f(255/255., 255 /255., 0/255.); //yellow
	glVertex2f(-1 * winWidth / 16 + baloonX, -1 * winHeight / 14 + baloonY); //left
	glVertex2f(-1 * winWidth / 25 + baloonX, -1 * winHeight / 14 + baloonY); //right
	glVertex2f(-1 * winWidth / 19 + baloonX, -1 * winHeight / 25 + baloonY); //middle

	glColor3f(255 / 255., 83 / 255., 26 / 255.); // orange
	glVertex2f(-1 * winWidth / 16.75 + baloonX, -1 * winHeight / 14 + baloonY); //left
	glVertex2f(-1 * winWidth / 23.00 + baloonX, -1 * winHeight / 14 + baloonY); //right
	glVertex2f(-1 * winWidth / 19.25 + baloonX, -1 * winHeight / 22 + baloonY); //middle

	glColor3f(255 / 255., 255 / 255., 0 / 255.); //yellow
	glVertex2f(-1 * winWidth / 17.70 + baloonX, -1 * winHeight / 14 + baloonY); //left
	glVertex2f(-1 * winWidth / 21.50 + baloonX, -1 * winHeight / 14 + baloonY); //right
	glVertex2f(-1 * winWidth / 19.30 + baloonX, -1 * winHeight / 19 + baloonY); //middle
	glEnd();

	//Part 8:weights rope

	glLineWidth(3);
	glColor3f(0,0,0); //cream
	glBegin(GL_LINES);
	//left rope
	glVertex2f(-1 * winWidth / 13 + baloonX, -1 * winHeight / 7 + baloonY);
	glVertex2f(-1 * winWidth / 13 + baloonX, -1 * winHeight / 8 + baloonY);
	//middle rope 
	glVertex2f(-1 * winWidth / 20 + baloonX, -1 * winHeight / 7 + baloonY);
	glVertex2f(-1 * winWidth / 20 + baloonX, -1 * winHeight / 8 + baloonY);
	//right rope
	glVertex2f(-1 * winWidth / 47 + baloonX, -1 * winHeight / 7 + baloonY);
	glVertex2f(-1 * winWidth / 47 + baloonX, -1 * winHeight / 8 + baloonY);
	glEnd();

	///Part 9:weights
	glColor3f(255/255., 204/255., 153/255.); //cream
	circle(-1 * winWidth / 13 + baloonX, -1 * winHeight / 6.5 + baloonY, winWidth / 125);
	circle(-1 * winWidth / 20 + baloonX, -1 * winHeight / 6.5 + baloonY, winWidth / 125);
	circle(-1 * winWidth / 47 + baloonX, -1 * winHeight / 6.5 + baloonY, winWidth / 125);
}

void displayBackgorund() {
	glColor3f(1, 1, 1);
	vprint(-1 * winWidth / 25, winHeight/2.2, GLUT_BITMAP_HELVETICA_18, "Iclal Buse Cogen");
	vprint(-1 * winWidth / 50, winHeight / 2.4, GLUT_BITMAP_HELVETICA_18, "22103300");


	if (nextBaloon == 0) {
		glColor3ub(0, 0, 0);
		vprint(-120, 0, GLUT_BITMAP_HELVETICA_18, "Click to Create Baloons (Max 10)");
	}

	glColor3ub(128, 255, 0);
	vprint(winWidth / 6, -1 * winHeight / 2.5, GLUT_BITMAP_HELVETICA_18, "'c' or 'C': change color");
	vprint(winWidth / 6, -1 * winHeight / 2.2, GLUT_BITMAP_HELVETICA_18, "Press <F1> to fly hot-air baloons");
}

void display() {
	
	glClearColor(58 / 255., 150 / 255., 242 / 255., 0); //background color
	glClear(GL_COLOR_BUFFER_BIT);
	drawGround();
	drawSun();
	drawCloud(cloudX,0);
	drawChimneys();
	displayBackgorund();
	drawRoad();
	
	//check if baloon is visible if so draw it
	for (int i = 0; i < 10; i++) {
		if(i==4)
			drawCloud(cloudX3, winHeight / 15); //display cloud in backside after 5th baloon
		if (baloons[i][0] == 1)
			drawHotAirBaloon(baloons[i][1], baloons[i][2],baloons[i][4], baloons[i][5], baloons[i][6]);
	}
	

	drawCloud(cloudX2,-1*winHeight/9);

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

	//to change color
	if ((key == 'c' || key == 'C') && !timerActivator)
	{
		for (int i = 0; i < 10; i++) {
			baloons[i][4] = rand() % 256;
			baloons[i][5] = rand() % 256;
			baloons[i][6] = rand() % 256;

		}
	}

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

// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
	if (key == GLUT_KEY_F1)
	{
		if(!timerActivator)
			timerActivator = !timerActivator;
	}

	// to refresh the window it calls display() function
	glutPostRedisplay();
}

void onSpecialKeyUp(int key, int x, int y)
{
	// Write your codes here.

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
	//each new click creates a ballon on the screen.
	if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && !timerActivator)
	{
		x = x - winWidth / 2;
		y = winHeight / 2 - y;

		if (nextBaloon < 10) {
			baloons[nextBaloon][0] = 1; //visibility
			baloons[nextBaloon][1] = x; //x-coordinate
			baloons[nextBaloon][2] = y; //y-coordinate
			baloons[nextBaloon][3] = rand()%5+2; //speed
			baloons[nextBaloon][4] = rand()%256; //red font
			baloons[nextBaloon][5] = rand() % 256; //green font
			baloons[nextBaloon][6] = rand() % 256; //blue font
			nextBaloon++; //ballon index
		}
	}
	// to refresh the window it calls display() function
	glutPostRedisplay();
}

// Event Handler for RESIZE event
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
	display();
}

void onMoveDown(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function   
	glutPostRedisplay();
}

// GLUT to OpenGL coordinate conversion:
//   x2 = x1 - winWidth / 2
//   y2 = winHeight / 2 - y1
void onMove(int x, int y) {
	// Write your codes here.



	// to refresh the window it calls display() function
	glutPostRedisplay();
}

#if TIMER_ON == 1
void onTimer(int v) {
	glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	//cloud movement
	cloudX += cloudSpeed;
	cloudX2 += cloudSpeed2;
	cloudX3 += cloudSpeed3;
	
	if (cloudX < -1 * winWidth / 1.45 || cloudX > winWidth / 4.8) {
		cloudSpeed *= -1;
	}
	if (cloudX2 < -1 * winWidth / 1.45 || cloudX2 > winWidth / 4.8) {
		cloudSpeed2 *= -1;
	}
	if (cloudX3 < -1 * winWidth / 1.45 || cloudX3 > winWidth / 4.8) {
		cloudSpeed3 *= -1;
	}

	if (timerActivator) {
		printf("Timer running!");
		// animate the current shape until it passes window size.
		for (int i = 0; i < 10; i++) {
			if (baloons[i][0] != 0) {
				baloons[i][2] += baloons[i][3];
				if (baloons[i][2] > winHeight/1.4) {
					baloons[i][0] = 0;
					nextBaloon--;
				}
			}
		}
		//stops timer if all ballons exit the screen
		if (nextBaloon == 0) {
			timerActivator = !timerActivator;
		}
	}

	//to refresh the window it calls display() function
	glutPostRedisplay(); //display()

}
#endif

void init() {
	//smoothing shapes
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main(int argc, char* argv[]) {

	// STEP #1: Create Window
	glutInit(&argc, argv);
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutCreateWindow("Travelling in Cappadocia");

	// STEP #2: Register Events
	glutDisplayFunc(display);  //Event = PAINT/DISPLAY, call display
	glutReshapeFunc(onResize); //Event = RESIZE, call onResize

	//STEP #3: Hover
	glutKeyboardFunc(onKeyDown);
	glutSpecialFunc(onSpecialKeyDown);

	glutKeyboardUpFunc(onKeyUp);
	glutSpecialUpFunc(onSpecialKeyUp);

	//mouse registration
	glutMouseFunc(onClick);
	glutMotionFunc(onMoveDown);
	glutPassiveMotionFunc(onMove);

	#if  TIMER_ON == 1
		// timer event
		glutTimerFunc(TIMER_PERIOD, onTimer, 0);
	#endif

	//random
	srand(time(0));

	// STEP #4: Start Event Dispatcher
	// Check event queue, if there is any event
	// Call appropriate event handler based on event type
	// Do this in an infinite loop
	glutMainLoop();

	return 0;

}