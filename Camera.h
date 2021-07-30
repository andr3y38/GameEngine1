#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

namespace GE {
	class Camera {
	public:
		Camera(glm::vec3 cam_pos, glm::vec3 cam_target, glm::vec3 cam_upDir, float fov, float ar, float near, float far) {
			// Initialise the camera variables to the values pass on the method parameters
			pos = cam_pos;
			up = cam_upDir;
			fovy = fov;
			target = cam_target;
			aspectR = ar;
			nearClip = near;
			farClip = far;
			updateCamMatrices();
			SDL_GetMouseState(&oldMouseX, &oldMouseY);	
		}

		~Camera() {

		}

		// Access methods
		float getPosX() {
			return pos.x;
		}

		float getPosY() {
			return pos.y;
		}

		float getPosZ() {
			return pos.z;
		}

		glm::vec3 getPos() {
			return pos;
		}

		glm::vec3 getTarget() {
			return target;
		}

		glm::vec3 getUpDir() {
			return up;
		}

		float getPitch() {
			return pitch;
		}

		float getYaw() {
			return yaw;
		}

		float getOldMouseX() {
			return oldMouseX;
		}


		float getOldMouseY() {
			return oldMouseY;
		}

		// Return camera view matrix
		// Used by draw  to send view matrix to vertex shader
		glm::mat4 getViewMatrix() {
			return viewMat;
		}

		// Return the camera projection matrix
		glm::mat4 getProjectionMatrix() {
			return projectionMat;
		}

		// Mutator methods
		void setPosX(float newX) {
			pos = glm::vec3(newX, pos.y, pos.z);
			updateCamMatrices();
		}

		void setPosY(float newY) {
			pos = glm::vec3(pos.x, newY, pos.z);
			updateCamMatrices();
		}

		void setPosZ(float newZ) {
			pos = glm::vec3(pos.x, pos.y, newZ);
			updateCamMatrices();
		}

		// Set position for all axes in one method
		void setPos(float newX, float newY, float newZ) {
			pos = glm::vec3(newX, newY, newZ);
			updateCamMatrices();
		}

		void setPos(glm::vec3 newPos) {
			pos = newPos;
			updateCamMatrices();
		}


		// Set new target
		void setTarget(glm::vec3 newTarget) {
			target = newTarget;
			updateCamMatrices();
		}

		// Set up direction on the camera
		void setUpDir(glm::vec3 newUp) {
			up = newUp;
			updateCamMatrices();
		}
		void setPitch(float newPitch) {

			pitch = newPitch;
			if (pitch > 70.0f) pitch = 70.0f;
			if (pitch < -70.0f) pitch = -70.0f;

		}
		void setYaw(float newYaw) {
			yaw = newYaw;
		}
		void setOldMouseX(float newOldMouseX) {
			oldMouseX = newOldMouseX;
		}

		void setOldMouseY(float newOldMouseY) {
			oldMouseY = newOldMouseY;
		}


		// Set the Field of View
		void setFov(float newFov) {
			fovy = newFov;
			updateCamMatrices();
		}

		// Set the aspect ration
		void setAspectRation(float newAR) {
			aspectR = newAR;
			updateCamMatrices();
		}

		// Set clipping planes for the camera
		void setNearClip(float newNearClip) {
			nearClip = newNearClip;
			updateCamMatrices();
		}

		void setFarClip(float newFarClip) {
			farClip = newFarClip;
			updateCamMatrices();
		}

		// Update the camera matrices in response to member variable changes
		void updateCamMatrices() {
			viewMat = glm::lookAt(pos, target, up);
			projectionMat = glm::perspective(glm::radians(fovy), aspectR, nearClip, farClip);
		}
	private:
		// Member variables
		// Camera view variables
		glm::vec3 pos;
		glm::vec3 target;
		glm::vec3 up;

		// Camera projection variables
		float fovy;
		float aspectR;
		float nearClip;
		float farClip;

		float pitch = 50.0f;
		float yaw = 50.0f;

		int oldMouseX, oldMouseY;
		// View and projection matrices
		glm::mat4 viewMat;
		glm::mat4 projectionMat;

		
	};
}