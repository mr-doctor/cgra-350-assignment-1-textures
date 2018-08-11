#pragma once

#include "cgra/mesh.hpp"
#include "cgra/shader.hpp"

#include "glm/glm.hpp"

class Application {
public:
    // The window object managed by GLFW
    GLFWwindow *m_window;

    // The shader program used for drawing
    cgra::Program m_program;
    // The mesh data
    cgra::Mesh m_mesh;

    // The current size of the viewport
    glm::vec2 m_viewportSize;
    // The current mouse position
    glm::vec2 m_mousePosition;

    // The translation of the mesh as a vec3
    glm::vec3 m_translation;
    // The scale of the mesh
    float m_scale;

    // A 4x4 matrix representing the rotation of the
    // mesh
    glm::mat4 m_rotationMatrix;

    // Whether or not the left, middle or right buttons are down.
    bool m_mouseButtonDown[3];

    Application(GLFWwindow *win)
        : m_window(win),
          m_viewportSize(1, 1), m_mousePosition(0, 0),
          m_translation(0), m_scale(1), m_rotationMatrix(1) {
        m_mouseButtonDown[0] = false;
        m_mouseButtonDown[1] = false;
        m_mouseButtonDown[2] = false;
    }

    void setWindowSize(int width, int height) {
        m_viewportSize.x = float(width);
        m_viewportSize.y = float(height);
    }

    void init();

    void createCube();
	void sphere_latlong();
	void sphere_from_cube();

    void drawScene();
    void doGUI();

    void onKey(int key, int scancode, int action, int mods);

    void onMouseButton(int button, int action, int mods);

    void onCursorPos(double xpos, double ypos);

    void onScroll(double xoffset, double yoffset);

	void apply_arcball(glm::vec2 current_mouse_XY);

	int m_divisions_lon = 10;
	int m_divisions_lat = 10;
	bool m_sphere_latlong = false;

	void cube_face(std::vector<glm::vec3> *points,
	          std::vector<glm::uvec3> *tris,
	          std::vector<glm::vec2> *textures,
	          int face,
	          float radius,
	          float step_lon,
	          float step_lat);

	bool m_sphere_from_cube = false;
	int m_sphere_mode = 0;

	GLuint load_DDS_texture(const char *path);

	GLuint texture;
	GLuint normal;

	void set_shaders(const char *vertex, const char *fragment);

	glm::vec2 generate_uv(glm::vec3 vec3);
};
