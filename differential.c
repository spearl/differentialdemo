/*
 *  Written by Landon Spear (spearl@colorado.edu)
 *  CSCI 5229 - Computer Graphics
 *
 *  Controlling the differential
 *     w
 *   a s d   b
 * 
 *  'a' to slow down the left wheel
 *  'd' to slow down the right wheel
 *  'w' to accelerate
 *  's' to deccelerate
 *  'b' for brakes (stop everything)
 * 
 *  Lighting
 *  'p' to switch modes (projections)
 *  'x' to toggle axes
 *  '0' snaps angles to 0,0
 *  arrows to rotate the world
 *  PgUp/PgDn zooms in/out
 *  +/- changes field of view of perspective
 *  F1 toggles smooth/flat shading
 *  F2 toggles local viewer mode
 *  F3 toggles light distance (1/5)
 *  F8 change ball increment
 *  F9 invert bottom normal
 *  'l' toggles lighting
 *  q/Q decrease/increase ambient light
 *  z/Z decrease/increase diffuse light
 *  x/X decrease/increase specular light
 *  e/E decrease/increase emitted light
 *  n/N decrease/increase shininess
 *  m  toggles light movement
 *  [] lower/rise light
 */
#include "CSCIx229.h"

int axes=0;       //  Display axes
int mode=1;       //  Projection mode
int move=1;       //  Move light
int th=100;         //  Azimuth of view angle
int ph=10;         //  Elevation of view angle
int fov=55;       //  Field of view (for perspective)
int light=1;      //  Lighting
double asp=1;     //  Aspect ratio
double dim=3.0;   //  Size of world
double last_t=0;  //  Last elapsed time
float speed=0.0;  //  Speed of driveshaft
float gear_speed=0.0; // speed of differential gears
double qh=0;      //  Wheel timing
double gh=0;      //  Gear timing
double dh=0;		//  Driveshaft timing
// Light values
int one       =   1;  // Unit value
int distance  =   5;  // Light distance
int inc       =  40;  // Cylinder increment
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
unsigned int texture[5]; // Texture names
// Gear and Big Wheel values
const double gear_size = 1;
const double gear_length = 0.4;
const double gear_inner = 0.6; // gear_size-gear_length for 45 degrees
const double tooth_len = 0.1;
const double wh_radius1 = 1.6;
const double wh_radius2 = 2.4;
const double wh_radius_hole = 1.2;
const double wh_length = .17;
const double wh_arm_len = 1.5;
const double wh_arm_width = 0.5;
const double wh_arm_rad = 0.3;
const double skinny_gear_radius = 0.4;
const int skinny_gear_num_teeth = 10;


/*
 *  Draw a cube
 *     at (x,y,z)
 *     dimentions (dx,dy,dz)
 *     rotated th about the y axis
 */
static void cube(double x,double y,double z,
                 double dx,double dy,double dz,
                 double th)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   glRotated(th,0,1,0);
   glScaled(dx,dy,dz);
   //  Cube
   glBegin(GL_QUADS);
   //  Front
   glNormal3f( 0, 0, 1);
   glTexCoord2f(0.7,0.7);
   glVertex3f(-1,-1, 1);
   glTexCoord2f(0.8,0.7);
   glVertex3f(+1,-1, 1);
   glTexCoord2f(0.8,0.8);
   glVertex3f(+1,+1, 1);
   glTexCoord2f(0.7,0.8);
   glVertex3f(-1,+1, 1);
   //  Back
   glNormal3f( 0, 0,-1);
   glTexCoord2f(0.3,0.3);
   glVertex3f(+1,-1,-1);
   glTexCoord2f(0.35,0.3);
   glVertex3f(-1,-1,-1);
   glTexCoord2f(0.35,0.35);
   glVertex3f(-1,+1,-1);
   glTexCoord2f(0.3,0.35);
   glVertex3f(+1,+1,-1);
   //  Right
   glNormal3f(+1, 0, 0);
   glTexCoord2f(0.2,0.2);
   glVertex3f(+1,-1,+1);
   glTexCoord2f(0.25,0.2);
   glVertex3f(+1,-1,-1);
   glTexCoord2f(0.25,0.25);
   glVertex3f(+1,+1,-1);
   glTexCoord2f(0.5,0.25);
   glVertex3f(+1,+1,+1);
   //  Left
   glNormal3f(-1, 0, 0);
   glTexCoord2f(0.6,0.6);
   glVertex3f(-1,-1,-1);
   glTexCoord2f(0.65,0.6);
   glVertex3f(-1,-1,+1);
   glTexCoord2f(0.65,0.65);
   glVertex3f(-1,+1,+1);
   glTexCoord2f(0.6,0.65);
   glVertex3f(-1,+1,-1);
   //  Top
   glNormal3f( 0,+1, 0);
   glTexCoord2f(0.1,0.1);
   glVertex3f(-1,+1,+1);
   glTexCoord2f(0.15,0.1);
   glVertex3f(+1,+1,+1);
   glTexCoord2f(0.15,0.15);
   glVertex3f(+1,+1,-1);
   glTexCoord2f(0.1,0.15);
   glVertex3f(-1,+1,-1);
   //  Bottom
   glNormal3f( 0,-one, 0);
   glTexCoord2f(0.9,0.9);
   glVertex3f(-1,-1,-1);
   glTexCoord2f(0.95,0.9);
   glVertex3f(+1,-1,-1);
   glTexCoord2f(0.95,0.95);
   glVertex3f(+1,-1,+1);
   glTexCoord2f(0.9,0.95);
   glVertex3f(-1,-1,+1);
   // End
   glEnd();
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  Draw a gear
 *     radius1 side at (x,y,z)
 *     dimentions (length,radius1,radius2)
 */
static void gear(double x,double y,double z,
                 double length,double radius1,
                 double radius2, int num_teeth)
{
   //  Set specular color to white
   float silver[] = {0.9,0.91,0.98,1};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   int i;
   float gear_angle = Atan((radius2-radius1)/length);
   //double radius2 = radius1 + length;
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,silver);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   
   // top
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,-1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0.0,0.0,0.0);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius1,Sin(t)*radius1,0.0);
   }
   glEnd();
   
   // bottom
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0.0,0.0,length);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius2,Sin(t)*radius2,length);
   }
   glEnd();
   
   // side
   glBegin(GL_TRIANGLE_STRIP);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glNormal3f(Cos(gear_angle)*Cos(t),
	              Cos(gear_angle)*Sin(t),
				     -Sin(gear_angle));
		glTexCoord2f(.1*Cos(t)+0.5,.1*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius1,Sin(t)*radius1,0.0);
	   glTexCoord2f(.15*Cos(t)+0.5,.15*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius2,Sin(t)*radius2,length);
   }
   glEnd();
   
   // teeth
   for(i = 0; i <= num_teeth; ++i) {
	   float const t = 360 * (float)i / (float)num_teeth;
	   // half way point
	   float const t2 = 360 * (float)(2*i+1) / (float)(2*num_teeth);
	   // mid tooth (for normal)
	   float const mid_t = (t+t2)/2;
	   // Front face
	   glBegin(GL_QUADS);
			glNormal3f(-Sin(gear_angle)*Cos(mid_t),
						  -Sin(gear_angle)*Sin(mid_t),
				        -Cos(gear_angle));
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*radius1,Sin(t)*radius1,0);
			glTexCoord2f(0.2,0);
			glVertex3f(Cos(t2)*radius1,Sin(t2)*radius1,0);
			glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t2)*(radius1+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(radius1+(tooth_len*Cos(gear_angle))),
				        -tooth_len*Sin(gear_angle));
			glTexCoord2f(0,0.2);
			glVertex3f(Cos(t)*(radius1+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(radius1+(tooth_len*Cos(gear_angle))),
				        -tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // Back face
	   glBegin(GL_QUADS);
			glNormal3f(Sin(gear_angle)*Cos(mid_t),
						  Sin(gear_angle)*Sin(mid_t),
				        Cos(gear_angle));
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*radius2,Sin(t)*radius2,length);
			glTexCoord2f(0.2,0);
			glVertex3f(Cos(t2)*radius2,Sin(t2)*radius2,length);
			glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t2)*(radius2+(tooth_len*Cos(gear_angle))),
					     Sin(t2)*(radius2+(tooth_len*Cos(gear_angle))),
					     length-tooth_len*Sin(gear_angle));
			glTexCoord2f(0,0.2);
			glVertex3f(Cos(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // top face
	   glBegin(GL_QUADS);
			glNormal3f(Cos(gear_angle)*Cos(mid_t),
						  Cos(gear_angle)*Sin(mid_t),
						  -Sin(gear_angle));
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*(radius1+(tooth_len*Cos(gear_angle))),
						  Sin(t)*(radius1+(tooth_len*Cos(gear_angle))),
						  -tooth_len*Sin(gear_angle));
			glTexCoord2f(0,0.2);
			glVertex3f(Cos(t2)*(radius1+(tooth_len*Cos(gear_angle))),
	                 Sin(t2)*(radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t2)*(radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(radius2+(tooth_len*Cos(gear_angle))),
				        length-tooth_len*Sin(gear_angle));
			glTexCoord2f(0.2,0);
			glVertex3f(Cos(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // side t
	   glBegin(GL_QUADS);
			glNormal3f(Sin(t),-Cos(t),0);
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*(radius1+(tooth_len*Cos(gear_angle))),
	                 Sin(t)*(radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0,0.2);
			glVertex3f(Cos(t)*(radius1),
	                 Sin(t)*(radius1), 0);
	      glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t)*(radius2),
				        Sin(t)*(radius2), length);
			glTexCoord2f(0.2,0);
			glVertex3f(Cos(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(radius2+(tooth_len*Cos(gear_angle))),
				        length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // side t2
	   glBegin(GL_QUADS);
			glNormal3f(-Sin(t),Cos(t),0);
			glTexCoord2f(0,0);
			glVertex3f(Cos(t2)*(radius1+(tooth_len*Cos(gear_angle))),
						  Sin(t2)*(radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0,0.2);
			glVertex3f(Cos(t2)*(radius1),
	                 Sin(t2)*(radius1), 0);
	      glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t2)*(radius2),
			   	     Sin(t2)*(radius2), length);
			glTexCoord2f(0.2,0);
			glVertex3f(Cos(t2)*(radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(radius2+(tooth_len*Cos(gear_angle))),
				        length-tooth_len*Sin(gear_angle));
	   glEnd();
   }
   
   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}

/*
 *  Draw a shaft (its just a cylinder)
 *     one end at (x,y,z)
 *     dimentions (length,radius)
 */
static void shaft(double x,double y,double z,
                 double length,double radius)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   int i;
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   
   // top
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,-1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0.0,0.0,0.0);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius,Sin(t)*radius,0.0);
   }
   glEnd();
   
   // bottom
   glBegin(GL_TRIANGLE_FAN);
   glNormal3f(0,0,1);
   glTexCoord2f(0.5,0.5);
   glVertex3f(0.0,0.0,length);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
	   glVertex3f(Cos(t)*radius,Sin(t)*radius,length);
   }
   glEnd();
   
   // side
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   glBegin(GL_TRIANGLE_STRIP);
   for(i = 0; i <= inc; ++i) {
	   float const t = 360 * (float)i / (float)inc;
	   glNormal3f(Cos(t),Sin(t),0);
	   //glTexCoord2f(.2+(float)(i)/inc,.1);
	   glTexCoord2f(0.2*Cos(t/12-20)+0.5,0.2*Sin(t/12-20)+0.5);
	   glVertex3f(Cos(t)*radius,Sin(t)*radius,0.0);
	   //glTexCoord2f(.2+(float)(i)/inc,.8);
	   glTexCoord2f(0.4*Cos(t/12-20)+0.5,0.4*Sin(t/12-20)+0.5);
	   glVertex3f(Cos(t)*radius,Sin(t)*radius,length);
   }
   glEnd();
   
   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
}
   
/*
 *  Draw the big wheel
 *  The big wheel has a hole in the middle
 *  and extrusions to turn the gears
 *  radius1 side at (x,y,z)
 *  dimentions (length,radius1,radius2) plus extrusions
 */
static void wheel(double x,double y,double z)
{
   //  Set specular color to white
   float white[] = {1,1,1,1};
   float Emission[] = {0.0,0.0,0.01*emission,1.0};
   int i;
   int num_teeth = 40;
   float gear_angle = Atan((wh_radius2-wh_radius1)/wh_length);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shinyvec);
   glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,white);
   glMaterialfv(GL_FRONT_AND_BACK,GL_EMISSION,Emission);
   //  Save transformation
   glPushMatrix();
   //  Offset, scale and rotate
   glTranslated(x,y,z);
   
   // Enable textures
   glEnable(GL_TEXTURE_2D);
   glTexEnvi(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
   glBindTexture(GL_TEXTURE_2D, texture[1]);
   
   // top
   glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,0,-1);
		for(i = 0; i <= inc; ++i) {
			float const t = 360 * (float)i / (float)inc;
			glTexCoord2f(.05*Cos(t)+0.5,.05*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius_hole,Sin(t)*wh_radius_hole,0.0);
			glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius1,Sin(t)*wh_radius1,0.0);
		}
   glEnd();
   
   // bottom
   glBegin(GL_TRIANGLE_STRIP);
		glNormal3f(0,0,1);
		for(i = 0; i <= inc; ++i) {
			float const t = 360 * (float)i / (float)inc;
			glTexCoord2f(.05*Cos(t)+0.5,.05*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius_hole,
			           Sin(t)*wh_radius_hole,
			           wh_length);
			glTexCoord2f(.3*Cos(t)+0.5,.3*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius2,Sin(t)*wh_radius2,wh_length);
		}
   glEnd();
   
   // side
   glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= inc; ++i) {
			float const t = 360 * (float)i / (float)inc;
			glNormal3f(Cos(gear_angle)*Cos(t),
						  Cos(gear_angle)*Sin(t),
				        -Sin(gear_angle));
			glTexCoord2f(.2*Cos(t)+0.5,.2*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius1,Sin(t)*wh_radius1,0.0);
			glTexCoord2f(.3*Cos(t)+0.5,.3*Sin(t)+0.5);
			glVertex3f(Cos(t)*wh_radius2,Sin(t)*wh_radius2,wh_length);
		}
   glEnd();
   
   // hole surface
   glBegin(GL_TRIANGLE_STRIP);
		for(i = 0; i <= inc; ++i) {
			float const t = 360 * (float)i / (float)inc;
			glNormal3f(-Cos(t),-Sin(t),0.0);
			glTexCoord2f((float)i*2/inc,0.6);
			glVertex3f(Cos(t)*wh_radius_hole,
						  Sin(t)*wh_radius_hole, 0.0);
			glTexCoord2f((float)i*2/inc,0.8);
			glVertex3f(Cos(t)*wh_radius_hole,
						  Sin(t)*wh_radius_hole, wh_length);
		}
   glEnd();
   
   // support arms
   cube(0, (wh_radius_hole+wh_radius1)/2, -wh_arm_len/2,
        wh_arm_width/2, wh_arm_rad/2, wh_arm_len/2, 0);
   cube(0, -(wh_radius_hole+wh_radius1)/2, -wh_arm_len/2,
        wh_arm_width/2, wh_arm_rad/2, wh_arm_len/2, 0);
   
   // teeth
   for(i = 0; i <= num_teeth; ++i) {
	   float const t = 360 * (float)i / (float)num_teeth;
	   // half way point
	   float const t2 = 360 * (float)(2*i+1) / (float)(2*num_teeth);
	   // mid tooth (for normal)
	   float const mid_t = (t+t2)/2;
	   // Front face
	   glBegin(GL_QUADS);
			glNormal3f(-Sin(gear_angle)*Cos(mid_t),
						  -Sin(gear_angle)*Sin(mid_t),
				        -Cos(gear_angle));
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*wh_radius1,Sin(t)*wh_radius1,0);
			glTexCoord2f(0,0.3);
			glVertex3f(Cos(t2)*wh_radius1,Sin(t2)*wh_radius1,0);
			glTexCoord2f(0.3,0.3);
			glVertex3f(Cos(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
				        -tooth_len*Sin(gear_angle));
			glTexCoord2f(0.3,0);
			glVertex3f(Cos(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
				        -tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // Back face
	   glBegin(GL_QUADS);
			glNormal3f(Sin(gear_angle)*Cos(mid_t),
						  Sin(gear_angle)*Sin(mid_t),
				        Cos(gear_angle));
			glTexCoord2f(0.1,0.1);
			glVertex3f(Cos(t)*wh_radius2,Sin(t)*wh_radius2,wh_length);
			glTexCoord2f(0.1,0.2);
			glVertex3f(Cos(t2)*wh_radius2,Sin(t2)*wh_radius2,wh_length);
			glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
					     Sin(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
					     wh_length-tooth_len*Sin(gear_angle));
			glTexCoord2f(0.2,0.1);
			glVertex3f(Cos(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        wh_length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // top face
	   glBegin(GL_QUADS);
			glNormal3f(Cos(gear_angle)*Cos(mid_t),
						  Cos(gear_angle)*Sin(mid_t),
						  -Sin(gear_angle));
			glTexCoord2f(0.2,0.2);
			glVertex3f(Cos(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
						  Sin(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
						  -tooth_len*Sin(gear_angle));
			glTexCoord2f(0.2,0.3);
			glVertex3f(Cos(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
	                 Sin(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0.3,0.3);
			glVertex3f(Cos(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        wh_length-tooth_len*Sin(gear_angle));
			glTexCoord2f(0.3,0.2);
			glVertex3f(Cos(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        wh_length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // side t
	   glBegin(GL_QUADS);
			glNormal3f(Sin(t),-Cos(t),0);
			glTexCoord2f(0,0);
			glVertex3f(Cos(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
	                 Sin(t)*(wh_radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0,0.1);
			glVertex3f(Cos(t)*(wh_radius1),
	                 Sin(t)*(wh_radius1), 0);
	      glTexCoord2f(0.1,0.1);
			glVertex3f(Cos(t)*(wh_radius2),
				        Sin(t)*(wh_radius2), wh_length);
			glTexCoord2f(0.1,0);
			glVertex3f(Cos(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        wh_length-tooth_len*Sin(gear_angle));
	   glEnd();
	   
	   // side t2
	   glBegin(GL_QUADS);
			glNormal3f(-Sin(t),Cos(t),0);
			glTexCoord2f(0.9,0.9);
			glVertex3f(Cos(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
						  Sin(t2)*(wh_radius1+(tooth_len*Cos(gear_angle))),
	                 -tooth_len*Sin(gear_angle));
	      glTexCoord2f(0.9,1);
			glVertex3f(Cos(t2)*(wh_radius1),
	                 Sin(t2)*(wh_radius1), 0);
	      glTexCoord2f(1,1);
			glVertex3f(Cos(t2)*(wh_radius2),
			   	     Sin(t2)*(wh_radius2), wh_length);
			glTexCoord2f(1,0.9);
			glVertex3f(Cos(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        Sin(t2)*(wh_radius2+(tooth_len*Cos(gear_angle))),
				        wh_length-tooth_len*Sin(gear_angle));
	   glEnd();
   }
   
   //  Undo transformations
   glPopMatrix();
   glDisable(GL_TEXTURE_2D);
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
static void ball(double x,double y,double z,double r)
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
   //  Undo transofrmations
   glPopMatrix();
}

/*
 *  OpenGL (GLUT) calls this routine to display the scene
 */
void display()
{
   const double len=2.0;  //  Length of axes
   const double skinny_gear_length = wh_radius2-wh_radius1;
   //  Erase the window and the depth buffer
   glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
   //  Enable Z-buffering in OpenGL
   glEnable(GL_DEPTH_TEST);

   //  Undo previous transformations
   glLoadIdentity();
   //  Perspective - set eye position
   if (mode)
   {
      double Ex = -2*dim*Sin(th)*Cos(ph);
      double Ey = +2*dim        *Sin(ph);
      double Ez = +2*dim*Cos(th)*Cos(ph);
      gluLookAt(Ex,Ey,Ez , 0,0,0 , 0,Cos(ph),0);
   }
   //  Orthogonal - set world orientation
   else
   {
      glRotatef(ph,1,0,0);
      glRotatef(th,0,1,0);
   }

   //  Flat or smooth shading
   glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

   //  Light switch
   if (light)
   {
      //  Translate intensity to color vectors
      float Ambient[]   = {0.01*ambient ,0.01*ambient ,0.01*ambient ,1.0};
      float Diffuse[]   = {0.01*diffuse ,0.01*diffuse ,0.01*diffuse ,1.0};
      float Specular[]  = {0.01*specular,0.01*specular,0.01*specular,1.0};
      //  Light position
      float Position[]  = {distance*Cos(zh),ylight,distance*Sin(zh),1.0};
      //  Draw light position as ball (still no lighting here)
      glColor3f(1,1,1);
      ball(Position[0],Position[1],Position[2] , 0.1);
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
   }
   else
     glDisable(GL_LIGHTING);

   /***  Draw scene  ***/
   glPushMatrix();
   glRotatef(qh,0,0,1);
   	glPushMatrix();
			glTranslatef(0,0,gear_inner+tooth_len);
			glRotatef(gh,0,0,1);
			gear(0,0,0,gear_length,gear_inner,gear_size,17);
			shaft(0,0,0,5,gear_inner*0.6);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,0,-(gear_inner+tooth_len));
			glRotatef(180,1,0,0);
			glRotatef(gh,0,0,1);
			gear(0,0,0,gear_length,gear_inner,gear_size,17);
			shaft(0,0,0,5,gear_inner*0.6);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,gear_inner+tooth_len,0);
			glRotatef(-90,1,0,0);
			glRotatef(-gh,0,0,1);
			gear(0,0,0,gear_length,gear_inner,gear_size,17);
			shaft(0,0,gear_length,wh_arm_rad*1.6,wh_arm_width*0.4);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(0,-(gear_inner+tooth_len),0);
			glRotatef(90,1,0,0);
			glRotatef(-gh,0,0,1);
			gear(0,0,0,gear_length,gear_inner,gear_size,17);
			shaft(0,0,gear_length,wh_arm_rad*1.6,wh_arm_width*0.4);
		glPopMatrix();
		glPushMatrix();
			wheel(0,0,gear_size);
		glPopMatrix();
   glPopMatrix();
   glPushMatrix();
		glTranslatef(-wh_radius1,0,
		             gear_size-(skinny_gear_radius+tooth_len));
		glRotatef(-90,0,1,0);
		glPushMatrix();
			glRotatef(dh,0,0,-1);
			gear(0,0,0,skinny_gear_length,skinny_gear_radius,
		        skinny_gear_radius+wh_length,10);
			shaft(0,0,0,4,.25);
		glPopMatrix();
	glPopMatrix();

   //  Draw axes - no lighting from here on
   glDisable(GL_LIGHTING);
   glColor3f(1,1,1);
   if (axes)
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
   Print("Speed=%f GearSpeed=%f Angle=%d,%d  Dim=%.1f FOV=%d Projection=%s Light=%s",
     speed, gear_speed, th,ph,dim,fov,mode?"Perpective":"Orthogonal",light?"On":"Off");
   if (light)
   {
      glWindowPos2i(5,45);
      Print("Model=%s LocalViewer=%s Distance=%d Elevation=%.1f",
            smooth?"Smooth":"Flat",
            local?"On":"Off",
            distance,
            ylight);
      glWindowPos2i(5,25);
      Print("Ambient=%d  Diffuse=%d Specular=%d Emission=%d Shininess=%.0f",
            ambient,diffuse,specular,emission,shinyvec[0]);
   }

   //  Render the scene and make it visible
   ErrCheck("display");
   glFlush();
   glutSwapBuffers();
}

/*
 *  GLUT calls this routine when the window is resized
 */
void idle()
{
   //  Elapsed time in seconds
   double t = glutGet(GLUT_ELAPSED_TIME)/1000.0;
   zh = fmod(90*t,360.0);
   qh += speed*(t-last_t);
   qh = fmod(qh,360.0);
   dh += speed*(4)*(t-last_t);
   dh = fmod(dh,360.0);
   gh += gear_speed*(t-last_t);
   gh = fmod(gh,360.0);
   last_t = t;
   //  Tell GLUT it is necessary to redisplay the scene
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
   else if (key == GLUT_KEY_PAGE_DOWN)
      dim += 0.1;
   //  PageDown key - decrease dim
   else if (key == GLUT_KEY_PAGE_UP && dim>1)
      dim -= 0.1;
   //  Smooth color model
   else if (key == GLUT_KEY_F1)
      smooth = 1-smooth;
   //  Local Viewer
   else if (key == GLUT_KEY_F2)
      local = 1-local;
   else if (key == GLUT_KEY_F3)
      distance = (distance==1) ? 5 : 1;
   //  Toggle ball increment
   else if (key == GLUT_KEY_F8)
      inc = (inc==10)?3:10;
   //  Flip sign
   else if (key == GLUT_KEY_F9)
      one = -one;
   //  Keep angles to +/-360 degrees
   th %= 360;
   ph %= 360;
   //  Update projection
   Project(mode?fov:0,asp,dim);
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
      th = ph = 0;
   // slow left wheel
   else if (ch == 'a' || ch == 'A') {
		if (gear_speed < speed)
			gear_speed += 10;
		if (gear_speed > speed)
			gear_speed = speed;
	}
	// slow right wheel
	else if (ch == 'd' || ch == 'D') {
		if (gear_speed > -speed)
			gear_speed-= 10;
		if (gear_speed < -speed)
			gear_speed = -speed;
	}
   // Accelerate
   else if (ch == 'w' || ch == 'W') {
		if (speed < 150)
			speed += 10;
		if (speed > 150)
			speed = 150;
		if (gear_speed < -speed)
			gear_speed = -speed;
	} 
	else if (ch == 's' || ch == 'S') {
		if (speed > 0)
			speed -= 10;
		if (speed < 0)
			speed = 0;
		if (gear_speed > speed)
			gear_speed = speed;
	}
	else if (ch == 'b' || ch == 'B') {
		speed = 0;
		gear_speed = 0;
	}
   //  Toggle axes
   else if (ch == 'x' || ch == 'X')
      axes = 1-axes;
   //  Toggle lighting
   else if (ch == 'l' || ch == 'L')
      light = 1-light;
   //  Switch projection mode
   else if (ch == 'p' || ch == 'P')
      mode = 1-mode;
   //  Toggle light movement
   else if (ch == 'm' || ch == 'M')
      move = 1-move;
   //  Move light
   else if (ch == '<')
      zh += 1;
   else if (ch == '>')
      zh -= 1;
   //  Change field of view angle
   else if (ch == '-' && ch>1)
      fov--;
   else if (ch == '+' && ch<179)
      fov++;
   //  Light elevation
   else if (ch=='[')
      ylight -= 0.1;
   else if (ch==']')
      ylight += 0.1;
   //  Ambient level
   else if (ch=='q' && ambient>0)
      ambient -= 5;
   else if (ch=='Q' && ambient<100)
      ambient += 5;
   //  Diffuse level
   else if (ch=='z' && diffuse>0)
      diffuse -= 5;
   else if (ch=='Z' && diffuse<100)
      diffuse += 5;
   //  Specular level
   else if (ch=='x' && specular>0)
      specular -= 5;
   else if (ch=='X' && specular<100)
      specular += 5;
   //  Emission level
   else if (ch=='e' && emission>0)
      emission -= 5;
   else if (ch=='E' && emission<100)
      emission += 5;
   //  Shininess level
   else if (ch=='n' && shininess>-1)
      shininess -= 1;
   else if (ch=='N' && shininess<7)
      shininess += 1;
   //  Translate shininess power to value (-1 => 0)
   shinyvec[0] = shininess<0 ? 0 : pow(2.0,shininess);
   //  Reproject
   Project(mode?fov:0,asp,dim);
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
   Project(mode?fov:0,asp,dim);
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
   glutInitWindowSize(400,400);
   glutCreateWindow("Differential");
   //  Set callbacks
   glutDisplayFunc(display);
   glutReshapeFunc(reshape);
   glutSpecialFunc(special);
   glutKeyboardFunc(key);
   glutIdleFunc(idle);
   // Set normalization
   glEnable(GL_NORMALIZE);
   // Load textures
   texture[0] = LoadTexBMP("metal.bmp");
   texture[1] = LoadTexBMP("metal_circles.bmp");
   
   //  Pass control to GLUT so it can interact with the user
   ErrCheck("init");
   glutMainLoop();
   return 0;
}
