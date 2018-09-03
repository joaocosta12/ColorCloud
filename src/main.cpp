#include <iostream>
#include <thread>
#include <stdio.h>
#include <vector>
#include <list>
#include <fstream>
#include <vector>

#include <GL/glut.h>

#include <opencv2/core/core.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;



struct PointK{
	double x, y, z;
	int intensity;
	int r, g, b;

	void draw(){
		glPointSize(1.0f);
        glBegin(GL_POINTS);
			glColor3f(r, g, b);
			glVertex3f(x, y, z);
			glRotatef(180.0f, 0.0f, 1.0f, 0.0f);	  
        glEnd();
	}
};

class ObjectScene{

	vector<PointK> pointsOS;
	double centroidX, centroidY, centroidZ;
	double limitSupX, limitInfX, limitSupY, limitInfY, limitSupZ, limitInfZ;

	public:
	void setpointsOS(PointK pointTemp){
		pointsOS.push_back(pointTemp);
	}

	vector<PointK> getpointsOS(){
		return pointsOS;	
	}
	
	void printXYZRGB(){
		for (vector<PointK>::iterator it = pointsOS.begin() ; it != pointsOS.end(); ++it)
			cout << (*it).x << " " << (*it).y << " " << (*it).z << " " << (*it).intensity << " " << (*it).b << " " << (*it).g << " " << (*it).r << " " << endl;
		
	}

	void centroid(){
		
		centroidX = 0;
		centroidY = 0;
		centroidZ = 0;

		for (vector<PointK>::iterator it = pointsOS.begin() ; it != pointsOS.end(); ++it){
			centroidX += (*it).x;
			centroidY += (*it).y;
			centroidZ += (*it).z;
		}

		centroidX = centroidX/(pointsOS.size());
		centroidY = centroidY/(pointsOS.size());
		centroidZ = centroidZ/(pointsOS.size());
				
	}
	
	void printCentroid(){
		cout << "centroid: (" << centroidX << ", " << centroidY << ", " << centroidZ << ")." << endl;
	}

	void createBox(){
		int contSupX = 0;
		int contInfX = 0;
		int contSupY = 0;
		int contInfY = 0;
		int contSupZ = 0;
		int contInfZ = 0;
		limitSupX = 0;
		limitInfX = 0;
		limitSupY = 0;
		limitInfY = 0;
		limitSupZ = 0;
		limitInfZ = 0;

		for (vector<PointK>::iterator it = pointsOS.begin() ; it != pointsOS.end(); ++it){
			
			if((*it).x > centroidX){
					contSupX ++;
					limitSupX += (*it).x;		
			}
			else{
					contInfX ++;
					limitInfX += (*it).x;		
			}

			if((*it).y > centroidY){
					contSupY ++;
					limitSupY += (*it).y;		
			}
			else{
					contInfY ++;
					limitInfY += (*it).y;		
			}
			
			if((*it).z > centroidZ){
					contSupZ ++;
					limitSupZ += (*it).z;		
			}
			else{
					contInfZ ++;
					limitInfZ += (*it).z;		
			}

		}

		limitSupX = limitSupX/contSupX;		
		limitInfX = limitInfX/contInfX;
		limitSupY = limitSupY/contSupY;		
		limitInfY = limitInfY/contInfY;
		limitSupZ = limitSupZ/contSupZ;		
		limitInfZ = limitInfZ/contInfZ;

	}

	void printBox(){
		cout << "X top: " << limitSupX << "| down: " << limitInfX << endl;	
		cout << "Y top: " << limitSupY << "| down: " << limitInfY << endl;
		cout << "Z top: " << limitSupZ << "| down: " << limitInfZ << endl;
	
	}
};


vector<PointK> colorCloud;

void display(void){
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear color and depth buffers
   glMatrixMode(GL_MODELVIEW);     // To operate on model-view matrix
 
   // Render a color-cube consisting of 6 quads with different colors
   glLoadIdentity();                 // Reset the model-view matrix
   glTranslatef(1.5f, 0.0f, -7.0f);  // Move right and into the screen

	//Draw the whole colored color
	for (vector<PointK>::iterator it = colorCloud.begin() ; it != colorCloud.end(); ++it){
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


void pointCloudGenerator(){

	//Reading the real images
    Mat img1, img2;
    img1 = imread("left2.png");
    img2 = imread("right2.png");

  
	//Calibration according to camera datas.
    double cm1[3][3] = {{9.597910e+02, 0.000000e+00, 6.960217e+02}, {0.000000e+00, 9.569251e+02, 2.241806e+02}, {0.000000e+00, 0.000000e+00, 1.000000e+00}};
    double cm2[3][3] = {{9.037596e+02, 0.000000e+00, 6.957519e+02}, {0.000000e+00, 9.019653e+02, 2.242509e+02}, {0.000000e+00, 0.000000e+00, 1.000000e+00}};
    double d1[1][5] = {{-3.691481e-01, 1.968681e-01, 1.353473e-03, 5.677587e-04, -6.770705e-02}};
    double d2[1][5] = {{-3.639558e-01, 1.788651e-01, 6.029694e-04, -3.922424e-04, -5.382460e-02}};

    Mat CM1 (3,3, CV_64FC1, cm1);
    Mat CM2 (3,3, CV_64FC1, cm2);
    Mat D1(1,5, CV_64FC1, d1);
    Mat D2(1,5, CV_64FC1, d2);

    cout << "Calibration matrix left:\n" << CM1 << endl;
    cout << "Distorstion matrix left:\n" << D1 << endl;
    cout << "Calibration matrix right:\n" << CM2 << endl;
    cout << "Distorstion matrix right:\n" << D2 << endl;


    double r[3][3] = {{9.995599e-01, 1.699522e-02, -2.431313e-02},{-1.704422e-02, 9.998531e-01, -1.809756e-03},{2.427880e-02, 2.223358e-03, 9.997028e-01}};
    double t[3][1] = {{-4.731050e-01}, {5.551470e-03}, {-5.250882e-03}};


    Mat R (3,3, CV_64FC1, r);
    Mat T (3,1, CV_64FC1, t);
    Mat R1, R2, T1, T2, Q, P1, P2;


	//Using stereoRectify to generate a Q matrix
    stereoRectify(CM1, D1, CM2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);

    cout << "R2:\n" << R2 << endl;
    cout << "P2:\n" << P2 << endl;

    double rr1[3][3] = {{9.998817e-01, 1.511453e-02, -2.841595e-03},{-1.511724e-02, 9.998853e-01, -9.338510e-04},{2.827154e-03, 9.766976e-04, 9.999955e-01}};
    double rr2[3][3] = {{9.998321e-01, -7.193136e-03, 1.685599e-02},{7.232804e-03, 9.999712e-01, -2.293585e-03},{-1.683901e-02, 2.415116e-03, 9.998553e-01}};
    double pp1[3][4] = {{7.215377e+02, 0.000000e+00, 6.095593e+02, 4.485728e+01},{0.000000e+00, 7.215377e+02, 1.728540e+02, 2.163791e-01},{0.000000e+00, 0.000000e+00, 1.000000e+00, 2.745884e-03}};
    double pp2[3][4] = {{7.215377e+02, 0.000000e+00, 6.095593e+02, -3.395242e+02},{0.000000e+00, 7.215377e+02, 1.728540e+02, 2.199936e+00},{0.000000e+00, 0.000000e+00, 1.000000e+00, 2.729905e-03}};
   

    Mat RR1 (3,3, CV_64FC1, rr1);
    Mat RR2 (3,3, CV_64FC1, rr2);
    Mat PP1 (3,4, CV_64FC1, pp1);
    Mat PP2 (3,4, CV_64FC1, pp2);

    Mat map11, map12, map21, map22;
    Size img_size = img1.size();
    initUndistortRectifyMap(CM1, D1, RR1, PP1, img_size, CV_16SC2, map11, map12);
    initUndistortRectifyMap(CM2, D2, RR2, PP2, img_size, CV_16SC2, map21, map22);
    Mat img1r, img2r;
    //remap(img1, img1r, map11, map12, INTER_LINEAR);
    //  remap(img2, img2r, map21, map22, INTER_LINEAR);
    //img1 = img1r;
    //img2 = img2r;


	//Create a Disparity matrix 
    int sadSize = 3;
    cv::StereoSGBM sbm;
    sbm.SADWindowSize = sadSize;
    sbm.numberOfDisparities = 144;//144; 128
    sbm.preFilterCap = 10; //63
    sbm.minDisparity = 0; //-39; 0
    sbm.uniquenessRatio = 10;
    sbm.speckleWindowSize = 100;
    sbm.speckleRange = 32;
    sbm.disp12MaxDiff = 1;
    sbm.fullDP = true;
    sbm.P1 = sadSize*sadSize*4;
    sbm.P2 = sadSize*sadSize*32;

    Mat disp, disp8;
    sbm(img1, img2, disp);

    //disp = imread("disp.png", CV_LOAD_IMAGE_GRAYSCALE);
    normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);


    Mat points0, points1;
    reprojectImageTo3D(disp, points0, Q, true);
    cvtColor(points0, points1, CV_BGR2GRAY);

    Mat img3;
    img3 = imread("semantica.png");
    
	ofstream semantics_file;
    semantics_file.open ("semantica.xyz");
    for(int i = 0; i < img3.rows; i++) {
        for(int j = 0; j < img3.cols; j++) {
                semantics_file << static_cast<unsigned>(img3.at<Vec3b>(i,j)[0]) << " " << static_cast<unsigned>(img3.at<Vec3b>(i,j)[1]) << " " << static_cast<unsigned>(img3.at<Vec3b>(i,j)[2])  << endl;
        }
    }
    semantics_file.close();
    
    ofstream point_cloud_file;
    point_cloud_file.open ("point_cloud.xyz");
    for(int i = 0; i < points0.rows; i++) {
        for(int j = 0; j < points0.cols; j++) {
            if(points0.at<Vec3f>(i,j)[2] < 10) {
                point_cloud_file << points0.at<Vec3f>(i,j)[0] << " " << points0.at<Vec3f>(i,j)[1] << " " << points0.at<Vec3f>(i,j)[2] << " " << static_cast<unsigned>(img1.at<uchar>(i,j)) << " " << static_cast<unsigned>(img1.at<uchar>(i,j)) << " " << static_cast<unsigned>(img1.at<uchar>(i,j)) << endl; 
            }
        }
    }
    point_cloud_file.close();


    ofstream color_cloud_file;
    color_cloud_file.open ("color_cloud.xyz");
    for(int i = 0; i < points0.rows; i++) {
        for(int j = 0; j < points0.cols; j++) {
            if(points0.at<Vec3f>(i,j)[2] < 10) {
                color_cloud_file << points0.at<Vec3f>(i,j)[0] << " " << points0.at<Vec3f>(i,j)[1] << " " << points0.at<Vec3f>(i,j)[2] << " " << static_cast<unsigned>(img1.at<uchar>(i,j)) << " " << static_cast<unsigned>(img3.at<Vec3b>(i,j)[0]) << " " << static_cast<unsigned>(img3.at<Vec3b>(i,j)[1]) << " " << static_cast<unsigned>(img3.at<Vec3b>(i,j)[2])  << endl;
            }
        }
    }
    color_cloud_file.close();


    imshow("Img1", img1);
    imshow("Img2", img2);
    imshow("Img3", img3);
    imshow("points", points0);
    imshow("points1", points1);

}

void fillingCloud(ObjectScene &car, ObjectScene &street, ObjectScene &sign, ObjectScene &tree, ObjectScene &sky, ObjectScene &sidewalk, ObjectScene &house, ObjectScene &error){

	// 			B	G	R
	//car -		255	0	255
	//street -	128	128	128
	//sign -	0	0	0
	//tree -	0	128	0
	//sky -		255	0	0
	//sidewalk	0	128	128
	//house -	0	0	255

	PointK pointsP;

	fstream reader_file;
    reader_file.open ("color_cloud.xyz");
		
	if(reader_file.is_open()){    
		
		while(!reader_file.eof()){

			reader_file >> pointsP.x >> pointsP.y >> pointsP.z >> pointsP.intensity >> pointsP.b >> pointsP.g >> pointsP.r;
			colorCloud.push_back(pointsP);		

			if(pointsP.b == 255 && pointsP.g == 0 && pointsP.r == 255)
				car.setpointsOS(pointsP);
			else if(pointsP.b == 128 && pointsP.g == 128 && pointsP.r == 128)
				street.setpointsOS(pointsP);
			else if(pointsP.b == 0 && pointsP.g == 0 && pointsP.r == 0)
				sign.setpointsOS(pointsP);
			else if(pointsP.b == 0 && pointsP.g == 128 && pointsP.r == 0)
				tree.setpointsOS(pointsP);
			else if(pointsP.b == 255 && pointsP.g == 0 && pointsP.r == 0)
				sky.setpointsOS(pointsP);
			else if(pointsP.b == 0 && pointsP.g == 128 && pointsP.r == 128)
				sidewalk.setpointsOS(pointsP);
			else if(pointsP.b == 0 && pointsP.g == 0 && pointsP.r == 255)
				house.setpointsOS(pointsP);
			else
				error.setpointsOS(pointsP);
		}
	}
	else{
	cout << "Arquivo não encontrado" << endl;	
	}
    reader_file.close();
	
}

void drawColorCloud(int argc, char** argv){
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE);
	glutInitWindowSize(1392, 512);
	glutInitWindowPosition(100,100);
	glutCreateWindow("Cloud");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	initGL();
	glutMainLoop();
}

int main(int argc, char** argv)
{

	pointCloudGenerator();
    waitKey(0);	

	ObjectScene car, street, sign, tree, sky, sidewalk, house, error;

	fillingCloud(car, street, sign, tree, sky, sidewalk, house, error);	

	//PRINTING COMPONENTES
	//car.printXYZRGB();
	//street.printXYZRGB();
	//sign.printXYZRGB();
	//tree.printXYZRGB();
	//sky.printXYZRGB();
	//sidewalk.printXYZRGB();
	//house.printXYZRGB();

	//CALCULATING CENTROID
	car.centroid();
	street.centroid();
	sign.centroid();
	tree.centroid();
	sky.centroid();
	sidewalk.centroid();
	house.centroid();
	
	//PRINTING CENTROID
	car.printCentroid();
	street.printCentroid();
	sign.printCentroid();
	tree.printCentroid();
	sky.printCentroid();
	sidewalk.printCentroid();
	house.printCentroid();
	
	//CALCULATING BOX
	car.createBox();
	street.createBox();
	sign.createBox();
	tree.createBox();
	sky.createBox();
	sidewalk.createBox();
	house.createBox();
	
	
	//PRINTING BOX
	cout << "car" << endl;
	car.printBox();
	//cout << "street" << endl;
	//street.printBox();
	//cout << "sign" << endl;
	//sign.printBox();
	//cout << "tree" << endl;
	//tree.printBox();
	//cout << "sky" << endl;
	//sky.printBox();
	//cout << "sidewalk" << endl;
	//sidewalk.printBox();
	//cout << "house" << endl;
	//house.printBox();


	//DESENHAR
	//drawColorCloud(argc, argv);


	

	
	
	return 0;
}
