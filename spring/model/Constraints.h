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


class ResistanceConstraint: public IConstraint
{
public:
    ResistanceConstraint(std::weak_ptr<INodeSystem> system, double viscosity)
        : IConstraint(system), viscosity(viscosity) {}
	void solve() override;
private:
    double viscosity;
};


class SpringConstraint: public IConstraint
{
public:
    SpringConstraint(std::weak_ptr<INodeSystem> system, size_t first, size_t second, double length, double stiffness)
        : IConstraint(system)
		, first(first)
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


class TorsionSpringConstraint: public IConstraint
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

	TorsionSpringConstraint(std::weak_ptr<INodeSystem> system, size_t left, size_t center, size_t right, double stiffness)
		: IConstraint(system)
		, left(left)
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


class NodeToNodeConstraint: public IActivatableConstraint
{
public:
	NodeToNodeConstraint(std::weak_ptr<INodeSystem> system, size_t from, size_t to, double value)
		: IActivatableConstraint(system)
		, from(from)
		, to(to)
		, value(value) {}

	void solve() override;

private:
	size_t from;
	size_t to;
    double value;
};


class SingleNodeConstantConstraint: public IActivatableConstraint
{
public:
	SingleNodeConstantConstraint(std::weak_ptr<INodeSystem> system, size_t node, glm::dvec3 force)
		: IActivatableConstraint(system)
		, node(node)
		, force(force) {}

	void solve() override;

private:
    size_t node;
    glm::dvec3 force;
};


#endif /* CONSTRAINTS_H_ */
