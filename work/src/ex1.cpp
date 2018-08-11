#include <algorithm>
#include <cmath>
#include <iostream>
#include <stdexcept>
#include <zconf.h>

#include "opengl.hpp"
#include "imgui.h"

#include "cgra/matrix.hpp"
#include "cgra/wavefront.hpp"

#include "ex1.hpp"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/euler_angles.hpp"

#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844

void Application::init() {

	texture = load_DDS_texture(CGRA_SRCDIR "/res/Texture.dds");
	normal = load_DDS_texture(CGRA_SRCDIR "/res/NormalMap.dds");
	set_shaders(CGRA_SRCDIR "/res/shaders/texture.vs.glsl",CGRA_SRCDIR "/res/shaders/texture.fs.glsl");
//	set_shaders(CGRA_SRCDIR "/res/shaders/simple.vs.glsl",CGRA_SRCDIR "/res/shaders/simple.fs.glsl");
//	set_shaders(CGRA_SRCDIR "/res/shaders/cook-torrance.vs.glsl",CGRA_SRCDIR "/res/shaders/simple.fs.glsl");
//	set_shaders(CGRA_SRCDIR "/res/shaders/oren-nayar.vs.glsl",CGRA_SRCDIR "/res/shaders/simple.fs.glsl");

//    createCube();
	sphere_latlong();
}

GLuint Application::load_DDS_texture(const char *path) {

	unsigned char header[124];

	FILE *fp;

	/* try to open the file */
	fp = fopen(path, "rb");
	if (fp == NULL)
		return 0;

	/* verify the type of file */
	char filecode[4];
	fread(filecode, 1, 4, fp);
	if (strncmp(filecode, "DDS ", 4) != 0) {
		fclose(fp);
		return 0;
	}

	/* get the surface desc */
	fread(&header, 124, 1, fp);

	unsigned int height = *(unsigned int *) &(header[8]);
	unsigned int width = *(unsigned int *) &(header[12]);
	unsigned int linearSize = *(unsigned int *) &(header[16]);
	unsigned int mipMapCount = *(unsigned int *) &(header[24]);
	unsigned int fourCC = *(unsigned int *) &(header[80]);
	unsigned char *buffer;
	unsigned int bufsize;
	/* how big is it going to be including all mipmaps? */
	bufsize = mipMapCount > 1 ? linearSize * 2 : linearSize;
	buffer = (unsigned char *) malloc(bufsize * sizeof(unsigned char));
	fread(buffer, 1, bufsize, fp);
	/* close the file pointer */
	fclose(fp);
	unsigned int format;
	switch (fourCC) {
		case FOURCC_DXT1:
			format = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
	}
	// Create one OpenGL texture
	GLuint textureID;
	glGenTextures(1, &textureID);

	// "Bind" the newly created texture : all future texture functions will modify this texture
	glBindTexture(GL_TEXTURE_2D, textureID);
	unsigned int blockSize = (format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
	unsigned int offset = 0;

	/* load the mipmaps */
	for (unsigned int level = 0; level < mipMapCount && (width || height); ++level) {
		unsigned int size = ((width + 3) / 4) * ((height + 3) / 4) * blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D, level, format, width, height,
		                       0, size, buffer + offset);

		offset += size;
		width /= 2;
		height /= 2;
	}
	free(buffer);
	// When MAGnifying the image (no bigger mipmap available), use LINEAR filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// When MINifying the image, use a LINEAR blend of two mipmaps, each filtered LINEARLY too
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	// Generate mipmaps, by the way.
	glGenerateMipmap(GL_TEXTURE_2D);
	return textureID;
}

void Application::set_shaders(const char * vertex, const char * fragment) {
	m_program = cgra::Program::load_program(vertex, fragment);
	glm::mat4 viewMatrix(1);
	viewMatrix[3] = glm::vec4(0, 0, -10, 1);
	m_program.setViewMatrix(viewMatrix);

	glm::vec3 rotation(1.0f, 1.0f, 0.0f);
	m_rotationMatrix = glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));
}

void print(std::string string) {
	std::cout << string << "\n" << std::flush;
}

void print(glm::vec3 vector) {
	std::cout << "[" << vector.x << ", " << vector.y << ", " << vector.z << "]" << std::endl;
}

void print(float f) {
	std::cout << f << std::endl;
}

glm::vec3 calculate_normalised_vector(glm::vec2 mouse_pos, glm::vec2 view_port_size) {
	// Convert to model coordinates
	glm::vec3 direction{mouse_pos.x / view_port_size.x * 2 - 1,
	                    -(mouse_pos.y / view_port_size.y * 2 - 1),
	                    0};

	float length = glm::length(direction);

	// Normalise
	if (length <= 1.0) {
		direction.z = (float) sqrt(1 - length);
	} else {
		direction = glm::normalize(direction);
	}
	return direction;
}

void Application::apply_arcball(glm::vec2 current_mouse_XY) {

	glm::vec3 current_arcball_direction = calculate_normalised_vector(current_mouse_XY, m_viewportSize);
	glm::vec3 previous_arcball_direction = calculate_normalised_vector(m_mousePosition, m_viewportSize);

	float angle = previous_arcball_direction.x * current_arcball_direction.x +
	              previous_arcball_direction.y * current_arcball_direction.y +
	              previous_arcball_direction.z * current_arcball_direction.z;

	glm::vec3 normal = glm::cross(previous_arcball_direction, current_arcball_direction);
	// If there is nothing to rotate around
	if (glm::length(normal) == 0) {
		return;
	}
	// Invert the matrix for camera coordinates
	glm::mat4 arcball_rotate = glm::inverse(glm::rotate(glm::mat4(), angle / -50, normal));
	// Apply matrix
	arcball_rotate *= m_rotationMatrix;
	m_rotationMatrix = arcball_rotate;
}

void Application::createCube() {

	cgra::Matrix<double> vertices(36, 3);
	cgra::Matrix<unsigned int> triangles(12, 3);

	vertices.setRow(0, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(1, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(2, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(3, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(4, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(5, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(6, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(7, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(8, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(9, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(10, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(11, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(12, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(13, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(14, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(15, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(16, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(17, { -1.0f, -1.0f, -1.0f });
	vertices.setRow(18, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(19, { -1.0f, -1.0f,  1.0f });
	vertices.setRow(20, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(21, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(22, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(23, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(24, {  1.0f, -1.0f, -1.0f });
	vertices.setRow(25, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(26, {  1.0f, -1.0f,  1.0f });
	vertices.setRow(27, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(28, {  1.0f,  1.0f, -1.0f });
	vertices.setRow(29, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(30, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(31, { -1.0f,  1.0f, -1.0f });
	vertices.setRow(32, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(33, {  1.0f,  1.0f,  1.0f });
	vertices.setRow(34, { -1.0f,  1.0f,  1.0f });
	vertices.setRow(35, {  1.0f, -1.0f,  1.0f });

	triangles.setRow(0, { 0, 1, 2 });
	triangles.setRow(1, { 3, 4, 5 });
	triangles.setRow(2, { 6, 7, 8 });
	triangles.setRow(3, { 9, 10, 11 });
	triangles.setRow(4, { 12, 13, 14 });
	triangles.setRow(5, { 15, 16, 17 });
	triangles.setRow(6, { 18, 19, 20 });
	triangles.setRow(7, { 21, 22, 23 });
	triangles.setRow(8, { 24, 25, 26 });
	triangles.setRow(9, { 27, 28, 29 });
	triangles.setRow(10, { 30, 31, 32 });
	triangles.setRow(11, { 33, 34, 35 });

	m_mesh.setData(vertices, triangles);
}

glm::vec3 sphere_facet(int i, int j, float thetaStep, float phiStep) {
	float theta = i * thetaStep;
	float phi = j * phiStep;
	glm::vec3 vertex(
			std::sin(theta) * std::cos(phi),
			std::sin(theta) * std::sin(phi),
			std::cos(theta));

	return vertex;
}

glm::vec2 Application::generate_uv(glm::vec3 vec3) {
	glm::vec3 n = glm::normalize(vec3 - glm::vec3(0.0, 0.0, 0.0));

	auto u = static_cast<float>(atan2(n.x, n.z) / (2 * glm::pi<float>()) + 0.5);
	auto v = static_cast<float>(n.y * 0.5 + 0.5);

	return glm::vec2(u, v);
}


void Application::sphere_latlong() {
	if (!m_sphere_latlong) {
		return;
	}
	/*m_divisions_lon = std::max(4, m_divisions_lon);
	m_divisions_lat = std::max(4, m_divisions_lat);*/

	float radius = 1.0f;

	std::vector<glm::vec3> points;
	std::vector<glm::uvec3> tris;
	std::vector<glm::vec2> textures;

	auto pi = glm::pi<float>();


	float thetaStep = pi / m_divisions_lat;
	float phiStep = (2 * pi) / m_divisions_lon;

	for (int i = 0; i <= m_divisions_lat; ++i) {
		for (int j = 0; j <= m_divisions_lon; ++j) {

			// i = xIndex, j = yIndex
			// build from top down

			glm::vec3 TL = sphere_facet(i, j, thetaStep, phiStep) * radius;
			glm::vec3 TR = sphere_facet(i + 1, j, thetaStep, phiStep) * radius;
			glm::vec3 BL = sphere_facet(i, j + 1, thetaStep, phiStep) * radius;
			glm::vec3 BR = sphere_facet(i + 1, j + 1, thetaStep, phiStep) * radius;

			auto indexTL = points.size();

			points.push_back(BL);
			textures.push_back(generate_uv(BL));
			points.push_back(BR);
			textures.push_back(generate_uv(BR));
			points.push_back(TL);
			textures.push_back(generate_uv(TL));
			points.push_back(TR);
			textures.push_back(generate_uv(TR));


			/*
			A given facet (2 triangles) of the sphere is represented with indices thus:
			0       1
			---------
			|      /|
			|     / |
			|    /  |
			|   /   |
			|  /    |
			| /     |
			---------
			2       3
			 */


			tris.emplace_back(indexTL, indexTL + 1, indexTL + 2);
			tris.emplace_back(indexTL + 2, indexTL + 1, indexTL + 3);
		}
	}

	cgra::Matrix<double> vertices(static_cast<unsigned int>(points.size()), 5);
	cgra::Matrix<unsigned int> triangles(static_cast<unsigned int>(tris.size()), 3);

	for (unsigned int k = 0; k < points.size(); k++) {
		vertices.setRow(k, { points[k].x, points[k].y, points[k].z, textures[k].x, textures[k].y });
	}

	for (unsigned int k = 0; k < tris.size(); k++) {
		triangles.setRow(k, {tris[k].x, tris[k].y, tris[k].z});
	}

	m_mesh.setData(vertices, triangles);
}

void Application::cube_face(std::vector<glm::vec3> *points,
                            std::vector<glm::uvec3> *tris,
                            std::vector<glm::vec2> *textures,
                            int face,
                            float radius,
                            float step_lon,
                            float step_lat) {
	for (int i = 0; i <= m_divisions_lon; ++i) {
		for (int j = 0; j <= m_divisions_lat; ++j) {
			glm::vec3 BL;
			glm::vec3 BR;
			glm::vec3 TL;
			glm::vec3 TR;

			if (face == 1 || face == 6) {
				BL = glm::vec3(i * step_lon - 1, j * step_lat - 1, (face == 1) ? -(radius) : (radius));
				BR = glm::vec3((i + 1) * step_lon - 1, j * step_lat - 1, (face == 1) ? -(radius) : (radius));
				TL = glm::vec3(i * step_lon - 1, (j + 1) * step_lat - 1, (face == 1) ? -(radius) : (radius));
				TR = glm::vec3((i + 1) * step_lon - 1, (j + 1) * step_lat - 1, (face == 1) ? -(radius) : (radius));
			} else if (face == 2 || face == 5) {
				BL = glm::vec3(i * step_lon - 1, (face == 2) ? (radius) : -(radius), j * step_lat - 1);
				BR = glm::vec3((i + 1) * step_lon - 1, (face == 2) ? (radius) : -(radius), j * step_lat - 1);
				TL = glm::vec3(i * step_lon - 1, (face == 2) ? (radius) : -(radius), (j + 1) * step_lat - 1);
				TR = glm::vec3((i + 1) * step_lon - 1, (face == 2) ? (radius) : -(radius), (j + 1) * step_lat - 1);
			} else if (face == 3 || face == 4) {
				BL = glm::vec3((face == 3) ? -(radius) : (radius), i * step_lon - 1, j * step_lat - 1);
				BR = glm::vec3((face == 3) ? -(radius) : (radius), (i + 1) * step_lon - 1, j * step_lat - 1);
				TL = glm::vec3((face == 3) ? -(radius) : (radius), i * step_lon - 1, (j + 1) * step_lat - 1);
				TR = glm::vec3((face == 3) ? -(radius) : (radius), (i + 1) * step_lon - 1, (j + 1) * step_lat - 1);
			}

			auto indexTL = points->size();

			points->push_back(BL);
			textures->push_back(generate_uv(BL));
			points->push_back(BR);
			textures->push_back(generate_uv(BR));
			points->push_back(TL);
			textures->push_back(generate_uv(TL));
			points->push_back(TR);
			textures->push_back(generate_uv(TR));

			/*
			A given facet (2 triangles) of the sphere is represented with indices thus:
			0       1
			---------
			|      /|
			|     / |
			|    /  |
			|   /   |
			|  /    |
			| /     |
			---------
			2       3
			 */

			if (face >= 4) {
				tris->emplace_back(indexTL, indexTL + 1, indexTL + 2);
				tris->emplace_back(indexTL + 2, indexTL + 1, indexTL + 3);
			} else {
				tris->emplace_back(indexTL, indexTL + 2, indexTL + 1);
				tris->emplace_back(indexTL + 2, indexTL + 3, indexTL + 1);
			}
		}
	}
}

void Application::sphere_from_cube() {

	std::vector<glm::vec3> points;
	std::vector<glm::uvec3> tris;
	std::vector<glm::vec2> textures;

	float step_lon = 2.0f / (m_divisions_lon + 1);
	float step_lat = 2.0f / (m_divisions_lat + 1);

	cube_face(&points, &tris, &textures, 1, 1.0f, step_lon, step_lat);
	cube_face(&points, &tris, &textures, 2, 1.0f, step_lon, step_lat);
	cube_face(&points, &tris, &textures, 3, 1.0f, step_lon, step_lat);
	cube_face(&points, &tris, &textures, 4, 1.0f, step_lon, step_lat);
	cube_face(&points, &tris, &textures, 5, 1.0f, step_lon, step_lat);
	cube_face(&points, &tris, &textures, 6, 1.0f, step_lon, step_lat);

	for (auto &point : points) {
		point = glm::normalize(point);
	}

	cgra::Matrix<double> vertices(static_cast<unsigned int>(points.size()), 3);
	cgra::Matrix<unsigned int> triangles(static_cast<unsigned int>(tris.size()), 3);

	for (unsigned int k = 0; k < points.size(); k++) {
		vertices.setRow(k, { points[k].x, points[k].y, points[k].z, textures[k].x, textures[k].y });
	}

	for (unsigned int k = 0; k < tris.size(); k++) {
		triangles.setRow(k, {tris[k].x, tris[k].y, tris[k].z});
	}

	m_mesh.setData(vertices, triangles);

}


void Application::drawScene() {
	GLint unf_texture = glGetUniformLocation(texture, "BaseMap");
	glUniform1i(unf_texture, 0);
	unf_texture = glGetUniformLocation(normal, "NormalMap");
	glUniform1i(unf_texture, 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, normal);

	// Calculate the aspect ratio of the viewport;
	// width / height
	float aspectRatio = m_viewportSize.x / m_viewportSize.y;
	// Calculate the projection matrix with a field-of-view of 45 degrees
	glm::mat4 projectionMatrix = glm::perspective(glm::radians(45.0f), aspectRatio, 0.1f, 100.0f);

	// Set the projection matrix
	m_program.setProjectionMatrix(projectionMatrix);
	glm::mat4 modelTransform = m_rotationMatrix * glm::mat4(1.0f);

	/************************************************************
	 *                                                          *
	 * Use `m_translation`, `m_scale`, and `m_rotationMatrix`   *
	 * to create the  `modelTransform` matrix.                  *
	 * The following glm functions will be useful:              *
	 *    `glm::translate`                                      *
	 *    `glm::scale`                                          *
	 ************************************************************/

	m_program.setModelMatrix(modelTransform);

	// Draw the mesh
	m_mesh.draw();
}

void Application::doGUI() {
	ImGui::SetNextWindowSize(ImVec2(450, 450), ImGuiSetCond_FirstUseEver);
	ImGui::Begin("Shapes");

	// Example for rotation, use glm to create a a rotation
	// matrix from this vector
	static glm::vec3 rotation(0.0f, 0.0f, 0.0f);
	if (ImGui::InputFloat3("Rotation", &rotation[0])) {
		// This block is executed if the input changes
		m_rotationMatrix = glm::rotate(glm::mat4(1.0f), 45.0f, glm::vec3(rotation[0], rotation[1], rotation[2]));
	}

	if (ImGui::SliderInt("Lateral Divisions", &m_divisions_lat, (m_sphere_latlong) ? 4 : 0, 80)) {
		(m_sphere_mode == 0) ? sphere_latlong() : sphere_from_cube();
	}

	if (ImGui::SliderInt("Longitudinal Divisions", &m_divisions_lon, (m_sphere_latlong) ? 4 : 0, 80)) {
		(m_sphere_mode == 0) ? sphere_latlong() : sphere_from_cube();
	}

	if (ImGui::Checkbox("Use sphere from latitude and longitude", &m_sphere_latlong)) {
		m_sphere_from_cube = false;
		m_sphere_mode = 0;
		m_divisions_lon = std::max(4, m_divisions_lon);
		m_divisions_lat = std::max(4, m_divisions_lat);
		sphere_latlong();
	}

	if (ImGui::Checkbox("Use sphere from cube", &m_sphere_from_cube)) {
		m_sphere_latlong = false;
		m_sphere_mode = 1;
		sphere_from_cube();
	}

	ImGui::End();
}


// Input Handlers

void Application::onMouseButton(int button, int action, int) {
	if (button >=0 && button < 3) {
		// Set the 'down' state for the appropriate mouse button
		m_mouseButtonDown[button] = action == GLFW_PRESS;
	}
}

void Application::onCursorPos(double xpos, double ypos) {

	// Make a vec2 with the current mouse position
	glm::vec2 currentMousePosition(xpos, ypos);

	// Get the difference from the previous mouse position
//    glm::vec2 mousePositionDelta = currentMousePosition - m_mousePosition;

	if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_LEFT]) {
		apply_arcball(currentMousePosition);
	} else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_MIDDLE]) {

	} else if (m_mouseButtonDown[GLFW_MOUSE_BUTTON_RIGHT]) {

	}

	// Update the mouse position to the current one
	m_mousePosition = currentMousePosition;
}

void Application::onKey(int key, int scancode, int action, int mods) {
	// `(void)foo` suppresses unused variable warnings
	(void)key;
	(void)scancode;
	(void)action;
	(void)mods;
}

void Application::onScroll(double xoffset, double yoffset) {
	// `(void)foo` suppresses unused variable warnings
	(void)xoffset;
	(void)yoffset;
}
