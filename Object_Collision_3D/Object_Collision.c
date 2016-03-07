#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#define PI 3.14159265358979323846
#define DEG2RAD(DEG) ((DEG)*((PI)/(180.0)))

//static GLfloat translate[]={0.0,0.0,0.0};
float velocity_cube1=0.1, velocity_cube2 = 0.2;
//angle of rotation
float xpos = 0, ypos = 0, zpos = 500, xrot = 0, yrot = 0, angle=0.0;
int x_old, y_old, xj, yj;

//positions of the cubes
float positionz1,positionz2;
float positionx1,positionx2;

//draw the cube
void cube (void) 
{
	glClear(GL_COLOR_BUFFER_BIT);
	if(positionx2>=500)
		velocity_cube2 = -0.2;
	else if(positionx2<=-500)
		velocity_cube2 = 0.2;
		
	if(positionx1>=500)
		velocity_cube1 = -0.1;
	else if(positionx1<=-500)
		velocity_cube1 = 0.1;

	if(fabs(positionx2-positionx1)<45){
		velocity_cube2 = -velocity_cube2;
		velocity_cube1 = -velocity_cube1;
	}
		
	positionx2 += velocity_cube2*0.5;
	positionx1 += velocity_cube1*0.5;
	
	glPushMatrix();
	glTranslatef(positionx2,0,0);
    glutSolidCube(50); //draw the cube
    glPopMatrix();
    glPushMatrix();
    glTranslatef(positionx1,0,0);
	glutSolidCube(50); //draw the cube
    glPopMatrix();
    glPushMatrix();
	glutSolidSphere(30,100,100);
    glPopMatrix();
    
    //glClear(GL_COLOR_BUFFER_BIT);
 	glColor3f(0.0, 0.0, 1.0);
  	glBegin(GL_LINES);										// x-z plane
  	GLfloat i;
  	for (i = -500; i <= 500; i += 20) {
    	//glVertex3f(i, 200,0); glVertex3f(i, 100,0);
    	//glVertex3f(200, i, 0); glVertex3f(100, i, 0);
		
		glColor3f(0.0, 0.0, 1.0);glVertex3f(i, 0, 500); glVertex3f(i, 0, -500);
    	glColor3f(0.0, 0.0, 1.0);glVertex3f(500, 0, i); glVertex3f(-500, 0, i);
    	glVertex3f(-500, 0, i); glVertex3f(-500, 500, i);
    	glVertex3f(500, 0, i); glVertex3f(500, 500, i);
    	
    	
  	}
  	for(i=0; i<=500; i += 20){
	  	glVertex3f(-500, i, -500); glVertex3f(500, i, -500);	
	  	glVertex3f(i, 0, -500); glVertex3f(i, 500, -500);
		glVertex3f(i-500, 0, -500); glVertex3f(i-500, 500, -500);
  		glVertex3f(-500, i, 500); glVertex3f(-500, i, -500);
    	glVertex3f(500, i, 500); glVertex3f(500, i, -500);
    	
	  }
   	glEnd();
   	
}

void init (void) 
{
	positionz1 = 0;
	positionx1 = 250;
	positionz2 = 0;
	positionx2 = 0;
}

void enable (void) {
    glEnable (GL_DEPTH_TEST); //enable the depth testing
    glEnable (GL_LIGHTING); //enable the lighting
    glEnable (GL_LIGHT0); //enable LIGHT0, our Diffuse Light
    glShadeModel (GL_SMOOTH); //set the shader to smooth shader
}

void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
}


void reshape (int w, int h) {
    glViewport (0, 0, (GLsizei)w, (GLsizei)h); //set the viewport to the current window specifications
    glMatrixMode (GL_PROJECTION); //set the matrix to projection
    glLoadIdentity ();
    gluPerspective (60, (GLfloat)w / (GLfloat)h, 1.0, 1000.0); //set the perspective (angle of sight, width, height,depth)
    glMatrixMode (GL_MODELVIEW); //set the matrix back to model

}
	
void mouse_motion(int x, int y){
	if ((x-x_old)>0){
	    yrot += 1;
	    if (yrot >360) 
			yrot -= 360;
	}			
	else if ((x-x_old)<0){
	    yrot -= 1;
	    if (yrot < -360) 
			yrot += 360;
	}
	    
	if ((y-y_old)>0){
	    xrot += 1;
	    if (xrot >360) 
			xrot -= 360;
	}
	else if ((y-y_old)<0){
		xrot -= 1;
		if (xrot < -360)
			xrot += 360;
	}
	y_old = y;          
	x_old = x;			
}

void mouse(int btn, int state, int x, int y)  {  
	
	int wh=500;
	if(btn==GLUT_LEFT_BUTTON && state == GLUT_DOWN){  
        x_old = x;  
        y_old = (wh-y);
    }     
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
void display (void) {
    glClearColor (1.0,1.0,0.0,1.0); //clear the screen to black
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear the color buffer and the depth buffer
    glLoadIdentity();  
    camera();
    enable();
    glTranslatef(0.0f,0.0f,-10.0f);
	//glutWireCube(2.0f);
	cube(); //call the cube drawing function
    glutSwapBuffers(); //swap the buffers
    angle++; //increase the angle
}

int main (int argc, char **argv) {
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH); //set the display to Double buffer, with depth
    glutInitWindowSize (500, 500); //set the window size
	//glutInitWindowPosition (100, 100); //set the position of the window
    glutCreateWindow ("A basic OpenGL Window"); //the caption of the window
    glutFullScreen();
	
	init (); //call the init function
    glutDisplayFunc (display); //use the display function to draw everything
    glutIdleFunc (display); //update any variables in display, display can be changed to anyhing, as long as you move the variables to be updated, in this case, angle++;
	//glutTimerFunc(25, update, 0);
	//glutIdleFunc(trans);
    glutReshapeFunc (reshape); //reshape the window accordingly
	glutKeyboardFunc (keyboard); //check the keyboard
	glutMotionFunc( mouse_motion);	
	glutMouseFunc(mouse); 
	glutMainLoop (); //call the main loop
    return 0;
}
