#pragma once
#include <vector>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/rotate_vector.hpp>

using namespace std;
using namespace glm;

static class Splines
{
public:
	static inline vec4 hermitBasis(double t);
	static void subdivide(double t0, double t1, double maxLineLength, vector<vec3>* param, vector<vec3>* curve);
		

};

