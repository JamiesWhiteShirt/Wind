#pragma once
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")
#define GLEW_STATIC

#include "glew.h"
#include <gl/GL.h>
#include <gl/GLU.h>
#include <vector>
#include <stack>
#include "noise.h"
#include "memutil.h"
#include <mutex>
#include "geometry.h"
#include <map>

#define MODELVIEW_MATRIX 0x0
#define PROJECTION_MATRIX 0x1

typedef GLuint SHADER_OBJECT;
typedef GLhandleARB SHADER_PROGRAM_OBJECT;
typedef GLint UNIFORM_LOCATION;
typedef GLuint TEXTURE_OBJECT;
typedef GLenum TEXTURE_PARAMETER;

using namespace std;

namespace gfxu
{
	class Vertex;
	class VertexUV;
	class VertexRGBA;
	class VertexUVRGBA;
	class Matrix;

	class Vertex
	{
	private:
	public:
		float x;
		float y;
		float z;
		Vertex(float x = 0.0f, float y = 0.0f, float z = 0.0f);
		Vertex(const Vertex& v);
		Vertex(const VertexUV& v);
		Vertex(const VertexRGBA& v);
		Vertex(const VertexUVRGBA& v);
	};

	class VertexUV
	{
	private:
	public:
		float x;
		float y;
		float z;
		float u;
		float v;
		VertexUV(float x = 0.0f, float y = 0.0f, float z = 0.0f, float u = 0.0f, float v = 0.0f);
		VertexUV(const Vertex& v);
		VertexUV(const VertexUV& v);
		VertexUV(const VertexRGBA& v);
		VertexUV(const VertexUVRGBA& v);
	};

	class VertexRGBA
	{
	private:
	public:
		float x;
		float y;
		float z;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		VertexRGBA(float x = 0.0f, float y = 0.0f, float z = 0.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
		VertexRGBA(float x, float y, float z, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		VertexRGBA(const Vertex& v);
		VertexRGBA(const VertexUV& v);
		VertexRGBA(const VertexRGBA& v);
		VertexRGBA(const VertexUVRGBA& v);
	};

	class VertexUVRGBA
	{
	private:
	public:
		float x;
		float y;
		float z;
		float u;
		float v;
		unsigned char r;
		unsigned char g;
		unsigned char b;
		unsigned char a;
		VertexUVRGBA(float x = 0.0f, float y = 0.0f, float z = 0.0, float u = 0.0f, float v = 0.0f, float r = 1.0f, float g = 1.0f, float b = 1.0f, float a = 1.0f);
		VertexUVRGBA(float x, float y, float z, float u, float v, unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		VertexUVRGBA(const Vertex& v);
		VertexUVRGBA(const VertexUV& v);
		VertexUVRGBA(const VertexRGBA& v);
		VertexUVRGBA(const VertexUVRGBA& v);
	};

	class Texture2D
	{
	private:
		void init(wstring fileName);
	protected:
		TEXTURE_OBJECT object;
		TEXTURE_PARAMETER magFilter;
		TEXTURE_PARAMETER minFilter;
		TEXTURE_PARAMETER wrap;
		bool mipmapped;
		bool uploaded;
		unsigned char* data;
		Texture2D(unsigned int width, unsigned int height, TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped);
	public:
		unsigned int width, height;
		Texture2D();
		Texture2D(unsigned int width, unsigned int height, Noise::NoiseGenerator2D* r, Noise::NoiseGenerator2D* g, Noise::NoiseGenerator2D* b, Noise::NoiseGenerator2D* a);
		Texture2D(wstring fileName);
		Texture2D(wstring fileName, TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped);
		~Texture2D();

		bool upload();
		void bind();
	};

	class TiledTexture : public Texture2D
	{
	public:
		class Icon
		{
		public:
			TiledTexture* texture;

			wstring file;
			unsigned int x;
			unsigned int y;
			unsigned int size;
			unsigned char* data;

			Icon(TiledTexture* texture, wstring file);
			~Icon();

			float u(float f);
			float v(float f);
		};

		TiledTexture(TEXTURE_PARAMETER magFilter, TEXTURE_PARAMETER minFilter, TEXTURE_PARAMETER wrap, bool mipmapped);
		~TiledTexture();

		Icon* getIcon(wstring file);
		bool compile();
	private:
		std::map<wstring, Icon*> icons;
	};

	class VertexStream
	{
	protected:
		vector<VertexUVRGBA> vertices;
		Vertex translation;
		TiledTexture::Icon* icon;
		VertexUVRGBA vertex;
		GLuint vao;
		GLuint vbo;
		bool ready;
		bool uploaded;
		bool released;
		std::mutex mutex;

		friend VertexStream& operator<<(VertexStream&, const VertexStream&);
	public:
		VertexStream();
		VertexStream(int size);
		virtual ~VertexStream();
		void put(float x, float y, float z);
		void put(float x, float y, float z, float u, float v);
		void put(const Vertex& v);
		void put(const VertexUV& v);
		void put(const VertexRGBA& v);
		void put(const VertexUVRGBA& v);
		template <typename T>
		void put(const T* vp, int length);
		void release();
		void setColor(unsigned char r, unsigned char g, unsigned char b, unsigned char a);
		void setUV(float u, float v);
		void setTranslation(float x, float y, float z);
		void setTranslation(Vertex v);
		void setIcon(TiledTexture::Icon* icon);
		void unbindIcon();
		void reserveAdditional(int size);

		void lock();
		void unlock();
		bool upload();
		void draw(GLenum mode = GL_TRIANGLES);
		void compress();
		bool isUploaded();
		bool isReleased();

		VertexUVRGBA* ptr();
		size_t length();
	};

	class Shader
	{
	protected:
		wstring fileName;
		SHADER_OBJECT object;
		bool compiled;
		bool _compile(GLuint type);
	public:
		Shader(wstring fileName);
		~Shader();
		virtual bool compile() = 0;

		friend class ShaderProgram;
	};

	class VertexShader : public Shader
	{
	public:
		VertexShader(wstring fileName = L"");
		virtual bool compile();
	};

	class GeometryShader : public Shader
	{
	public:
		GeometryShader(wstring fileName = L"");
		virtual bool compile();
	};

	class FragmentShader : public Shader
	{
	public:
		FragmentShader(wstring fileName = L"");
		virtual bool compile();
	};

	class ShaderProgram
	{
	private:

		VertexShader* vShader;
		GeometryShader* gShader;
		FragmentShader* fShader;
		SHADER_PROGRAM_OBJECT object;
		bool okay;
		bool create();
	public:
		ShaderProgram(VertexShader* vShader = nullptr, GeometryShader* gShader = nullptr, FragmentShader* fShader = nullptr);
		~ShaderProgram();
		void bind();

		UNIFORM_LOCATION modelview;
		UNIFORM_LOCATION projection;
		UNIFORM_LOCATION const_color;
		UNIFORM_LOCATION cam_pos;
		UNIFORM_LOCATION fog_color;
		UNIFORM_LOCATION fog_dist;
		UNIFORM_LOCATION texture1;

		static ShaderProgram* current;
	};

	class MatrixStack
	{
	private:
		std::stack<geom::Matrix> stack;
		geom::Matrix topmost;
		geom::Matrix unstack();
	public:
		bool changed;

		MatrixStack();
		void push(geom::Matrix mat = geom::Matrix::identity());
		void mult(geom::Matrix mat);
		void identity();
		void pop();
		void clear();
		const geom::Matrix getTopmost();
	};

	template<typename A>
	class Uniform
	{
	private:
		A val;
	public:
		bool changed;

		Uniform()
			:changed(true)
		{

		}
		void set(A value)
		{
			val = value;
			changed = true;
		}
		A get()
		{
			return val;
		}
	};

	class Uniforms
	{
	private:
		static bool forceUpload;
	public:
		static MatrixStack MMS;
		static MatrixStack PMS;
		static Uniform<geom::Vector> color;
		static Uniform<geom::Vector> camPos;
		static Uniform<geom::Vector> fogColor;
		static Uniform<float> fogDist;

		static void setColor(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);
		static void setFogColor(float r = 1.0, float g = 1.0, float b = 1.0, float a = 1.0);
		static void uploadChanges();
		static void reset();

		static void setForceUpload();
	};

	bool getError(const char* message = "OpenGL error");

	VertexStream& operator<<(VertexStream& vStream, const Vertex& v);
	VertexStream& operator<<(VertexStream& vStream, const VertexUV& v);
	VertexStream& operator<<(VertexStream& vStream, const VertexRGBA& v);
	VertexStream& operator<<(VertexStream& vStream, const VertexUVRGBA& v);
	VertexStream& operator<<(VertexStream& vStream, const VertexStream& vStream_in);
}

namespace GameStates
{
	class Camera
	{
	private:

	public:
		geom::Vector pos;
		geom::Vector rot;
		
		Camera();
	};

	class GameState
	{
	private:
	public:
		Camera cam;
		int FOV;
		bool devEnabled;

		GameState();
		~GameState();
	};

	extern std::mutex mut;
	extern bool newPendingAvailable;
	extern GameState* processedState;
	extern GameState* pendingState;
	extern GameState* renderingState;

	void staticInit();
	void cleanup();
	void swapProcessedPending();
	void swapPendingRendering();
}