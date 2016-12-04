#include "Splines.h"

inline vec4 Splines::hermitBasis(double t) {
	return vec4(2 * pow(t, 3) - 3 * pow(t, 2) + 1, -2 * pow(t, 3) + 3 * pow(t, 2), pow(t, 3) - 2 * pow(t, 2) + t, pow(t, 3) - pow(t, 2));
}

void Splines::subdivide(double t0, double t1, double maxLineLength, vector<vec3>* param, vector<vec3>* curve) {
	double tMid = (t0 + t1) / 2;
	vec4 hMid = hermitBasis(tMid);
	vec4 hP = vec4(6 * (pow(tMid, 2) - tMid) + 1, -6 * (pow(tMid, 2) + tMid), 3 * pow(tMid, 2) - 4 * tMid + 1, 3 * pow(tMid, 2) - 2 * tMid);
	vec4 hPP = vec4(6 * (2 * tMid - 1), -6 * (2 * tMid + 1), 6 * tMid - 4, 6 * tMid - 2);
	vec3 pt0, pt1, ptMid, pointP, pointPP;

	for (int i = 0; i < param->size(); i++)
	{
		ptMid += hMid[i] * param->at(i);
		pointP += hP[i] * param->at(i);
		pointPP += hPP[i] * param->at(i);
	}

	vec3 a = cross(pointP, pointPP);
	double b = length(a) / pow(length(pointP), 3);

	if (b < maxLineLength && maxLineLength > 0.001)
	{
		subdivide(t0, tMid, maxLineLength * 0.6, param, curve);
		subdivide(tMid, t1, maxLineLength * 0.6, param, curve);
	}
	else
	{
		curve->push_back(ptMid);
	}
}

