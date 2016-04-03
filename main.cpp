#include <stdio.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>
#include <math.h>

//For the obj loader
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <float.h>

float gTotalTimeElapsed = 0;
int gTotalFrames = 0;
GLuint gTimer;

void load_mesh(std::string fileName);

struct Vector3
{
	float x, y, z;
};

void init_timer()
{
    glGenQueries(1, &gTimer);
}

void start_timing()
{
    glBeginQuery(GL_TIME_ELAPSED, gTimer);
}

float stop_timing()
{
    glEndQuery(GL_TIME_ELAPSED);

    GLint available = GL_FALSE;
    while (available == GL_FALSE)
        glGetQueryObjectiv(gTimer, GL_QUERY_RESULT_AVAILABLE, &available);

    GLint result;
    glGetQueryObjectiv(gTimer, GL_QUERY_RESULT, &result);

    float timeElapsed = result / (1000.0f * 1000.0f * 1000.0f);
    return timeElapsed;
}
     
void computeLocation() {
    
    /*
     * Set up Model Transform
     */

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0,0.0,0.0,0.0,0.0,-1.0,0.0,1.0,0.0);
    glTranslatef(0.1,-1.0,-1.5);
    glScalef(10.0,10.0,10.0);

    /*
     * Set up Projection Transform
     */

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.1, .1, -.1, .1, .1, 1000);
    glViewport(0, 0, 512, 512);
}
    
void init() {
    
    /*
     * Clear depth buffer, enable depth test
     * 
     */
     
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1000);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    
    /*
     * Set up Transform pipeline
     * 
     */ 
    computeLocation();
    
    /*
     * Enable Lighting
     * 
     */
    
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_NORMALIZE);
    
    /*
     * Set up light parameters.
     * 
     */
     
    GLfloat Ia[] = { .2, .2, .2, 1 };
    GLfloat light_pos[] = { 1, 1, 1, 0 };
    GLfloat la[] = {0,0,0}; //ambient
    GLfloat ld[] = {1,1,1}; //diffuse
    GLfloat ls[] = {0,0,0}; //sepcular
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ia);
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, la);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
    glLightfv(GL_LIGHT0, GL_SPECULAR,ls);

    /*
     * Set up Material Parameters
     * 
     */

    GLfloat ka[] = { 1, 1, 1}; //ambient
    GLfloat kd[] = { 1, 1, 1}; //specular
    GLfloat ks[] = { 0, 0, 0}; //diffuse
    GLfloat p = 0;

    glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, p);
    
}
    
// Draw image.
void draw() {
    
    //Do the Clears
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearDepth(1000);
    glewInit(); //nobody said I had to put this here, but piazza said it fixes the segfault

    //Start timer ever frame
    start_timing();

    //Do the draw
    load_mesh("../bunny.obj");
    
    float timeElapsed = stop_timing();
    gTotalFrames++;
    gTotalTimeElapsed += timeElapsed;
    float fps = gTotalFrames / gTotalTimeElapsed;
    char string[1024] = {0};
    sprintf(string, "OpenGL Bunny: %0.2f FPS", fps);
    glutSetWindowTitle(string);

    //Reload the scene
    glutPostRedisplay();
    glutSwapBuffers();
}
 
int main(int argc, char **argv) {
    //Initialize the GUI window
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowPosition(50, 100);    // Set up display window.
    glutInitWindowSize(512, 512);
    glutCreateWindow("Bunny");

    init();
    glutDisplayFunc(draw);
    init_timer();
    glutMainLoop();
    return 0;
}
 
/*
 * Code for doing the mesh loading
 * Should move this to another file, 
 * but i probably wont
 */ 
 
struct Triangle
{
    unsigned int indices[3];
};

std::vector<Vector3> gPositions;
std::vector<Vector3> gNormals;
std::vector<Triangle> gTriangles;

void tokenize(char* string, std::vector<std::string>& tokens, const char* delimiter)
{
	char* token = strtok(string, delimiter);
	while (token != NULL)
	{
		tokens.push_back(std::string(token));
		token = strtok(NULL, delimiter);
	}
}

int face_index(const char* string)
{
	int length = strlen(string);
	char* copy = new char[length + 1];
	memset(copy, 0, length+1);
	strcpy(copy, string);

	std::vector<std::string> tokens;
	tokenize(copy, tokens, "/");
	delete[] copy;
	if (tokens.front().length() > 0 && tokens.back().length() > 0 && atoi(tokens.front().c_str()) == atoi(tokens.back().c_str()))
	{
		return atoi(tokens.front().c_str());
	}
	else
	{
		printf("ERROR: Bad face specifier!\n");
		exit(0);
	}
}

void load_mesh(std::string fileName)
{
	std::ifstream fin(fileName.c_str());
	if (!fin.is_open())
	{
		printf("ERROR: Unable to load mesh from %s!\n", fileName.c_str());
		exit(0);
	}

	float xmin = FLT_MAX;
	float xmax = -FLT_MAX;
	float ymin = FLT_MAX;
	float ymax = -FLT_MAX;
	float zmin = FLT_MAX;
	float zmax = -FLT_MAX;

	while (true)
	{
		char line[1024] = {0};
		fin.getline(line, 1024);

		if (fin.eof())
			break;

		if (strlen(line) <= 1)
			continue;

		std::vector<std::string> tokens;
		tokenize(line, tokens, " ");

		if (tokens[0] == "v")
		{
			float x = atof(tokens[1].c_str());
			float y = atof(tokens[2].c_str());
			float z = atof(tokens[3].c_str());

			xmin = std::min(x, xmin);
			xmax = std::max(x, xmax);
			ymin = std::min(y, ymin);
			ymax = std::max(y, ymax);
			zmin = std::min(z, zmin);
			zmax = std::max(z, zmax);

			Vector3 position = {x, y, z};
			gPositions.push_back(position);
		}
		else if (tokens[0] == "vn")
		{
			float x = atof(tokens[1].c_str());
			float y = atof(tokens[2].c_str());
			float z = atof(tokens[3].c_str());
			Vector3 normal = {x, y, z};
			gNormals.push_back(normal);
		}
		else if (tokens[0] == "f")
		{
			unsigned int a = face_index(tokens[1].c_str());
			unsigned int b = face_index(tokens[2].c_str());
			unsigned int c = face_index(tokens[3].c_str());
			Triangle triangle;
			triangle.indices[0] = a - 1;
			triangle.indices[1] = b - 1;
			triangle.indices[2] = c - 1;
			gTriangles.push_back(triangle);
		}
	}
    
    int k0, k1, k2;
    Vector3 v0, v1, v2;
    Vector3 n0, n1, n2;
    
    glBegin(GL_TRIANGLES);
    for ( int i = gTriangles.size()-1 ; i >= 0; i--){
        k0 = gTriangles[i].indices[0];
        k1 = gTriangles[i].indices[1];
        k2 = gTriangles[i].indices[2];
        
        v0 = gPositions[k0];
        v1 = gPositions[k1];
        v2 = gPositions[k2];
        
        n0 = gNormals[k0];
        n1 = gNormals[k1];
        n2 = gNormals[k2]; 

//        k2 = gTriangles[i].indices[0];
//        k1 = gTriangles[i].indices[1];
//        k0 = gTriangles[i].indices[2];
//        
//        v2 = gPositions[k0];
//        v1 = gPositions[k1];
//        v0 = gPositions[k2];
//        
//        n2 = gNormals[k0];
//        n1 = gNormals[k1];
//        n0 = gNormals[k2]; 
       
        glVertex3f(v0.x, v0.y, v0.z);
        glNormal3f(n0.x, n0.y, n0.z);
        
        glVertex3f(v1.x, v1.y, v1.z);
        glNormal3f(n1.x, n1.y, n1.z);

        glVertex3f(v2.x, v2.y, v2.z);
        glNormal3f(n2.x, n2.y, n2.z);
//        
//        glVertex3f(v1.x, v1.y, v1.z);
//        glNormal3f(n1.x, n1.y, n1.z);
//        
//        glVertex3f(v0.x, v0.y, v0.z);
//        glNormal3f(n0.x, n0.y, n0.z);
    }
    glEnd();
    
	fin.close();

//	printf("Loaded mesh from %s. (%lu vertices, %lu normals, %lu triangles)\n", fileName.c_str(), gPositions.size(), gNormals.size(), gTriangles.size());
//	printf("Mesh bounding box is: (%0.4f, %0.4f, %0.4f) to (%0.4f, %0.4f, %0.4f)\n", xmin, ymin, zmin, xmax, ymax, zmax);
}