/*
 * Node.h
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODE_H_
#define NODE_H_

#include <glm/glm.hpp>


class Node
{
public:
	Node(double mass)
		: mass(mass), fixed(false) {}

	Node(double mass, const glm::dvec3& initialPos)
		: Node(mass), position(initialPos) {}

	Node(double mass, const glm::dvec3& initialPos, const glm::dvec3& initialVel)
		: Node(mass, initialPos), velocity(initialVel) {}

	void resetForce() {
		totalForce = glm::dvec3(0.0);
	}

	void applyForce(const glm::dvec3& force) {
		totalForce += force;
	}

	void update(const glm::dvec3& _position, const glm::dvec3& _velocity) {
		if (fixed)
			return;
		position = _position;
		velocity = _velocity;
		acceleration = totalForce / mass;
	}

	glm::dvec3 getPosition() const {
		return position;
	}

	glm::dvec3 getVelocity() const {
		if (fixed)
			return {0.0, 0.0, 0.0};
		return velocity;
	}

	glm::dvec3 getAcceleration() const {
		if (fixed)
			return {0.0, 0.0, 0.0};
		return acceleration;
	}

	void fix(bool fixed) {
		this->fixed = fixed;
	}

	bool isFixed() const {
		return fixed;
	}

private:
	double mass;
	glm::dvec3 totalForce;
	glm::dvec3 position, velocity, acceleration;
	bool fixed;
};

#endif /* NODE_H_ */
