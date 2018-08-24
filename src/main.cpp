#include <iostream>
#include <thread>
#include <stdio.h>
#include <vector>
#include <list>
#include <fstream>
#include <vector>

#include <GL/glut.h>

using namespace std;

struct Point{
	double x, y, z;
	int intensidade;
	int r, g, b;

	void draw(){
		glPointSize(1.0f);
        glBegin(GL_POINTS);
			glColor3f(r, g, b);
			glVertex3f(x, y, z);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);	  
        glEnd();
	//cout << r << " " << g << " " << b  << endl;
	}
};

vector<Point> colorCloud;

void display(void){
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
   // Render a color-cube consisting of 6 quads with different colors
   glLoadIdentity();                 // Reset the model-view matrix
   glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen


	for (vector<Point>::iterator it = colorCloud.begin() ; it != colorCloud.end(); ++it){
		(*it).draw();	
	}
	glutSwapBuffers();
}

void initGL() {
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set background color to black and opaque
   glClearDepth(1.0f);                   // Set background depth to farthest
   glEnable(GL_DEPTH_TEST);   // Enable depth testing for z-culling
   glDepthFunc(GL_LEQUAL);    // Set the type of depth-test
   glShadeModel(GL_SMOOTH);   // Enable smooth shading
   glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Nice perspective corrections
}

void reshape(GLsizei width, GLsizei height) {  // GLsizei for non-negative integer
   // Compute aspect ratio of the new window
   if (height == 0) height = 1;                // To prevent divide by 0
   GLfloat aspect = (GLfloat)width / (GLfloat)height;
 
   // Set the viewport to cover the new window
   glViewport(0, 0, width, height);
 
   // Set the aspect ratio of the clipping volume to match the viewport
   glMatrixMode(GL_PROJECTION);  // To operate on the Projection matrix
   glLoadIdentity();             // Reset
   // Enable perspective projection with fovy, aspect, zNear and zFar
   gluPerspective(90.0f, aspect, 0.1f, 100.0f);
}

int main(int argc, char** argv)
{

	
	Point points;

	fstream reader_file;
    reader_file.open ("color_cloud.xyz");
	
	
	if(reader_file.is_open()){    
		
		while(!reader_file.eof()){
		reader_file >> points.x >> points.y >> points.z >> points.intensidade >> points.b >> points.g >> points.r;
		colorCloud.push_back(points);		

		//cout << points.x << " " << points.y << " "  << points.z << " "  << points.intensidade << " "  << points.b << " "  << points.g << " "  << points.r << " "  << endl;
		}
	}
	else{
	cout << "Arquivo não encontrado" << endl;	
	}
    reader_file.close();

	//LEITURA
	//	for (vector<Point>::iterator it = colorCloud.begin() ; it != colorCloud.end(); ++it)
	//		cout << (*it).x << " " << (*it).y << " " << (*it).z << " " << (*it).intensidade << " " << (*it).b << " " << (*it).g << " " << (*it).r << " " << endl;
	



	//DESENHAR

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(1392, 512);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Cloud");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	initGL();
	glutMainLoop();


	

	
	
	return 0;
}