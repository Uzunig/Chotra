#ifndef CAMERA_H
#define CAMERA_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

namespace Chotra {

    
    // ����������� ����� ������, ������� ������������ ������� ������ � ��������� ��������������� �������� ����, ������� � ������� ��� ������������� � OpenGL
    class Camera {
    public:
        // �������� ������
        glm::vec3 Position;
        glm::vec3 Front;
        glm::vec3 Up;
        glm::vec3 Right;
        glm::vec3 WorldUp;
        // ���� ������
        float Yaw;
        float Pitch;

        glm::vec3 movementDirection; 

        // ��������� ������
        float MovementSpeed; //�������� �������� �� ������� �������
        float MouseSensitivity; //����������� �������������� �������� ���� �� ������ � ���� ������
        float Zoom = 45.0f;

        Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f),
            float yaw = -90.f, float pitch = -10.0f);

        glm::mat4 GetViewMatrix();
        void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true);
        void ProcessKeyboard(float deltaTime);
        void ProcessMouseScroll(float yoffset);

    private:
        void updateCameraVectors();
    };
} // namspace Chotra

#endif
