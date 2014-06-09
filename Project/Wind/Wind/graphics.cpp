#define _USE_MATH_DEFINES

#include "graphics.h"
#include "ioutil.h"
#include <vector>
#include "window.h"
#include <string>
#include <math.h>
#include "lodepng.h"
#include "threads.h"

using namespace gfxu;
using namespace GameStates;

int vs = 0;

Vertex::Vertex(float x, float y, float z)
	: x(x), y(y), z(z)
{}
Vertex::Vertex(const Vertex& v)
	: x(v.x), y(v.y), z(v.z)
{}
Vertex::Vertex(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z)
{}
Vertex::Vertex(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z)
{}
Vertex::Vertex(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z)
{}

VertexUV::VertexUV(float x, float y, float z, float u, float v)
	: x(x), y(y), z(z), u(u), v(v)
{}
VertexUV::VertexUV(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), u(0.0f), v(0.0f)
{}
VertexUV::VertexUV(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), u(v.u), v(v.v)
{}
VertexUV::VertexUV(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), u(0.0f), v(0.0f)
{}
VertexUV::VertexUV(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), u(v.u), v(v.v)
{}

VertexRGBA::VertexRGBA(float x, float y, float z, float r, float g, float b, float a)
	: x(x), y(y), z(z), r(r * 255.0f), g(g * 255.0f), b(b * 255.0f), a(a * 255.0f)
{}
VertexRGBA::VertexRGBA(float x, float y, float z, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	: x(x), y(y), z(z), r(r), g(g), b(b), a(a)
{}
VertexRGBA::VertexRGBA(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), r(255), g(255), b(255), a(255)
{}
VertexRGBA::VertexRGBA(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), r(255), g(255), b(255), a(255)
{}
VertexRGBA::VertexRGBA(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), r(v.r), g(v.g), b(v.b), a(v.a)
{}
VertexRGBA::VertexRGBA(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), r(v.r), g(v.g), b(v.b), a(v.a)
{}

VertexUVRGBA::VertexUVRGBA(float x, float y, float z, float u, float v, float r, float g, float b, float a)
	: x(x), y(y), z(z), u(u), v(v), r(r * 255.0f), g(g * 255.0f), b(b * 255.0f), a(a * 255.0f)
{}
VertexUVRGBA::VertexUVRGBA(float x, float y, float z, float u, float v, unsigned char r, unsigned char g, unsigned char b, unsigned char a)
	: x(x), y(y), z(z), u(u), v(v), r(r), g(g), b(b), a(a)
{}
VertexUVRGBA::VertexUVRGBA(const Vertex& v)
	: x(v.x), y(v.y), z(v.z), u(0.0f), v(0.0f), r(255), g(255), b(255), a(255)
{}
VertexUVRGBA::VertexUVRGBA(const VertexUV& v)
	: x(v.x), y(v.y), z(v.z), u(v.u), v(v.v), r(255), g(255), b(255), a(255)
{}
VertexUVRGBA::VertexUVRGBA(const VertexRGBA& v)
	: x(v.x), y(v.y), z(v.z), u(0.0f), v(0.0f), r(v.r), g(v.g), b(v.b), a(v.a)
{}
VertexUVRGBA::VertexUVRGBA(const VertexUVRGBA& v)
	: x(v.x), y(v.y), z(v.z), u(v.u), v(v.v), r(v.r), g(v.g), b(v.b), a(v.a)
{}



VertexStream::VertexStream()
	: vertices(vector<VertexUVRGBA>()), vertex(), ready(false), uploaded(false), released(false)
{

}
VertexStream::VertexStream(int size)
	: vertices(vector<VertexUVRGBA>()), vertex(), ready(false), uploaded(false), released(false)
{
	vertices.reserve(size);
}
VertexStream::~VertexStream()
{
	if(isUploaded())
	{
		renderThread.taskList.put(Tasks::TaskVBOVAOSet(vbo, vao));
	}
}
void VertexStream::put(float x, float y, float z)
{
	vertex.x = x + translation.x;
	vertex.y = y + translation.y;
	vertex.z = z + translation.z;

	vertices.push_back(vertex);

	vs++;
}
void VertexStream::put(float x, float y, float z, float u, float v)
{
	setUV(u, v);
	put(x, y, z);
}
void VertexStream::put(const Vertex& v)
{
	put(v.x, v.y, v.z);
}
void VertexStream::put(const VertexUV& v)
{
	setUV(v.u, v.v);
	put(v.x, v.y, v.z);
}
void VertexStream::put(const VertexRGBA& v)
{
	setColor(v.r, v.g, v.b, v.a);
	put(v.x, v.y, v.z);
}
void VertexStream::put(const VertexUVRGBA& v)
{
	setUV(v.u, v.v);
	setColor(v.r, v.g, v.b, v.a);
	put(v.x, v.y, v.z);
}
template <typename T>
void VertexStream::put(const T* vp, int length)
{
	for(int i = 0; i < length; i++)
	{
		put(vp[i]);
	}
}
void VertexStream::release()
{
	released = true;
	vertices.clear();
}
void VertexStream::setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a)
{
	vertex.r = r;
	vertex.g = g;
	vertex.b = b;
	vertex.a = a;
}
void VertexStream::setUV(float u, float v)
{
	if(icon != nullptr)
	{
		vertex.u = icon->u(u);
		vertex.v = icon->v(v);
	}
	else
	{
		vertex.u = u;
		vertex.v = v;
	}
}

void VertexStream::setTranslation(float x, float y, float z)
{
	translation = Vertex(x, y, z);
}
void VertexStream::setTranslation(Vertex vertex)
{
	translation = vertex;
}

void VertexStream::setIcon(TiledTexture::Icon* icon)
{
	this->icon = icon;
}

void VertexStream::unbindIcon()
{
	setIcon(nullptr);
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
size_t VertexStream::length()
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

		getError("Vertex stream upload error");

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
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexUVRGBA), (GLvoid*)offsetof(VertexUVRGBA, u));
		//glVertexAttribPointer(2, 4, GL_UNSIGNED_SHORT, GL_TRUE, sizeof(VertexUVRGBA), (GLvoid*)offsetof(VertexUVRGBA, r));
		glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(VertexUVRGBA), (GLvoid*)offsetof(VertexUVRGBA, r));

		if(getError("Vertex stream upload error"))
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

void VertexStream::compress()
{
	vertices.shrink_to_fit();
}

bool VertexStream::isUploaded()
{
	return uploaded;
}

bool VertexStream::isReleased()
{
	return released;
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
	file.close();
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
		glGetShaderiv(object, GL_INFO_LOG_LENGTH, &len);
		if(len > 0)
		{
			GLchar* str = new GLchar[len];
			glGetShaderInfoLog(object, len, nullptr, str);
			GLWindow::instance->postError(str, "Shader compliation error");

			delete[] str;
		}
		else
		{
			GLWindow::instance->postError("The shader had a compilation error, but returned no log.", "Shader compliation error");
		}

		return false;
	}
	
	compiled = true;
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
	if(vShader || gShader || fShader)
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

		getError("Unable to attach shaders.");

		glLinkProgram(object);
		GLint linkStatus;

		glGetObjectParameterivARB(object, GL_OBJECT_LINK_STATUS_ARB, &linkStatus);

		if((linkStatus == GL_FALSE) & (GLWindow::instance != NULL))
		{
			GLint len;
			glGetProgramiv(object, GL_INFO_LOG_LENGTH, &len);
			if(len > 0)
			{
				GLchar* str = new GLchar[len];
				glGetProgramInfoLog(object, len, nullptr, str);
				GLWindow::instance->postError(str, "Linking error");
				delete[] str;
			}
			else
			{
				GLWindow::instance->postError("The program has a linking error, but returned no log.", "Linking error");
			}

			GlobalThread::stop = true;
			return okay;
		}
		else
		{
			glUseProgram(object);
			modelview = glGetUniformLocation(object, "modelview");
			projection = glGetUniformLocation(object, "projection");
			const_color = glGetUniformLocation(object, "const_color");
			cam_pos = glGetUniformLocation(object, "cam_pos");
			fog_color = glGetUniformLocation(object, "fog_color");
			fog_dist = glGetUniformLocation(object, "fog_dist");
			texture1 = glGetUniformLocation(object, "texture_1");
		
			if(texture1 >= 0)
			{
				glUniform1i(texture1, 0);
			}

			okay = true;
			return okay;
		}
	}

	return false;
}

void ShaderProgram::bind()
{
	glUseProgram(object);
	gfxu::getError();
	current = this;

	Uniforms::setForceUpload();
}

ShaderProgram* ShaderProgram::current;



MatrixStack::MatrixStack()
{
	clear();
}

geom::Matrix MatrixStack::unstack()
{
	std::stack<geom::Matrix> stack2 = stack;
	geom::Matrix mat = geom::Matrix::identity();
	while(!stack2.empty())
	{
		mat = stack2.top() * mat;
		stack2.pop();
	}

	return mat;
}

void MatrixStack::push(geom::Matrix mat)
{
	stack.push(mat);
	topmost = topmost * mat;
	changed = true;
}

void MatrixStack::mult(geom::Matrix mat)
{
	stack.top() = stack.top() * mat;
	topmost = topmost * mat;
	changed = true;
}

void MatrixStack::identity()
{
	stack.top() = geom::Matrix::identity();
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
	stack.push(geom::Matrix::identity());
	topmost = geom::Matrix::identity();
	changed = true;
}

const geom::Matrix MatrixStack::getTopmost()
{
	return topmost;
}



void Uniforms::setColor(float r, float g, float b, float a)
{
	color.set(geom::Vector(r, g, b, a));
}

void Uniforms::setFogColor(float r, float g, float b, float a)
{
	fogColor.set(geom::Vector(r, g, b, a));
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

	if(forceUpload | (camPos.changed & (ShaderProgram::current->cam_pos >= 0)))
	{
		geom::Vector vec = camPos.get();
		glUniform4f(ShaderProgram::current->cam_pos, vec.x, vec.y, vec.z, vec.w);
	}

	if(forceUpload | (fogColor.changed & (ShaderProgram::current->fog_color >= 0)))
	{
		geom::Vector vec = fogColor.get();
		glUniform4f(ShaderProgram::current->fog_color, vec.x, vec.y, vec.z, vec.w);
	}

	if(forceUpload | (fogDist.changed & (ShaderProgram::current->fog_dist >= 0)))
	{
		glUniform1f(ShaderProgram::current->fog_dist, fogDist.get());
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
Uniform<geom::Vector> Uniforms::color;
Uniform<geom::Vector> Uniforms::camPos;
Uniform<geom::Vector> Uniforms::fogColor;
Uniform<float> Uniforms::fogDist;



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
	data = IOUtil::readPNG(fileName, width, height);
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

	uploaded = true;

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

TiledTexture::TiledTexture(TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped)
	: Texture2D(1, 1, magFilter, minFilter, wrap, mipmapped)
{

}

TiledTexture::~TiledTexture()
{

}

TiledTexture::Icon* TiledTexture::getIcon(wstring file)
{
	auto iconIter = icons.find(file);

	if(iconIter == icons.end())
	{
		Icon* icon = new TiledTexture::Icon(this, file);

		icons[file] = icon;
		return icon;
	}
	else
	{
		return iconIter->second;
	}
}

bool compareIcon(const TiledTexture::Icon* first, const TiledTexture::Icon* second)
{
	return first->size > second->size;
}

inline unsigned int coord_decode(unsigned int coord)
{
	unsigned int res = 0;
	for(unsigned int i = 0; i < 16; i++)
	{
		res |= (coord & 1) << i;
		coord >>= 2;
	}

	return res;
}

bool TiledTexture::compile()
{
	vector<TiledTexture::Icon*> sortedIcons;

	for(auto iter = icons.begin(); iter != icons.end(); ++iter)
	{
		sortedIcons.push_back(iter->second);
	}

	std::sort(sortedIcons.begin(), sortedIcons.end(), compareIcon);

	unsigned int area = 0;
	for(auto iter = sortedIcons.begin(); iter != sortedIcons.end(); ++iter)
	{
		Icon* icon = *iter;

		area += icon->size * icon->size;
	}

	width = 1;
	height = 1;

	while(width * height < area)
	{
		if(width <= height)
		{
			width *= 2;
		}
		else
		{
			height *= 2;
		}
	}

	data = new unsigned char[width * height * 4];

	for(unsigned int i = 0; i < width * height * 4; i++)
	{
		data[i] = 0;
	}

	int index = 0;

	for(auto iter = sortedIcons.begin(); iter != sortedIcons.end(); ++iter)
	{
		Icon* icon = *iter;

		icon->x = coord_decode(index);
		icon->y = coord_decode(index >> 1);

		index += icon->size * icon->size;

		for(unsigned int x = 0; x < icon->size; x++)
		{
			for(unsigned int y = 0; y < icon->size; y++)
			{
				unsigned int iconIndex = (x + (icon->size - y - 1) * icon->size) * 4;
				unsigned int textureIndex = (x + icon->x + (height - y - icon->y - 1) * width) * 4;

				data[textureIndex] = icon->data[iconIndex];
				data[textureIndex + 1] = icon->data[iconIndex + 1];
				data[textureIndex + 2] = icon->data[iconIndex + 2];
				data[textureIndex + 3] = icon->data[iconIndex + 3];
			}
		}
	}

	return true;
}

TiledTexture::Icon::Icon(TiledTexture* texture, wstring file)
	: texture(texture), file(file), x(0), y(0)
{
	unsigned int width;
	unsigned int height;

	data = IOUtil::readPNG(file, width, height);

	if(((width & (width - 1)) != 0) | ((height & (height - 1)) != 0) | (width != height))
	{
		GLWindow::instance->postError("Invalid icon proportions", "Icon load error");
	}
	else
	{
		size = width;
	}
}

TiledTexture::Icon::~Icon()
{
	delete[] data;
}

float TiledTexture::Icon::u(float f)
{
	return (x + f * size) / texture->width;
}

float TiledTexture::Icon::v(float f)
{
	return (texture->height - y - size + f * size) / texture->height;
}



bool gfxu::getError(const char* caption)
{
	int error = glGetError();
	if(error != GL_NO_ERROR)
	{
		const char* errorString = (const char*)gluErrorString(error);
		GLWindow::instance->postError(errorString, caption);
		
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