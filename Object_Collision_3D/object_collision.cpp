
#ifdef __APPLE_CC__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <GL/gl.h>
#include <cmath>
#include <stdlib.h>
#include<math.h>
#include<stdio.h>
// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat YELLOW[] = {1, 1, 0};
GLfloat BLUE[] = {0, 0, 1};
GLfloat GREEN[] = {0, 1, 0};
GLfloat BROWN[] = {0.64, 0.16, 0.16};
GLfloat DARK_GREEN[] = {0, 0.39215, 0 }; 
GLfloat MAGENTA[] = {1, 0, 1};
GLuint texture; //the array for our texture

int x_old, y_old, j;
float xpos = 4, ypos = 3, zpos = 7, xrot = 0, yrot = 0, angle=0.0;

GLuint LoadTexture( const char * filename, int width, int height ){
    GLuint texture;
    unsigned char * data;
    FILE * file;
    file = fopen( filename, "rb" );
    if ( file == NULL ) 
		return 0;
    
	data = (unsigned char *)malloc( width * height * 3 );
    fread( data, width * height * 3, 1, file );
    fclose( file );
    glGenTextures( 1, &texture ); //generate the texture with the loaded data
    glBindTexture( GL_TEXTURE_2D, texture ); //bind the textureto it’s array
    //glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE ); //set texture environment parameters
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
  //  glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data ); 
    free( data ); //free the texture
    return texture; //return whether it was successfull
}

void FreeTexture( GLuint texture ){
	glDeleteTextures( 1, &texture ); 
}



class Vector {

	public:
		double x;
		double y;
		double z;

		Vector(double x, double y, double z):x(x), y(y), z(z) {
			
		}
		
		double angle_btwn_vectors(Vector vec){
			double angle = x*vec.x + y*vec.y +  z*vec.z;
			return angle;
		}
		Vector perpendicular(){
			return Vector(-z, 0 , x);
		}
		
		Vector crossProduct(Vector v) {
			return Vector (y*v.z - z*v.y, z*v.x - x*v.z, x*v.y - y*v.x);
		}
		double magnitude () {
			return sqrt((x*x) + (y*y) + (z*z));
		}
		Vector normalize () {
			double magnitude = sqrt((x*x) + (y*y) + (z*z));
			if(magnitude==0)
				return Vector(0,0,0);
				
			return Vector(x/magnitude, y/magnitude, z/magnitude);
		}
		double distance_btwn_vectors(Vector vec){
			return sqrt((x - vec.x)*(x - vec.x) + (y - vec.y)*(y- vec.y) + (z - vec.z)*(z - vec.z));
		}
		Vector add(Vector vec){
			return Vector(x + vec.x, y + vec.y, z + vec.z);
		}
		Vector reduce_magnitude(){
			return Vector(x*0.9, y*0.9, z*0.9);
		}
		Vector multiplyconstant (double scalar) {
			printf("%lf %lf %lf \n", x, y, z);
			return Vector (x*scalar, y*scalar, z*scalar);
		}
		
		Vector multiply_matrix (double angle) {
			double m[3][3];
			angle = (angle * 0.01745329251);                                //degree to radian conversion
	     	printf("\nAngle = %f, cos(%f) = %f\n",angle,angle,cosf(angle));
	    	m[0][0] = cosf(angle);
	     	m[0][1] = sinf(angle);
	     	m[0][2] = 0; //refX * (1 - cosf(angle)) + refY*(sinf(angle));
	     	m[1][0] = -sinf(angle);
	     	m[1][1] = cosf(angle);
	     	m[1][2] = 0; //refY * (1 - cosf(angle)) + refX*(sinf(angle));
	     	printf("%lf\n", z);
			 return Vector(x*m[0][0] + z * m[0][1] ,y, x*m[1][0] + z * m[1][1] );
		}
};

class Checkerboard {
	int displayListId;
	int width;
	int depth;

	public:
  		Checkerboard(int width, int depth): width(width), depth(depth) {}
  		double centerx() {return width / 2;}
  		double centerz() {return depth / 2;}
  		void create() {
		    //glBindTexture( GL_TEXTURE_2D, texture );
			displayListId = glGenLists(1);		    
			glNewList(displayListId, GL_COMPILE);
			//GLfloat lightPosition[] = {4, 3, 7, 1};
		    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
			glBegin(GL_QUADS);
		    glNormal3d(0, 1, 0);
		    for (int x = 0; x < width - 1; x++) {
		    	for (int z = 0; z < depth - 1; z++) {
        			//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                    //(x + z) % 2 == 0 ? GREEN : DARK_GREEN );	
					glTexCoord2d(0.0,0.0);glVertex3d(x, 0, z);
        			glTexCoord2d(1.0,0.0);glVertex3d(x+1, 0, z);
        			glTexCoord2d(1.0,1.0);glVertex3d(x+1, 0, z+1);
        			glTexCoord2d(0.0,1.0);glVertex3d(x, 0, z+1);
        			//glBindTexture(GL_TEXTURE_2D, 0);
      			}
    		}
			
    		glEnd();
			glEndList();
  		}
	 	void draw() {
    		glCallList(displayListId);
  		}		
};
void draw_border(){
			glBegin(GL_QUADS);
		    glNormal3d(0, 1, 0);
		    glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, BROWN);
			for (int z = 0; z < 47; z++) {
				glVertex3d(0, 0, z);
    			glVertex3d(0, 1, z);
    			glVertex3d(0, 1, z+1);
    			glVertex3d(0, 0, z+1);
      		}
      		for (int z = 0; z < 47; z++) {	
				glVertex3d(23, 0, z);
    			glVertex3d(23, 1, z);
    			glVertex3d(23, 1, z+1);
    			glVertex3d(23, 0, z+1);
      		}
			for (int x = 0; x < 23; x++) {
    			
				glVertex3d(x, 0, 0);
    			glVertex3d(x, 1, 0);
    			glVertex3d(x+1, 1, 0);
    			glVertex3d(x+1, 0, 0);
      		}
			for (int x = 0; x < 23; x++) {        			
				glVertex3d(x, 0, 47);
    			glVertex3d(x, 1, 47);
    			glVertex3d(x+1, 1, 47);
    			glVertex3d(x+1, 0, 47);
      		}	
      		
      		for (int z = -1; z < 48; z++) {
				glVertex3d(-1, 0, z);
    			glVertex3d(-1, 1, z);
    			glVertex3d(-1, 1, z+1);
    			glVertex3d(-1, 0, z+1);
      		}
      		for (int z = -1; z < 48; z++) {	
				glVertex3d(24, 0, z);
    			glVertex3d(24, 1, z);
    			glVertex3d(24, 1, z+1);
    			glVertex3d(24, 0, z+1);
      		}
			for (int x = -1; x < 24; x++) {
    			
				glVertex3d(x, 0, -1);
    			glVertex3d(x, 1, -1);
    			glVertex3d(x+1, 1, -1);
    			glVertex3d(x+1, 0, -1);
      		}
			for (int x = -1; x < 24; x++) {        			
				glVertex3d(x, 0, 48);
    			glVertex3d(x, 1, 48);
    			glVertex3d(x+1, 1, 48);
    			glVertex3d(x+1, 0, 48);
      		}
      		for (int x = -1; x < 24; x++) {        			
				glVertex3d(x, 1, 47);
    			glVertex3d(x+1, 1, 47);
    			glVertex3d(x+1, 1, 48);
    			glVertex3d(x, 1, 48);
      		}
    		glEnd();
}
class Ball {

	public:
		double radius;
		GLfloat* color;
		double x;
		double y;
		double z;
		Vector velocity;
		double accelaration;
		
		Ball(double r, GLfloat* c, double x, double y, double z, Vector velocity, double accelaration):
      	radius(r), color(c), y(y), x(x), z(z) , velocity(velocity), accelaration(accelaration){

  		}
  
  		void update() {
			//velocity += accelaration;	
			if(x<1 || x > 22 ){
				velocity.x = -velocity.x;
			}
			if(z<1 || z > 46){
				velocity.z = -velocity.z;
			}
			
			x += velocity.x;
			y += velocity.y;
			z += velocity.z;
			velocity.reduce_magnitude();
			//glBindTexture(GL_TEXTURE_2D, 0);
			//glBindTexture( GL_TEXTURE_2D, texture );
	    	glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glutSolidSphere(radius, 30, 30);
			glPopMatrix();
			//glBindTexture( GL_TEXTURE_2D, 0 );
		}

		void detect_collision(Ball &two){
			j++;
			
			if( Vector(x,y,z).distance_btwn_vectors( Vector(two.x, two.y , two.z)) < 2){
				printf("%lf %lf  \n", z, two.z);
				printf("%lf %lf  \n", x, two.x);
				//printf("hello %d ", j);
				double v1 = velocity.magnitude();
				double v2 = two.velocity.magnitude();
				//v1
				Vector temp2(0,0,0),temp4(0,0,0);
				Vector direction = Vector((two.x - x), (two.y - y), (two.z -z)).normalize();
				double cos_angle = direction.angle_btwn_vectors(velocity.normalize());
				double sin_angle = sqrt(1 - (cos_angle*cos_angle));
				Vector temp1 = direction.multiplyconstant(v1*cos_angle); //two.velocity.
				
				if((direction.crossProduct(velocity.normalize())).y < 0)
						temp2 = (direction.perpendicular()).multiplyconstant(v1*sin_angle);		//velocity
					else
						temp2 = (direction.perpendicular()).multiplyconstant(-1*v1*sin_angle);
				
				direction = direction.multiplyconstant(-1);
				cos_angle = direction.angle_btwn_vectors(two.velocity.normalize());
				sin_angle = sqrt(1 - (cos_angle*cos_angle));
				Vector temp3 = direction.multiplyconstant(v2*cos_angle); //velocity.
				if((direction.crossProduct(two.velocity.normalize())).y < 0)
						temp4 = (direction.perpendicular()).multiplyconstant(v2*sin_angle);		//two.velocity
					else
						temp4 = (direction.perpendicular()).multiplyconstant(-1*v2*sin_angle);
				
				velocity = temp2.add(temp3);
				two.velocity = temp1.add(temp4);
				/*if(v1 > v2){
					Vector direction = Vector((two.x - x), (two.y - y), (two.z -z)).normalize();
					double cos_angle = direction.angle_btwn_vectors(velocity.normalize());
					double sin_angle = sqrt(1 - (cos_angle*cos_angle));
					two.velocity = two.velocity.add(direction.multiplyconstant(v1*cos_angle));
					if((direction.crossProduct(velocity.normalize())).y < 0)
						velocity = (direction.perpendicular()).multiplyconstant(v1*sin_angle);
					else
						velocity = (direction.perpendicular()).multiplyconstant(-1*v1*sin_angle);
				}else {
					Vector direction = Vector(-(two.x - x), -(two.y - y), -(two.z -z)).normalize();
					double cos_angle = direction.angle_btwn_vectors(two.velocity.normalize());
					double sin_angle = sqrt(1 - (cos_angle*cos_angle));
					velocity = velocity.add(direction.multiplyconstant(v2*cos_angle));
					if((direction.crossProduct(two.velocity.normalize())).y < 0)
						two.velocity = (direction.perpendicular()).multiplyconstant(v2*sin_angle);
					else
						two.velocity = (direction.perpendicular()).multiplyconstant(-1*v2*sin_angle);
				}*/
			}
		}
};



	// Global variables: a camera, a checkerboard and some balls.
	Checkerboard checkerboard(24 , 48);
	//Camera camera;
	Ball balls[] = {
	  	Ball(1, WHITE,11,1, 45, Vector(0, 0, 0), 0),
	  	Ball(1, RED, 10, 1, 22, Vector(0, 0, 0),0),
	  	Ball(1, RED, 12, 1, 22, Vector(0, 0, 0),0),
		Ball(1, RED, 14, 1, 22, Vector(0, 0, 0),0),
		Ball(1, YELLOW, 11, 1, 24, Vector(0, 0, 0),0),
	  	Ball(1, YELLOW, 13, 1, 24, Vector(0, 0, 0),0),
		Ball(1, BLUE, 12, 1, 26, Vector(0, 0, 0),0)
	};


void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);  //rotate our camera on teh x-axis (left and right)
    glRotatef(yrot,0.0,1.0,0.0);  //rotate our camera on the y-axis (up and down)
    glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
}

// Application-specific initialization: Set up global lighting parameters
// and create display lists.
void init() {
	  glEnable(GL_DEPTH_TEST);
	  glEnable( GL_TEXTURE_2D );
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
  	//sizeof balls / sizeof(Ball)
  	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);
  	checkerboard.draw();
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
	
	draw_border();  	
  	for(int i=0; i<6;i++){
  		for(int j=i+1; j<7; j++){
  			balls[i].detect_collision(balls[j]);
  		}
  	}
  	for (int i = 0; i < 7; i++) {
    	balls[i].update();
  	}
	
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
    if(key == 'k'){
    	printf("%lf \n", balls[0].velocity.z);
		balls[0].velocity = Vector(0,0,-0.5);//balls[0].velocity.multiply_matrix(30);
    	
    }
    
	if (key==27){
	    exit(0);
    }
}

void special(int key, int, int) {
	switch (key) {
    	case GLUT_KEY_LEFT: 
			balls[0].velocity = balls[0].velocity.multiply_matrix(30);
    		break;
    	case GLUT_KEY_RIGHT: 
			balls[0].velocity = balls[0].velocity.multiply_matrix(-30);
    		break;
    	case GLUT_KEY_UP:  break;
    	case GLUT_KEY_DOWN: break;
    
  	}
  	glutPostRedisplay();
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
	glutSpecialFunc(special);
	glutKeyboardFunc (keyboard);
	glutMotionFunc( mouse_motion);	
	glutMouseFunc(mouse);
	glutTimerFunc(100, timer, 0);
	init();
	texture = LoadTexture( "C:/Users/Husain/Documents/GL/Object-Collision-in-3D/Object_Collision_3D/texture2.bmp", 256, 256 ); //load our texture
	glutMainLoop();
	FreeTexture(texture);
	//return 0;
}
