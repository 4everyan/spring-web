/*
 * Constraints.cpp
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#include "Constraints.h"


void ResistanceForce::solve() {
	if (auto sys = system.lock()) {

		for (size_t i = 0; i < sys->getNumberOfNodes(); ++i ) {
			auto node = sys->getNode(i);
			node->applyForce(-viscosity * node->getVelocity());
		}
	}
}


void SpringForce::solve() {

	if (auto sys = system.lock()) {

		auto firstNode = sys->getNode(first);
		auto secondNode = sys->getNode(second);

	    glm::dvec3 p = firstNode->getPosition() - secondNode->getPosition();
	    glm::dvec3 f = stiffness * (glm::length(p) - length) * p/glm::length(p);

	    firstNode->applyForce(-f);
	    secondNode->applyForce(f);
	}
}


void TorsionSpringForce::solve() {

	if (auto sys = system.lock()) {

		auto leftNode = sys->getNode(left);
		auto centerNode = sys->getNode(center);
		auto rightNode = sys->getNode(right);

		glm::dvec3 p1 = leftNode->getPosition() - centerNode->getPosition();
		glm::dvec3 p2 = rightNode->getPosition() - centerNode->getPosition();

		double cos_theta = glm::dot(p1,p2) / glm::length(p1) / glm::length(p2);
		double theta = ::acos(cos_theta); // всегда в пределах [0;pi]
		glm::dvec3 a;
		glm::dvec3 b;

		if( fabs(theta - defaultAngle) > angleCompareEpsilon ) {
			double k = stiffness * (theta - defaultAngle)/sin(theta);
			a = k/glm::length(p1) * (p2/glm::length(p2) - p1/glm::length(p1) * cos_theta);
			b = k/glm::length(p2) * (p1/glm::length(p1) - p2/glm::length(p2) * cos_theta);
		}
		else {
			// используя разложение синуса и косинуса в ряд Тейлора в окрестности точки theta=pi
			// sin(x) = pi - x, при x -> pi
			// cos(x) = -1,     при x -> pi
			// запишем упрощённые формулы вычисления векторов a и b, лишённые проблемы деления на 0
			glm::dvec3 p12norm = glm::normalize(p1) + glm::normalize(p2);
			a = -stiffness/glm::length(p1) * p12norm;
			b = -stiffness/glm::length(p2) * p12norm;
		}
		leftNode->applyForce(a);
		rightNode->applyForce(b);
		centerNode->applyForce(-a - b);
	}
}


void NodeToNodeForce::solve() {

	if (!enabled)
		return;

	if (auto sys = system.lock()) {

		auto fromNode = sys->getNode(from);
		auto toNode = sys->getNode(to);
	    glm::dvec3 f = toNode->getPosition() - fromNode->getPosition();
	    fromNode->applyForce(glm::normalize(f) * value);
	}
}


void SingleNodeConstantForce::solve() {

	if (!enabled)
		return;

	if (auto sys = system.lock()) {
		auto ptr = sys->getNode(node);
		ptr->applyForce(force);
	}
}
