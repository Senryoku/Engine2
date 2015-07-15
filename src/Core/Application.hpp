#pragma once

#include <GL/gl3w.h>

#include <GLFW/glfw3.h>

#include <GUISystem.hpp>
#include <TimeManager.hpp>
#include <ResourcesManager.hpp>
#include <Scene.hpp>
#include <Framebuffer.hpp>
#include <Camera.hpp>
#include <Query.hpp>
#include <Buffer.hpp>
#include <Blur.hpp>

class Application
{
public:
	Application();
	Application(int argc, char* argv[]);
	virtual ~Application();

	void init(const std::string& windowName = "Default Window");
	
	void clean();
	
	virtual void run();
	
	virtual void run_init();
	
	virtual void in_loop_timing();
	
	virtual void in_loop_fps_camera();
	
	virtual void in_loop_update();
	
	virtual void in_loop_render();
	
	virtual void gui_render();
	
	virtual void offscreen_render(const glm::mat4& p, const glm::mat4& v) {};
	
	template<typename T>
	T rand()
	{
		static std::random_device rd;
		static std::mt19937 gen(rd());
		static std::uniform_real_distribution<T> uniform_dist(0, 1);
		return uniform_dist(gen);
	}

	void screen(const std::string& path) const;
	
	Scene& getScene() { return _scene; }
	
protected:
	struct GPUViewProjection
	{
		glm::mat4	view;
		glm::mat4	projection;
	};
	
	// Window settings
	GLFWwindow*	_window;
	int				_width = 1366;
	int				_height = 720;
	bool 			_fullscreen = false;
	bool 			_msaa = false;
	
	GUISystem		_gui;

	// MainCamera
	Camera		_camera;
	float		_fov = 60.0;
	glm::vec3 	_resolution;
	glm::mat4 	_projection;
	glm::vec4 	_mouse = glm::vec4(0.0);
	
	GPUViewProjection	_gpuCameraData;
	UniformBuffer		_camera_buffer;

	bool 		_controlCamera = true;
	double 		_mouse_x = 0.0, 
				_mouse_y = 0.0;
	
	// Post Process settings
	float		_exposure = 2.0f;
	float		_bloom = 1.2f;
	int			_bloomBlur = 1;
	int			_bloomDownsampling = 1;
	glm::vec3	_ambiant = glm::vec3(0.06);
	
	float		_minVariance = 0.0000001f;
	int			_aoSamples = 8;
	float		_aoThreshold = 1.0f;
	float		_aoRadius = 10.0f;

	// Time Management
	float		_timescale = 0.5;
	float 		_time = 0.f;
	float		_frameTime;
	float		_frameRate;
	bool		_paused = false;
	
	/**
	 * G-Buffer:
	 *  Color0 : Color (xyz) and MaterialInfo (w)
	 *  Color1 : World Position (xyz) and Depth (w)
	 *  Color2 : Encoded Normal (xy), F0 (z) and R (w)
	**/
	Framebuffer<Texture2D, 3>	_offscreenRender;
	
	Scene							_scene;
	
	bool	_debug = true;

	// Callbacks (GLFW)
	virtual void error_callback(int error, const char* description);
	virtual void resize_callback(GLFWwindow* window, int width, int height);
	virtual void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	virtual void char_callback(GLFWwindow* window, unsigned int codepoint);
	virtual void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	virtual void mouse_position_callback(GLFWwindow* window, double xpos, double ypos);
	virtual void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
	virtual void drop_callback(GLFWwindow* window, int count, const char ** paths);
	
	// Static
	
	// Singleton
	static Application* s_instance;
	static Application& getInstance() { return *s_instance; }
	
	// Static callbacks (C-like callbacks)
	static void s_error_callback(int error, const char* description)
	{ getInstance().error_callback(error, description); }
	
	static void s_resize_callback(GLFWwindow* window, int width, int height)
	{ getInstance().resize_callback(window, width, height); }
	
	static void s_key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{ getInstance().key_callback(window, key, scancode, action, mods); }
	
	static void s_char_callback(GLFWwindow* window, unsigned int codepoint)
	{ getInstance().char_callback(window, codepoint); }
	
	static void s_mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{ getInstance().mouse_button_callback(window, button, action, mods); }
	
	static void s_mouse_position_callback(GLFWwindow* window, double xpos, double ypos)
	{ getInstance().mouse_position_callback(window, xpos, ypos); }
	
	static void s_scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
	{ getInstance().scroll_callback(window, xoffset, yoffset); }
	
	static void s_drop_callback(GLFWwindow* window, int count, const char ** paths)
	{ getInstance().drop_callback(window, count, paths); }
};
