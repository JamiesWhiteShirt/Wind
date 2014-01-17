#define _USE_MATH_DEFINES

#include "geometry.h"
#include <math.h>

using namespace geom;

Vector::Vector(Vector &vec)
	: x(vec.x), y(vec.y), z(vec.z), w(vec.w)
{
	
}

Vector::Vector(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{
	
}

Vector Vector::normalize()
{
	return *this /= ~*this;
}

Vector Vector::wDivide()
{
	x /= w;
	y /= w;
	z /= w;
	return *this;
}

Vector Vector::operator+(const Vector &vec) const
{
	return Vector(x + vec.x, y + vec.y, z + vec.z);
}

Vector Vector::operator-(const Vector &vec) const
{
	return Vector(x - vec.x, y - vec.y, z - vec.z);
}

Vector Vector::operator*(const Vector &vec) const
{
	return Vector(x * vec.x, y * vec.y, z * vec.z);
}

Vector Vector::operator*(const Matrix &mat) const
{
	Vector result;
	float f = mat[0];

	result.x = x * mat[0] + y * mat[1] + z * mat[2] + w * mat[3];
	result.y = x * mat[4] + y * mat[5] + z * mat[6] + w * mat[7];
	result.z = x * mat[8] + y * mat[9] + z * mat[10] + w * mat[11];
	result.w = x * mat[12] + y * mat[13] + z * mat[14] + w * mat[15];

	return result;
}

Vector Vector::operator*(float f) const
{
	return Vector(x * f, y * f, z * f);
}

Vector Vector::operator/(const Vector &vec) const
{
	return Vector(x / vec.x, y / vec.y, z / vec.z);
}

Vector Vector::operator/(float f) const
{
	return Vector(x / f, y / f, z / f);
}

Vector Vector::operator&(const Vector &vec) const
{
	return Vector(y * vec.z - z * vec.y, z * vec.x - x * vec.z, x * vec.y - y * vec.x);
}

float Vector::operator|(const Vector &vec) const
{
	return x * vec.x + y * vec.y + z * vec.z;
}

Vector Vector::operator-() const
{
	return Vector(-x, -y, -z);
}

float Vector::operator~() const
{
	return sqrtf(x * x + y * y + z * z);
}

Vector Vector::operator+=(const Vector &vec)
{
	return Vector(x += vec.x, y += vec.y, z += vec.z);
}

Vector Vector::operator-=(const Vector &vec)
{
	return Vector(x -= vec.x, y -= vec.y, z -= vec.z);
}

Vector Vector::operator*=(const Vector &vec)
{
	return Vector(x *= vec.x, y *= vec.y, z *= vec.z);
}

Vector Vector::operator*=(const Matrix &mat)
{
	//return Vector(x * vec.x, y * vec.y, z * vec.z, w * vec.w);
	return *this;
}

Vector Vector::operator*=(float f)
{
	return Vector(x *= f, y *= f, z *= f);
}

Vector Vector::operator/=(const Vector &vec)
{
	return Vector(x /= vec.x, y /= vec.y, z /= vec.z);
}

Vector Vector::operator/=(float f)
{
	return Vector(x /= f, y /= f, z /= f);
}



int Matrix::index(int x, int y) const
{
	return x + (y << 2);
}

Matrix::Matrix()
{
	for(int i = 0; i < 16; i++)
	{
		data[i] = 0.0f;
	}
}

Matrix::Matrix(float* data)
{
	for(int i = 0; i < 16; i++)
	{
		this->data[i] = data[i];
	}
}

const float& Matrix::operator[](int index) const
{
	return data[index];
}

float& Matrix::operator[](int index)
{
	return data[index];
}


Matrix Matrix::operator*(const Matrix& mat) const
{
	Matrix result;

	for(int i = 0; i < 4; i++)
	{
		for(int j = 0; j < 4; j++)
		{
			float value = 0.0f;
			for(int k = 0; k < 4; k++)
			{
				value += data[index(k, j)] * mat.data[index(i, k)];
			}
			result[index(i, j)] = value;
		}
	}

	return result;
}

Matrix Matrix::operator*(const float& scalar) const
{
	Matrix result;

	for(int i = 0; i < 16; i++)
	{
		result[i] = data[i] * scalar;
	}

	return result;
}

Vector Matrix::operator*(const Vector& vert) const
{
	Vector result;

	result.x = vert.x * data[0] + vert.y * data[1] + vert.z * data[2] + vert.w * data[3];
	result.y = vert.x * data[4] + vert.y * data[5] + vert.z * data[6] + vert.w * data[7];
	result.z = vert.x * data[8] + vert.y * data[9] + vert.z * data[10] + vert.w * data[11];
	result.w = vert.x * data[12] + vert.y * data[13] + vert.z * data[14] + vert.w * data[15];

	return result;
}

Matrix& Matrix::operator=(const Matrix& mat)
{
	for(int i = 0; i < 16; i++)
	{
		data[i] = mat.data[i];
	}

	return *this;
}

Matrix Matrix::identity()
{
	Matrix result;
	result[0] = 1.0f;
	result[5] = 1.0f;
	result[10] = 1.0f;
	result[15] = 1.0f;

	return result;
}

Matrix Matrix::scale(float x, float y, float z)
{
	Matrix result;
	result[0] = x;
	result[5] = y;
	result[10] = z;
	result[15] = 1.0f;

	return result;
}

Matrix Matrix::translate(float x, float y, float z)
{
	Matrix result = identity();
	result[3] = x;
	result[7] = y;
	result[11] = z;

	return result;
}

Matrix Matrix::ortho2D(float left, float bottom, float right, float top)
{
	float matData[] = {
		2.0f / (right - left), 0.0f, 0.0f, (right + left) / (left - right),
		0.0f, 2.0f / (top - bottom), 0.0f, (top + bottom) / (bottom - top),
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Matrix(matData);
}

Matrix Matrix::ortho(float left, float bottom, float n, float right, float top, float f)
{
	float matData[] = {
		2.0f / (right - left), 0.0f, 0.0f, (right + left) / (left - right),
		0.0f, 2.0f / (top - bottom), 0.0f, (top + bottom) / (bottom - top),
		0.0f, 0.0f, 2.0f / (f - n), (f + n) / (n - f),
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Matrix(matData);
}

Matrix Matrix::rotate(float degrees, float x, float y, float z)
{
	float s = (float)sin(degrees * M_PI / 180.0f);
	float c = (float)cos(degrees * M_PI / 180.0f);

	float matData[] = {
		x + (y + z) * c, z * -s, y * s, 0.0f,
		z * s, y + (x + z) * c, x * -s, 0.0f,
		y * -s, x * s, z + (x + y) * c, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};

	return Matrix(matData);
}

Matrix Matrix::perspective(float fov, float aspect, float n, float f)
{
	float frustumDepth = f - n;
    float oneOverDepth = 1 / frustumDepth;

	float matData[] = {
		(1.0f / (float)tan(fov * M_PI / 360.0f)) / aspect, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / (float)tan(fov * M_PI / 360.0f), 0.0f, 0.0f,
		0.0f, 0.0f, f * oneOverDepth, -f * n * oneOverDepth,
		0.0f, 0.0f, 1.0f, 0.0f
	};

	return Matrix(matData);
}

Plane::Plane(Vector pos, Vector normal)
	: pos(pos), normal(normal)
{

}

Ray::Ray(Vector pos, Vector dir)
	: pos(pos), dir(dir)
{

}

Vector Ray::trace(float t)
{
	return pos + dir * t;
}

Triangle::Triangle(Vector vec1, Vector vec2, Vector vec3)
	: vec1(vec1), vec2(vec2), vec3(vec3)
{

}

Vector Triangle::normal()
{
	return (vec2 - vec1) & (vec3 - vec1);
}

Quad::Quad(Vector vec1, Vector vec2, Vector vec3, Vector vec4)
	: vec1(vec1), vec2(vec2), vec3(vec3), vec4(vec4)
{

}

Vector Quad::normal()
{
	return (vec2 - vec1) & (vec3 - vec1);
}

Line::Line(Vector vec1, Vector vec2)
	: vec1(vec1), vec2(vec2)
{

}

Vector Line::getV() const
{
	return vec2 - vec1;
}

Vector Line::trace(float t)
{
	return getV() * t + vec1;
}

AxisAlignedFace::AxisAlignedFace(Vector pos, float width, float height)
	: pos(pos), width(width), height(height)
{

}

AxisAlignedXY::AxisAlignedXY(Vector pos, float width, float height)
	: AxisAlignedFace(pos, width, height)
{

}

Vector AxisAlignedXY::getNormal()
{
	return Vector(0.0f, 0.0f, 1.0f);
}

bool AxisAlignedXY::inside(Vector vec)
{
	return vec.x >= pos.x & vec.x <= pos.x + width & vec.y >= pos.y & vec.y <= pos.y + height;
}

AxisAlignedXZ::AxisAlignedXZ(Vector pos, float width, float height)
	: AxisAlignedFace(pos, width, height)
{

}

Vector AxisAlignedXZ::getNormal()
{
	return Vector(0.0f, 1.0f, 0.0f);
}

bool AxisAlignedXZ::inside(Vector vec)
{
	return vec.x >= pos.x & vec.x <= pos.x + width & vec.z >= pos.z & vec.z <= pos.z + height;
}

AxisAlignedYZ::AxisAlignedYZ(Vector pos, float width, float height)
	: AxisAlignedFace(pos, width, height)
{

}

Vector AxisAlignedYZ::getNormal()
{
	return Vector(1.0f, 0.0f, 0.0f);
}

bool AxisAlignedYZ::inside(Vector vec)
{
	return vec.y >= pos.y & vec.y <= pos.y + width & vec.z >= pos.z & vec.z <= pos.z + height;
}

AxisAlignedCube::AxisAlignedCube(Vector pos, Vector size)
	: pos(pos), size(size)
{

}

AxisAlignedYZ AxisAlignedCube::getLeft()
{
	return AxisAlignedYZ(pos, size.z, size.y);
}

AxisAlignedYZ AxisAlignedCube::getRight()
{
	return AxisAlignedYZ(Vector(pos.x + size.x, pos.y, pos.z), size.z, size.y);
}

AxisAlignedXZ AxisAlignedCube::getBottom()
{
	return AxisAlignedXZ(pos, size.x, size.z);
}

AxisAlignedXZ AxisAlignedCube::getTop()
{
	return AxisAlignedXZ(Vector(pos.x, pos.y + size.y, pos.z), size.x, size.z);
}

AxisAlignedXY AxisAlignedCube::getFront()
{
	return AxisAlignedXY(pos, size.x, size.y);
}

AxisAlignedXY AxisAlignedCube::getBack()
{
	return AxisAlignedXY(Vector(pos.x, pos.y, pos.z + size.z), size.x, size.y);
}

bool AxisAlignedCube::inside(Vector vec)
{
	return vec.x >= pos.x & vec.x <= pos.x + size.x & vec.y >= pos.y & vec.y <= pos.y + size.y & vec.z >= pos.z & vec.z <= pos.z + size.z;
}