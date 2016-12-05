/*
 *  Projections
 *
 *  Draw 27 cubes to demonstrate orthogonal & prespective projections
 *
 *  Key bindings:
 *  m          Toggle between perspective and orthogonal
 *  +/-        Changes field of view for perspective
 *  a          Toggle axes
 *  arrows     Change view angle
 *  PgDn/PgUp  Zoom in and out
 *  0          Reset view angle
 *  ESC        Exit
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include "CSCIx229.h"
//  OpenGL with prototypes for glext
#define GL_GLEXT_PROTOTYPES
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#define PI 3.1416



int axes=0;       //  Display axes
int mode=0;
int move = 1;//  Projection mode
int th=0;         //  Azimuth of view angle
int ph=0;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=5;   //  Size of world
double rep=1;  //  Repetition
double X=1;    //  Top Right X
double Y=1;    //  Top Right Y
unsigned int texture[10];  //  Texture names

double targh;
int loop = 0;

//first person
int rot = -90.0;
int fp = 1;
//  Camera coords
double camX = 0;
double camZ = 0;
// first person coords
double fpX = 6.5;
double fpY = -.18;
double fpZ = 0;


//Throw values
int throwing  = 0;
double throwloop = 0;
double throwX = 6.25;
double throwY = -.18;
double throwZ = 0;
double diffTime=0;


// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  10;  // Ball increment
int smooth    =   1;  // Smooth/Flat shading
int local     =   0;  // Local Viewer Model
int emission  =   0;  // Emission intensity (%)
int ambient   =  30;  // Ambient intensity (%)
int diffuse   = 100;  // Diffuse intensity (%)
int specular  =   0;  // Specular intensity (%)
int shininess =   0;  // Shininess (power of two)
float shinyvec[1];    // Shininess (value)
int zh        =  90;  // Light azimuth
float ylight  =   0;  // Elevation of light

// Get mouse position
int xMpos, yMpos;
//glfwGetMousePos(&xMpos, &yMpos);

//5SetMousePos(600/2, 600/2);

/*
 *  Convenience routine to output raster text
 *  Use VARARGS to make this more flexible
 */
#define LEN 8192  //  Maximum length of text string
void Print(const char* format , ...)
{
    char    buf[LEN];
    char*   ch=buf;
    va_list args;
    //  Turn the parameters into a character string
    va_start(args,format);
    vsnprintf(buf,LEN,format,args);
    va_end(args);
    //  Display the characters one at a time at the current raster position
    while (*ch)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*ch++);
}

/*
 *  Draw vertex in polar coordinates with normal
 */
static void Vertex(double th,double ph)
{
    double x = Sin(th)*Cos(ph);
    double y = Cos(th)*Cos(ph);
    double z =         Sin(ph);
    //  For a sphere at the origin, the position
    //  and normal vectors are the same
    glNormal3d(x,y,z);
    glVertex3d(x,y,z);
}

/*
 *  Draw a ball
 *     at (x,y,z)
 *     radius (r)
 */
void ball(double x,double y,double z,double r)
{
    int th,ph;
    float yellow[] = {1.0,1.0,0.0,1.0};
    float Emission[]  = {0.0,0.0,0.01*emission,1.0};
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glScaled(r,r,r);
    //  White ball
    glColor3f(1,1,1);
    glMaterialfv(GL_FRONT,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT,GL_SPECULAR,yellow);
    glMaterialfv(GL_FRONT,GL_EMISSION,Emission);
    //  Bands of latitude
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    //  Undo transformations
    glPopMatrix();
}



/*
 *  Set projection
 */
static void myProject()
{
    //  Tell OpenGL we want to manipulate the projection matrix
    glMatrixMode(GL_PROJECTION);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective transformation
    if(fp) {
        gluPerspective(fov,asp,dim/100,4*dim);
    }
    else {
        if (mode)
            gluPerspective(fov,asp,dim/4,4*dim);
        //  Orthogonal projection
        else
            glOrtho(-asp*dim,+asp*dim, -dim,+dim, -dim,+dim);
    }
    //  Switch to manipulating the model matrix
    glMatrixMode(GL_MODELVIEW);
    //  Undo previous transformations
    glLoadIdentity();
}

/*
 *  Draw the field
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void field()
{
    //  Select texture if textures are on

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , mode?GL_REPLACE:GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,texture[0]);

    //  Set specular color to white
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    //  Draw object in selected color and with the above texture applied
    glColor3f(1,1, 1);


    //grass
    //front
    glPushMatrix();

    glTranslated(0,-.7,0);
    glRotated(0,0,1,0);
    glScaled(8,.02,4);

    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();

    glDisable(GL_TEXTURE_2D);

    glColor3f(1,1,1);
    // front
    glBegin(GL_QUADS);
    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,1); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,1); glVertex3f(-1,+1, 1);
    glEnd();

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,1); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(1,1); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,1); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,1); glVertex3f(-1,-1,+1);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();


}

void seats(){

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , mode?GL_REPLACE:GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,texture[1]);

    glPushMatrix();

    glTranslated(10,1, 0);
    glRotated(-30,0,0,1);
    //ssglRotated(30,0, 0, 1);
    glScaled(.02,2,4);

    int repShort = 3;
    int repLong = 4;

    /**********************  short side ************************/
    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(repShort,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1,-1);



    glColor3f(1,1,1);
    // front

    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1, 1);
    glEnd();

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(repShort,repShort); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,repShort); glVertex3f(-1,-1,+1);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();



    /**********************  short side ************************/

    glPushMatrix();

    glTranslated(-10,1, 0);
    glRotated(30,0,0,1);
    //ssglRotated(30,0, 0, 1);
    glScaled(.02,2,4);

    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(repShort,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1,-1);



    glColor3f(1,1,1);
    // front

    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1, 1);
    glEnd();

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repShort); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(repShort,repShort); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,repShort); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repShort,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repShort,repShort); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,repShort); glVertex3f(-1,-1,+1);
    glEnd();

    glBegin(GL_QUADS);
    glNormal3f (0,1,0);
    glTexCoord2f(0,0); glVertex3f(1, 0, 1);
    glTexCoord2f(0,1); glVertex3f(0,0,1);
    glTexCoord2f(1,0); glVertex3f(1, 0, 0);
    glTexCoord2f(1, 1); glVertex3f(0, 0,0);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();


    /**********************  long side ************************/
    //long side
    glPushMatrix();

    glTranslated(0,1, 5.5);
    glRotated(30,1,0,0);
    //ssglRotated(30,0, 0, 1);
    glScaled(8,2,.01);

    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);



    glColor3f(1,1,1);
    // front

    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1, 1);
    glEnd();

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,-1,+1);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();



    /**********************  long side ************************/
    glPushMatrix();

    glTranslated(0,1, -5.5);
    glRotated(-30,1,0,0);
    //ssglRotated(30,0, 0, 1);
    glScaled(8,2,.01);

    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);



    glColor3f(1,1,1);
    // front

    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1, 1);
    glEnd();

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,-1,+1);
    glEnd();
    //  Undo transformations and textures
    glPopMatrix();

}

void target(double x, double y, double z){
    /**********************  long side ************************/

    glEnable(GL_TEXTURE_2D);
    glTexEnvi(GL_TEXTURE_ENV , GL_TEXTURE_ENV_MODE , mode?GL_REPLACE:GL_MODULATE);
    glBindTexture(GL_TEXTURE_2D,texture[2]);

    glPushMatrix();
    glTranslated(x,y-.3, z);
    //glRotated(30,0,0, 0);
    //ssglRotated(30,0, 0, 1);
    glScaled(.03,.1,.05);

    int repLong = 4;
    //  Top
    glBegin(GL_QUADS);
    glNormal3f( 0,+1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,+1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,+1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);


    glColor3f(1,1,1);
    // front

    glNormal3f( 0, 0, 1);
    glTexCoord2f(0,0); glVertex3f(-1,-1, 1);
    glTexCoord2f(1,0); glVertex3f(+1,-1, 1);
    glTexCoord2f(1,repLong); glVertex3f(+1,+1, 1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1, 1);
    glEnd();

    //target

    //  Back
    glNormal3f( 0, 0,-1);
    glTexCoord2f(0,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(1,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(1,repLong); glVertex3f(-1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,-1);
    glEnd();

    //  Right
    glBegin(GL_QUADS);
    glNormal3f(+1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(+1,-1,+1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,+1,-1);
    glTexCoord2f(0,repLong); glVertex3f(+1,+1,+1);
    glEnd();
    // left
    glBegin(GL_QUADS);
    glNormal3f(-1, 0, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(-1,-1,+1);
    glTexCoord2f(repLong,repLong); glVertex3f(-1,+1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,+1,-1);
    glEnd();
    //  Bottom
    glBegin(GL_QUADS);
    glNormal3f( 0,-1, 0);
    glTexCoord2f(0,0); glVertex3f(-1,-1,-1);
    glTexCoord2f(repLong,0); glVertex3f(+1,-1,-1);
    glTexCoord2f(repLong,repLong); glVertex3f(+1,-1,+1);
    glTexCoord2f(0,repLong); glVertex3f(-1,-1,+1);
    glEnd();

    glDisable(GL_TEXTURE_2D);
    glPopMatrix();


    /* ******* Top Cylinder ******* */


    int i,k;
    glEnable(GL_TEXTURE_2D);
    //  Save transformation
    glPushMatrix();
    //  Offset and scale
    glTranslated(x,-.14,z);
    glRotated(90,0,1,0);
    glScaled(.07,.07,.007);
    //  Head & Tail
    glColor3f(1,1,1);
    for (i=1;i>=-1;i-=2)
    {
        glBindTexture(GL_TEXTURE_2D, texture[3]);
        glNormal3f(0,0,i);
        glBegin(GL_TRIANGLE_FAN);
        glTexCoord2f(0.5,0.5);
        glVertex3f(0,0,i);
        for (k=0;k<=360;k+=10)
        {
            glTexCoord2f(0.5*Cos(k)+0.5,0.5*Sin(k)+0.5);
            glVertex3f(i*Cos(k),Sin(k),i);
        }
        glEnd();
    }
    //  Edge
    glDisable(GL_TEXTURE_2D);
    glColor3f(1, 1, 1);
    glBegin(GL_QUAD_STRIP);
    for (k=0;k<=360;k+=10)
    {
        glNormal3f(Cos(k),Sin(k),0);
        glVertex3f(Cos(k),Sin(k),+1);
        glVertex3f(Cos(k),Sin(k),-1);
    }
    glEnd();
    //  Undo transformations
    glPopMatrix();

}

void throw(double x, double y, double z){
    int th,ph;
    //  Save transformation
    glPushMatrix();
    //  Offset, scale and rotate
    glTranslated(x,y,z);
    glRotatef(90, 0,1,0);
    glScaled(.01,.01,.025);

    glColor3f(.4, .2, 0);
    for (ph=-90;ph<90;ph+=inc)
    {
        glBegin(GL_QUAD_STRIP);
        for (th=0;th<=360;th+=2*inc)
        {
            Vertex(th,ph);
            Vertex(th,ph+inc);
        }
        glEnd();
    }
    glPopMatrix();

}

/*
 *  Adapted from Example 7 given on Moodle
 *  Draw a cylinder
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the z axis
 *     given top & bottom color
 *     given side color
 */
void drawCylinder(double x,double y,double z,
                  double dx,double dy,double dz,
                  double th, char lcol, char scol) {
    double radius = 0.3, height = 0.2;

    //  Set specular color to white
    float white[] = {1,1,1,1};
    float black[] = {0,0,0,1};
    glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
    glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
    glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,black);

    glPushMatrix();

    //  Transform cylinder
    glTranslated(x,y,z);
    glRotated(90,0,0,1);
    glScaled(dx,.2,dz);


    int j;
    glBegin(GL_TRIANGLE_STRIP);
    for (j = 0; j <= 360; j++) {
        double x = radius * Cos(j);
        double y = height;
        double z = radius * Sin(j);

        //glColor3f(j/306, j/360, j/360);

        glNormal3d(Cos(j), 1, Sin(j));

        glVertex3d(x, y, z);
        glVertex3d(x, -y, z);
    }
    glEnd();

    double i;

    glNormal3d(0,1,0);

    /* Top of Cylinder */
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, height, 0.0);

    for(i = 0.0; i < 360; i+=.125) {
        glVertex3d(radius * cos(i), height, radius * sin(i));
    }
    glEnd();

    glNormal3d(0,-1,0);

    /* Bottom of Cylinder */
    glBegin(GL_TRIANGLE_FAN);
    glVertex3d(0.0, -height, 0.0);

    for(i = 0.0; i < 360; i+=.125) {
        glVertex3d(radius * cos(i), -height, radius * sin(i));
    }
    glEnd();

    glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{

    const double len=1.5;  //  Length of axes
    //  Erase the window and the depth buffer
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    //  Enable Z-buffering in OpenGL
    glEnable(GL_DEPTH_TEST);
    //  Undo previous transformations
    glLoadIdentity();
    //  Perspective - set eye position
    if(fp){
        camX = +2*dim*Sin(rot);
        camZ = -2*dim*Cos(rot);
        gluLookAt(fpX,fpY,fpZ, camX+fpX,fpY,camZ+fpZ, 0,1,0);
    }
    else
    {
        //  Perspective - set eye position
        if (mode)
        {
            double px = -2*dim*Sin(th)*Cos(ph);
            double py = +2*dim        *Sin(ph);
            double pz = +2*dim*Cos(th)*Cos(ph);
            gluLookAt(px,py,pz , 0,0,0 , 0,Cos(ph),0);
            dim=3;
        }
        //  Orthogonal - set world orientation
        else
        {
            glRotatef(ph,1,0,0);
            glRotatef(th,0,1,0);
        }

    }

    //  smooth shading
    glShadeModel(GL_SMOOTH);

    //  Translate intensity to color vectors
    float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
    float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
    float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
    //  Light position
    float Position[]  = {distance*Cos(zh),3,distance*Sin(zh),1.0};
    //  Draw light position as ball (still no lighting here)
    glColor3f(1,1,1);
    ball(Position[0],Position[1],Position[2] , 0.1);
    //ball(0, 3, 3, .1);
    //  OpenGL should normalize normal vectors
    glEnable(GL_NORMALIZE);
    //  Enable lighting
    glEnable(GL_LIGHTING);
    //  Location of viewer for specular calculations
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER,local);
    //  glColor sets ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    //  Enable light 0
    glEnable(GL_LIGHT0);
    //  Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0,GL_AMBIENT ,Ambient);
    glLightfv(GL_LIGHT0,GL_DIFFUSE ,Diffuse);
    glLightfv(GL_LIGHT0,GL_SPECULAR,Specular);
    glLightfv(GL_LIGHT0,GL_POSITION,Position);

    /* ********* Field and Throw *********** */
    field();
    seats();
    target(targh+5, 0, 0);
    target(.5*targh+5, 0, -1.5* targh +-.5);
    if(loop >= 0){
        if(targh > -1){
            target(targh+5, 0, .5);
        }
        else{
            target(targh+5, 0, targh+.5+1 );
        }
    }



    throw(throwX+throwloop, throwY, 0);
    //drawCylinder(0, 0, 0, 1, 1, 1, 0, 'b', 'b');
    //  Draw axes
    glDisable(GL_LIGHTING);

    glColor3f(1,1,1);
    if (1)
    {
        glBegin(GL_LINES);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(len,0.0,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,len,0.0);
        glVertex3d(0.0,0.0,0.0);
        glVertex3d(0.0,0.0,len);
        glEnd();
        //  Label axes
        glRasterPos3d(len,0.0,0.0);
        Print("X");
        glRasterPos3d(0.0,len,0.0);
        Print("Y");
        glRasterPos3d(0.0,0.0,len);
        Print("Z");
    }
    //  Display parameters
    glWindowPos2i(5,5);
    Print("Angle=%d,%d  Dim=%.1f FOV=%d  thowing = %d throwloop = %f",
          th,ph,dim,fov, throwing, throwloop );
        //  Render the scene and make it visible
    glFlush();
    glutSwapBuffers();
}

void idle()
{
    //  Elapsed time in seconds
    double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
    //  Tell GLUT it is necessary to redisplay the scene
    zh = fmod(90*t,360.0);

    if(targh <= -3) { loop = 1;}
    targh = fmod(t, 7)*-1/2;
    if(throwing == 0){
        diffTime = t;
        throwloop = 0;
    }
    else {
        throwloop =fmod(t-diffTime, 10)*-1;
    }
    if(throwloop <= -2){ throwing =0; throwloop=0;};
    glutPostRedisplay();

}

/*
 *  GLUT calls this routine when an arrow key is pressed
 */
void special(int key,int x,int y)
{
    //  Right arrow key - increase angle by 5 degrees
    if (key == GLUT_KEY_RIGHT)
        th += 5;
    //  Left arrow key - decrease angle by 5 degrees
    else if (key == GLUT_KEY_LEFT)
        th -= 5;
    //  Up arrow key - increase elevation by 5 degrees
    else if (key == GLUT_KEY_UP)
        ph += 5;
    //  Down arrow key - decrease elevation by 5 degrees
    else if (key == GLUT_KEY_DOWN)
        ph -= 5;
    //  PageUp key - increase dim
    else if (key == GLUT_KEY_PAGE_UP)
        dim += 0.1;
    //  PageDown key - decrease dim
    else if (key == GLUT_KEY_PAGE_DOWN && dim>1)
        dim -= 0.1;
    //  Keep angles to +/-360 degrees
    th %= 360;
    ph %= 360;
    //  Update projection
    myProject();
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();
}

/*
 *  GLUT calls this routine when a key is pressed
 */
void key(unsigned char ch,int x,int y)
{
    //  Exit on ESC
    if (ch == 27)
        exit(0);
    //  Reset view angle
    else if (ch == '0')
    {
        fpX = 6.5;
        fpY = -.18;
        fpZ =  0;
        rot = -90;
        th = ph = 0;
    }
    //  Toggle axes
    //  Switch display mode
    else if (ch == 'm' || ch == 'M')
        mode = 1-mode;
    //  Change field of view angle
    else if (ch == '-' && ch>1)
        fov--;
    else if (ch == '+' && ch<179)
        fov++;

    //  Toggle first person
    else if (ch == 'f')
    {
        fp = 1-fp;
    }
    //  first person controls
    else if (fp) {
        double dt = 0.05;
        if (ch == 'w' || ch == 'W'){
            fpX += camX*dt;
            fpZ += camZ*dt;
            throwX -= .5;
        }
        else if (ch == 'a' || ch == 'A'){
            rot -= 3;

        }
        else if (ch == 's' || ch == 'S'){
            fpX -= camX*dt;
            fpZ -= camZ*dt;
            throwX += .5;
        }
        else if (ch == 'd' || ch == 'D'){
            rot += 3;
        }

        //  Keep angles to +/-360 degrees
        rot %= 360;
    }
    //  Change field of view angle
    else if (ch == '-' && ch>1)
        fov--;
    else if (ch == '+' && ch<179)
        fov++;

    if(ch == 't')
    {
        throwing = fmod(throwing+1,2);
        throwloop= 0;
    }


    //  Reproject
    myProject();
    //  Animate if requested
    glutIdleFunc(move?idle:NULL);
    //  Tell GLUT it is necessary to redisplay the scene
    glutPostRedisplay();

}

/*
 *  GLUT calls this routine when the window is resized
 */
void reshape(int width,int height)
{
    //  Ratio of the width to the height of the window
    asp = (height>0) ? (double)width/height : 1;
    //  Set the viewport to the entire window
    glViewport(0,0, width,height);
    //  Set projection
    myProject();
}

/*
 *  Start up GLUT and tell it what to do
 */
int main(int argc,char* argv[])
{
    //  Initialize GLUT
    glutInit(&argc,argv);
    //  Request double buffered, true color window with Z buffering at 600x600
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(600,600);
    glutCreateWindow("Projections");
    //  Set callbacks
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutSpecialFunc(special);
    glutKeyboardFunc(key);
    glutIdleFunc(idle);

    texture[0] = LoadTexBMP("field.bmp");
    texture[1] = LoadTexBMP("crowd.bmp");
    texture[2] = LoadTexBMP("leather.bmp");
    texture[3] = LoadTexBMP("target.bmp");
    texture[4] = LoadTexBMP("field.bmp");
    //  Pass control to GLUT so it can interact with the user
    glutMainLoop();
    return 0;
}
