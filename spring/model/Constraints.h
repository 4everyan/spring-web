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


class ResistanceForce: public IConstraint
{
public:
    ResistanceForce(std::weak_ptr<INodeSystem> system, double viscosity)
        : IConstraint(system), viscosity(viscosity) {}
	void solve() override;
private:
    double viscosity;
};


class SpringConstraint: public IConstraint
{
public:
    SpringConstraint(std::weak_ptr<INodeSystem> system, double length, double stiffness)
        : IConstraint(system)
		, length(length)
		, stiffness(stiffness) {}

	void setFirstNode(const std::shared_ptr<INode>& firstNode) {
		this->firstNode = firstNode;
	}

	void setSecondNode(const std::shared_ptr<INode>& secondNode) {
		this->secondNode = secondNode;
	}

	void solve() override;

private:
    double length;
    double stiffness;
	std::shared_ptr<INode> firstNode;
	std::shared_ptr<INode> secondNode;
};


class TorsionSpringConstraint: public IConstraint
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

	TorsionSpringConstraint(std::weak_ptr<INodeSystem> system, double stiffness)
		: IConstraint(system), stiffness(stiffness) {}

	void setCenter(std::shared_ptr<INode> center) {
		this->center = center;
	}

	void setLeft(std::shared_ptr<INode> left) {
		this->left = left;
	}

	void setRight(std::shared_ptr<INode> right) {
		this->right = right;
	}

	void solve() override;

private:
	double stiffness;
	std::shared_ptr<INode> left;
	std::shared_ptr<INode> center;
	std::shared_ptr<INode> right;
};


class NodeToNodeConstraint: public IActivatableConstraint
{
public:
	NodeToNodeConstraint(std::weak_ptr<INodeSystem> system, double value)
		: IActivatableConstraint(system)
		, value(value) {}

	void setFromNode(const std::shared_ptr<INode>& fromNode) {
		this->fromNode = fromNode;
	}

	void setToNode(const std::shared_ptr<INode>& toNode) {
		this->toNode = toNode;
	}

	void solve() override;

private:
    double value;
	std::shared_ptr<INode> fromNode;
	std::shared_ptr<INode> toNode;
};


class SingleNodeConstantConstraint: public IActivatableConstraint
{
public:
	SingleNodeConstantConstraint(std::weak_ptr<INodeSystem> system, std::shared_ptr<INode> node, glm::dvec3 force)
		: IActivatableConstraint(system), force(force) {}
	void solve() override;
private:
    std::shared_ptr<INode> node;
    glm::dvec3 force;
};


#endif /* CONSTRAINTS_H_ */
