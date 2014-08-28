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
    ResistanceForce(double viscosity)
        : viscosity(viscosity) {}
	void solve() override;
private:
    double viscosity;
};


class SpringForce: public IConstraint
{
public:
    SpringForce(size_t first, size_t second, double length, double stiffness)
        : first(first)
		, second(second)
		, length(length)
		, stiffness(stiffness) {}

	void solve() override;

private:
	size_t first;
	size_t second;
    double length;
    double stiffness;
};


class TorsionSpringForce: public IConstraint
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

	TorsionSpringForce(size_t left, size_t center, size_t right, double stiffness)
		: left(left)
		, center(center)
		, right(right)
		, stiffness(stiffness) {}

	void solve() override;

private:
	size_t left;
	size_t center;
	size_t right;
	double stiffness;
};


class NodeToNodeForce: public IActivatableConstraint
{
public:
	NodeToNodeForce(size_t from, size_t to, double value)
		: from(from)
		, to(to)
		, value(value) {}

	void solve() override;

private:
	size_t from;
	size_t to;
    double value;
};


class SingleNodeConstantForce: public IActivatableConstraint
{
public:
	SingleNodeConstantForce(size_t node, glm::dvec3 force)
		: node(node)
		, force(force) {}

	void solve() override;

private:
    size_t node;
    glm::dvec3 force;
};


#endif /* CONSTRAINTS_H_ */
