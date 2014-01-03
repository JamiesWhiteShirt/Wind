#define _USE_MATH_DEFINES

#include "graphics.h"
#include "ioutil.h"
#include <vector>
#include "window.h"
#include <string>
#include <math.h>
#include "lodepng.h"

using namespace gfxu;
using namespace GameStates;

Vertex::Vertex(float x, float y, float z, float w)
	: x(x), y(y), z(z), w(w)
{}
Vertex::Vertex(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{}
Vertex::Vertex(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{}
Vertex::Vertex(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{}
Vertex::Vertex(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w)
{}
Vertex Vertex::operator*(const Matrix& mat)
{
	Vertex result;

	result.x = x * mat.data[0] + y * mat.data[1] + z * mat.data[2] + w * mat.data[3];
	result.y = x * mat.data[4] + y * mat.data[5] + z * mat.data[6] + w * mat.data[7];
	result.z = x * mat.data[8] + y * mat.data[9] + z * mat.data[10] + w * mat.data[11];
	result.w = x * mat.data[12] + y * mat.data[13] + z * mat.data[14] + w * mat.data[15];

	return result;
}

VertexUV::VertexUV(float x, float y, float z, float w, float u, float v)
	: x(x), y(y), z(z), w(w), u(u), v(v)
{}
VertexUV::VertexUV(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(0.0f), v(0.0f)
{}
VertexUV::VertexUV(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(v.u), v(v.v)
{}
VertexUV::VertexUV(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(0.0f), v(0.0f)
{}
VertexUV::VertexUV(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(v.u), v(v.v)
{}

VertexRGBA::VertexRGBA(float x, float y, float z, float w, float r, float g, float b, float a)
	: x(x), y(y), z(z), w(w), r(r), g(g), b(b), a(a)
{}
VertexRGBA::VertexRGBA(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}
VertexRGBA::VertexRGBA(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}
VertexRGBA::VertexRGBA(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), r(v.r), g(v.g), b(v.b), a(v.a)
{}
VertexRGBA::VertexRGBA(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), r(v.r), g(v.g), b(v.b), a(v.a)
{}

VertexUVRGBA::VertexUVRGBA(float x, float y, float z, float w, float u, float v, float r, float g, float b, float a)
	: x(x), y(y), z(z), w(w), u(u), v(v), r(r), g(g), b(b), a(a)
{}
VertexUVRGBA::VertexUVRGBA(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(0.0f), v(0.0f), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}
VertexUVRGBA::VertexUVRGBA(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(v.u), v(v.v), r(1.0f), g(1.0f), b(1.0f), a(1.0f)
{}
VertexUVRGBA::VertexUVRGBA(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(0.0f), v(0.0f), r(v.r), g(v.g), b(v.b), a(v.a)
{}
VertexUVRGBA::VertexUVRGBA(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), w(v.w), u(v.u), v(v.v), r(v.r), g(v.g), b(v.b), a(v.a)
{}



VertexStream::VertexStream()
	: vertices(vector<VertexUVRGBA>()), vertex(), ready(false), uploaded(false)
{

}
VertexStream::VertexStream(int size)
	: vertices(vector<VertexUVRGBA>()), vertex(), ready(false), uploaded(false)
{
	vertices.reserve(size);
}
VertexStream::~VertexStream()
{
	if(isUploaded())
	{
		glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
	}
}
void VertexStream::put(float x, float y, float z, float w)
{
	vertex.x = x;
	vertex.y = y;
	vertex.z = z;

	vertices.push_back(vertex);
}
void VertexStream::put(float x, float y, float z, float w, float u, float v)
{
	setUV(u, v);
	put(x, y, z, w);
}
void VertexStream::put(const Vertex& v)
{
	put(v.x, v.y, v.z, v.w);
}
void VertexStream::put(const VertexUV& v)
{
	setUV(v.u, v.v);
	put(v.x, v.y, v.z, v.w);
}
void VertexStream::put(const VertexRGBA& v)
{
	setColor(v.r, v.g, v.b);
	put(v.x, v.y, v.z, v.w);
}
void VertexStream::put(const VertexUVRGBA& v)
{
	setUV(v.u, v.v);
	setColor(v.r, v.g, v.b);
	put(v.x, v.y, v.z, v.w);
}
template <typename T>
void VertexStream::put(const T* vp, int length)
{
	for(int i = 0; i < length; i++)
	{
		put(vp[i]);
	}
}
void VertexStream::clear()
{
	vertices.clear();
	if(ready)
	{
		if(isUploaded())
		{
			glDeleteBuffers(1, &vbo);
			glGenBuffers(1, &vbo);
			glBindBuffer(GL_ARRAY_BUFFER, vbo);
			glEnableVertexAttribArray(0);
			glEnableVertexAttribArray(1);
			glEnableVertexAttribArray(2);
			glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)(sizeof(float) * 4));
			glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)(sizeof(float) * 6));
		}

		uploaded = false;
	}
}
void VertexStream::setColor(float r, float g, float b)
{
	vertex.r = r;
	vertex.g = g;
	vertex.b = b;
}
void VertexStream::setUV(float u, float v)
{
	vertex.u = u;
	vertex.v = v;
}

void VertexStream::reserveAdditional(int size)
{
	vertices.reserve(vertices.size() + size);
}

void VertexStream::lock()
{
	mutex.lock();
}

void VertexStream::unlock()
{
	mutex.unlock();
}


VertexUVRGBA* VertexStream::ptr()
{
	return &vertices[0];
}
int VertexStream::length()
{
	return vertices.size();
}

bool VertexStream::upload()
{
	if(!ready)
	{
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		ready = true;
	}

	if(!isUploaded())
	{
		lock();

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(VertexUVRGBA), (const void*)&vertices[0], GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)(sizeof(float) * 4));
		glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)(sizeof(float) * 6));

		if(glGetError() != GL_NO_ERROR)
		{
			unlock();
			return false;
		}
		else
		{
			uploaded = true;
		}

		unlock();
	}

	return true;
}

void VertexStream::draw(GLenum mode)
{
	if(vertices.size() > 0 && upload())
	{
		Uniforms::uploadChanges();
		glBindVertexArray(vao);
		glDrawArrays(mode, 0, vertices.size());
	}
}

bool VertexStream::isUploaded()
{
	return uploaded;
}



bool Shader::_compile(GLuint type)
{
	if(compiled)
	{
		return true;
	}

	std::ifstream file(fileName);

	if(!file.good())
	{
		file.close();
		return false;
	}

	std::string str = std::string((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	const char* strPtr = str.c_str();
	const int strLen = str.length();

	if(!strLen)
	{
		return false;
	}

	object = glCreateShader(type);

	glShaderSource(object, 1, &strPtr, &strLen);
	glCompileShader(object);

	GLint compileStatus;

	glGetObjectParameterivARB(object, GL_OBJECT_COMPILE_STATUS_ARB, &compileStatus);

	if((compileStatus == GL_FALSE) & (GLWindow::instance != NULL))
	{
		GLint len;
		GLchar* str = new GLchar[1024];
		wstring title = L"Shader error in " + fileName;
		glGetShaderInfoLog(object, 1024, &len, str);
		if(len > 0)
		{
			wchar_t* wideStr = new wchar_t[len + 1];
			mbstowcs_s(NULL, wideStr, len + 1, str, len);
			GLWindow::instance->messageBox(wideStr, title.c_str(), MB_OK | MB_ICONERROR);

			delete[] wideStr;
		} else
		{
			GLWindow::instance->messageBox(L"The shader had a compilation error but returned no log.", title.c_str(), MB_OK | MB_ICONERROR);
		}

		delete[] str;
		file.close();
		return false;
	}
	
	compiled = true;
	file.close();
	return compiled;
}

Shader::Shader(wstring fileName)
	: fileName(fileName), compiled(false)
{
	
}
Shader::~Shader()
{
	if(compiled)
	{
		glDeleteShader(object);
	}
}

VertexShader::VertexShader(wstring fileName)
	: Shader(fileName)
{

}

bool VertexShader::compile()
{
	return _compile(GL_VERTEX_SHADER);
}

GeometryShader::GeometryShader(wstring fileName)
	: Shader(fileName)
{

}

bool GeometryShader::compile()
{
	return _compile(GL_GEOMETRY_SHADER);
}

FragmentShader::FragmentShader(wstring fileName)
	: Shader(fileName)
{

}

bool FragmentShader::compile()
{
	return _compile(GL_FRAGMENT_SHADER);
}


ShaderProgram::ShaderProgram(VertexShader* vShader, GeometryShader* gShader, FragmentShader* fShader)
	: vShader(vShader), gShader(gShader), fShader(fShader), okay(false)
{
	create();
}

ShaderProgram::~ShaderProgram()
{
	glUseProgram(0);
	glDeleteProgram(object);
}

bool ShaderProgram::create()
{
	object = glCreateProgramObjectARB();

	if(vShader)
	{
		if(!vShader->compile())
		{
			return okay;
		}
		glAttachShader(object, vShader->object);
	}

	if(gShader)
	{
		if(!gShader->compile())
		{
			return okay;
		}
		glAttachShader(object, gShader->object);
	}

	if(fShader)
	{
		if(!fShader->compile())
		{
			return okay;
		}
		glAttachShader(object, fShader->object);
	}

	glLinkProgram(object);
	GLint linkStatus;

	glGetObjectParameterivARB(object, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

	if((linkStatus == GL_FALSE) & (GLWindow::instance != NULL))
	{
		GLint len;
		GLchar* str = new GLchar[1024];
		wstring title = L"Linking error";
		glGetProgramInfoLog(object, 1024, &len, str);
		if(len > 0)
		{
			wchar_t* wideStr = new wchar_t[len + 1];
			mbstowcs_s(NULL, wideStr, len + 1, str, len);
			GLWindow::instance->messageBox(wideStr, title.c_str(), MB_OK | MB_ICONERROR);

			delete[] wideStr;
		} else
		{
			GLWindow::instance->messageBox(L"The program had a linking error but returned no log.", title.c_str(), MB_OK | MB_ICONERROR);
		}

		delete[] str;

		return okay;
	}
	else
	{
		glUseProgram(object);
		modelview = glGetUniformLocation(object, "modelview");
		projection = glGetUniformLocation(object, "projection");
		const_color = glGetUniformLocation(object, "const_color");
		texture1 = glGetUniformLocation(object, "texture_1");
		
		if(texture1 >= 0)
		{
			glUniform1i(texture1, 0);
		}

		okay = true;
		return okay;
	}
}

void ShaderProgram::bind()
{
	glUseProgram(object);
	current = this;

	Uniforms::setForceUpload();
}

ShaderProgram* ShaderProgram::current;



int Matrix::index(int x, int y)
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

float& Matrix::operator[](int index)
{
	return data[index];
}

Matrix Matrix::operator*(const Matrix& mat)
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

Matrix Matrix::operator*(const float& scalar)
{
	Matrix result;

	for(int i = 0; i < 16; i++)
	{
		result[i] = data[i] * scalar;
	}

	return result;
}

Vertex Matrix::operator*(const Vertex& vert)
{
	Vertex result;

	result.x = vert.x * data[0] + vert.y * data[1] + vert.z * data[2] + vert.w * data[3];
	result.y = vert.x * data[4] + vert.y * data[5] + vert.z * data[6] + vert.w * data[7];
	result.z = vert.x * data[8] + vert.y * data[9] + vert.z * data[10] + vert.w * data[11];
	result.w = vert.x * data[12] + vert.y * data[13] + vert.z * data[14] + vert.w * data[15];

	return result;
}

Matrix& Matrix::operator=(const Matrix& mat)
{
	memcpy(data, mat.data, 16 * sizeof(float));

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



MatrixStack::MatrixStack()
{
	clear();
}

Matrix MatrixStack::unstack()
{
	std::stack<Matrix> stack2 = stack;
	Matrix mat = Matrix::identity();
	while(!stack2.empty())
	{
		mat = stack2.top() * mat;
		stack2.pop();
	}

	return mat;
}

void MatrixStack::push(Matrix mat)
{
	stack.push(mat);
	topmost = topmost * mat;
	changed = true;
}

void MatrixStack::mult(Matrix mat)
{
	stack.top() = stack.top() * mat;
	topmost = topmost * mat;
	changed = true;
}

void MatrixStack::identity()
{
	stack.top() = Matrix::identity();
	topmost = unstack();
	changed = true;
}

void MatrixStack::pop()
{
	stack.pop();
	topmost = unstack();
	changed = true;
}

void MatrixStack::clear()
{
	while(!stack.empty())
	{
		stack.pop();
	}
	stack.push(Matrix::identity());
	topmost = Matrix::identity();
	changed = true;
}

const Matrix MatrixStack::getTopmost()
{
	return topmost;
}



VecUniform::VecUniform()
	: changed(true)
{

}

void VecUniform::set(geom::Vector vec)
{
	this->vec = vec;
	changed = true;
}

void VecUniform::set(float f1, float f2, float f3, float f4)
{
	vec = geom::Vector(f1, f2, f3, f4);
	changed = true;
}

geom::Vector VecUniform::get()
{
	return vec;
}



void Uniforms::uploadChanges()
{
	if(forceUpload | (MMS.changed & (ShaderProgram::current->modelview >= 0)))
	{
		glUniformMatrix4fv(ShaderProgram::current->modelview, 1, false, MMS.getTopmost().data);
		MMS.changed = false;
	}

	if(forceUpload | (PMS.changed & (ShaderProgram::current->projection >= 0)))
	{
		glUniformMatrix4fv(ShaderProgram::current->projection, 1, false, PMS.getTopmost().data);
		PMS.changed = false;
	}

	if(forceUpload | (color.changed & (ShaderProgram::current->const_color >= 0)))
	{
		geom::Vector vec = color.get();
		glUniform4f(ShaderProgram::current->const_color, vec.x, vec.y, vec.z, vec.w);
	}
}

void Uniforms::reset()
{
	MMS.clear();
	PMS.clear();
}

void Uniforms::setForceUpload()
{
	MMS.changed = PMS.changed = true;
}

bool Uniforms::forceUpload = false;
MatrixStack Uniforms::MMS;
MatrixStack Uniforms::PMS;
VecUniform Uniforms::color;



Texture2D::Texture2D()
	: data(nullptr), width(0), height(0), object(0), magFilter(GL_LINEAR), minFilter(GL_LINEAR), wrap(GL_REPEAT), mipmapped(false), uploaded(false)
{

}

Texture2D::Texture2D(unsigned int width, unsigned int height, TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped)
	: width(width), height(height), data(new unsigned char[width * height * 4]), object(0), magFilter(magFilter), minFilter(minFilter), wrap(wrap), mipmapped(mipmapped), uploaded(false)
{

}

Texture2D::Texture2D(unsigned int width, unsigned int height, Noise::NoiseGenerator2D* r, Noise::NoiseGenerator2D* g, Noise::NoiseGenerator2D* b, Noise::NoiseGenerator2D* a)
	: width(width), height(height), data(new unsigned char[width * height* 4]), object(0), magFilter(GL_LINEAR), minFilter(GL_LINEAR), wrap(GL_CLAMP_TO_EDGE), mipmapped(false), uploaded(false)
{
	for(unsigned int i = 0; i < width; i++)
	{
		for(unsigned int j = 0; j < height; j++)
		{
			data[(i + j * width) * 4] = (unsigned char)((r == nullptr ? 1.0f : r->getNoise(i, j)) * 255.0f);
			data[(i + j * width) * 4 + 1] = (unsigned char)((g == nullptr ? 1.0f : g->getNoise(i, j)) * 255.0f);
			data[(i + j * width) * 4 + 2] = (unsigned char)((b == nullptr ? 1.0f : b->getNoise(i, j)) * 255.0f);
			data[(i + j * width) * 4 + 3] = (unsigned char)((a == nullptr ? 1.0f : a->getNoise(i, j)) * 255.0f);
		}
	}
}

Texture2D::Texture2D(wstring fileName)
	: object(0), magFilter(GL_LINEAR), minFilter(GL_LINEAR), wrap(GL_REPEAT), mipmapped(false), uploaded(false)
{
	init(fileName);
}

Texture2D::Texture2D(wstring fileName, TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped)
	: object(0), magFilter(magFilter), minFilter(minFilter), wrap(wrap), mipmapped(mipmapped), uploaded(false)
{
	init(fileName);
}

void Texture2D::init(wstring fileName)
{
	char* nFileName = new char[fileName.length() + 1];
	std::wcstombs(nFileName, fileName.c_str(), sizeof(char) * (fileName.length() + 1));
	std::vector<unsigned char> image;
	if(!lodepng::decode(image, width, height, nFileName))
	{
		data = new unsigned char[width * height * 4];

		for(unsigned int i = 0; i < height; i++)
		{
			memcpy(&data[i * width * 4], &image[(height - 1 - i) * width * 4], sizeof(unsigned char) * width * 4);
		}
	}
}

Texture2D::~Texture2D()
{
	if(data != nullptr)
	{
		delete[] data;
	}

	if(uploaded)
	{
		glDeleteTextures(1, &object);
	}
}

bool Texture2D::upload()
{
	glGenTextures(1, &object);
	glBindTexture(GL_TEXTURE_2D, object);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	if(mipmapped)
	{
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	return true;
}

void Texture2D::bind()
{
	if(!uploaded)
	{
		if(!upload())
		{
			return;
		}
	}
	else
	{
		glBindTexture(GL_TEXTURE_2D, object);
	}
}



bool gfxu::getError()
{
	int error = glGetError();
	if(error != GL_NO_ERROR)
	{
		return true;
	} else
	{
		return false;
	}
}

VertexStream& gfxu::operator<<(VertexStream& vStream, const Vertex& v)
{
	vStream.put(v);
	return vStream;
}
VertexStream& gfxu::operator<<(VertexStream& vStream, const VertexUV& v)
{
	vStream.put(v);
	return vStream;
}
VertexStream& gfxu::operator<<(VertexStream& vStream, const VertexRGBA& v)
{
	vStream.put(v);
	return vStream;
}
VertexStream& gfxu::operator<<(VertexStream& vStream, const VertexUVRGBA& v)
{
	vStream.put(v);
	return vStream;
}
VertexStream& gfxu::operator<<(VertexStream& vStream, const VertexStream& vStream_in)
{
	vStream.vertices.insert(vStream.vertices.end(), vStream_in.vertices.begin(), vStream_in.vertices.end());

	return vStream;
}