
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cmath>

// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat GREEN[] = {0, 1, 0};
GLfloat MAGENTA[] = {1, 0, 1};
int x_old, y_old;
float xpos = 4, ypos = 3, zpos = 7, 
	xrot = 0, yrot = 0, angle=0.0;

// A camera.  It moves horizontally in a circle centered at the origin of
// radius 10.  It moves vertically straight up and down.
/*class Camera {
	double theta;      // determines the x and z positions
	double y;          // the current y position
	double dTheta;     // increment in theta for swinging the camera around
	double dy;         // increment in y for moving the camera up/down

	public:
		Camera(): theta(0), y(3), dTheta(0.04), dy(0.2) {}
		double getX() {return 10 * cos(theta);}
		double getY() {return y;}
		double getZ() {return 10 * sin(theta);}
		void moveRight() {theta += dTheta;}
		void moveLeft() {theta -= dTheta;}
		void moveUp() {y += dy;}
		void moveDown() {if (y > dy) y -= dy;}
};
*/
// A ball.  A ball has a radius, a color, and bounces up and down between
// a maximum height and the xz plane.  Therefore its x and z coordinates
// are fixed.  It uses a lame bouncing algorithm, simply moving up or
// down by 0.05 units at each frame.
class Ball {
	double radius;
	GLfloat* color;
	double maximumHeight;
	double x;
	double y;
	double z;
	int direction;
	double velocity;
	double accelaration;
	
	public:
		Ball(double r, GLfloat* c, double h, double x, double z):
      	radius(r), color(c), maximumHeight(h), direction(-1), y(h), x(x), z(z) , velocity(0), accelaration(-0.01){

  		}
  
  	void update() {
			velocity += accelaration;
			
			y += velocity;
    	if (y > maximumHeight) {
      		y = maximumHeight; direction = -1;
      						velocity *= -1;

    	} else if (y < radius) {
      		y = radius; direction = 1;
      						velocity *= -1;

    	}
    	glPushMatrix();
		glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
		glTranslated(x, y, z);
		glutSolidSphere(radius, 30, 30);
		glPopMatrix();
	}
};

class Cube {
	double side;
	GLfloat* color;
	int direction;
	double x;
	double y;
	double z;
	
	public:
		Cube(double side, GLfloat* c, double x, double y, double z):
		side(side), color(c), x(x), y(y), z(z), direction(1){
			
		}	
		
		void update(){
			x += direction* 0.05;
			
			if(x > 16) {
				x = 16;
				direction = -1;
			}else if(x<0){
				x = 0;
				direction = 1;

			}
			glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x,y,z);
			glutSolidCube(side); //draw the cube
    		glPopMatrix();
		}
};

// A checkerboard class.  A checkerboard has alternating red and white
// squares.  The number of squares is set in the constructor.  Each square
// is 1 x 1.  One corner of the board is (0, 0) and the board stretches out
// along positive x and positive z.  It rests on the xz plane.  I put a
// spotlight at (4, 3, 7).
class Checkerboard {
	int displayListId;
	int width;
	int depth;

	public:
  		Checkerboard(int width, int depth): width(width), depth(depth) {}
  		double centerx() {return width / 2;}
  		double centerz() {return depth / 2;}
  		void create() {
		    displayListId = glGenLists(1);
		    glNewList(displayListId, GL_COMPILE);
		    GLfloat lightPosition[] = {4, 3, 7, 1};
		    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		    glBegin(GL_QUADS);
		    glNormal3d(0, 1, 0);
		    
		    for (int x = 0; x < width - 1; x++) {
		      	for (int z = 0; z < depth - 1; z++) {
        			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                    (x + z) % 2 == 0 ? RED : WHITE);
        			
					glVertex3d(x, 0, z);
        			glVertex3d(x+1, 0, z);
        			glVertex3d(x+1, 0, z+1);
        			glVertex3d(x, 0, z+1);
      			}
      			
    		}
    		for (int x = 0; x < width - 1; x++) {
		      	for (int z = 0; z < depth - 1; z++) {
        			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                    (x + z) % 2 == 0 ? RED : WHITE);
        			glVertex3d(x, z, 0);
        			glVertex3d(x+1, z, 0);
        			glVertex3d(x+1, z+1, 0);
        			glVertex3d(x, z+1, 0);
      			}
    		}
/*    		for (int x = 0; x < width - 1; x++) {
		      	for (int z = 0; z < depth - 1; z++) {
        			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                    (x + z) % 2 == 0 ? RED : WHITE);
        			glVertex3d(0, z, x);
        			glVertex3d(0, z, x+1);
        			glVertex3d(0, z+1, x+1);
        			glVertex3d(0, z+1, x);
      			}
    		}*/
    		
    		glEnd();
    		glEndList();
  		}
	 	void draw() {
    		glCallList(displayListId);
  		}		
};

// Global variables: a camera, a checkerboard and some balls.
Checkerboard checkerboard(16, 16);
//Camera camera;
Ball balls[] = {
  	Ball(1, GREEN, 10, 6, 1),
  	Ball(1.5, MAGENTA, 12, 3, 4),
  	Ball(0.4, WHITE, 15, 1, 7)
};
Cube cube(2, GREEN, 4,5,2);


void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
}

// Application-specific initialization: Set up global lighting parameters
// and create display lists.
void init() {
	  glEnable(GL_DEPTH_TEST);
	  glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	  glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	  glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	  glMaterialf(GL_FRONT, GL_SHININESS, 30);
	  glEnable(GL_LIGHTING);
	  glEnable(GL_LIGHT0);
	  checkerboard.create();
}

// Draws one frame, the checkerboard then the balls, from the current camera
// position.
void display() {
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glLoadIdentity();
/*  	gluLookAt(camera.getX(), camera.getY(), camera.getZ(),
            checkerboard.centerx(), 0.0, checkerboard.centerz(),
            0.0, 1.0, 0.0);*/
    camera();
  	checkerboard.draw();
  	for (int i = 0; i < sizeof balls / sizeof(Ball); i++) {
    	balls[i].update();
  	}
  	cube.update();
  	glFlush();
  	glutSwapBuffers();
  	angle++;
}

// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
	  glViewport(0, 0, w, h);
	  glMatrixMode(GL_PROJECTION);
	  glLoadIdentity();
	  gluPerspective(40.0, GLfloat(w) / GLfloat(h), 1.0, 150.0);
	  glMatrixMode(GL_MODELVIEW);
}

// Requests to draw the next frame.
void timer(int v) {
  glutPostRedisplay();
  glutTimerFunc(1000/60, timer, v);
}

// Moves the camera according to the key pressed, then ask to refresh the
// display.
/*void special(int key, int, int) {
  switch (key) {
    case GLUT_KEY_LEFT: camera.moveLeft(); break;
    case GLUT_KEY_RIGHT: camera.moveRight(); break;
    case GLUT_KEY_UP: camera.moveUp(); break;
    case GLUT_KEY_DOWN: camera.moveDown(); break;
    
  }
  glutPostRedisplay();
}*/
/*void keyboard(unsigned char key, int x, int y){
	if(key==27){
		exit(0);
	}	
}
*/
void mouse_motion(int x, int y){
	if ((x-x_old)>0){
		//camera.moveRight();
	    yrot += 1;
	    if (yrot >360) 
			yrot -= 360;
	}			
	else if ((x-x_old)<0){
		//camera.moveLeft();
	    yrot -= 1;
	    if (yrot < -360) 
			yrot += 360;
	}
	    
	if ((y-y_old)>0){
		//camera.moveUp();
	    xrot += 1;
	    if (xrot >360) 
			xrot -= 360;
	}
	else if ((y-y_old)<0){
		//camera.moveDown();
		xrot -= 1;
		if (xrot < -360)
			xrot += 360;
	}
	y_old = y;          
	x_old = x;			
}

void keyboard (unsigned char key, int x, int y) {
    if (key=='q'){
    	xrot += 1;
    	if (xrot >360) 
			xrot -= 360;
    }

    if (key=='z'){
	    xrot -= 1;
    	if (xrot < -360) 
			xrot += 360;
    }

    if (key=='w'){
    	float xrotrad, yrotrad;
    	yrotrad = (yrot / 180 * 3.141592654f);
    	xrotrad = (xrot / 180 * 3.141592654f); 
    	xpos += sin(yrotrad) ;
    	zpos -= (cos(yrotrad)) ;
    	ypos -= (sin(xrotrad)) ;
    }

    if (key=='s'){
	    float xrotrad, yrotrad;
    	yrotrad = (yrot / 180 * 3.141592654f);
    	xrotrad = (xrot / 180 * 3.141592654f); 
    	xpos -=(sin(yrotrad));
    	zpos += (cos(yrotrad)) ;
    	ypos += (sin(xrotrad));
    }

    if (key=='d'){
    	yrot += 1;
    	if (yrot >360) 
			yrot -= 360;
    }
    
    if (key=='a'){
	    yrot -= 1;
    	if (yrot < -360)
			yrot += 360;
    }
    
	if (key==27){
	    exit(0);
    }
}



void mouse(int btn, int state, int x, int y)  {  
	
	int wh=500;
	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN){  
        x_old = x;  
        y_old = (wh-y);
    }     
}  


// Initializes GLUT and enters the main loop.
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	//glutInitWindowPosition(80, 80);
	//glutInitWindowSize(800, 600);
	glutCreateWindow("Collision of Objects");
	glutFullScreen();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	//glutSpecialFunc(special);
	glutKeyboardFunc (keyboard);
	glutMotionFunc( mouse_motion);	
	glutMouseFunc(mouse);
	glutTimerFunc(100, timer, 0);
	init();
	glutMainLoop();
}
