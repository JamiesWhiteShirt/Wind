#pragma once

namespace geom
{
	class Matrix;

	class Vector
	{
	public:
		float x;
		float y;
		float z;
		float w;

		Vector(Vector &vec);
		Vector(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);

		Vector normalize();

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

	class Triangle
	{
	public:
		Vector vec1;
		Vector vec2;
		Vector vec3;

		Triangle(Vector vec1, Vector vec2, Vector vec3);

		Vector normal();
	};

	class Line
	{
	public:
		Vector vec1;
		Vector vec2;

		Line(Vector vec1, Vector vec2);

		Vector operator&(const Line &l);

		Vector getV() const;
	};

	bool intersects(Triangle tri, Line l);
};