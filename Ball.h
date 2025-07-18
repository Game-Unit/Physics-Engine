#pragma once

#include "raylib.h"
#include <list>
#include <algorithm>

class Ball
{
private:
    float m_xPosition = 0;
    float m_yPosition = 0;
    float m_radius = 0;
    float m_mass = 0;
    float m_dragCoefficient = 0;
    float m_coefficientOfElasticity = 1; // A coefficient of 1 means the collision will be purely elastic

    Vector2 m_velocity = {};
    Vector2 m_externalForces = {};

    int ppm = 50;
    float gravity = 9.81f;
    float airDensity = 1.2f;

    float m_deltaTime = 0;

public:
    Ball(float posX, float posY, float radius, float mass, float dragCoefficient, float coefficientOfElasticity);
    ~Ball();

    void Update();
    float GetRadius() { return m_radius; }
    Vector2 GetVelocity() { return m_velocity; }
    Vector2 GetVelocityInMeters() { return Vector2{m_velocity.x / ppm, m_velocity.y / ppm}; }
    float GetMass() { return m_mass; }
    void AddForce(Vector2 force);
    void SetVelocity(Vector2 velocity) { m_velocity = velocity; }
    Vector2 GetPositionAsVector();
    void SetPosition(Vector2 newPos) { m_xPosition = newPos.x; m_yPosition = newPos.y; }
    float GetElasticity() const { return m_coefficientOfElasticity; }

    void Collide(Ball* other);
};