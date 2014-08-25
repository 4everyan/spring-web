/*
 * Constraints.h
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#ifndef CONSTRAINTS_H_
#define CONSTRAINTS_H_

#include <memory>
#include <glm/glm.hpp>
#include "IConstraint.h"
#include "INodeSystem.h"
#include "Node.h"


class ResistanceForce: public IConstraint
{
public:
    ResistanceForce(std::weak_ptr<INodeSystem> system, double viscosity)
        : IConstraint(system), viscosity(viscosity) {}
	void solve() override;
private:
    double viscosity;
};


class SpringForce: public IConstraint
{
public:
    SpringForce(std::weak_ptr<INodeSystem> system, double length, double stiffness)
        : IConstraint(system)
		, length(length)
		, stiffness(stiffness) {}

	void setFirstNode(const std::shared_ptr<Node>& firstNode) {
		this->firstNode = firstNode;
	}

	void setSecondNode(const std::shared_ptr<Node>& secondNode) {
		this->secondNode = secondNode;
	}

	void solve() override;

private:
    double length;
    double stiffness;
	std::shared_ptr<Node> firstNode;
	std::shared_ptr<Node> secondNode;
};


class TorsionSpringForce: public IConstraint
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

	TorsionSpringForce(std::weak_ptr<INodeSystem> system, double stiffness)
		: IConstraint(system), stiffness(stiffness) {}

	void setCenter(std::shared_ptr<Node> center) {
		this->center = center;
	}

	void setLeft(std::shared_ptr<Node> left) {
		this->left = left;
	}

	void setRight(std::shared_ptr<Node> right) {
		this->right = right;
	}

	void solve() override;

private:
	double stiffness;
	std::shared_ptr<Node> left;
	std::shared_ptr<Node> center;
	std::shared_ptr<Node> right;
};


class NodeToNodeForce: public IActivatableConstraint
{
public:
	NodeToNodeForce(std::weak_ptr<INodeSystem> system, double value)
		: IActivatableConstraint(system)
		, value(value) {}

	void setFromNode(const std::shared_ptr<Node>& fromNode) {
		this->fromNode = fromNode;
	}

	void setToNode(const std::shared_ptr<Node>& toNode) {
		this->toNode = toNode;
	}

	void solve() override;

private:
    double value;
	std::shared_ptr<Node> fromNode;
	std::shared_ptr<Node> toNode;
};


class SingleNodeConstantForce: public IActivatableConstraint
{
public:
	SingleNodeConstantForce(std::weak_ptr<INodeSystem> system, std::shared_ptr<Node> node, glm::dvec3 force)
		: IActivatableConstraint(system), force(force) {}
	void solve() override;
private:
    std::shared_ptr<Node> node;
    glm::dvec3 force;
};


#endif /* CONSTRAINTS_H_ */
