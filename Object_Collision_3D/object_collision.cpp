#include <GL/glut.h>
#include <GL/gl.h>
#include <cmath>
#include <stdlib.h>
#include<math.h>
#include<stdio.h>
#include<string>
using namespace std;
// Colors
GLfloat WHITE[] = {1, 1, 1};
GLfloat RED[] = {1, 0, 0};
GLfloat YELLOW[] = {1, 1, 0};
GLfloat BLUE[] = {0, 0, 1};
GLfloat GREEN[] = {0, 1, 0};
GLfloat BROWN[] = {0.64, 0.16, 0.16};
int wi, hi;
int score=0;
float power_var = 1;

int final_pos = 10;
int NB_TEXTURES = 2;
static GLuint texName[2];

int x_old, y_old, j;
float xpos = 10, ypos = 45, zpos = 70, xrot = 45, yrot = 0,yrotn=0, angle=0.0;

GLuint texture[2];

struct Image {
    unsigned long sizeX;
    unsigned long sizeY;
    char *data;
};

typedef struct Image Image;

int ImageLoad(char *filename, Image *image) {
    FILE *file;
    unsigned long size; // size of the image in bytes.
    unsigned long i; // standard counter.
    unsigned short int plane; // number of planes in image

    unsigned short int bpp; // number of bits per pixel
    char temp; // temporary color storage for
    if ((file = fopen(filename, "rb"))==NULL)
    {
        printf("File Not Found : %s\n",filename);
        return 0;
    }
    // seek through the bmp header, up to the width/height:
    fseek(file, 18, SEEK_CUR);
    // read the width
        if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
        printf("Error reading width from %s.\n", filename);
        return 0;
    }
    if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
        printf("Error reading height from %s.\n", filename);
        return 0;
    }
    printf("%d ", image->sizeX);
    size = image->sizeX * image->sizeY * 3;
    // read the planes
    if ((fread(&plane, 2, 1, file)) != 1) {
        printf("Error reading planes from %s.\n", filename);
        return 0;
    }
    if (plane != 1) {
            printf("Planes from %s is not 1: %u\n", filename, plane);
        return 0;
    }
    // read the bitsperpixel
    if ((i = fread(&bpp, 2, 1, file)) != 1) {
        printf("Error reading bpp from %s.\n", filename);

    return 0;
    }
    if (bpp != 24) {
        printf("Bpp from %s is not 24: %u\n", filename, bpp);
        return 0;
    }
    // seek past the rest of the bitmap header.
    fseek(file, 24, SEEK_CUR);
    // read the data.
    image->data = (char *) malloc(size);
    if (image->data == NULL) {
        printf("Error allocating memory for color-corrected image data");
        return 0;
    }
    if ((i = fread(image->data, size, 1, file)) != 1) {
        printf("Error reading image data from %s.\n", filename);
        return 0;
    }
    for (i=0;i<size;i+=3) { // reverse all of the colors. (bgr -> rgb)
        temp = image->data[i];
        image->data[i] = image->data[i+2];
        image->data[i+2] = temp;
    }
    // we're done.
    return 1;
}

Image * loadTexture(char* file){
    Image *image1;
    image1 = (Image *) malloc(sizeof(Image));
    if (image1 == NULL) {
        printf("Error allocating space for image");
        exit(0);
    }
    //pic.bmp is a 64x64 picture
    if (!ImageLoad(file, image1)) {
        exit(1);
    }
    return image1;
}

void init_textures()
{
    Image* images[NB_TEXTURES];

    images[0] = loadTexture("textures/pool_texture.bmp");
    if(images[0] == NULL)
    {
        printf("pool_texture.bmp was not returned from loadTexture\n");
        exit(0);
    }

    images[1] = loadTexture("textures/wood_texture.bmp");
    if(images[1] == NULL)
    {
        printf("wood_texture.bmp was not returned from loadTexture\n");
        exit(0);
    }

    glGenTextures(NB_TEXTURES, texName);

    int i;
    for (i=0; i<NB_TEXTURES; ++i)
    {
        glBindTexture(GL_TEXTURE_2D, texName[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, images[i]->sizeX,images[i]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE,
                    images[i]->data);
    }
}


void FreeTexture( GLuint texture ){
	glDeleteTextures( 1, &texture ); 
}

void drawBitmapText(char *string,float x,float y,float z)
{
    char *c;
    glRasterPos3f(x, y,z);
    for (c=string; *c != '\0'; c++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, *c);
    }
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
			return Vector(x*0.99, y*0.99, z*0.99);
		}
		
		Vector multiplyconstant (double scalar) {
		//	printf("%lf %lf %lf \n", x, y, z);
			return Vector (x*scalar, y*scalar, z*scalar);
		}
		
		Vector multiply_matrix (double angle) {
			double m[3][3];
			angle = (angle * 0.01745329251);                                //degree to radian conversion
	     //	printf("\nAngle = %f, cos(%f) = %f\n",angle,angle,cosf(angle));
	    	m[0][0] = cosf(angle);
	     	m[0][1] = sinf(angle);
	     	m[0][2] = 0; 
	     	m[1][0] = -sinf(angle);
	     	m[1][1] = cosf(angle);
	     	m[1][2] = 0; 
	     //	printf("%lf\n", z);
			 return Vector(x*m[0][0] + z * m[0][1] ,y, x*m[1][0] + z * m[1][1] );
		}
};

class Table {
	int displayListId;
	int width;
	int depth;

	public:
  		Table(int width, int depth): width(width), depth(depth) {}
  		double centerx() {return width / 2;}
  		double centerz() {return depth / 2;}
  		void create() {
			displayListId = glGenLists(1);		    
			glNewList(displayListId, GL_COMPILE);
			//GLfloat lightPosition[] = {4, 3, 7, 1};
		    //glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
		    glBegin(GL_QUADS);
			for(int x = 3; x < width-4; x++) {
		    	
				glTexCoord2d(0.0,0.0);glVertex3d(x, 0, 0);
        		glTexCoord2d(1.0,0.0);glVertex3d(x+1, 0, 0);
        		glTexCoord2d(1.0,1.0);glVertex3d(x+1, 0, 4);
        		glTexCoord2d(0.0,1.0);glVertex3d(x, 0, 4);
		    }
		    for(int x = 3; x < width-4; x++) {
		    	glTexCoord2d(0.0,0.0);glVertex3d(x, 0, 45);
        		glTexCoord2d(1.0,0.0);glVertex3d(x+1, 0, 45);
        		glTexCoord2d(1.0,1.0);glVertex3d(x+1, 0, 48);
        		glTexCoord2d(0.0,1.0);glVertex3d(x, 0, 48);
		    }

		    for (int x = 0; x < width - 1; x++) {
		    	for (int z = 3; z < depth - 3; z++) {
        			//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
                    //(x + z) % 2 == 0 ? GREEN : DARK_GREEN );	
					glTexCoord2d(0.0,0.0);glVertex3d(x, 0, z);
        			glTexCoord2d(1.0,0.0);glVertex3d(x+1, 0, z);
        			glTexCoord2d(1.0,1.0);glVertex3d(x+1, 0, z+1);
        			glTexCoord2d(0.0,1.0);glVertex3d(x, 0, z+1);
        			
      			}
    		}
    		glEnd();
			glEndList();
  		}
	 	void draw() {
    		glCallList(displayListId);
  		}		
};
void room(){
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
	glBegin(GL_QUADS);
	/*glVertex3d(-10,-10,-10);
	glVertex3d(40,-10,-10);
	
	glVertex3d(-10,-10,-10);
	glVertex3d(-10,-10,60);
	
	glVertex3d(-10,-10,-10);
	glVertex3d(-10,70,-10);
	
	glVertex3d(-10,-10,-10);
	glVertex3d(60,-10,-10);*/
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE,
     //               DARK_GREEN );
    glVertex3d(-10,-10,-10);
	glVertex3d(60,-10,-10);
	glVertex3d(60,70,-10);
	glVertex3d(-10,70,-10);
	 
	glVertex3d(-10,-10,-10);
	glVertex3d(-10,70,-10);
	glVertex3d(-10,70,60);
	glVertex3d(-10,-10,60);               
	glEnd();
}
/*void draw_net(){
	glPushMatrix();
	glBegin(GL_LINE_LOOP) ;             
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
	for(int i=0;i<3;i++){
		for(int j=0;j<3;j++){
			glVertex3d(0,0,0);
			glVertex3d(,0,0);
			glVertex3d(1,0,1);
			glVertex3d(0,0,1);
		}
	}
	glEnd();
	glPopMatrix();
}*/
void draw_border(){
	int x, z;
	glBegin(GL_QUADS);
	glNormal3d(0, 1, 0);
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, BROWN);
	glTexCoord2d(0.0,0.0);glVertex3d(0, 0, 0);
	glTexCoord2d(1.0,0.0);glVertex3d(0, 1, 0);
	glTexCoord2d(1.0,1.0);glVertex3d(0, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(0, 0, 48);
	
	glTexCoord2d(0.0,0.0);glVertex3d(23, 0, 0);
	glTexCoord2d(1.0,0.0);glVertex3d(23, 1, 0);
	glTexCoord2d(1.0,1.0);glVertex3d(23, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(23, 0, 48);

	glTexCoord2d(0.0,0.0);glVertex3d(0, 0, 0);
	glTexCoord2d(1.0,0.0);glVertex3d(0, 1, 0);
	glTexCoord2d(1.0,1.0);glVertex3d(23, 1, 0);
	glTexCoord2d(0.0,1.0);glVertex3d(23, 0, 0);
        			
	glTexCoord2d(0.0,0.0);glVertex3d(0, 0, 47);
	glTexCoord2d(1.0,0.0);glVertex3d(0, 1, 47);
	glTexCoord2d(1.0,1.0);glVertex3d(23, 1, 47);
	glTexCoord2d(0.0,1.0);glVertex3d(23, 0, 47);

	glTexCoord2d(0.0,0.0);glVertex3d(-1, 0, -1);
	glTexCoord2d(1.0,0.0);glVertex3d(-1, 1, -1);
	glTexCoord2d(1.0,1.0);glVertex3d(-1, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(-1, 0, 48);
	
	glTexCoord2d(0.0,0.0);glVertex3d(24, 0, -1);
	glTexCoord2d(1.0,0.0);glVertex3d(24, 1, -1);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(24, 0, 48);

	glTexCoord2d(0.0,0.0);glVertex3d(-1, 0, -1);
	glTexCoord2d(1.0,0.0);glVertex3d(-1, 1, -1);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, -1);
	glTexCoord2d(0.0,1.0);glVertex3d(24, 0, -1);

	glTexCoord2d(0.0,0.0);glVertex3d(-1, 0, 48);
	glTexCoord2d(1.0,0.0);glVertex3d(-1, 1, 48);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(24, 0, 48);

	glTexCoord2d(0.0,0.0);glVertex3d(-1, 1, 47);
	glTexCoord2d(1.0,0.0);glVertex3d(24, 1, 47);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, 48);
	glTexCoord2d(0.0,1.0);glVertex3d(-1, 1, 48);
	
	glTexCoord2d(0.0,0.0);glVertex3d(-1, 1, -1);
	glTexCoord2d(1.0,0.0);glVertex3d(-1, 1, 0);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, 0);
	glTexCoord2d(0.0,1.0);glVertex3d(24, 1, -1);

	glTexCoord2d(0.0,0.0);glVertex3d(-1, 1, 0);
	glTexCoord2d(1.0,0.0);glVertex3d(-1, 1, 47);
	glTexCoord2d(1.0,1.0);glVertex3d(0, 1, 47);
	glTexCoord2d(0.0,1.0);glVertex3d(0, 1, 0);

	glTexCoord2d(0.0,0.0);glVertex3d(23, 1, 0);
	glTexCoord2d(1.0,0.0);glVertex3d(23, 1, 47);
	glTexCoord2d(1.0,1.0);glVertex3d(24, 1, 47);
	glTexCoord2d(0.0,1.0);glVertex3d(24, 1, 0);

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
		int out;
		
		Ball(double r, GLfloat* c, double x, double y, double z, Vector velocity, double accelaration, int o):
      	radius(r), color(c), y(y), x(x), z(z) , velocity(velocity), accelaration(accelaration), out(o){
  		}
  
  		void update() {
			//velocity += accelaration;	
			if(out ==1 && color != WHITE){
				
				if(y < -1){
					score++;
					velocity = Vector(0,0,0);
					x = final_pos;
					y = 1;
					z = -10;
					final_pos = final_pos + 2;	
				}
			}
			else{
				if(((x<3 && z<3) || (x<3 && z>45) || ( x>21 && z>45) || (x>21 && z<3)) && color != WHITE){
				//score++;
				velocity.y = -0.6;
				out = 1;
				}
				if(x<1){
					velocity.x = -velocity.x;
					x = 1;
				}
				if(x>22){
					velocity.x = -velocity.x;
					x = 22;
				}
				if(z<1){
					velocity.z = -velocity.z;
					z = 1;
				}
				if(z>46){
					velocity.z = -velocity.z;
					z = 46;
				}
				
				
					
			}
			x += velocity.x;
				y += velocity.y;
				z += velocity.z;
			velocity = velocity.reduce_magnitude();
	    	glPushMatrix();
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color);
			glTranslated(x, y, z);
			glutSolidSphere(radius, 30, 30);
			glPopMatrix();
		}

		void detect_collision(Ball &two){
			j++;
			
			if( Vector(x,y,z).distance_btwn_vectors( Vector(two.x, two.y , two.z)) < 1.9){
				double v1 = velocity.magnitude();
				double v2 = two.velocity.magnitude();
				//v1
				
				Vector temp2(0,0,0),temp4(0,0,0);
				Vector direction = Vector((two.x - x), (two.y - y), (two.z -z)).normalize();
				double cos_angle = direction.angle_btwn_vectors(velocity.normalize());
				double sin_angle = sqrt(1 - (cos_angle*cos_angle));
				Vector temp1 = direction.multiplyconstant(v1*cos_angle+0.01); //two.velocity.
				
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
			}
		}
};

Ball balls[] = {
  	Ball(1, WHITE,11,1, 45, Vector(0, 0, 0), 0,0),
  	Ball(1, RED, 10, 1, 22, Vector(0, 0, 0),0,0),
  	Ball(1, RED, 12, 1, 22, Vector(0, 0, 0),0, 0),
	Ball(1, RED, 14, 1, 22, Vector(0, 0, 0),0, 0),
	Ball(1, YELLOW, 11, 1, 24, Vector(0, 0, 0),0, 0),
  	Ball(1, YELLOW, 13, 1, 24, Vector(0, 0, 0),0, 0),
	Ball(1, BLUE, 12, 1, 26, Vector(0, 0, 0),0, 0)
};

class Arrow{
	double arrow_x[7] = {10.5, 11.5, 11.5, 12, 11, 10, 10.5};
	double arrow_z[7] = {40, 40, 37, 37,   36,   37,   37};
	
	public:
		Arrow(){}
		
		void rotate_arrow(int direction){
			for(int i=0;i<7;i++){
				arrow_x[i] -= balls[0].x;
				arrow_z[i] -= balls[0].z;
				Vector temp = Vector(arrow_x[i], 0, arrow_z[i]).multiply_matrix((double) direction);
				arrow_x[i] = temp.x + balls[0].x;
				arrow_z[i] = temp.z + balls[0].z;
			}
		}

		void translate_arrow(){
			for(int i=0;i<7;i++){
				
				arrow_x[i] += balls[0].velocity.x;
				arrow_z[i] += balls[0].velocity.z;
			}
		}

		Vector direction_of_arrow(){
			return Vector(arrow_x[2] - arrow_x[1], 0, arrow_z[2] - arrow_z[1]).normalize();
		}

		void draw_arrow(){
			glPushMatrix();
			glBegin(GL_LINE_LOOP) ;             
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, RED);
			for(int i=0;i<7;i++){
				glVertex3d(arrow_x[i],1, arrow_z[i]);
			}
			glEnd();
			glPopMatrix();
			//glFlush(); 
		}
		
};

Table table(24 , 48);
//Camera camera;
Arrow arrow;

void launch_ball(){
	Vector direction = arrow.direction_of_arrow();
	//printf("%lf %lf \n", direction.x, direction.z);	
	balls[0].velocity = Vector(0.4*power_var*direction.x, 0, 0.4*power_var*direction.z);
}

void camera (void) {
    glRotatef(xrot,1.0,0.0,0.0);  
    glRotatef(yrot,0.0,1.0,0.0);  
    glTranslated(-xpos,-ypos,-zpos); //translate the screen to the position of our camera
}

void init() {
	glEnable(GL_DEPTH_TEST);
	glEnable( GL_TEXTURE_2D );
	glLightfv(GL_LIGHT0, GL_DIFFUSE, WHITE);
	glLightfv(GL_LIGHT0, GL_SPECULAR, WHITE);
	glMaterialfv(GL_FRONT, GL_SPECULAR, WHITE);
	glMaterialf(GL_FRONT, GL_SHININESS, 30);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	table.create();
}

void display() {
  	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  	glLoadIdentity();
	
	camera();
  	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	table.draw();
    glDisable(GL_TEXTURE_2D);

  	glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texName[1]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	draw_border();
    glDisable(GL_TEXTURE_2D);
	
	//char str[50], power[50];
	//snprintf(power, sizeof(power), "Power : %f X", power_var);
	//drawBitmapText(power, 25, 8, -4);
    //snprintf(str, sizeof(str), "Score : %d", score);
	  	
  	
  	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	
	glLoadIdentity();
	glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
	gluOrtho2D(0.0, 500, 0.0, 500);
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRasterPos2i(50, 10);
	char power[50], str[50];
	snprintf(power, sizeof(power), "Power : %.2f X", power_var);
	snprintf(str, sizeof(str), "Score : %d", score);
	void * font = GLUT_BITMAP_TIMES_ROMAN_24;//GLUT_BITMAP_9_BY_15;
	for (int i = 0; i<15; ++i)
	{
	  //char c = *i;
	  glColor3d(1.0, 0.0, 0.0);
	  glutBitmapCharacter(font, power[i]);
	}
		glLoadIdentity();
	//glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, WHITE);
	glRasterPos2i(400, 10);
	for (int i = 0; i<10; ++i)
	{
	  //char c = *i;
	  glColor3d(1.0, 0.0, 0.0);
	  glutBitmapCharacter(font, str[i]);
	}
	
	glMatrixMode(GL_PROJECTION); //swapped this with...
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW); //...this
	glPopMatrix();
	
  	if(score == 6){
  		drawBitmapText("Congratulations! You won.", 3, 8, 24);
  	}

	  arrow.draw_arrow();
  	//draw_net();
	//room();
	for(int i=0; i<6;i++){
  		for(int j=i+1; j<7; j++){
  			balls[i].detect_collision(balls[j]);
  		}
  	}
  	for (int i = 0; i < 7; i++) {
    	balls[i].update();
  	}
	arrow.translate_arrow();
	
  	glFlush();
  	glutSwapBuffers();
  	//angle++;
}

// On reshape, constructs a camera that perfectly fits the window.
void reshape(GLint w, GLint h) {
	wi = w;
	hi = h;
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
void special(int key, int, int) {
	switch (key) {
    	case GLUT_KEY_LEFT: 
			arrow.rotate_arrow(5);
			//balls[0].velocity = balls[0].velocity.multiply_matrix(30);
    		break;
    	case GLUT_KEY_RIGHT: 
    		arrow.rotate_arrow(-5);
			//balls[0].velocity = balls[0].velocity.multiply_matrix(-30);
    		break;
    	case GLUT_KEY_UP:
			launch_ball();
			power_var = 1;
    		break;
    	case GLUT_KEY_DOWN: break;
  	}
  	glutPostRedisplay();
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
    //	printf("%lf \n", balls[0].velocity.z);
		//balls[0].velocity = Vector(0,0,-0.5);//balls[0].velocity.multiply_matrix(30);
    	
    }
    if(key == 'p'){
    	power_var = power_var + 0.25;
    }
	if (key==27){
	    exit(0);
    }
}

// Initializes GLUT and enters the main loop.
int main(int argc, char** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Snooker");
	//glutInitWindowPosition(80, 60);
  	//glutInitWindowSize(500, 500);
	glutFullScreen();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutSpecialFunc(special);
	glutKeyboardFunc (keyboard);
	glutMotionFunc( mouse_motion);	
	glutMouseFunc(mouse);
	glutTimerFunc(100, timer, 0);
	init();
	init_textures();
	glutMainLoop();
	//FreeTexture(texture);
	//return 0;
}
