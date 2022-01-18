#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#include <windows.h>
#include <GL/glut.h>

#define pi (2*acos(0.0))
#define bulletNum 50

double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;
double rotate1=0,rotate2=0,rotate3=0,rotate4=0,rotate5=0,rotate6=0,rotate7=0,rotate8=0;


struct point
{
	double x,y,z;
};

struct bullet{
    double d1, d2, d3, d4;
};

struct point multVectScalar(struct point p, double a)
{
	struct point ret;

	ret.x=p.x*a;
	ret.y=p.y*a;
	ret.z=p.z*a;

	return ret;
};

struct point sumVect(struct point p,struct point q)
{
	struct point ret;

	ret.x=p.x+q.x;
	ret.y=p.y+q.y;
	ret.z=p.z+q.z;

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

struct point rotateAroundAxis(struct point v, struct point k, double a)
{
    struct point ret;

    ret = multVectScalar(v,cos(a*pi/180.0));
    ret = sumVect(ret,multVectScalar(crossMult(k,v),sin(a*pi/180.0)));

    return ret;
};


struct point pos, up, right, look;
struct bullet bulletArr[bulletNum];
int bulletCount = 0;

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,2);
		glVertex3f( a,-a,2);
		glVertex3f(-a,-a,2);
		glVertex3f(-a, a,2);
	}glEnd();
}


void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
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
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
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


void drawRightHemiSphere(double radius,int slices,int stacks)
{
    int toggle = 0;
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].y=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
		    toggle = 1- toggle;
            glColor3f(toggle,toggle,toggle);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }glEnd();
		}
	}
}

void drawLeftHemiSphere(double radius,int slices,int stacks)
{
    int toggle = 1;
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].z=-r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].y=-h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
		    toggle = 1- toggle;
            glColor3f(toggle,toggle,toggle);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }glEnd();
		}
	}
}


void drawSphere(double radius,int slices,int stacks)
{
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
        glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
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
			}glEnd();
		}
	}
}

void drawCylinder(double radius, double height,int slices,int stacks)
{
    int toggle = 0;
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		//h=radius*sin(((double)i/(double)stacks)*(pi/2));
		h=height*(double)i/(double)stacks;
		//r=radius*cos(((double)i/(double)stacks)*(pi/2));
		r = radius;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].y=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
		    toggle = 1- toggle;
            glColor3f(toggle,toggle,toggle);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }glEnd();
		}
	}
}


void drawOutwardsSphere(double radius,int slices,int stacks)
{
    int toggle = 0;
	struct point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		r= 2*radius-r;
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].y=h;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{

		for(j=0;j<slices;j++)
		{
		    toggle = 1- toggle;
            glColor3f(toggle,toggle,toggle);
			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
            }glEnd();
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

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
            right = rotateAroundAxis(right,up,3);
            look = rotateAroundAxis(look,up,3);
			break;
		case '2':
            right = rotateAroundAxis(right,up,-3);
            look = rotateAroundAxis(look,up,-3);
			break;
        case '3':
            look = rotateAroundAxis(look,right,3);
            up = rotateAroundAxis(up,right,3);
            break;
        case '4':
            look = rotateAroundAxis(look,right,-3);
            up = rotateAroundAxis(up,right,-3);
            break;
        case '5':
            right = rotateAroundAxis(right,look,3);
            up = rotateAroundAxis(up,look,3);
            break;
        case '6':
            right = rotateAroundAxis(right,look,-3);
            up = rotateAroundAxis(up,look,-3);
            break;
        case 'q':
            if(rotate1<=45){
                rotate1+=3;
            }
            break;
        case 'w':
            if(rotate1>=-45){
                rotate1-=3;
            }
            break;
        case 'e':
            if(rotate2<=45){
                rotate2+=3;
            }
            break;
        case 'r':
            if(rotate2>=-45){
                rotate2-=3;
            }
            break;
        case 'a':
            if(rotate3<=45){
                rotate3+=3;
            }
            break;
        case 's':
            if(rotate3>=-45){
                rotate3-=3;
            }
            break;
        case 'd':
            if(rotate4<=45){
                rotate4+=3;
            }
            break;
        case 'f':
            if(rotate4>=-45){
                rotate4-=3;
            }
            break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			cameraHeight -= 3.0;
			pos = sumVect(pos,multVectScalar(look,-2));
			break;
		case GLUT_KEY_UP:		// up arrow key
			cameraHeight += 3.0;
			pos = sumVect(pos,multVectScalar(look,2));
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
            pos = sumVect(pos,multVectScalar(right,2));
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			pos = sumVect(pos,multVectScalar(right,-2));
			break;

		case GLUT_KEY_PAGE_UP:
			pos = sumVect(pos,multVectScalar(up,2));
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos = sumVect(pos,multVectScalar(up,-2));
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


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
                double angleLim = atan2(200,500)*180/pi;
                double angleLim2 = atan2(200,470)*180/pi;
                if(fabs(rotate1)<angleLim &&
                   fabs(rotate2)<angleLim &&
                   fabs(rotate2+rotate3)<angleLim2)
                {
                    bulletArr[bulletCount].d1 = rotate1;
                    bulletArr[bulletCount].d2 = rotate2;
                    bulletArr[bulletCount].d3 = rotate3;
                    bulletArr[bulletCount].d4 = rotate4;
                    bulletCount ++;
                }
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



void display(){

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
	//gluLookAt(0,0,200,	0,0,0,	0,1,0);
	gluLookAt(pos.x,pos.y,pos.z,	pos.x+look.x,pos.y+look.y,pos.z+look.z,	up.x,up.y,up.z);
	//gluLookAt(pos.x, pos.y, pos.z, pos.x + look.x, pos.y + look.y, pos.z + look.z, up.x, up.y, up.z);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();
	drawGrid();

    //glColor3f(1,0,0);
    //drawSquare(10);
    ///board
    float boardDist = 500;
    float boardSide = 200;
    //float maxAngle1 = atan(boardSide/boardDist)*180/pi;
    //float maxAngle2 = atan(boardSide/boardDist)*180/pi;
    glPushMatrix();
    {
    	glRotatef(90,1,0,0);
        glTranslatef(0, 0, -boardDist);
        glColor3f(1, 1, 1);
        drawSquare(boardSide);
    }
    glPopMatrix();

    //glColor3f(1,1,1);
    ///left half sphere
    glPushMatrix();
    {
        glRotatef(rotate1,0,0,1);       ///for q,w
        drawLeftHemiSphere(30,30,30);
        ///right half sphere
        glRotatef(rotate2,1,0,0);       ///for e,r
        drawRightHemiSphere(30,30,30);
        ///small half sphere on top of the cylinder
        glTranslatef(0,30,0);
        glRotatef(rotate3,1,0,0);       ///for a,s
        glRotatef(rotate4,0,1,0);       ///for d,f
        glTranslatef(0,10,0);
        drawLeftHemiSphere(10,30,30);
        ///the cylinder
        drawCylinder(10,50,30,30);
        ///end of the cylinder
        glTranslatef(0,50,0);
        drawOutwardsSphere(10,30,30);
    }
    glPopMatrix();

    glColor3f(1,0,0);
    for(int i = 0; i<bulletCount; i++){
        glPushMatrix();
        {
            glRotatef(bulletArr[i].d1, 0,0,1);
            glRotatef(bulletArr[i].d2, 1,0,0);
            glTranslatef(0,30,0);
            glRotatef(bulletArr[i].d3, 1, 0, 0);
            glRotatef(bulletArr[i].d4, 0, 1, 0);
            glTranslatef(0,10,0);
            glTranslatef(0,50,0);
            glTranslatef(0, 409, 0);
            glRotatef(90,1,0,0);
            drawSquare(4);
        }
        glPopMatrix();
    }
    //drawSS();

    //drawCircle(30,24);

    //drawCone(20,50,24);

	//drawSphere(30,24,20);




	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	//angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=1;
	cameraHeight=150.0;
	cameraAngle=1.0;
	angle=0;
	/*********/
    up.x = 0;
    up.y = 0;
    up.z = 1;

    right.x = -1/sqrt(2);
    right.y = 1/sqrt(2);
    right.z = 0;

    look.x = -1/sqrt(2);
    look.y = -1/sqrt(2);
    look.z = 0;

    pos.x = 100;
    pos.y = 100;
    pos.z = 0;

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

int main(int argc, char **argv){
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
