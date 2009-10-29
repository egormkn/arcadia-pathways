/*
 Twines is a fast and simple method for drawing a curve through control points
 Copyright (C) 1998  James Marsh
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/***
 *
 *  graph.c
 *
 *  A program to display the different twines offered by the library
 *
 *  History
 *  5/12/1998 Started
 *
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <time.h>
#include <twine.h>

#define QUIT (300)
#define NEWCURVE (400)
#define ZOOM (500)
#define POINTS (600)

float control_point[11]; /* Array to hold control points */
TW_TWINE curve; /* Twine to describe curve */
int Behaviour;
int legend;
float zoom;

static void generate_twine(void);
static void update_behaviour(void);
static void print_string(char * string);

static void Idle( void )
{
   /* update animation vars */
   glutPostRedisplay();
}


static void Display( void )
{
  float sample;
  float extent;
  TW_VALUE value;
  TW_ERROR result;
  char string[100];
  int point;

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
   
   glPushMatrix();
   /* draw stuff here */
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, zoom );

   if (-6==zoom)
     extent=20;
   else
     extent=40;
   
   /* draw axes on which to plot curve */
   glLineWidth(1.0);
   glBegin (GL_LINES);
   {
     float scale;
     glColor3f (0.15, 0.0, 0.0);

     if(legend)
     for(scale=-extent;scale<=extent;scale++)
       {
	 glVertex2f (scale, extent);
	 glVertex2f (scale, -extent);
	 
	 glVertex2f (extent,scale);
	 glVertex2f (-extent,scale);
       }
     glColor3f (0.5, 0.0, 0.0);
   
     glVertex2f (-extent, 0);
     glVertex2f (extent, 0);
     
     glVertex2f (0, extent);
     glVertex2f (0, -extent);

   }
   glEnd ();

   /* draw curve */
   glLineWidth(3.0);
   glBegin (GL_LINE_STRIP);
   {     
     glColor3f (0.0, 0.6, 0.0);
     
     for (sample = -extent; sample <= -10; sample += 0.01)
       {
	 result=TW_EvaluateTwine(&value, curve, sample);
	 if (result==TW_ERROR_OK)
	   {
	       glVertex2f ((GLfloat) sample, (GLfloat) value);
	   }
       }
     
     glVertex2f(-10.0,control_point[0]);
     
     if(legend)
       glColor3f (0.0, 0.0, 0.8);

     for (sample = -10.0; sample <= 10.0; sample += 0.01)
       {
	 result=TW_EvaluateTwine(&value, curve, sample);
	 if (result==TW_ERROR_OK)
	   {
	     glVertex2f ((GLfloat) sample, (GLfloat) value);
	   }
       }

     glVertex2f(10.0,control_point[10]);

     if(legend)
       glColor3f (0.0, 0.6, 0.0);
     
     for (sample = 10.0; sample <= extent; sample += 0.01)
       {
	 result=TW_EvaluateTwine(&value, curve, sample);
	 if (result==TW_ERROR_OK)
	   {
	     glVertex2f ((GLfloat) sample, (GLfloat) value);
	   }
       }
   }
   glEnd ();
   
   if(legend)
     {
       /* plot control points as dots. */
       glPointSize (5.0);
       glColor3f (1.0, 1.0, 0.0);
       glBegin (GL_POINTS);
       {
	 for(point=0; point<11; point++)
	   {
	     glVertex2f(point*2-10, control_point[point]);
	   }
       }
       glEnd ();       
       
       glColor3f (0.8, 0.8, 0.8);
       
       for(point=0; point<11; point++)
	 {
	   glRasterPos2d(point*2-10+0.2,control_point[point]+0.2);
	   sprintf(string,"(%.2f, %.2f)",point*2.0-10.0,control_point[point]);
	   print_string(string);
	 } 
     }
   glLoadIdentity();
   glTranslatef(0.0,0.0,-6.0);

   glColor3f (1.0,0.0,0.0);
   glRasterPos2d(20.25,0);
   print_string("interval");
   
   glRasterPos2d(-0.8,20.25);
   print_string("value");


   glColor3f (1.0, 1.0, 1.0);
   glRasterPos2d(-23,23);
   print_string("Twine Behaviour : ");
   switch (Behaviour)
     {
     case TW_TYPE_Limit:
       print_string("Limited Range");  
       break;
     case TW_TYPE_ContinueSmooth:
       print_string("Continuous Smooth");  
       break;
     case TW_TYPE_ContinueLinear:
       print_string("Continuous Linear");  
       break;
     case TW_TYPE_ContinueConstant:
       print_string("Continuous Constant");  
       break;
     case TW_TYPE_Repeat:
       print_string("Repeating");  
       break;
     }
   glColor3f (0.8, 0.8, 0.8);
   glRasterPos2d(-23,21);
   print_string("(Right mouse button for menu.)");

   glColor3f (0.8, 0.6, 0.4);

   glRasterPos2d(-23,-22);
   print_string("Keys:  <1> Limited Range   <2> Continuous Smooth   <3> "
		"Continuous Linear   <4> Continuous Constant");
   glRasterPos2d(-23,-23);
   print_string("       <5> Repeating   <Space> Generate New Curve   <Escape> "
		"Quit   <Enter> Zoom ");


   if (40==extent)
     print_string("In    ");
   else
     print_string("Out   ");
   
   if (legend)
     print_string("<Tab> Hide Points");
   else
     print_string("<Tab> Display Points");
   
   glPopMatrix();
   
   glutSwapBuffers();
}

static void
print_string(char * string)
{
  int c;
  for(c=0; c<strlen(string);c++)
    {
      glutBitmapCharacter(GLUT_BITMAP_8_BY_13 , string[c]);
    }
}

static void 
Reshape( int width, int height )
{
   glViewport( 0, 0, width, height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glFrustum( -20.0, 20.0, -20.0, 20.0, 5.0, 50.0 );
   glMatrixMode( GL_MODELVIEW );
   glLoadIdentity();
   glTranslatef( 0.0, 0.0, zoom );
}


static void 
Key( unsigned char key, int x, int y )
{
  TW_ERROR result;
  switch (key) {
  case 27:
    exit(0);
  case 9:
    legend=1-legend;
    break;
  case 13:
    if (-6==zoom)
      zoom = -12;
    else
      zoom = -6;
    break;
  case ' ':
    result=TW_DeleteTwine(curve);
    if (result)
      {
	fprintf(stderr, TW_GetErrorMessage(result));
	exit(1);		
      }
    generate_twine();
    break;
  case '1':
    Behaviour=TW_TYPE_Limit;
    update_behaviour();
    break;
  case '2':
    Behaviour=TW_TYPE_ContinueSmooth;
    update_behaviour();
    break;
  case '3':
    Behaviour=TW_TYPE_ContinueLinear;
    update_behaviour();
    break;
  case '4':
    Behaviour=TW_TYPE_ContinueConstant;
    update_behaviour();
    break;
  case '5':
    Behaviour=TW_TYPE_Repeat;
    update_behaviour();
    break;
  }
  glutPostRedisplay();
}


static void
SpecialKey( int key, int x, int y )
{
   switch (key) {
      case GLUT_KEY_UP:
         break;
      case GLUT_KEY_DOWN:
         break;
      case GLUT_KEY_LEFT:
         break;
      case GLUT_KEY_RIGHT:
         break;
   }
   glutPostRedisplay();
}


static void Init( void )
{
   /* setup lighting, etc */
  glClearColor (0.0, 0.0, 0.0, 1.0);
  glShadeModel (GL_FLAT);
}

void
generate_twine(void)
{
  int point_index;
  TW_ERROR result;
  TW_VALUE value;

  result=TW_InitialiseTwine(&curve, Behaviour);
  if (result)
    {
      fprintf(stderr, TW_GetErrorMessage(result));
      exit(1);		
    }
  
  /* Randomly generate control points */
  /*  printf("Randomly generating control points:\n");*/
  for(point_index=0;point_index<11;point_index++)
    {
      control_point[point_index]=(((float)rand()/(float)RAND_MAX)*20)-10;
      result=TW_AddControl(curve,point_index*2-10,control_point[point_index]);
      /*      printf("(%d, %f)\n",point_index*2-10,control_point[point_index]);*/
      /*        if (result)
	{
	  fprintf(stderr, TW_GetErrorMessage(result));
	  exit(1);		
	}
      */
    }  
  /*  for(point_index=0;point_index<11;point_index++)
    {
      result=TW_EvaluateTwine(&value, curve, point_index*2-10);
      printf("(%lf, %lf)\n",control_point[point_index], value);
    }
    */ 
}

static void
BehaviourMenu(int entry)
{
  TW_ERROR result;

  switch(entry)
    {
    case POINTS:
      legend = 1-legend;
      break;
    case ZOOM:
      if (-6==zoom)
	zoom = -12;
      else
	zoom = -6;
      break;
    case QUIT:
      exit(0);
    case NEWCURVE:
      result=TW_DeleteTwine(curve);
      if (result)
	{
	  fprintf(stderr, TW_GetErrorMessage(result));
	  exit(1);		
	}
      generate_twine();
      break;
    default:
      Behaviour=entry;
      update_behaviour();
    }
 
  glutPostRedisplay();
}

static void 
update_behaviour(void)
{   
  TW_ERROR result;

  result=TW_SetTwineType (curve, Behaviour);
  if (result)
    {
      fprintf(stderr, TW_GetErrorMessage(result));
      exit(1);		
    }
}

int
main( int argc, char *argv[] )
{
  srand(time(NULL));  
  generate_twine();
  zoom=-6.0;
  legend=0;
  Behaviour=TW_TYPE_Limit;
  glutInit( &argc, argv );
  glutInitWindowSize( 1024, 768 );
  glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );
  
  glutCreateWindow(argv[0]);
  glutFullScreen();    

  Init();
  
  glutReshapeFunc( Reshape );
  glutKeyboardFunc( Key );
  glutSpecialFunc( SpecialKey );
  glutDisplayFunc( Display );
  glutIdleFunc( Idle );

  glutCreateMenu(BehaviourMenu);
  glutAddMenuEntry("Limited Range",TW_TYPE_Limit );
  glutAddMenuEntry("Continuous Smooth",TW_TYPE_ContinueSmooth );
  glutAddMenuEntry("Continuous Linear",TW_TYPE_ContinueLinear );
  glutAddMenuEntry("Continuous Constant",TW_TYPE_ContinueConstant );
  glutAddMenuEntry("Repeating",TW_TYPE_Repeat );
  glutAddMenuEntry("Toggle Zoom", ZOOM);  
  glutAddMenuEntry("Toggle Points", POINTS);  
  glutAddMenuEntry("Generate New Curve", NEWCURVE);
  glutAddMenuEntry("Quit", QUIT);
  glutAttachMenu(GLUT_RIGHT_BUTTON);
  
  glutMainLoop();
  return 0;
}
