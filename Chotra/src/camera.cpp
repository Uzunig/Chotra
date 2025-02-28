#include "camera.h"

namespace Chotra {

    Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
        : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(10.0f), MouseSensitivity(0.1f) {

        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    glm::mat4 Camera::GetProjectionMatrix(const unsigned int& width, const unsigned int& height)
    {
        return glm::perspective(glm::radians(Zoom), (float)width / (float)height, 0.1f, 500.0f);
    }

    // Возвращает матрицу вида, вычисленную с использованием углов Эйлера и LookAt-матрицы 
    glm::mat4 Camera::GetViewMatrix() {
        return glm::lookAt(Position, Position + Front, Up);
    }


    //Обрабатываем входные данные, полученные от любой клавиатуроподобной системы ввода. Принимаем входной параметр в виде определенного камерой перечисления (для абстрагирования его от оконных систем)
    void Camera::ProcessKeyboard(float deltaTime) {

        glm::vec3 movement = movementDirection * MovementSpeed * deltaTime;  
        Position += Front * movement.z;
        Position += Right * movement.x;
    }


    //Обрабатываем входные данные, полученные от системы ввода с помощью мыши. Ожидаем в качестве параметров значения смещения как в направлении X, так и в направлении Y.
    void Camera::ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch) {

        xoffset *= MouseSensitivity;   //Смещение мыши по экрану переводим в угол с помощью коэффициента MouseSensitivity
        yoffset *= MouseSensitivity;

        Yaw += xoffset;     //Обновляем углы Эйлера
        Pitch += yoffset;

        // Убеждаемся, что когда тангаж выходит за пределы обзора, экран не переворачивается
        if (constrainPitch) {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // Обновляем значения вектора-прямо, вектора-вправо и вектора-вверх, используя обновленные значения углов Эйлера
        updateCameraVectors();
    }


    // Обрабатывает входные данные, полученные от события колеса прокрутки мыши. Интересуют только входные данные на вертикальную ось колесика
    void Camera::ProcessMouseScroll(float yoffset) {

        if (Zoom >= 1.0f && Zoom <= 45.0f)
            Zoom -= yoffset;
        if (Zoom <= 1.0f)
            Zoom = 1.0f;
        if (Zoom >= 45.0f)
            Zoom = 45.0f;
    }

    // Вычисляет вектор-прямо по (обновленным) углам Эйлера камеры
    void Camera::updateCameraVectors() {

        // Вычисляем новый вектор-прямо
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // Также пересчитываем вектор-вправо и вектор-вверх
        Right = glm::normalize(glm::cross(Front, WorldUp));  // Нормализуем векторы, потому что их длина становится стремится к 0 тем больше, чем больше вы смотрите вверх или вниз, что приводит к более медленному движению.
        Up = glm::normalize(glm::cross(Right, Front));
    }

} // namspace Chotra
