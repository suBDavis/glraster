#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
    
 double user_theta  = 0;
 double user_height = 0;
     
 void computeLocation() {
     glMatrixMode(GL_PROJECTION);        // Set projection parameters.
     glLoadIdentity();
     glFrustum(-.1, .1, -.1, .1, .1, 1000);
     gluLookAt(0,0,0,0,0,-1,0,1,0);
     
     glViewport(0, 0, 512, 512);
 }
    
 // Initializes information for drawing within OpenGL.
 void init() {
//     GLfloat sun_direction[] = { 0.0, 2.0, -1.0, 1.0 };
//     GLfloat sun_intensity[] = { 0.7, 0.7, 0.7, 1.0 };
//     GLfloat ambient_intensity[] = { 0.3, 0.3, 0.3, 1.0 };
    
//     glClearColor(1.0, 1.0, 1.0, 0.0);   // Set window color to white.
     computeLocation();
    
//     glEnable(GL_DEPTH_TEST);            // Draw only closest surfaces
    
//     glEnable(GL_LIGHTING);              // Set up ambient light.
//     glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_intensity);
    
//     glEnable(GL_LIGHT0);                // Set up sunlight.
//     glLightfv(GL_LIGHT0, GL_POSITION, sun_direction);
//     glLightfv(GL_LIGHT0, GL_DIFFUSE, sun_intensity);
    
//     glEnable(GL_COLOR_MATERIAL);        // Configure glColor().
//     glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
 }
    
 // Draws the current image.
 void draw() {
     glClear(GL_COLOR_BUFFER_BIT); // Clear window.
     glColor3f(1.0, 1.0, 1.0);
//     glShadeModel(GL_SMOOTH);
     glMatrixMode(GL_MODELVIEW);
     glLoadIdentity();
     glTranslatef(0,0,-7);
     glScalef(2,2,2);
     
     glutSolidSphere(1.0, 32, 16);
     glutSwapBuffers();
 }
    
 // Arranges that the window will be redrawn roughly everyms.
 void idle() {
     static int lastTime = 0;                // time of last redraw
     int time = glutGet(GLUT_ELAPSED_TIME);  // current time
    
     if(lastTime == 0 || time >= lastTime + 40) {
         lastTime = time;
         glutPostRedisplay();
     }
 }
    
 // When window becomes visible, we want the window to
 // continuously repaint itself.
 void visible(int vis) {
     glutIdleFunc(vis == GLUT_VISIBLE ? idle : NULL);
 }
    
 // Called when a "special" key is pressed
 void special(int k, int x, int y) {
     switch(k) {
     case GLUT_KEY_UP:    user_height += 0.1; break;
     case GLUT_KEY_DOWN:  user_height -= 0.1; break;
     case GLUT_KEY_LEFT:  user_theta  += 0.1; break;
     case GLUT_KEY_RIGHT: user_theta  -= 0.1; break;
     }
     computeLocation();
     glutPostRedisplay();
 }
    
 int main(int argc, char **argv) {
     glutInit(&argc, argv);
     glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);
     glutInitWindowPosition(50, 100);    // Set up display window.
     glutInitWindowSize(300, 300);
     glutCreateWindow("Sphere");
    
     init();
     glutDisplayFunc(draw);
//     glutVisibilityFunc(visible);
//     glutSpecialFunc(special);
     glutMainLoop();
     return 0;
 }