#include "Ball.h"
#include "raymath.h"
#include <stdio.h>
#include <string>

Ball::Ball(float posX, float posY, float radius, float mass, float dragCoefficient, float coefficientOfElasticity)
{
    m_xPosition = posX;
    m_yPosition = posY;
    m_radius = radius / ppm; // converts form pixels to meters
    m_mass = mass;
    m_dragCoefficient = dragCoefficient;
    m_coefficientOfElasticity = coefficientOfElasticity;
}

Ball::~Ball()
{
}

void Ball::Update()
{
    m_deltaTime = GetFrameTime();

    DrawCircle(m_xPosition, m_yPosition, m_radius * ppm, RED);

    Vector2 positionInMeters = {m_xPosition / ppm, m_yPosition / ppm};
    Vector2 velocityInMeters = {m_velocity.x / ppm, m_velocity.y / ppm};

    // Newton's second law of motion
    float crossSectionalArea = PI * (m_radius * m_radius);

    Vector2 airResistance = {};
    float speed = Vector2Length(velocityInMeters);

    // air resistance formula: Fd = 1/2 * ρ * v² * Cd * A

    if (speed > 0.0f)
    {
        float dragMagnitude = 0.5f * airDensity * (speed * speed) * m_dragCoefficient * crossSectionalArea;
        Vector2 dragDirection = Vector2Scale(velocityInMeters, -1.0f / speed);
        airResistance = Vector2Scale(dragDirection, dragMagnitude);
    }

    Vector2 gravityForce = Vector2{0, m_mass * gravity};
    Vector2 netForce = Vector2Add(Vector2Add(gravityForce, airResistance), m_externalForces); // + otherForces
    m_externalForces = {};
    Vector2 acceleration = Vector2Scale(netForce, 1.0f / m_mass /*better to do 1.0f / m_mass for multiplying vectors insted of static cast*/);

    velocityInMeters = Vector2Add(velocityInMeters, Vector2Scale(acceleration, m_deltaTime));
    positionInMeters = Vector2Add(positionInMeters, Vector2Scale(velocityInMeters, m_deltaTime));

    // debug:
    // std::string yVelocityString = "Velocity: " + std::to_string(velocityInMeters.y);
    // DrawText(yVelocityString.c_str(), 0, 0, 25, BLACK);
    // DrawText(("mass: " + std::to_string(m_mass)).c_str(), 0, 25, 25, BLACK);
    // DrawText(("accel: " + std::to_string(acceleration.y)).c_str(), 0, 50, 25, BLACK);

    if (positionInMeters.y >= 1000 / ppm /*screen height to pixels*/ - m_radius)
    {
        positionInMeters.y = 1000 / ppm - m_radius;
        velocityInMeters.y = -velocityInMeters.y * m_coefficientOfElasticity;
    }
    if (positionInMeters.y <= 0 / ppm - m_radius)
    {
        positionInMeters.y = 0 - m_radius;
        velocityInMeters.y = -velocityInMeters.y * m_coefficientOfElasticity;
    }
    if (positionInMeters.x >= 1000 / ppm /*screen height to pixels*/ - m_radius)
    {
        positionInMeters.x = 1000 / ppm - m_radius;
        velocityInMeters.x = -velocityInMeters.x * m_coefficientOfElasticity;
    }
    if (positionInMeters.x <= 0 / ppm - m_radius)
    {
        positionInMeters.x = 0 - m_radius;
        velocityInMeters.x = -velocityInMeters.x * m_coefficientOfElasticity;
    }

    // converts the positionInMeters back to pixels and sets the position
    m_xPosition = positionInMeters.x * ppm;
    m_yPosition = positionInMeters.y * ppm;

    // converts the velocityInMeters back to pixels and sets the velocity
    m_velocity = {velocityInMeters.x * ppm, velocityInMeters.y * ppm};
}

void Ball::AddForce(Vector2 force)
{
    m_externalForces = Vector2Add(m_externalForces, force);
    // DrawText(("extern: " + std::to_string(externalForces.x)).c_str(), 0, 75, 25, BLACK);
}

void Ball::Collide(Ball *other)
{
    // Documentation link
    // https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics6collisionresponse/2017%20Tutorial%206%20-%20Collision%20Response.pdf
    // https://www.youtube.com/watch?v=dJNFPv9Mj-Y

    Vector2 pos1 = GetPositionAsVector();
    Vector2 pos2 = other->GetPositionAsVector();

    // point of impact
    Vector2 impactVector = Vector2Subtract(pos2, pos1);
    float distance = Vector2Length(impactVector);

    if (distance <= (m_radius + other->GetRadius()) * ppm)
    {
        // correct Position
        // resolve overlap (separate the balls so they don’t sink into each other)
        float overlap = distance - (m_radius + other->GetRadius()) * ppm;

        Vector2 dir = Vector2Normalize(impactVector); 
        dir = Vector2Scale(dir, overlap * 0.5f);

        // applys correction
        m_xPosition += dir.x;
        m_yPosition += dir.y;
        other->SetPosition(Vector2Subtract(other->GetPositionAsVector(), dir));
        //...

        Vector2 v1 = GetVelocity();
        Vector2 v2 = other->GetVelocity();
        float m1 = GetMass();
        float m2 = other->GetMass();

        // sum of the masses
        float mSum = m1 + m2;
        // velocity differance
        Vector2 vDiff = Vector2Subtract(v2, v1);

        // ball one (this)
        float num = Vector2DotProduct(vDiff, impactVector);
        float den = mSum * distance * distance;

        // coefficient of elasticity (elasticity)
        float e = std::min(m_coefficientOfElasticity, other->m_coefficientOfElasticity);

        // change in this velocity
        // Vector2 normal = Vector2Normalize(impactVector);

        // Vector2 deltaVA = Vector2Scale(normal, (1 + e) * 2 * m2 * num / den / distance);
        
        Vector2 normal = Vector2Normalize(impactVector);
        
        float impulseMagnitudeA = (1 + e) * 2 * m2 * num / den / distance;
        
        Vector2 deltaVA = Vector2Scale(normal, impulseMagnitudeA);

        SetVelocity(Vector2Add(v1, deltaVA));
        //...

        // ball 2 (other)
        // change in this velocity

        float impulseMagnitudeB = -(1 + e) * 2 * m1 * num / den / distance;

        Vector2 deltaVB = Vector2Scale(normal, impulseMagnitudeB);

        // Vector2 deltaVB = Vector2Scale(impactVector, -(1 + e) * 2 * m1 * num / den);

        other->SetVelocity(Vector2Add(v2, deltaVB));
        //...
    }
}

Vector2 Ball::GetPositionAsVector()
{
    return Vector2{m_xPosition, m_yPosition};
}