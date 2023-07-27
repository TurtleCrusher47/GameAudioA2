#include "CollisionManager.h"

#include <algorithm>
#include <iostream>
using namespace std;

/**
 @brief Sphere-Sphere Collision Test
 */
bool CCollisionManager::SphereSphereCollision(  const glm::vec3 sphereCentreA, 
                                                const float sphereRadiusA,
                                                const glm::vec3 sphereCentreB, 
                                                const float sphereRadiusB)
{
    return (glm::distance(sphereCentreA, sphereCentreB) > sphereRadiusA + sphereRadiusA);
}

/**
 @brief Box-Box Collision Test
 */
bool CCollisionManager::BoxBoxCollision(    const glm::vec3 aMin, 
                                            const glm::vec3 aMax,
                                            const glm::vec3 bMin, 
                                            const glm::vec3 bMax)
{
    // true == collision, false == no collision
    return  (aMin.x <= bMax.x && aMax.x >= bMin.x) &&
        (aMin.y <= bMax.y && aMax.y >= bMin.y) &&
        (aMin.z <= bMax.z && aMax.z >= bMin.z);
}

/**
 @brief Ray-Sphere Collision Test
 */
bool CCollisionManager::RaySphereCollision( const glm::vec3 sphereCentre, 
                                            const float sphereRadius,
                                            const glm::vec3 rayOrigin, 
                                            const glm::vec3 rayDirection,
                                            float& t)
{
    glm::vec3 oc = rayOrigin - sphereCentre;
    float a = glm::dot(rayDirection, rayDirection);
    float b = 2.0f * dot(oc, rayDirection);
    float c = dot(oc, oc) - sphereRadius * sphereRadius;
    float discriminant = b * b - 4 * a * c;

    // If discriminant < 0, the line of the ray does not intersect the sphere(missed);
    if (discriminant < 0.0)
    {
        return false;
    }
    else {
        // If discriminant = 0, the line of the ray just touches the sphere in one point(tangent);
        // If discriminant > 0, the line of the ray touches the sphere in two points(intersected).
        // If both t are positive, ray is facing the sphereand intersecting.
        // If one t positive, one t negative, ray is shooting from inside.
        // If both t are negative, ray is shooting away from the sphere, which is technically impossible.

        // Check for the negative/smaller result of the sqrt
        float numerator = -b - sqrt(discriminant);
        if (numerator > 0.0)
        {
            // Calculate the parameter, t
            t = numerator / (2.0f * a);
            return true;
        }

        // Check for the positive/larger result of the sqrt
        numerator = -b + sqrt(discriminant);
        if (numerator > 0.0)
        {
            // Calculate the parameter, t
            t = numerator / (2.0f * a);
            return true;
        }
        else
        {
            return false;
        }
    }

    /*
    // If you wish to calculate the hit point of this collision
    glm::vec3 hitPoint = rayOrigin + t * rayDirection;
    */

    return true;
}

/**
 @brief Ray-Box Collision Test
 */
bool CCollisionManager::RayBoxCollision(const glm::vec3 boxMin, 
                                        const glm::vec3 boxMax,
                                        const glm::vec3 rayOrigin, 
                                        const glm::vec3 rayDirection,
                                        float& t)
{
    float t1 = (boxMin.x - rayOrigin.x) / rayDirection.x;
    float t2 = (boxMax.x - rayOrigin.x) / rayDirection.x;
    float t3 = (boxMin.y - rayOrigin.y) / rayDirection.y;
    float t4 = (boxMax.y - rayOrigin.y) / rayDirection.y;
    float t5 = (boxMin.z - rayOrigin.z) / rayDirection.z;
    float t6 = (boxMax.z - rayOrigin.z) / rayDirection.z;
    float t7 = fmax(fmax(fmin(t1, t2), fmin(t3, t4)), fmin(t5, t6));
    float t8 = fmin(fmin(fmax(t1, t2), fmax(t3, t4)), fmax(t5, t6));

    bool bResult = false;
    if (t8 < 0 || t7 > t8)
        bResult = false;
    else
    {
        t = t7;
        bResult = true;
    }

    /*
// If you wish to calculate the hit point of this collision
glm::vec3 hitPoint = rayOrigin + t * rayDirection;
*/

    return bResult;
}

/**
 @brief Ray-Box Collision Test : Overloaded
 */
bool CCollisionManager::RayBoxCollision(const glm::vec3 boxMin, 
                                        const glm::vec3 boxMax,
                                        const glm::vec3 rayStart,
                                        const glm::vec3 rayEnd)
{
    glm::vec3 ray = rayEnd - rayStart;
    glm::vec3 rayDirection = glm::normalize(ray);
    
    float t = FLT_MAX;
    if (RayBoxCollision(boxMin, boxMax, rayStart, rayDirection, t) == true)
    {
        /*
        // If you wish to calculate the hit point of this collision
        glm::vec3 hitPoint = rayOrigin + t * rayDirection;
        */
        if (t >= 0.0f && t <= 1.0f)
            return true;
    }
    return false;
}
