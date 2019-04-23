#pragma once

// stlib
#include <fstream> // stdout, stderr..
#include <vector>

// glfw
#define NOMINMAX
#include <gl3w.h>
#include <GLFW/glfw3.h>

// Simple utility macros to avoid mistyping directory name, name has to be a string literal
// audio_path("audio.ogg") -> data/audio/audio.ogg
// Get defintion of PROJECT_SOURCE_DIR from:
#include "project_path.hpp"

#define shader_path(name) PROJECT_SOURCE_DIR "./shaders/" name

#define data_path PROJECT_SOURCE_DIR "./data"
#define fonts_path(name)  data_path "/fonts/" name
#define textures_path(name)  data_path "/textures/" name
#define audio_path(name) data_path  "/audio/" name
#define mesh_path(name) data_path  "/meshes/" name
#define level_path(name) data_path "/levels/" name


static float g_tolerance = 0.000001f;

// Not much math is needed and there are already way too many libraries linked (:
// If you want to do some overloads..
struct vec2 { float x, y; };
struct vec3 { float x, y, z; };
struct mat3 { vec3 c0, c1, c2; };

// Utility functions
float dot(vec2 l, vec2 r);
float dot(vec3 l, vec3 r);
mat3  mul(const mat3& l, const mat3& r);
vec2  normalize(vec2 v);
vec2  add(vec2 v1, vec2 v2);
vec2  subtract(vec2 v1, vec2 v2);
vec2  negateVec(vec2 v);
float vecLength(vec2 v);
vec2 rotateVec(vec2 v, float rotation);
vec2 scalarMultiply(vec2 v, float s);
std::pair<float, float> vec2ToPair(vec2 v);

std::string path(std::string name);


//implemented according to Unit interval(0,1) https://en.wikipedia.org/wiki/Cubic_Hermite_spline
float hermiteSplineVal(float startPos, float endPos, float startSlope, float endSlope, float intervalPos);

// OpenGL utilities
// cleans error buffer
void gl_flush_errors();
bool gl_has_errors();

// Single Vertex Buffer element for non-textured meshes (colored.vs.glsl)
struct Vertex
{
	vec3 position;
	vec3 color;
};

// Single Vertex Buffer element for textured sprites (textured.vs.glsl)
struct TexturedVertex
{
	vec3 position;
	vec2 texcoord;
};

// Texture wrapper
struct Texture
{
	Texture();
	~Texture();

	GLuint id;
	GLuint depth_render_buffer_id;
	int width;
	int height;
	
	// Loads texture from file specified by path
	bool load_from_file(const char* path);
	// Screen texture
	bool create_from_screen(GLFWwindow const * const window);
	bool is_valid()const; // True if texture is valid
};

// A Mesh is a collection of a VertexBuffer and an IndexBuffer. A VAO
// represents a Vertex Array Object and is the container for 1 or more Vertex Buffers and 
// an Index Buffer
struct Mesh
{
	GLuint vao;
	GLuint vbo;
	GLuint ibo;
};

// Container for Vertex and Fragment shader, which are then put(linked) together in a
// single program that is then bound to the pipeline.
struct Effect
{
	bool load_from_file(const char* vs_path, const char* fs_path);
	void release();

	GLuint vertex;
	GLuint fragment;
	GLuint program;
};

// Helper container for all the information we need when rendering an object together
// with its transform.
struct Renderable
{
	Mesh mesh;
	Effect effect;
	mat3 transform;
	vec2 m_position;	// Window coordinates
	vec2 m_scale;		// 1.f is as big as the associated texture
	float m_rotation;	// in radians
	std::vector<vec2> vertex_coords;
	std::vector<vec3> local_vertex_coords;
	float color[3] = { 1.f, 1.f, 1.f };

	// projection contains the orthographic projection matrix. As every Renderable::draw()
	// renders itself it needs it to correctly bind it to its shader.
	virtual void draw(const mat3& projection) = 0;

	// sets the vertex coordinates of the object relative to the world
	void set_world_vertex_coord();

	// gets the vertex coordinates of the object
	std::vector<vec2> get_vertex_coord();

	// gl Immediate mode equivalent, see the Rendering and Transformations section in the
	// specification pdf
	void apply_transformations(vec2 position, float rotation, vec2 scale);
	void transform_begin();
	void transform_scale(vec2 scale);
	void transform_rotate(float radians);
	void transform_translate(vec2 pos);
	void transform_end();
};

struct MTV
{
	vec2 normal;
	float magnitude;
	bool isCollided;
};

// Enum for salmon directions
enum Direction {up, down, left, right, none};