/*********
   CTIS164 - Template Source Program
----------
STUDENT : MUHAMMAD BILAL
BILKENT ID: 22101368
SECTION : 4
HOMEWORK: 2
----------
PROBLEMS:
THE DISPLAY ANGLE GLITCHES AT TIMES (IT CONSTANTLY STRUGGLES BETWEEN TWO VALUES)
ITS NOT A PROBLEM BUT I PREFER TO USE THE NEGATIVE ANGLES (so it might be a problem in terms of using only positive angles)
MY CODE MIGHT NOT BE EFFICIENT AS I HAVE USED SOME CODE SEGMENTS TWICE, SO I KNOW ITS A PROBLEM
----------
ADDITIONAL FEATURES:
TWO DIFFERENT MODES: STATIC AND AUTOMATIC
THE AUTOMATIC MODE HAS THE AUTOMATIC BARREL
TWO DIFFERENT COMPLEX SHAPES
DESIGN (extra)
THE ENEMY DEAD COUNT
THE AUTOMATIC STATE KEEPS ROTATING, AND THROWS ONLY WHEN IN LINE OF SIGHT
DREW EXTRA THINGS FOR MAKING THE SCREEN LOOK ENTICING (SPIRALS AND THE MOON)


ADDITIONAL NOTES: 
KINDLY USE THE x86 version while running my code and change the gl to GL (i used the gl folder on my PC)
MY PROGRAM MEETS ALL THE MINIMUM REQUIREMENTS. THE PROBLEMS MENTIONED ABOVE ARE JUST EXTRA PROBLEMS THAT I ENCOUNTERED DURING MY CODING SESSIONS
*********/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <gl/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#define UPBOWND 50  //bounds for the automatic mode
#define LOWBOWND -50
#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800
#define REAPPEARTIME 100
#define REAPPEARTIME2 30

#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON         1 // 0:disable timer, 1:enable timer
#define SECDIVIDEBY16 62.5  //the divide factor for the 16 milliseconds refresh
#define D2R 0.0174532
int reappear = 0;  //variables for the modes
int EnemyDeadcount = 0;
float angleattack;
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height
bool lineofsight = false;
bool lineofsight2 = false;
int count = 0;
float timercount = 60;  //60 seconds counter
bool spacebar = false;
bool STOP = false;



const char MODE[10][10] = { "AUTO", "MANUAL" };
typedef struct
{

    float canonX, canonY, radius;
    float angle;
 
   
}Canons;
Canons canon= {-WINDOW_WIDTH/2 + 100, 0,0,0};
typedef struct
{

    int r, g, b;
}color_t;
color_t color = { 255,255,255};  //color struct for random number

typedef struct
{

    float X, Y;
    float radius;  //balls radius
    float angle;
    float active;

}balls;
typedef struct  //ball struct
{
    float BatX;
    float BatY;
  
}bat;  //bat struct
bat enemy;
balls ball;  
typedef struct
{
    float dy, dx, m, c;

}line_t;  //line struct
line_t line;
float speed = 10;
float length = 20;
float iniX2, iniY2;
float iniX, iniY;
bool UP = true;  //upper and lower bounds
bool DOWN = false;
#define START 0
#define RUN 1   //game states (including automatic and static barrels)
#define GAMEOVER 2
#define AUTOMATICBARREL 3 
#define STATICBARREL 4
int currentMode = STATICBARREL;
bool isEnemyThere = true;
int currentstate = START;
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
void drawellipse(int x, int y, float Rx, float Ry) { 

    float angle;
    glBegin(GL_POLYGON);
    for (int i = 0; i < 360; i++) {
        angle = i * D2R;
        glVertex2f(x + Rx * cos(angle), y + Ry * sin(angle));

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
void drawbullet()  //draw the ball
{
    if (ball.active)
    {

        glColor3f(1, 1, 1);
        circle(ball.X, ball.Y, ball.radius);
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
float distance(float x1, float y1, float x2, float y2)
{
    float dis = sqrtf(powf(x2 - x1, 2) + powf(y2 - y1, 2));
    return dis;
}
// display text with variables.
// vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
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

// vprint2(-50, 0, 0.35, "00:%02d", timeCounter);
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
void backgroundGradientandplatform(float x, float y, float width, float height, float r, float g, float b)  //gradient background
{

    glBegin(GL_QUADS);
    glColor3f(r, g, b);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glColor3f(r + 0.3, g + 0.3, b + 0.3);
    glVertex2f(x + width, y - height);
    glVertex2f(x, y - height);
    glEnd();

}
void Spikes(float x, float y)  //spikes for design
{
   
    glBegin(GL_LINES);
    glVertex2f(x, y);
    for (float angle = 0; angle <= 360; angle += 45)
    {

        glVertex2f(x + 12 * cos(angle * D2R), y + 12 * sin(angle * D2R));
        glVertex2f(x, y);

    }
    glEnd();
}
void timer()  //timer fucntion for the count (60 seconds)
{
    glColor3f(0, 0, 0);
    glRectf(-winWidth / 2, -winHeight / 2, winWidth / 2, -winHeight / 2+60);
    glColor3f(1, 1, 1);
    vprint(-winWidth / 2 + 100, -winHeight / 2 + 20 , GLUT_BITMAP_HELVETICA_18, "Timer : %.2f", timercount);

}
void halfcircle(float x, float y, float r)
{
   /* canon.canonX = x;
    canon.canonY = y;
    canon.radius = r;*/
  
  
    glBegin(GL_POLYGON);
    for (float angle = 90; angle <= 270; angle += 10)
    {
        glVertex2f(x + r * cos(angle * D2R), y + r * sin(angle * D2R));

    }
    glEnd();
     
}
void halfcircle2(float x, float y, float r)  //another half circle for the car
{
  
    glBegin(GL_POLYGON);
    for (float angle = 90; angle >= -90; angle -= 10)
    {
        glVertex2f(x + r * cos(angle * D2R), y + r * sin(angle * D2R));

    }
    glEnd();

}
void barrelaim()  //this is the barrel aim with two modes
{
    switch (currentMode)  //two different barrel modes 
    {
    case AUTOMATICBARREL:
        iniX = canon.canonX;
        iniY = canon.canonY;
        iniY2 = 100 * sin(canon.angle * D2R);
        iniX2 = 100 * cos(canon.angle * D2R);
        break;
    case STATICBARREL:  //static mode
        iniX = canon.canonX;
        iniY = canon.canonY;
        if (STOP == false)
        {
            iniY2 = 100 * sin(canon.angle*D2R);
            iniX2 = 100 * cos(canon.angle * D2R);
        }
        else
        {
            iniY2 = 100 * sin(angleattack * D2R);
            iniX2 = 100 * cos(angleattack * D2R);
        }
       
        break;
    }
   
    glBegin(GL_LINES);
    glVertex2f(iniX, iniY);
    glVertex2f(iniX + iniX2 , iniY + iniY2);
    glEnd();

}
void angles()  //angle between the enemy and the canon
{
     
     float dy = enemy.BatY - iniY;
     float dx = enemy.BatX - iniX;

     angleattack = atan2(dy, dx) / D2R;
     //if ((int)angleattack == (int)canon.angle)  //typecasting for equating angles
     //{
     //    fire = true; //checking if the angles are equal, then fire only
     //}
     
}
void drawline(float x, float y, float x2, float y2)
{
    glLineWidth(1.2);
    glBegin(GL_LINES);
    glVertex2f(x, y);
    glVertex2f(x2, y2);
    glEnd();
    glLineWidth(1);
}
//void checkConnection()  //extra fucntion for checking the line of sight
//{
//   
//        glColor3f(1, 1, 1);
//        drawline(iniX, iniY, enemy.BatX, enemy.BatY);
//    
//    
//
//}
void drawEnemy()  //enemy code
{
   
    glColor3f(0, 0, 0);
    glBegin(GL_TRIANGLES);  //enemy wings
    glVertex2f(enemy.BatX - 30, enemy.BatY + 15);
    glVertex2f(enemy.BatX + 30, enemy.BatY + 15);
    glVertex2f(enemy.BatX, enemy.BatY - 20);
    glEnd();
    glColor3ub(color.r, color.g, color.b);
    drawellipse(enemy.BatX, enemy.BatY, 10, 20);  //the remaining body of the enemy
    circle(enemy.BatX, enemy.BatY + 20, 8);
    glColor3f(1, 0, 0);
    glPointSize(3);
    glBegin(GL_POINTS);
    glVertex2f(enemy.BatX - 4, enemy.BatY + 22);
    glVertex2f(enemy.BatX + 4, enemy.BatY + 22);
    glEnd();
    drawline(enemy.BatX - 4, enemy.BatY - 20, enemy.BatX - 10, enemy.BatY - 30);
    drawline(enemy.BatX + 4, enemy.BatY - 20, enemy.BatX + 10, enemy.BatY - 30);
    drawline(enemy.BatX - 4, enemy.BatY + 20, enemy.BatX - 10, enemy.BatY + 30);
    drawline(enemy.BatX + 4, enemy.BatY + 20, enemy.BatX + 10, enemy.BatY + 30);
    glColor3f(0, 0, 0);
    drawellipse(enemy.BatX, enemy.BatY + 17, 3,2);

 
}
void RandomGenerate()  //random generator for the enemy
{
    enemy.BatX = rand() % (650 - -(200) + 1 ) - 200;
    enemy.BatY = rand() % (300 - (-300) + 1) - 300;
    color.r =  (rand() % 255);  //generate 3 different colors
    color.g = (rand() % 255);
    color.b = (rand() % 255);

}
int checkifthepointisonline(float pointx, float pointy)  //checking if the point is on line
{
     line.dy = enemy.BatY - iniY;
    line.dx = enemy.BatX - iniX;
    line.m= line.dy / line.dx;
    line.c = enemy.BatY - (line.m * (enemy.BatX));   //y=mx+c

    if ((int)pointy == (int)(((line.m) * pointx) + line.c))
    {
        return 1;  //return either true or false based on if the point is on the line or not (the barrels edge)
      
    }
    else
    {
        return 0;
      
    }
    
}
void drawCanon()  //canon on the car
{
    glColor3f(0.6, 0.1, 0);
    halfcircle(canon.canonX, canon.canonY, 20);
    halfcircle2(canon.canonX + 60, canon.canonY, 20);
    glRectf(canon.canonX + 20 * cos(90 * D2R), canon.canonY + 20 * sin(90 * D2R), (canon.canonX + 60) + 20 * cos(-90 * D2R), canon.canonY + 20 * sin(-90 * D2R));  //THIS WORKED!! body
    glColor4f(1, 1, 1, 0.7);
    glColor3f(0.6, 0.1, 0);
    glColor4f(0, 0, 0, 0.7);
    circle(canon.canonX, canon.canonY - 20, 15);
    circle(canon.canonX + 60, canon.canonY - 20, 15);
    glColor4f(1, 1, 1, 0.7);
    Spikes(canon.canonX, canon.canonY - 20);
    Spikes(canon.canonX + 60, canon.canonY - 20);
    circle(canon.canonX + 70, canon.canonY + 9, 2);
    barrelaim();
   
 

}
void spiral(float x, float y, float r, float start, float end, float decr)  //spiral display for design
{
    glBegin(GL_LINE_STRIP);
 
    for (float angle = start; angle <= end; angle += 10)
    {
        glVertex2f(x + r * cos(angle * D2R), y + r * sin(angle * D2R));
        r -= decr;

    }
    glEnd();

}
void DrawMoon()  //moon display
{
    glColor3f(1, 1, 1);
    circle(canon.canonX+30, canon.canonY + 250, 40);
    glColor3f(0, 0, 0);
   
    circle(canon.canonX + 20, canon.canonY+260, 1.5);
    circle(canon.canonX + 50, canon.canonY + 230, 1.5);
    circle(canon.canonX + 10, canon.canonY + 220, 1.5);
    circle(canon.canonX + 15, canon.canonY + 240, 1.5);
    circle(canon.canonX + 10, canon.canonY + 280, 1.5);
    circle(canon.canonX + 40, canon.canonY + 280, 1.5);

    


}

void display() {
    //
    // clear window to black
    //
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    backgroundGradientandplatform(-winWidth/2, winHeight/2, WINDOW_WIDTH,WINDOW_HEIGHT,0.05, 0.05, 0.1);

    switch (currentstate)
    {

    case START:
        //drawclicktostart
        glColor3f(1, 1, 1);
        vprint(-80, 0, GLUT_BITMAP_TIMES_ROMAN_24, "Click to Start!");
        break;

    case RUN:  //run mode
        backgroundGradientandplatform(canon.canonX - 15, canon.canonY - 35, 80, 400, 0.5, 0.6, 0.6);  //drawing the required things for the run state
        drawbullet();
        drawCanon();
        glColor3f(1, 1, 1);
        timer();
        vprint(canon.canonX + 25, canon.canonY, GLUT_BITMAP_HELVETICA_10, "%0.0f", canon.angle);
        vprint(winWidth / 2 - 100, -winHeight / 2 + 20, GLUT_BITMAP_HELVETICA_18, "22101368");
        vprint(-winWidth / 2 + 20, winHeight / 2 - 30, GLUT_BITMAP_HELVETICA_18, "MODE: "); 
        vprint(-winWidth / 2 + 20, winHeight / 2 - 50, GLUT_BITMAP_8_BY_13, "(press Spacebar to change the mode)");

        vprint(winWidth / 2 - 130, winHeight / 2 - 20, GLUT_BITMAP_HELVETICA_12, "MUHAMMAD BILAL");
        DrawMoon();
        glColor3f(0, 0, 0);
        spiral(canon.canonX+20,canon.canonY-100, 40, 0,720, 0.5);
        spiral(canon.canonX + 20, canon.canonY - 300, 40, 0, 720, 0.5);
        spiral(canon.canonX + 20, canon.canonY - 200, 40, 0, 720, 0.5);
       

        glColor3f(0, 1, 0);
        if (spacebar == false)   //specific to modes
        {
            currentMode = STATICBARREL;
            vprint(-winWidth / 2 + 70, winHeight / 2 - 30, GLUT_BITMAP_HELVETICA_18, "%10s", MODE[1]);


        }
        else
        {
            currentMode = AUTOMATICBARREL;
            vprint(-winWidth / 2 + 70, winHeight / 2 - 30, GLUT_BITMAP_HELVETICA_18, "%10s", MODE[0]);

        }

        if (EnemyDeadcount != -1)
        {
            vprint(0, -winHeight / 2 + 20, GLUT_BITMAP_HELVETICA_18, "Hit Count: %d", EnemyDeadcount);

        }


        // checkConnection();
        if (isEnemyThere)  //if enemy exists, draw it
        {
            drawEnemy();

        }
        break;
    case GAMEOVER:
        glColor3f(1, 0, 0);
        vprint(0, 0, GLUT_BITMAP_TIMES_ROMAN_24, "GAMEOVER!");
        glColor3f(1, 1, 1);
        vprint(-80, -50, GLUT_BITMAP_TIMES_ROMAN_24, "YOUR EARNED : ");
        glColor3f(0, 1, 0);
        vprint(120, -50, GLUT_BITMAP_TIMES_ROMAN_24, " %d POINTS!", EnemyDeadcount);

        break;
    }
  

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
    if (key == 32)
    {

        spacebar=!spacebar;  //for changing the mode
        canon.angle = 0;
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

//
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
//
void onSpecialKeyDown(int key, int x, int y)
{
    // Write your codes here.
    switch (key) {
    case GLUT_KEY_UP: up = true; break;
    case GLUT_KEY_DOWN: down = true; break;
    case GLUT_KEY_LEFT: left = true; break;
    case GLUT_KEY_RIGHT: right = true; break;
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

    if (button == GLUT_LEFT_BUTTON && stat == GLUT_DOWN && currentstate == START)
    {

        currentstate = RUN;  //changes the state
    }
   

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

void IncrementAngle(Canons* canon, float increment)  //this increment angle function is only for the automatic barrel
{

    if (canon->angle > LOWBOWND && canon->angle < UPBOWND && UP)   //it checks for the bounds
    {

        canon->angle += increment;  //increments it

        DOWN = true;
        if (canon->angle == UPBOWND - 1)  //then send it down 
        {
            UP = !UP;
            DOWN = true;
        }

    }
    else if (canon->angle < UPBOWND && canon->angle > LOWBOWND && DOWN)  //if down, send it up
    {
        canon->angle -= increment;

        if (canon->angle == LOWBOWND + 1)
        {
            DOWN = !DOWN;
            UP = true;
        }
    }

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
   
    switch (currentMode)
    {
       case AUTOMATICBARREL:  //automatic mode
        if (currentstate == RUN)
        {
            // ImplementFastShooting();

            if (isEnemyThere)
            {
                angles(); //checks for angles
            }
            count++;

            if (timercount > 0)
            {
                timercount -= 1 / (SECDIVIDEBY16);  //the same counter as in static mode
            }
            else {

                currentstate = GAMEOVER;  //gameover state

            }
            if (count == 250)  //count for regenerating the bat
            {

                RandomGenerate();
                count = 0;




            }

            IncrementAngle(&canon, 0.5);
            if (isEnemyThere)
            {
                if (checkifthepointisonline(iniX + iniX2, iniY + iniY2) == 1)  //FIXED IT!!!!!  //checks for line of sight
                { 
                    lineofsight = true;
                }
                else
                {
                    lineofsight = false;
                }

            }


            if (!ball.active && lineofsight) //only going to set it once!
            {
                //its the same code in the static mode
                ball.angle = angleattack;
                ball.X = iniX;
                ball.Y = iniY;
                ball.active = true;
            }

            if (ball.active)
            {
                ball.X += speed * cos(ball.angle * D2R);
                ball.Y += speed * sin(ball.angle * D2R);


            }

            if (ball.X > winWidth / 2 || ball.Y > winHeight / 2)
            {
                ball.active = false;
            }
            if (distance(ball.X, ball.Y, enemy.BatX, enemy.BatY) <= 15 && distance(ball.X, ball.Y, enemy.BatX, enemy.BatY) >= 10)
            {

                EnemyDeadcount += 1;
                isEnemyThere = false;
            }
            if (isEnemyThere == false)
            {
                reappear++;
            }
            if (reappear == REAPPEARTIME)
            {

                reappear = 0;
                isEnemyThere = true;
            }
         

        }
        break;

       case STATICBARREL:  //static barrel is another mode
          
           if (currentstate == RUN)  //checks if the mode is RUN
           {
               if (isEnemyThere)
               {
                   angles();

               }
          
               if (timercount > 0)  //displays timer
               {
                   timercount -= 1 / (SECDIVIDEBY16);
               }
               else {

                   currentstate = GAMEOVER;  //gameover state

               }
             
               if (canon.angle < angleattack)  //checks if its above or below
               {
                   canon.angle += 0.15;
               }
               else if (canon.angle > angleattack)
               {
                   canon.angle -= 0.15;
               }
               if (isEnemyThere)
               {
                   if (checkifthepointisonline(iniX + iniX2, iniY + iniY2) == 1)  //FIXED IT!!!!!   //line of sight
                   {
                       lineofsight = true;
                       STOP = true;
                   }
                   else
                   {

                       lineofsight = false;
                       
                       STOP = false;
                   }

               }


               if (!ball.active && lineofsight && isEnemyThere) //only going to set it once!
               {

                   ball.angle = angleattack;  //sets the balls position
                   ball.X = iniX;
                   ball.Y = iniY;
                   ball.active = true;
               }

               if (ball.active)  //moves it
               {
                   ball.X += speed * cos(ball.angle * D2R);
                   ball.Y += speed * sin(ball.angle * D2R);


               }

               if (ball.X > winWidth / 2 || ball.Y > winHeight / 2)  //if its outside the window, regenerate the ball
               {
                   ball.active = false;
               }
               if (distance(ball.X, ball.Y, enemy.BatX, enemy.BatY) <= 15 && distance(ball.X, ball.Y, enemy.BatX, enemy.BatY) >= 10)  //checks for collision
               {

                   EnemyDeadcount += 1;  //dead count
                   isEnemyThere = false;
                   RandomGenerate(); //generates randomly
               }
               if (isEnemyThere == false)
               {
                   reappear++;  //reappear the bat
               }
               if (reappear == REAPPEARTIME2) //for static mode 
               {

                   reappear = 0;
                   isEnemyThere = true;
               }

           }
           
           break;
    }
    // to refresh the window it calls display() function
    glutPostRedisplay(); // display()

}
#endif

void Init() {

    // Smoothing shapes
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    srand(time(NULL));
   
       ball = { canon.canonX + 20 , canon.canonY, 5, 0, false};  //intializing the ball

    

}
void main(int argc, char* argv[]) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
    glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
    //glutInitWindowPosition(100, 100);
    glutCreateWindow("SMASH THE BATS");

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