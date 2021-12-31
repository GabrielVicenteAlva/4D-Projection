#pragma once
#include <iostream>
#include <vector>
#include <math.h>
#include <fstream>
#include "vsr/vsr.h"

using Vec = vsr::euclidean_vector<4,float>;
using Biv = vsr::euclidean_bivector<4,float>;
using Tri = vsr::euclidean_trivector<4,float>;
using Rot = vsr::euclidean_rotor<4,float>;

struct Edge;
struct Face;
struct Point {
	Vec pos;
	Vec translated;
	Vec projected;
	bool visible;
	bool facing_cam;
	float dist_cam;
	int ind;
	Point(ifstream&);
	void update();
	vector<Point*> adjacents;
	vector<Face*> faces;
};
struct Edge {
	Point *point1,*point2;
	Edge(ifstream&);
	bool visible();
};

struct Face {
	vector<Point*> points;
	Tri normal;
	Face(ifstream&);
	void update();
};

struct Camera{
	Vec pos;
	Rot rot;
	Vec dir;
	Camera();
	Vec apply(Vec);
	void update();
};
extern Camera camera;
extern bool show_all_faces;

extern vector<Point*> points;
extern vector<Edge*> edges;
extern vector<Face*> faces;
extern Vec center;

extern vector<bool> pressed_keys;
void check_keys();

extern bool coltune;

void proj_init();
