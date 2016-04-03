#include <stdio.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <math.h>
//For the obj loader
#include <string.h>
#include <string>
#include <vector>
#include <queue>
#include <fstream>
#include <float.h>
    
double user_theta  = 0;
double user_height = 0;
 
void load_mesh(std::string fileName);
     
void computeLocation() {
    /*
     * Set up Projection Transform
     */
     
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum(-.1, .1, -.1, .1, .1, 1000);
    gluLookAt(0,0,0,0,0,-1,0,1,0);
    glViewport(0, 0, 512, 512);
    
    /*
     * Set up Model Transform
     */
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0.1,-1,-1.5);
    glScalef(10,10,10);
}
    
void init() { 
    /*
     * Clear depth buffer, enable depth test
     * 
     */
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    
    /*
     * Set up Transform pipeline
     * 
     */ 
    computeLocation();
    
    /*
     * Lighting Params
     * Enable Lighting
     */
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    
    /*
     * Set up light parameters.
     * Set up Material Parameters
     */
     
    GLfloat Ia[] = { 0, 0, 0, 0 };
    GLfloat light_pos[] = { 1, 1, 1, 0 }; //homogeneous vector with w=0
    GLfloat la[] = {0,0,0,0}; //ambient
    GLfloat ld[] = {1,1,1,0}; //diffuse
    GLfloat ls[] = {0,0,0,0}; //sepcular
    
//    glClearColor(0, 0, 0, 0.0);   // Set window color to white.
    
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, Ia);
    
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, la);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, ld);
    glLightfv(GL_LIGHT0, GL_SPECULAR,ls);
    
    GLfloat ka[] = { 1, 1, 1, 0};
    GLfloat kd[] = { 1, 1, 1, 0};
    GLfloat ks[] = { 0, 0, 0, 0};
    GLfloat p = 0;
    
    glMaterialfv(GL_FRONT, GL_AMBIENT, ka);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, kd);
    glMaterialfv(GL_FRONT, GL_SPECULAR, ks);
    glMaterialf(GL_FRONT, GL_SHININESS, p);
    
}
    
// Draw image.
void draw() {
    load_mesh("../bunny.obj");
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
 
 int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB);
    glutInitWindowPosition(50, 100);    // Set up display window.
    glutInitWindowSize(512, 512);
    glutCreateWindow("Bunny");

    init();
    glutDisplayFunc(draw);
    glutVisibilityFunc(visible);
    glutMainLoop();
    return 0;
}
 
/*
 * Code for doing the mesh loading
 */ 
 
struct Vector3
{
	float			x, y, z;
};

struct Triangle
{
	unsigned int 	indices[3];
};

std::vector<Vector3>	gPositions;
std::vector<Vector3>	gNormals;
std::vector<Triangle>	gTriangles;

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
    for (unsigned int i = 0 ; i < gTriangles.size(); i++){
        k0 = gTriangles[i].indices[0];
		k1 = gTriangles[i].indices[1];
		k2 = gTriangles[i].indices[2];
        
        v0 = gPositions[k0];
        v1 = gPositions[k1];
        v2 = gPositions[k2];
        
        n0 = gNormals[k0];
        n1 = gNormals[k1];
        n2 = gNormals[k2]; 
       
        glVertex3f(v0.x, v0.y, v0.z);
        glNormal3f(n0.x, n0.y, n0.z);
        
        glVertex3f(v1.x, v1.y, v1.z);
        glNormal3f(n1.x, n1.y, n1.z);

        glVertex3f(v2.x, v2.y, v2.z);
        glNormal3f(n2.x, n2.y, n2.z);
    }
    glEnd();
    
	fin.close();

	printf("Loaded mesh from %s. (%lu vertices, %lu normals, %lu triangles)\n", fileName.c_str(), gPositions.size(), gNormals.size(), gTriangles.size());
	printf("Mesh bounding box is: (%0.4f, %0.4f, %0.4f) to (%0.4f, %0.4f, %0.4f)\n", xmin, ymin, zmin, xmax, ymax, zmax);
}