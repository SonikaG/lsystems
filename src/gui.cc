#include "gui.h"
#include "config.h"
#include <jpegio.h>
//#include "bone_geometry.h"
#include <iostream>
#include <debuggl.h>
#include <glm/gtc/matrix_access.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

namespace {
	// Intersect a cylinder with radius 1/2, height 1, with base centered at
	// (0, 0, 0) and up direction (0, 1, 0).
	bool IntersectCylinder(const glm::vec3& origin, const glm::vec3& direction,
			float radius, float height, float* t)
	{
		//FIXME perform proper ray-cylinder collision detection


		return true;
	}
}

GUI::GUI(GLFWwindow* window)
	:window_(window)
{
	glfwSetWindowUserPointer(window_, this);
	glfwSetKeyCallback(window_, KeyCallback);
	glfwSetCursorPosCallback(window_, MousePosCallback);
	glfwSetMouseButtonCallback(window_, MouseButtonCallback);

	glfwGetWindowSize(window_, &window_width_, &window_height_);
	float aspect_ = static_cast<float>(window_width_) / window_height_;
	//std::cout << "window width: " << window_width_ << " window_height_: " << window_height_ << "\n";
	projection_matrix_ = glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);

	projection_matrix_[3][2] = - kFar / (kFar - kNear);
}

GUI::~GUI()
{
}

void GUI::assignMesh(Mesh* mesh)
{
	mesh_ = mesh;
	center_ = mesh_->getCenter();
}

void GUI::keyCallback(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window_, GL_TRUE);
		return ;
	}
	if (key == GLFW_KEY_J && action == GLFW_RELEASE) {
		//FIXME save out a screenshot using SaveJPEG
		unsigned char* pixels = new unsigned char[3 * window_width_ * window_height_];
		glReadPixels(0, 0, window_width_, window_height_, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		SaveJPEG("saved_file.jpg", window_width_, window_height_, pixels);
	}

	//if(key == GLFW_KEY_M)

	if (captureWASDUPDOWN(key, action))
		return ;
	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_RIGHT) {
		float roll_speed;
		if (key == GLFW_KEY_RIGHT)
			roll_speed = -roll_speed_;
		else
			roll_speed = roll_speed_;
		// FIXME: actually roll the bone here
		if(current_bone_ != NULL)
		{
			current_bone_->rollBone(roll_speed);
			mesh_->skeleton.generateVertices();
			mesh_->skeleton.regenerateHighlightBone(current_bone_);
			mesh_->skeleton.regenerateNormalVertices(current_bone_);
			mesh_->skeleton.regenerateBinormalVertices(current_bone_);
			pose_changed_ = true;
		}
	} else if (key == GLFW_KEY_C && action != GLFW_RELEASE) {
		fps_mode_ = !fps_mode_;
	} else if (key == GLFW_KEY_LEFT_BRACKET && action == GLFW_RELEASE) {
		//current_bone_--;
		//current_bone_ += mesh_->getNumberOfBones();
		//current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_RIGHT_BRACKET && action == GLFW_RELEASE) {
		//current_bone_++;
		//current_bone_ += mesh_->getNumberOfBones();
		//current_bone_ %= mesh_->getNumberOfBones();
	} else if (key == GLFW_KEY_T && action != GLFW_RELEASE) {
		transparent_ = !transparent_;
		mesh_->show = !mesh_->show;
	}
}

void GUI::mousePosCallback(double mouse_x, double mouse_y)
{
	last_x_ = current_x_;
	last_y_ = current_y_;
	current_x_ = mouse_x;
	current_y_ = window_height_ - mouse_y;
	float delta_x = current_x_ - last_x_;
	float delta_y = current_y_ - last_y_;
	if (sqrt(delta_x * delta_x + delta_y * delta_y) < 1e-15)
		return;
	
	glm::vec3 mouse_direction = glm::normalize(glm::vec3(delta_x, delta_y, 0.0f));
	glm::vec2 mouse_start = glm::vec2(last_x_, last_y_);
	glm::vec2 mouse_end = glm::vec2(current_x_, current_y_);
	glm::vec2 mouse_delta = mouse_end - mouse_start;

	glm::uvec4 viewport = glm::uvec4(0, 0, window_width_, window_height_);

	bool drag_camera = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_RIGHT;
	bool drag_bone = drag_state_ && current_button_ == GLFW_MOUSE_BUTTON_LEFT;

	Ray mouse_ray;	

	setRay(mouse_ray, mouse_x, mouse_y);

	if (drag_camera) {
		glm::vec3 axis = glm::normalize(
				orientation_ *
				glm::vec3(mouse_direction.y, -mouse_direction.x, 0.0f)
				);
		orientation_ =
			glm::mat3(glm::rotate(rotation_speed_, axis) * glm::mat4(orientation_));
		tangent_ = glm::column(orientation_, 0);
		up_ = glm::column(orientation_, 1);
		look_ = glm::column(orientation_, 2);
	} else if (drag_bone && current_bone_ != NULL) {
		// FIXME: Handle bone rotation
		if(std::abs(mouse_delta.x) <= 0.1f && std::abs(mouse_delta.y) <= 0.1f)
			return;

		glm::vec3 zAxis = glm::normalize(center_ - eye_);
		glm::vec3 xAxis = glm::normalize(glm::cross(zAxis, up_));
		glm::vec3 yAxis = -glm::normalize(glm::cross(xAxis, zAxis));

		float angle_magnitude = rotation_speed_ * glm::length(mouse_delta);

		glm::vec3 mouse_vector = glm::normalize(xAxis*mouse_delta.x + yAxis*mouse_delta.y);

		glm::vec3 rotation_axis = glm::normalize(glm::cross(mouse_vector, zAxis));

		current_bone_->rotateBone(rotation_axis, angle_magnitude);

		mesh_->skeleton.generateVertices();
		mesh_->skeleton.regenerateHighlightBone(current_bone_);
		mesh_->skeleton.regenerateNormalVertices(current_bone_);
		mesh_->skeleton.regenerateBinormalVertices(current_bone_);
		pose_changed_ = true;
		return ;
	}

	// FIXME: highlight bones that have been moused over
	if(!drag_bone)
	{
		Bone* highlight_bone = NULL;
		highlightBones(mouse_ray, highlight_bone);
		current_bone_ = highlight_bone;
		//if(current_bone_)
		//	std::cout << "current bone id: " << current_bone_->getJid() << "\n";
	}
}

void GUI::mouseButtonCallback(int button, int action, int mods)
{
	drag_state_ = (action == GLFW_PRESS);
	current_button_ = button;
}

void GUI::updateMatrices()
{
	// Compute our view, and projection matrices.
	if (fps_mode_)
		center_ = eye_ + camera_distance_ * look_;
	else
		eye_ = center_ - camera_distance_ * look_;

	view_matrix_ = glm::lookAt(eye_, center_, up_);
	light_position_ = glm::vec4(eye_, 1.0f);

	aspect_ = static_cast<float>(window_width_) / window_height_;
	projection_matrix_ =
		glm::perspective((float)(kFov * (M_PI / 180.0f)), aspect_, kNear, kFar);
	model_matrix_ = glm::mat4(1.0f);

	projection_matrix_[3][2] = - kFar / (kFar - kNear);
}

MatrixPointers GUI::getMatrixPointers() const
{
	MatrixPointers ret;
	ret.projection = &projection_matrix_[0][0];
	ret.model= &model_matrix_[0][0];
	ret.view = &view_matrix_[0][0];
	return ret;
}

// bool GUI::setCurrentBone(int i)
// {
// 	if (i < 0 || i >= mesh_->getNumberOfBones())
// 		return false;
// 	current_bone_ = i;
// 	return true;
// }

bool GUI::captureWASDUPDOWN(int key, int action)
{
	if (key == GLFW_KEY_W) {
		if (fps_mode_)
			eye_ += zoom_speed_ * look_;
		else
			camera_distance_ -= zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_S) {
		if (fps_mode_)
			eye_ -= zoom_speed_ * look_;
		else
			camera_distance_ += zoom_speed_;
		return true;
	} else if (key == GLFW_KEY_A) {
		if (fps_mode_)
			eye_ -= pan_speed_ * tangent_;
		else
			center_ -= pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_D) {
		if (fps_mode_)
			eye_ += pan_speed_ * tangent_;
		else
			center_ += pan_speed_ * tangent_;
		return true;
	} else if (key == GLFW_KEY_DOWN) {
		if (fps_mode_)
			eye_ -= pan_speed_ * up_;
		else
			center_ -= pan_speed_ * up_;
		return true;
	} else if (key == GLFW_KEY_UP) {
		if (fps_mode_)
			eye_ += pan_speed_ * up_;
		else
			center_ += pan_speed_ * up_;
		return true;
	}
	return false;
}


// Delegrate to the actual GUI object.
void GUI::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->keyCallback(key, scancode, action, mods);
}

void GUI::MousePosCallback(GLFWwindow* window, double mouse_x, double mouse_y)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mousePosCallback(mouse_x, mouse_y);
}

void GUI::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	GUI* gui = (GUI*)glfwGetWindowUserPointer(window);
	gui->mouseButtonCallback(button, action, mods);
}

const glm::mat4 GUI::MVP() const
{
	return projection_matrix_;
}

void GUI::setRay(Ray& ray, double mouse_x, double mouse_y)
{
	float aspect_ = static_cast<float>(window_width_) / window_height_;

	ray.p = eye_;

	float z_val = 100.0f;
	float y_val = z_val * glm::tan((float)(kFov/2 * (M_PI / 180.0f)));
	float x_val = aspect_ * y_val;

	float n_x = ((2.0f*mouse_x) / (window_width_)) - 1.0f;
	float n_y = -1.0f * (((2.0f*mouse_y) / (window_height_)) - 1.0f);

	glm::vec3 zAxis = glm::normalize(center_ - eye_);
	glm::vec3 xAxis = glm::normalize(glm::cross(zAxis, up_));
	glm::vec3 yAxis = glm::normalize(glm::cross(xAxis, zAxis));

	ray.v = n_x * x_val * xAxis + n_y * y_val * yAxis + z_val * zAxis;
}

void GUI::highlightBones(const Ray& ray, Bone*& bone)
{
	mesh_->skeleton.highlightBones(ray, bone);
}