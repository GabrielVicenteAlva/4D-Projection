#include <GL/glut.h>
#include <math.h>
#include <unistd.h>

#include "projection4d.hpp"


void setColor(float dist) {
	dist = 1/(1+exp(1.5*dist-5));
	// cout << dist << endl;
	glColor3f(dist*1.+.00, dist*1.+.00, 0.00);
}
void drawSphere(float x, float y, float z, float s) {
	glPushMatrix();
	glTranslatef(x,y,z);
	setColor(s);
	glutSolidSphere(.13/s,12,12);
	glPopMatrix();
}

void drawCilynder(
	float x1, float y1, float z1,
	float x2, float y2, float z2
) {
	float mat[16];
	glMultMatrixf(mat);
}

float t=0.;
float lightX = 0.0, lightY = 20.0, lightZ = 10.0;
using Vec3D = vsr::euclidean_vector<4,float>;
using Rot3D = vsr::euclidean_rotor<4,float>;
Vec3D cam_pos(4.,0.,0.);
Vec3D cam_up(0.,1.,0.);
Rot3D cam_rot(1);

void display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode( GL_MODELVIEW );
    GLfloat Lt0diff[] = {1.0,1.0,1.0,1.0};
	GLfloat Lt0pos[] = {lightX, lightY, lightZ, 1.0f};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, Lt0diff);
    glLightfv(GL_LIGHT0, GL_POSITION, Lt0pos);

	glLoadIdentity();
	// camera.pos.print();
	// cout<<"\n\n";
	gluLookAt(
		cam_pos[0],cam_pos[1],cam_pos[2],
		0.,0.,0.,
		cam_up[0],cam_up[1],cam_up[2]
	);
	
	for(Point *P: points)
		P->update();
	if(!show_all_faces) {
		for(Face *F: faces)
			F->update();
		for(Point *P: points)
			P->visible = P->visible && P->facing_cam;
	}
	// Edges
	glLineWidth(5.);
	for(Edge *E: edges) {
		if(!E->visible())
			continue;
		glBegin(GL_LINES);
			setColor(E->point1->dist_cam);
			glVertex3f(
				E->point1->projected[0],
				E->point1->projected[1],
				E->point1->projected[2]
			);
			setColor(E->point2->dist_cam);
			glVertex3f(
				E->point2->projected[0],
				E->point2->projected[1],
				E->point2->projected[2]
			);
		glEnd();
	}
	// Points
	for(Point *P: points) {
		if(!P->visible)
			continue;
		setColor(P->dist_cam);
		// cout << P->projected(0,0) << ' ' << P->projected[1] << ' ' << P->projected[2] << ' ' << P->projected[3] << endl;
		drawSphere(
			P->projected[0],
			P->projected[1],
			P->projected[2],
			P->dist_cam
		);
	}
	glFlush();
}

void idle() {
	usleep(20000);
	check_keys();
	glutPostRedisplay();
}

void keyDown (unsigned char key, int x, int y)
{
    std::cout << "keydown " << key << "\n";
    // Save which key is being pressed
	pressed_keys[key] = true;
	if(key=='<')
		show_all_faces ^= 1;
		
	if(key=='m'){
		for(Face *F: faces)
			F->normal.print();
	}
}

void keyUp (unsigned char key, int x, int y)
{
    std::cout << "keyup " << key << "\n";
	pressed_keys[key] = false;
}

void initRender() {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	
	glClearColor(0.00, 0.00, 0.00, 1.0);
	glColor3f(1.0, 1.0, 1.0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(50.0, 1.0, 1.0, 150.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

int lastX=0.,lastY=0.;
void motionFunc(int x,int y) {
	float difX = x - lastX;
	float difY = y - lastY;
	// cout<<difX<<' '<<difY<<endl;
	if(abs(difX)<8 && abs(difY)<8) {
		cam_rot = cam_rot*Vec3D(-difX/200.,difY/200.,1.)*Vec3D(0,0,1.);
		cam_pos = cam_rot*Vec3D(0.,0.,4.)*!cam_rot;
		cam_up = cam_rot*Vec3D(0.,1.,0.)*!cam_rot;
	}
	lastX = x;
	lastY = y;
}

/* Main function: GLUT runs as a console application starting at main()  */
int main(int argc, char** argv) {
	proj_init();
	glutInit(&argc, argv);                 // Initialize GLUT
	glutInitWindowSize(600, 600);   // Set the window's initial width & height
	glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
	glutCreateWindow("OpenGL Setup Test"); // Create a window with the given title
	glutDisplayFunc(display); // Register display callback handler for window re-paint
	glutIdleFunc(idle); 
    glutKeyboardFunc(keyDown);
    glutKeyboardUpFunc(keyUp);
    glutMotionFunc(motionFunc);
    initRender();
	glutMainLoop();           // Enter the infinitely event-processing loop
	return 0;
}
