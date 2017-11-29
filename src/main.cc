#include <GL/glew.h>
#include <dirent.h>

#include "bone_geometry.h"
#include "procedure_geometry.h"
#include "render_pass.h"
#include "config.h"
#include "gui.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include <glm/gtx/component_wise.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/io.hpp>
#include <debuggl.h>

int window_width = 800, window_height = 600;
const std::string window_title = "Skinning";

const char* vertex_shader =
#include "shaders/default.vert"
;

const char* geometry_shader =
#include "shaders/default.geom"
;

const char* fragment_shader =
#include "shaders/default.frag"
;

const char* floor_fragment_shader =
#include "shaders/floor.frag"
;

// FIXME: Add more shaders here.
const char* bone_fragment_shader =
#include "shaders/bone.frag"
;

const char* bone_vertex_shader =
#include "shaders/bone.vert"
;

const char* bone_geometry_shader =
#include "shaders/bone.geom"
;

const char* cylinder_fragment_shader =
#include "shaders/cylinder.frag"
;

const char* normal_fragment_shader =
#include "shaders/normal.frag"
;

const char* binormal_fragment_shader =
#include "shaders/binormal.frag"
;

const char* animated_vertex_shader =
#include "shaders/animated.vert"
;

void ErrorCallback(int error, const char* description) {
	std::cerr << "GLFW Error: " << description << "\n";
}

GLFWwindow* init_glefw()
{
	if (!glfwInit())
		exit(EXIT_FAILURE);
	glfwSetErrorCallback(ErrorCallback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 4);
	auto ret = glfwCreateWindow(window_width, window_height, window_title.data(), nullptr, nullptr);
	CHECK_SUCCESS(ret != nullptr);
	glfwMakeContextCurrent(ret);
	glewExperimental = GL_TRUE;
	CHECK_SUCCESS(glewInit() == GLEW_OK);
	glGetError();  // clear GLEW's error for it
	glfwSwapInterval(1);
	const GLubyte* renderer = glGetString(GL_RENDERER);  // get renderer string
	const GLubyte* version = glGetString(GL_VERSION);    // version as a string
	std::cout << "Renderer: " << renderer << "\n";
	std::cout << "OpenGL version supported:" << version << "\n";

	return ret;
}

// void vertices_fill(std::vector<glm::vec4>& cylinder_vertices)
// {
// 	for(int i = 0; i < BONE_LINE_COUNT*2; ++i)
// 		cylinder_vertices.push_back(glm::vec4(0.0f,0.0f,0.0f,0.0f));
// }

int main(int argc, char* argv[])
{
	if (argc < 2) {
		std::cerr << "Input model file is missing" << std::endl;
		std::cerr << "Usage: " << argv[0] << " <PMD file>" << std::endl;
		return -1;
	}
	GLFWwindow *window = init_glefw();
	GUI gui(window);

	std::vector<glm::vec4> floor_vertices;
	std::vector<glm::uvec3> floor_faces;
	//std::vector<glm::vec4> cylinder_vertices;
	//vertices_fill(cylinder_vertices);

	//cylinder_vertices.push_back(glm::vec4(0.0f,8.0f,0.0f,1.0f));
	//cylinder_vertices.push_back(glm::vec4(2.0f,8.0f,0.0f,1.0f));
	create_floor(floor_vertices, floor_faces);

	// FIXME: add code to create bone and cylinder geometry
	// std::vector<glm::

	Mesh mesh;
	mesh.loadpmd(argv[1]);
	std::cout << "Loaded object  with  " << mesh.vertices.size()
		<< " vertices and " << mesh.faces.size() << " faces.\n";

	glm::vec4 mesh_center = glm::vec4(0.0f);
	for (size_t i = 0; i < mesh.vertices.size(); ++i) {
		mesh_center += mesh.vertices[i];
	}
	mesh_center /= mesh.vertices.size();

	/*
	 * GUI object needs the mesh object for bone manipulation.
	 */
	gui.assignMesh(&mesh);

	glm::vec4 light_position = glm::vec4(0.0f, 100.0f, 0.0f, 1.0f);
	MatrixPointers mats; // Define MatrixPoFinters here for lambda to capture
	/*
	 * In the following we are going to define several lambda functions to bind Uniforms.
	 * 
	 * Introduction about lambda functions:
	 *      http://en.cppreference.com/w/cpp/language/lambda
	 *      http://www.stroustrup.com/C++11FAQ.html#lambda
	 */
	auto matrix_binder = [](int loc, const void* data) {
		glUniformMatrix4fv(loc, 1, GL_FALSE, (const GLfloat*)data);
	};
	auto bone_matrix_binder = [&mesh](int loc, const void* data) {
		auto nelem = mesh.getNumberOfBones();
		glUniformMatrix4fv(loc, nelem, GL_FALSE, (const GLfloat*)data);
	};
	auto vector_binder = [](int loc, const void* data) {
		glUniform4fv(loc, 1, (const GLfloat*)data);
	};
	auto vector3_binder = [](int loc, const void* data) {
		glUniform3fv(loc, 1, (const GLfloat*)data);
	};
	auto float_binder = [](int loc, const void* data) {
		glUniform1fv(loc, 1, (const GLfloat*)data);
	};
	auto int_binder = [](int loc, const void* data){
		glUniform1iv(loc, 1, (const GLint*)data);
	};
	/*
	 * These lambda functions below are used to retrieve data
	 */
	auto std_model_data = [&mats]() -> const void* {
		return mats.model;
	}; // This returns point to model matrix
	glm::mat4 floor_model_matrix = glm::mat4(1.0f);
	auto floor_model_data = [&floor_model_matrix]() -> const void* {
		return &floor_model_matrix[0][0];
	}; // This return model matrix for the floor.
	auto std_view_data = [&mats]() -> const void* {
		return mats.view;
	};
	auto std_camera_data  = [&gui]() -> const void* {
		return &gui.getCamera()[0];
	};
	auto std_proj_data = [&mats]() -> const void* {
		return mats.projection;
	};
	auto std_light_data = [&light_position]() -> const void* {
		return &light_position[0];
	};
	auto alpha_data  = [&gui]() -> const void* {
		static const float transparet = 0.5; // Alpha constant goes here
		static const float non_transparet = 1.0;
		if (gui.isTransparent())
			return &transparet;
		else
			return &non_transparet;
	};

	glm::mat4* deformed = mesh.Ds.data();
	glm::mat4* undeformed = mesh.Us.data();

	auto undeformed_matrices_data = [undeformed]() -> const void* {
		return &undeformed[0][0];
	};
	auto deformed_matrices_data = [deformed]() -> const void* {
		return &deformed[0][0];
	};

	int bone_size = mesh.getNumberOfBones();
	auto bone_count_data = [bone_size]() -> const void* {
		return &bone_size;
	};

	// FIXME: add more lambdas for data_source if you want to use RenderPass.
	//        Otherwise, do whatever you like here
	ShaderUniform std_model = { "model", matrix_binder, std_model_data };
	ShaderUniform floor_model = { "model", matrix_binder, floor_model_data };
	ShaderUniform std_view = { "view", matrix_binder, std_view_data };
	ShaderUniform std_camera = { "camera_position", vector3_binder, std_camera_data };
	ShaderUniform std_proj = { "projection", matrix_binder, std_proj_data };
	ShaderUniform std_light = { "light_position", vector_binder, std_light_data };
	ShaderUniform object_alpha = { "alpha", float_binder, alpha_data };
	// FIXME: define more ShaderUniforms for RenderPass if you want to use it.
	//        Otherwise, do whatever you like here
	ShaderUniform undeformed_matrices = { "Us", bone_matrix_binder, undeformed_matrices_data};
	ShaderUniform deformed_matrices = { "Ds", bone_matrix_binder, deformed_matrices_data};
	ShaderUniform bone_count_uni = { "bone_count", int_binder, bone_count_data};

	std::vector<glm::vec2>& uv_coordinates = mesh.uv_coordinates;
	// RenderDataInput object_pass_input;
	// object_pass_input.assign(0, "vertex_position", nullptr, mesh.vertices.size(), 4, GL_FLOAT);
	// object_pass_input.assign(1, "normal", mesh.vertex_normals.data(), mesh.vertex_normals.size(), 4, GL_FLOAT);
	// object_pass_input.assign(2, "uv", uv_coordinates.data(), uv_coordinates.size(), 2, GL_FLOAT);
	// object_pass_input.assign_index(mesh.faces.data(), mesh.faces.size(), 3);
	// object_pass_input.useMaterials(mesh.materials);
	// RenderPass object_pass(-1,
	// 		object_pass_input,
	// 		{
	// 		  animated_vertex_shader,
	// 		  geometry_shader,
	// 		  fragment_shader
	// 		},
	// 		{ std_model, std_view, std_proj,
	// 		  std_light,
	// 		  std_camera, object_alpha },
	// 		{ "fragment_color" }
	// 		);

	RenderDataInput object_pass_input;
	object_pass_input.assign(0, "vertex_position", nullptr, mesh.vertices.size(), 4, GL_FLOAT);
	object_pass_input.assign(1, "normal", mesh.vertex_normals.data(), mesh.vertex_normals.size(), 4, GL_FLOAT);
	object_pass_input.assign(2, "uv", uv_coordinates.data(), uv_coordinates.size(), 2, GL_FLOAT);
	// std::cout << "mesh weights array size: " << mesh.weights_array.size() << "\n";
	// std::cout << "bone count: " << mesh.getNumberOfBones() << "\n";
	// std::cout << "division: " << (mesh.weights_array.size()/mesh.getNumberOfBones()) << "\n";
	// std::cout << "assign length: " << mesh.vertices.size() << "\n";
	
	object_pass_input.assign(3, "weights1", mesh.weights_array.data(), mesh.weights_array.size()/4, 4, GL_FLOAT);
	object_pass_input.assign(4, "weights2", mesh.weights_array.data(), mesh.weights_array.size()/4, 4, GL_FLOAT);
	object_pass_input.assign(5, "weights3", mesh.weights_array.data(), mesh.weights_array.size()/4, 4, GL_FLOAT);

	object_pass_input.assign(6, "bone_ids1", mesh.bone_ids.data(), mesh.bone_ids.size()/4, 4, GL_INT);
	object_pass_input.assign(7, "bone_ids2", mesh.bone_ids.data(), mesh.bone_ids.size()/4, 4, GL_INT);
	object_pass_input.assign(8, "bone_ids3", mesh.bone_ids.data(), mesh.bone_ids.size()/4, 4, GL_INT);

	object_pass_input.assign_index(mesh.faces.data(), mesh.faces.size(), 3);
	object_pass_input.useMaterials(mesh.materials);
	RenderPass object_pass(-1,
			object_pass_input,
			{
			  animated_vertex_shader,
			  geometry_shader,
			  fragment_shader
			},
			{ std_model, std_view, std_proj,
			  std_light,
			  std_camera, object_alpha,
			  undeformed_matrices, deformed_matrices, bone_count_uni},
			{ "fragment_color" },
			true
			);

	// FIXME: Create the RenderPass objects for bones here.
	//        Otherwise do whatever you like.
	RenderDataInput bone_pass_input;
	bone_pass_input.assign(0, "vertex_position", mesh.skeleton.getVertices().data(), mesh.skeleton.getVertices().size(), 4, GL_FLOAT);
	RenderPass bone_pass(-1,
			bone_pass_input,
			{ bone_vertex_shader, bone_geometry_shader, bone_fragment_shader},
			{ std_model, std_view, std_proj, std_light },
			{ "fragment_color" }
			);

	RenderDataInput cylinder_pass_input;
	cylinder_pass_input.assign(0, "vertex_position", mesh.skeleton.getCylinderVertices().data(), mesh.skeleton.getCylinderVertices().size(), 4, GL_FLOAT);
	RenderPass cylinder_pass(-1,
			cylinder_pass_input,
			{ bone_vertex_shader, bone_geometry_shader, cylinder_fragment_shader},
			{ std_model, std_view, std_proj, std_light },
			{ "fragment_color" }
			);

	RenderDataInput normal_pass_input;
	normal_pass_input.assign(0, "vertex_position", mesh.skeleton.getNormalVertices().data(), mesh.skeleton.getNormalVertices().size(), 4, GL_FLOAT);
	RenderPass normal_pass(-1,
			normal_pass_input,
			{ bone_vertex_shader, bone_geometry_shader, normal_fragment_shader},
			{ std_model, std_view, std_proj, std_light },
			{ "fragment_color" }
			);

	RenderDataInput binormal_pass_input;
	binormal_pass_input.assign(0, "vertex_position", mesh.skeleton.getBinormalVertices().data(), mesh.skeleton.getBinormalVertices().size(), 4, GL_FLOAT);
	RenderPass binormal_pass(-1,
			binormal_pass_input,
			{ bone_vertex_shader, bone_geometry_shader, binormal_fragment_shader},
			{ std_model, std_view, std_proj, std_light },
			{ "fragment_color" }
			);

	RenderDataInput floor_pass_input;
	floor_pass_input.assign(0, "vertex_position", floor_vertices.data(), floor_vertices.size(), 4, GL_FLOAT);
	floor_pass_input.assign_index(floor_faces.data(), floor_faces.size(), 3);
	RenderPass floor_pass(-1,
			floor_pass_input,
			{ vertex_shader, geometry_shader, floor_fragment_shader},
			{ floor_model, std_view, std_proj, std_light },
			{ "fragment_color" }
			);
	float aspect = 0.0f;
	std::cout << "center = " << mesh.getCenter() << "\n";

	bool draw_floor = true;
	bool draw_skeleton = true;
	bool draw_object = true;
	bool draw_cylinder = true;

	while (!glfwWindowShouldClose(window)) {
		// Setup some basic window stuff.
		glfwGetFramebufferSize(window, &window_width, &window_height);
		glViewport(0, 0, window_width, window_height);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_MULTISAMPLE);
		glEnable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDepthFunc(GL_LESS);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glCullFace(GL_BACK);

		gui.updateMatrices();
		mats = gui.getMatrixPointers();

		//int current_bone = gui.getCurrentBone();
#if 1
		draw_cylinder = true;
		//draw_cylinder = (current_bone != -1 && gui.isTransparent());
#else
		draw_cylinder = true;
#endif
		// FIXME: Draw bones first.
		if(draw_skeleton && mesh.show)
		{
			//if(mesh.isDirty)
			//{
				bone_pass.updateVBO(0,
							  mesh.skeleton.getVertices().data(),
							  mesh.skeleton.getVertices().size());
				mesh.isDirty = false;
			//}
			bone_pass.setup();
			// Draw our lines
			CHECK_GL_ERROR(glDrawArrays(GL_LINES, 0, mesh.skeleton.getVertices().size() * 2));

			//mouse_pass.setup();

			//CHECK_GL_ERROR(glDrawArrays(GL_LINES, 0, mouse_vertices.size() * 2));
		}

		if(draw_cylinder && mesh.show)
		{
			cylinder_pass.updateVBO(0,
							  mesh.skeleton.getCylinderVertices().data(),
							  mesh.skeleton.getCylinderVertices().size());
			cylinder_pass.setup();

			CHECK_GL_ERROR(glDrawArrays(GL_LINES, 0, mesh.skeleton.getCylinderVertices().size() * 2));

			normal_pass.updateVBO(0,
							  mesh.skeleton.getNormalVertices().data(),
							  mesh.skeleton.getNormalVertices().size());	

			normal_pass.setup();

			CHECK_GL_ERROR(glDrawArrays(GL_LINES, 0, mesh.skeleton.getBinormalVertices().size() * 2));	

			binormal_pass.updateVBO(0,
							  mesh.skeleton.getBinormalVertices().data(),
							  mesh.skeleton.getBinormalVertices().size());	

			binormal_pass.setup();

			CHECK_GL_ERROR(glDrawArrays(GL_LINES, 0, mesh.skeleton.getBinormalVertices().size() * 2));	
				
		}

		// Then draw floor.
		if (draw_floor) {
			floor_pass.setup();
			// Draw our triangles.
			CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, floor_faces.size() * 3, GL_UNSIGNED_INT, 0));
		}
		if (draw_object) {
			if (gui.isPoseDirty()) {
				mesh.updateAnimation();
				object_pass.updateVBO(0,
						      mesh.animated_vertices.data(),
						      mesh.animated_vertices.size());
#if 0
				// For debugging if you need it.
				for (int i = 0; i < 4; i++) {
					std::cerr << " Vertex " << i << " from " << mesh.vertices[i] << " to " << mesh.animated_vertices[i] << std::endl;
				}
#endif
				gui.clearPose();
			}
			object_pass.setup();
			int mid = 0;
			while (object_pass.renderWithMaterial(mid))
				mid++;
#if 0	
			// For debugging also
			if (mid == 0) // Fallback
				CHECK_GL_ERROR(glDrawElements(GL_TRIANGLES, mesh.faces.size() * 3, GL_UNSIGNED_INT, 0));
#endif
		}
		// Poll and swap.
		glfwPollEvents();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
#if 0
	for (size_t i = 0; i < images.size(); ++i)
		delete [] images[i].bytes;
#endif
	exit(EXIT_SUCCESS);
}
