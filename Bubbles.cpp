#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>
#include<iostream>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))

using namespace std;

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle,prevSpeed;
int callCount = 0, bNo = 0;

double bubbleX, bubbleY, bubbleAngle[5], speed;

struct point
{
    double x,y,z;
};

struct bubble{
    struct point centre, velocity;
    bool inCircle, inBubble[5];
};

struct point getUnitVect(struct point p){
    struct point ret;

    double s = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);
    ret.x = p.x/s;
    ret.y = p.y/s;
    ret.z = p.z/s;

    return ret;
};

double vectModulus(struct point p){

    double s = sqrt(p.x*p.x+p.y*p.y+p.z*p.z);

    return s;
};


struct point sumVect(struct point p,struct point q)
{
	struct point ret;

	ret.x=p.x+q.x;
	ret.y=p.y+q.y;
	ret.z=p.z+q.z;

	return ret;
};

struct point oppositeVect(struct point p)
{
	struct point ret;

	ret.x=-p.x;
	ret.y=-p.y;
	ret.z=-p.z;

	return ret;
};

struct point multVectScalar(struct point p, double a)
{
	struct point ret;

	ret.x=p.x*a;
	ret.y=p.y*a;
	ret.z=p.z*a;

	return ret;
};

struct point crossMult(struct point v,struct point w)
{
    struct point ret;

    ret.x = v.y*w.z - v.z*w.y;
    ret.y = v.z*w.x - v.x*w.z;
    ret.z = v.x*w.y - v.y*w.x;

    return ret;
};

double dotMult(struct point p, struct point q)
{
    double s = p.x*q.x + p.y*q.y + p.z*q.z;

    return s;
};

struct bubble bubbles[5];
struct point origin;

void drawAxes()
{
    if(drawaxes==1)
    {
        glColor3f(1.0, 1.0, 1.0);
        glBegin(GL_LINES);
        {
            glVertex3f( 100,0,0);
            glVertex3f(-100,0,0);

            glVertex3f(0,-100,0);
            glVertex3f(0, 100,0);

            glVertex3f(0,0, 100);
            glVertex3f(0,0,-100);
        }
        glEnd();
    }
}


void drawGrid()
{
    int i;
    if(drawgrid==1)
    {
        glColor3f(0.6, 0.6, 0.6);	//grey
        glBegin(GL_LINES);
        {
            for(i=-8; i<=8; i++)
            {

                if(i==0)
                    continue;	//SKIP the MAIN axes

                //lines parallel to Y-axis
                glVertex3f(i*10, -90, 0);
                glVertex3f(i*10,  90, 0);

                //lines parallel to X-axis
                glVertex3f(-90, i*10, 0);
                glVertex3f( 90, i*10, 0);
            }
        }
        glEnd();
    }
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
    glBegin(GL_QUADS);
    {
        glVertex3f( a, a,2);
        glVertex3f( a,-a,2);
        glVertex3f(-a,-a,2);
        glVertex3f(-a, a,2);
    }
    glEnd();
}


void drawCircle(double radius,int segments, struct point center)
{
    int i;
    struct point points[100];
    //glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi)+center.x;
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi)+center.y;
    }
    //draw segments using generated points
    for(i=0; i<segments; i++)
    {
        glBegin(GL_LINES);
        {
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void drawCone(double radius,double height,int segments)
{
    int i;
    double shade;
    struct point points[100];
    //generate points
    for(i=0; i<=segments; i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0; i<segments; i++)
    {
        //create shading effect
        if(i<segments/2)shade=2*(double)i/(double)segments;
        else shade=2*(1.0-(double)i/(double)segments);
        glColor3f(shade,shade,shade);

        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,0,height);
            glVertex3f(points[i].x,points[i].y,0);
            glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}


void drawSphere(double radius,int slices,int stacks)
{
    struct point points[100][100];
    int i,j;
    double h,r;
    //generate points
    for(i=0; i<=stacks; i++)
    {
        h=radius*sin(((double)i/(double)stacks)*(pi/2));
        r=radius*cos(((double)i/(double)stacks)*(pi/2));
        for(j=0; j<=slices; j++)
        {
            points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
            points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
            points[i][j].z=h;
        }
    }
    //draw quads using generated points
    for(i=0; i<stacks; i++)
    {
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
        for(j=0; j<slices; j++)
        {
            glBegin(GL_QUADS);
            {
                //upper hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
            }
            glEnd();
        }
    }
}


void drawSS()
{
    glColor3f(1,0,0);
    drawSquare(20);

    glRotatef(angle,0,0,1);
    glTranslatef(110,0,0);
    glRotatef(2*angle,0,0,1);
    glColor3f(0,1,0);
    drawSquare(15);

    glPushMatrix();
    {
        glRotatef(angle,0,0,1);
        glTranslatef(60,0,0);
        glRotatef(2*angle,0,0,1);
        glColor3f(0,0,1);
        drawSquare(10);
    }
    glPopMatrix();

    glRotatef(3*angle,0,0,1);
    glTranslatef(40,0,0);
    glRotatef(4*angle,0,0,1);
    glColor3f(1,1,0);
    drawSquare(5);
}

void keyboardListener(unsigned char key, int x,int y)
{
    switch(key)
    {

    case '1':
        drawgrid=1-drawgrid;
        break;
    case 'p':
        if(speed==0){
            speed = prevSpeed;
        }else{
            prevSpeed = speed;
            speed = 0;
        }
        break;

    default:
        break;
    }
}


void specialKeyListener(int key, int x,int y)
{
    switch(key)
    {
    case GLUT_KEY_DOWN:		//down arrow key
        cameraHeight -= 3.0;
        if(speed-.005 >0){
            speed-=.005;
            cout<<"Speed: "<<speed<<endl;
        }
        break;
    case GLUT_KEY_UP:		// up arrow key
        cameraHeight += 3.0;
        if(speed+.005 <1){
            speed+=.005;
            cout<<"Speed: "<<speed<<endl;
        }
        break;

    case GLUT_KEY_RIGHT:
        cameraAngle += 0.03;
        break;
    case GLUT_KEY_LEFT:
        cameraAngle -= 0.03;
        break;

    case GLUT_KEY_PAGE_UP:
        break;
    case GLUT_KEY_PAGE_DOWN:
        break;

    case GLUT_KEY_INSERT:
        break;

    case GLUT_KEY_HOME:
        break;
    case GLUT_KEY_END:
        break;

    default:
        break;
    }
}


void mouseListener(int button, int state, int x, int y) 	//x, y is the x-y of the screen (2D)
{
    switch(button)
    {
    case GLUT_LEFT_BUTTON:
        if(state == GLUT_DOWN) 		// 2 times?? in ONE click? -- solution is checking DOWN or UP
        {
            drawaxes=1-drawaxes;
        }
        break;

    case GLUT_RIGHT_BUTTON:
        //........
        break;

    case GLUT_MIDDLE_BUTTON:
        //........
        break;

    default:
        break;
    }
}



void display()
{
    callCount++;

    if(callCount<1000)  bNo = 1;
    else if(callCount<2000)  bNo = 2;
    else if(callCount<3000)  bNo = 3;
    else if(callCount<4000)  bNo = 4;
    else if(callCount<5000)  bNo = 5;
    //clear the display
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);	//color black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /********************
    / set-up camera here
    ********************/
    //load the correct matrix -- MODEL-VIEW matrix
    glMatrixMode(GL_MODELVIEW);

    //initialize the matrix
    glLoadIdentity();

    //now give three info
    //1. where is the camera (viewer)?
    //2. where is the camera looking?
    //3. Which direction is the camera's UP direction?

    //gluLookAt(100,100,100,	0,0,0,	0,0,1);
    //gluLookAt(200*cos(cameraAngle), 200*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
    gluLookAt(0,0,200,	0,0,0,	0,1,0);


    //again select MODEL-VIEW
    glMatrixMode(GL_MODELVIEW);


    /****************************
    / Add your objects from here
    ****************************/
    //add objects

    drawAxes();
    drawGrid();

    //glColor3f(0,1,0);
    //drawSquare(100);

    //glColor3f(1,1,1);
    //drawSquare(85);
    glColor3f(0, 1, 0);
    glBegin(GL_LINES);
    {
        glVertex3f( -100,-100,0);
        glVertex3f(100,-100,0);

        glVertex3f(100,-100,0);
        glVertex3f(100, 100,0);

        glVertex3f(100,100, 0);
        glVertex3f(-100,100,0);

        glVertex3f(-100,100,0);
        glVertex3f(-100,-100,0);
    }
    glEnd();

    glColor3f(1, 0, 0);
    struct point mainCenter;
    mainCenter.x = 0;
    mainCenter.y = 0;
    mainCenter.z = 0;
    drawCircle(60,40,mainCenter);

    //glTranslatef(bubbleX,bubbleY,0);
    //drawCircle(10,10);

    for(int i = 0; i<bNo; i++){
        //glTranslatef(bubbles[i].centre.x,bubbles[i].centre.y,0);
        drawCircle(10,10,bubbles[i].centre);
    }

    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

    //drawSphere(30,24,20);




    //ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
    glutSwapBuffers();
}


void animate()
{
    angle+=0.05;

    for(int i =0; i<bNo; i++){
        //cout<<speed<<endl;
        struct point tempPos = sumVect(bubbles[i].centre,multVectScalar(bubbles[i].velocity,speed));
        if(!bubbles[i].inCircle){       ///proceed to bounce off square
            if(tempPos.x>=-90 && tempPos.x<=90 && tempPos.y>=-90 && tempPos.y<=90){
                bubbles[i].centre = tempPos;
            }else{
                if(tempPos.x >= 90|| tempPos.x <= -90)
                    bubbles[i].velocity.x = -bubbles[i].velocity.x;
                if(tempPos.y >= 90|| tempPos.y <= -90)
                    bubbles[i].velocity.y = -bubbles[i].velocity.y;
            }
        }else{          ///within circle

            struct point vect = sumVect(tempPos,oppositeVect(origin));
            struct point prevCentre;
            if(vectModulus(vect)<=50){              ///inside circle after translation?
                prevCentre = bubbles[i].centre;
                bubbles[i].centre = tempPos;        ///translate

                ///bounce off other bubbles
                for(int j =0; j<5; j++){
                    if(i==j || !bubbles[j].inCircle || bubbles[i].inBubble[j])    continue;
                    struct point vect = sumVect(bubbles[j].centre,oppositeVect(bubbles[i].centre));
                    if(!bubbles[i].inBubble[j] && vectModulus(vect)<=20){
                        ///for i bubble
                        struct point proj = multVectScalar(getUnitVect(vect),dotMult(multVectScalar(bubbles[i].velocity,speed),vect)/vectModulus(vect));
                        struct point perpProj = sumVect(multVectScalar(bubbles[i].velocity,speed),oppositeVect(proj));
                        struct point res = sumVect(perpProj,oppositeVect(proj));
                        bubbles[i].velocity = getUnitVect(res);
                        bubbles[i].centre = prevCentre;
                    }
                }


            }else{                                  ///bounce off circle
                struct point vect = sumVect(bubbles[i].centre,oppositeVect(origin));

                struct point proj = multVectScalar(getUnitVect(vect),dotMult(vect,multVectScalar(bubbles[i].velocity,speed))/vectModulus(vect));        ///proj of A along B = A.B/mod(B)*unit(B)
                struct point perpProj = sumVect(multVectScalar(bubbles[i].velocity,speed),oppositeVect(proj));      ///proj of A perp of B = A - proj of A along B
                struct point res = sumVect(perpProj,oppositeVect(proj));    ///newdir = perp - proj
                bubbles[i].velocity = getUnitVect(res);

            }

        }

        struct point vect = sumVect(bubbles[i].centre,oppositeVect(origin));        ///AB = B -A
        if(!bubbles[i].inCircle && vectModulus(vect) <=50){    ///if inside circle
            cout<<"inside "<<i<<endl;
            bubbles[i].inCircle = true;
        }

        for(int j= 0 ; j<5; j++){       ///if bubbles overlap
            if(i==j)    continue;
            struct point vect = sumVect(bubbles[j].centre,oppositeVect(bubbles[i].centre));
            if(vectModulus(vect)>=20){
                bubbles[i].inBubble[j] = false;
            }
            if(!bubbles[i].inBubble[j] && vectModulus(vect)<20){
                if(!bubbles[i].inCircle||!bubbles[j].inCircle){
                    bubbles[i].inBubble[j] = true;
                }
            }
        }

        ///bounce of each other
        if(bubbles[i].inCircle){
            for(int j =0; j<5; j++){
                if(i==j || !bubbles[j].inCircle || bubbles[i].inBubble[j])    continue;


                struct point vect = sumVect(bubbles[j].centre,oppositeVect(bubbles[i].centre));
                struct point vect2 = oppositeVect(vect);
                if(!bubbles[i].inBubble[j] && vectModulus(vect)<=20){
                    ///for i bubble
                    struct point proj = multVectScalar(getUnitVect(vect),dotMult(multVectScalar(bubbles[i].velocity,speed),vect)/vectModulus(vect));
                    struct point perpProj = sumVect(multVectScalar(bubbles[i].velocity,speed),oppositeVect(proj));
                    struct point res = sumVect(perpProj,oppositeVect(proj));
                    bubbles[i].velocity = getUnitVect(res);
                }
            }
        }
    }
    //codes for any changes in Models, Camera
    glutPostRedisplay();
}

void init()
{
    //codes for initialization
    drawgrid=0;
    drawaxes=0;
    cameraHeight=150.0;
    cameraAngle=1.0;
    angle=0;
    ///origin point
    origin.x = 0;
    origin.y = 0;
    origin.z = 0;
    ///bubble pos
    struct point tempMov;
    for(int i = 0; i<5; i++){
        tempMov.x = rand()%100+1;
        tempMov.y = rand()%100+1;
        tempMov.z = 0;
        tempMov = getUnitVect(tempMov);
        printf("bubble: %d velocity x: %lf y: %lf\n",i,tempMov.x,tempMov.y);
        bubbles[i].velocity = tempMov;
        bubbles[i].centre.x = -90;
        bubbles[i].centre.y = -90;
        bubbles[i].centre.z = 0;
        printf("bubble: %d centre x: %lf y: %lf\n",i,bubbles[i].centre.x,bubbles[i].centre.y);
        bubbles[i].inCircle = false;
        for(int j=0; j<5; j++){
            if(i!=j)
                bubbles[i].inBubble[j] = true;
        }
    }
    speed = .01;        ///speed
    bubbleX = -90;
    bubbleY = -90;
    //clear the screen
    glClearColor(0,0,0,0);

    /************************
    / set-up projection here
    ************************/
    //load the PROJECTION matrix
    glMatrixMode(GL_PROJECTION);

    //initialize the matrix
    glLoadIdentity();

    //give PERSPECTIVE parameters
    gluPerspective(80,	1,	1,	1000.0);
    //field of view in the Y (vertically)
    //aspect ratio that determines the field of view in the X direction (horizontally)
    //near distance
    //far distance
}

int main(int argc, char **argv)
{
    srand(time(0));
    glutInit(&argc,argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(0, 0);
    glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

    glutCreateWindow("My OpenGL Program");

    init();

    glEnable(GL_DEPTH_TEST);	//enable Depth Testing

    glutDisplayFunc(display);	//display callback function
    glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

    glutKeyboardFunc(keyboardListener);
    glutSpecialFunc(specialKeyListener);
    glutMouseFunc(mouseListener);

    glutMainLoop();		//The main loop of OpenGL

    return 0;
}

