#pragma once

#define INTERSECTION_NONE 0x0
#define INTERSECTION_VECTOR 0x1
#define INTERSECTION_LINE 0x2
#define INTERSECTION_TRIANGLE 0x3
#define INTERSECTION_QUAD 0x4
#define INTERSECTION_RAY 0x5
#define INTERSECTION_CUBE 0x6

namespace geom
{
	class Intersectable
	{

	};

	class Matrix;

	class Vector : public Intersectable
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector(const Vector &vec);
		Vector(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

		Vector normalize();
		Vector wDivide();

		Vector operator+(const Vector &vec) const;
		Vector operator-(const Vector &vec) const;
		Vector operator*(const Vector &vec) const;
		Vector operator*(const Matrix &mat) const;
		Vector operator*(float f) const;
		Vector operator/(const Vector &vec) const;
		Vector operator/(float f) const;
		Vector operator&(const Vector &vec) const;
		float operator|(const Vector &vec) const;
		Vector operator-() const;
		float operator~() const;
		Vector operator+=(const Vector &vec);
		Vector operator-=(const Vector &vec);
		Vector operator*=(const Vector &vec);
		Vector operator*=(const Matrix &mat);
		Vector operator*=(float f);
		Vector operator/=(const Vector &vec);
		Vector operator/=(float f);
	};

	class Matrix
	{
	private:
		int index(int x, int y) const;
	public:
		float data[16];

		Matrix();
		Matrix(float* data);

		const float& operator[](int index) const;
		float& operator[](int index);
		Matrix operator*(const Matrix& mat) const;
		Matrix operator*(const float& scalar) const;
		Vector operator*(const Vector& vert) const;
		Matrix& operator=(const Matrix& mat);

		static Matrix identity();
		static Matrix scale(float x = 1.0f, float y = 1.0f, float z = 1.0f);
		static Matrix translate(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		static Matrix ortho2D(float left, float bottom, float right, float top);
		static Matrix ortho(float left, float bottom, float near, float right, float top, float far);
		static Matrix rotate(float degrees, float x, float y, float z);
		static Matrix perspective(float fov, float aspect, float n, float f);
	};

	class Plane : public Intersectable
	{
	public:
		Vector pos;
		Vector normal;

		Plane(Vector pos = Vector(), Vector normal = Vector(1.0f));
	};

	class Ray : public Intersectable
	{
	public:
		Vector pos;
		Vector dir;

		Ray(Vector pos = Vector(), Vector dir = Vector(1.0f));
		
		Vector trace(float t);
	};

	class Triangle : public Intersectable
	{
	public:
		Vector vec1;
		Vector vec2;
		Vector vec3;

		Triangle(Vector vec1 = Vector(), Vector vec2 = Vector(), Vector vec3 = Vector());

		Vector normal();
	};

	class Quad : public Intersectable
	{
	public:
		Vector vec1;
		Vector vec2;
		Vector vec3;
		Vector vec4;

		Quad(Vector vec1 = Vector(), Vector vec2 = Vector(), Vector vec3 = Vector(), Vector vec4 = Vector());

		Vector normal();
	};

	class Line : public Intersectable
	{
	public:
		Vector vec1;
		Vector vec2;

		Line(Vector vec1 = Vector(), Vector vec2 = Vector());

		Vector getV() const;
		Vector trace(float t);
	};

	class AxisAlignedFace
	{
	public:
		Vector pos;
		float width;
		float height;

		AxisAlignedFace(Vector pos, float width, float height);

		virtual Vector getNormal() = 0;
		virtual bool inside(Vector vec) = 0;
	};

	class AxisAlignedXY : public AxisAlignedFace
	{
	public:
		AxisAlignedXY(Vector pos = Vector(), float width = 0.0f, float height = 0.0f);

		Vector getNormal();
		bool inside(Vector vec);
	};

	class AxisAlignedXZ : public AxisAlignedFace
	{
	public:
		AxisAlignedXZ(Vector pos = Vector(), float width = 0.0f, float height = 0.0f);

		Vector getNormal();
		bool inside(Vector vec);
	};

	class AxisAlignedYZ : public AxisAlignedFace
	{
	public:
		AxisAlignedYZ(Vector pos = Vector(), float width = 0.0f, float height = 0.0f);

		Vector getNormal();
		bool inside(Vector vec);
	};

	class AxisAlignedCube : public Intersectable
	{
	public:
		Vector pos;
		Vector size;

		AxisAlignedCube(Vector pos = Vector(), Vector size = Vector());

		AxisAlignedCube expand(float f);

		AxisAlignedYZ getLeft();
		AxisAlignedYZ getRight();
		AxisAlignedXZ getBottom();
		AxisAlignedXZ getTop();
		AxisAlignedXY getFront();
		AxisAlignedXY getBack();

		bool inside(Vector vec);
	};

	class Intersection
	{
	public:
		short type;

		Intersection();
		Intersection(Triangle tri, Line line);
		Intersection(Plane plane, Line line);
		Intersection(Quad quad, Line line);
		Intersection(AxisAlignedXY face, Line line);
		Intersection(AxisAlignedXZ face, Line line);
		Intersection(AxisAlignedYZ face, Line line);
		Intersection(AxisAlignedCube cube, Line line);
		Intersection(Triangle tri, Ray ray);
		Intersection(Plane plane, Ray ray);
		Intersection(Quad quad, Ray ray);
		Intersection(AxisAlignedXY face, Ray ray);
		Intersection(AxisAlignedXZ face, Ray ray);
		Intersection(AxisAlignedYZ face, Ray ray);
		Intersection(AxisAlignedCube cube, Ray ray);
		Intersection(AxisAlignedCube cube1, AxisAlignedCube cube2);
	};

	class TracedIntersection : public Intersection
	{
	public:
		Intersectable* result;
		unsigned int amount;

		TracedIntersection();
		TracedIntersection(Triangle tri, Line line);
		TracedIntersection(Plane plane, Line line);
		TracedIntersection(Quad quad, Line line);
		TracedIntersection(AxisAlignedXY face, Line line);
		TracedIntersection(AxisAlignedXZ face, Line line);
		TracedIntersection(AxisAlignedYZ face, Line line);
		TracedIntersection(AxisAlignedCube cube, Line line);
		TracedIntersection(Triangle tri, Ray ray);
		TracedIntersection(Plane plane, Ray ray);
		TracedIntersection(Quad quad, Ray ray);
		TracedIntersection(AxisAlignedXY face, Ray ray);
		TracedIntersection(AxisAlignedXZ face, Ray ray);
		TracedIntersection(AxisAlignedYZ face, Ray ray);
		TracedIntersection(AxisAlignedCube cube, Ray ray);
		TracedIntersection(AxisAlignedCube cube1, AxisAlignedCube cube2);

		~TracedIntersection();
	};
};