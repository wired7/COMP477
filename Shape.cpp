#include "Shape.h"

Plane::Plane(vec3 point, vec3 normal)
{
	this->point = point;
	this->normal = normalize(normal);
}

Plane::Plane(vec3 point1, vec3 point2, vec3 point3)
{
	point = point1;
	normal = normalize(cross(point2 - point1, point3 - point1));
}

bool Plane::intersects(vec3 origin, vec3 direction)
{
	return dot(normal, direction);
}

float Plane::intersection(vec3 origin, vec3 direction)
{
	float t = -1;
	if (Plane::intersects(origin, direction))
		t = dot(normal, point - origin) / dot(normal, direction);
	return t;
}

vec3 Plane::getNormal(vec3 point)
{
	return normal;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Sphere::Sphere(vec3 center, double radius)
{
	this->center = center;
	this->radius = radius;
}

bool Sphere::intersects(vec3 origin, vec3 direction)
{
	double discriminant = pow(dot(direction, origin - center), 2) - pow(length(direction), 2) * (pow(length(origin - center), 2) - radius * radius);
	return discriminant > 0;
}

float Sphere::intersection(vec3 origin, vec3 direction)
{
	double t = -1;
	if (intersects(origin, direction))
	{
		double d = dot(direction, origin - center);
		double e = sqrt(pow(d, 2) - pow(length(direction), 2) * (pow(length(origin - center), 2) - radius * radius));
		double t1 = (e - d) / pow(length(direction), 2);
		double t2 = -(d + e) / pow(length(direction), 2);

		if (t1 > 0 && (t1 <= t2 || t2 < 0))
			t = t1;
		else if (t2 > 0 && (t2 <= t1 || t1 < 0))
			t = t2;
	}
	return t;
}

vec3 Sphere::getNormal(vec3 point)
{
	return normalize(point - center);
}

double Sphere::getRadius()
{
	return radius;
}

void Sphere::setPosition(vec3 pos) {
	center = pos;
}

void Sphere::translate(vec3 trans) {
	center += trans;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Triangle::Triangle(vec3 pt1, vec3 pt2, vec3 pt3)
{
	point1 = pt1;
	point2 = pt2;
	point3 = pt3;
	point = point1;
	normal = normalize(cross(point2 - point1, point3 - point1));
}

float Triangle::area(vec3 pt1, vec3 pt2, vec3 pt3)
{
	return 0.5f * length(cross(pt2 - pt1, pt3 - pt1));
}

bool Triangle::intersects(vec3 origin, vec3 direction)
{
	if (Plane::intersects(origin, direction))
	{
		vec3 p = origin + Plane::intersection(origin, direction) * direction;

		vec3 v0 = point3 - point1;
		vec3 v1 = point2 - point1;
		vec3 v2 = p - point1;

		// Compute dot products	
		float dot00 = dot(v0, v0);
		float dot01 = dot(v0, v1);
		float dot02 = dot(v0, v2);
		float dot11 = dot(v1, v1);
		float dot12 = dot(v1, v2);

		// Compute barycentric coordinates
		float d = dot00 * dot11 - dot01 * dot01;
		float u = (dot11 * dot02 - dot01 * dot12) / d;
		float v = (dot00 * dot12 - dot01 * dot02) / d;

		// Check if point is in triangle
		return (u >= 0) && (v >= 0) && (u + v < 1);
	}
	return false;
}

float Triangle::intersection(vec3 origin, vec3 direction)
{
	float t = -1;
	if (intersects(origin, direction))
		t = Plane::intersection(origin, direction);
	return t;
}