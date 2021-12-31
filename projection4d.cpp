#include "projection4d.hpp"

Point::Point(ifstream &stream) {
	float x,y,z,w;
	stream >> x >> y >> z >> w;
	pos = Vec(x,y,z,w);
	ind = points.size();
}
float dist_f = 2.;
void Point::update() {
	translated = camera.apply(pos);
	dist_cam = translated.norm();
	visible = translated[3]>=.5;
	facing_cam = false;
	projected = translated*dist_f/translated[3];
}
Edge::Edge(ifstream &stream) {
	int p1,p2;
	stream >> p1 >> p2;
	point1 = points[p1];
	point2 = points[p2];
	point1->adjacents.push_back(point2);
	point2->adjacents.push_back(point1);
}
bool Edge::visible() {
	return point1->visible && point2->visible;
}

Face::Face(ifstream &stream){
	int p;
	Point *point;
	while(true) {
		stream >> p;
		if(p==-1)
			break;
		point = ::points[p];
		points.push_back(point);
		point->faces.push_back(this);
	}
	Vec v0 = points[0]->pos;
	Vec v1 = points[1]->pos;
	Vec v2 = points[2]->pos;
	Vec v3;
	
	for(int i=3;i<points.size();i++) {
		v3 = points[i]->pos;
		normal = (v3-v0)^(v2-v0)^(v1-v0);
		if(normal[0]!=0.)
			break;
	}
	
	(v3-v0).print();
	(v2-v0).print();
	(v1-v0).print();
	
	normal.print();
	if(((v0-center)^normal)[0] < 0)
		normal = -normal;
	normal.print();
	std::cout<<"\n\n\n";
}
void Face::update() {
	/*(points[0]->pos-camera.pos).print();
	normal.print();
	((points[0]->pos-camera.pos)^normal).print();
	cout<<"\n\n\n";*/
	if(((points[0]->pos-camera.pos)^normal)[0] < 0)
		for(Point *p:points)
			p->facing_cam = true;
}

Camera::Camera() {
	pos = Vec(0,0,0,0);
	rot = Rot(1);
}

Vec Camera::apply(Vec v) {
	return !rot*(v-pos)*rot;
}
void Camera::update() {
	dir = rot*Vec(0,0,0,1)*!rot;
}

Camera camera;
int n_points;
int n_edges;
int n_faces;
vector<Point*> points;
vector<Edge*> edges;
vector<Face*> faces;
Vec center;
bool show_all_faces = false;

Rot rotXW(float ang) {
	return Vec(cos(ang/2),0,0,sin(ang/2))*Vec(1,0,0,0);
}
Rot rotYW(float ang) {
	return Vec(0,cos(ang/2),0,sin(ang/2))*Vec(0,1,0,0);
}
Rot rotZW(float ang) {
	return Vec(0,0,cos(ang/2),sin(ang/2))*Vec(0,0,1,0);
}

// Camera control
vector<bool> pressed_keys(127,false);

Rot rotXW_CW = rotXW(-0.02);
Rot rotXW_CCW = rotXW(0.02);
Rot rotYW_CW = rotYW(-0.02);
Rot rotYW_CCW = rotYW(0.02);
Rot rotZW_CW = rotZW(-0.02);
Rot rotZW_CCW = rotZW(0.02);

bool coltune = 1.5;

void check_keys() {
// X axis
	if(pressed_keys['1'])
	    camera.pos += camera.rot*Vec(0.025,0.,0.,0.)*!camera.rot;
	if(pressed_keys['q'])
	    camera.pos += camera.rot*Vec(-.025,0.,0.,0.)*!camera.rot;
	if(pressed_keys['a'])
	    camera.rot *= rotXW_CW;
	if(pressed_keys['z'])
	    camera.rot *= rotXW_CCW;
	if(pressed_keys['A']) {
	    camera.rot = rotXW_CW*camera.rot;
	    camera.pos = rotXW_CW*camera.pos*!rotXW_CW;
	}
	if(pressed_keys['Z']){
	    camera.rot = rotXW_CCW*camera.rot;
	    camera.pos = rotXW_CCW*camera.pos*!rotXW_CCW;
	}
// Y axis
	if(pressed_keys['2'])
	    camera.pos += camera.rot*Vec(0.,0.025,0.,0.)*!camera.rot;
	if(pressed_keys['w'])
	    camera.pos += camera.rot*Vec(0.,-.025,0.,0.)*!camera.rot;
	if(pressed_keys['s'])
	    camera.rot *= rotYW(-.02);
	if(pressed_keys['x'])
	    camera.rot *= rotYW(0.02);
	if(pressed_keys['S']) {
	    camera.rot = rotYW_CW*camera.rot;
	    camera.pos = rotYW_CW*camera.pos*!rotYW_CW;
	}
	if(pressed_keys['X']){
	    camera.rot = rotYW_CCW*camera.rot;
	    camera.pos = rotYW_CCW*camera.pos*!rotYW_CCW;
	}
// Z axis
	if(pressed_keys['3'])
	    camera.pos += camera.rot*Vec(0.,0.,0.025,0.)*!camera.rot;
	if(pressed_keys['e'])
	    camera.pos += camera.rot*Vec(0.,0.,-.025,0.)*!camera.rot;
	if(pressed_keys['d'])
	    camera.rot *= rotZW(-.02);
	if(pressed_keys['c'])
	    camera.rot *= rotZW(0.02);
	if(pressed_keys['D']) {
	    camera.rot = rotZW_CW*camera.rot;
	    camera.pos = rotZW_CW*camera.pos*!rotZW_CW;
	}
	if(pressed_keys['C']){
	    camera.rot = rotZW_CCW*camera.rot;
	    camera.pos = rotZW_CCW*camera.pos*!rotZW_CCW;
	}
// Z axis
	if(pressed_keys['4'])
	    camera.pos += camera.rot*Vec(0.,0.,0.,0.025)*!camera.rot;
	if(pressed_keys['r'])
	    camera.pos += camera.rot*Vec(0.,0.,0.,-.025)*!camera.rot;

// Color
	if(pressed_keys['n'])
	    coltune *= .95;
	if(pressed_keys['m'])
	    coltune /= .95;

	camera.update();
}

void proj_init() {
	// Read data
	ifstream data_file("data.txt");
	// Number of points
	data_file >> n_points;
	// Read point coordinates
	for(int i=0;i<n_points;i++) {
		Point *p = new Point(data_file);
		points.push_back(p);
		center += p->pos;
	}
	// Center of the polytope
	center /= n_points;
	// Number of Edges
	data_file >> n_edges;
	// Indices of edges' vertices
	for(int i=0;i<n_edges;i++) {
		edges.push_back( new Edge(data_file) );
	}
	// Number of faces
	data_file >> n_faces;
	// Indices of faces' vertices
	for(int i=0;i<n_faces;i++) {
		faces.push_back( new Face(data_file) );
	}
	data_file.close();
	// Init camera
}
