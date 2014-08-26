/*
 * NodeSystemBuilder.cpp
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#include <set>
#include <list>
#include <vector>
#include <memory>
#include "NodeSystemBuilder.h"
#include "Constraints.h"


class Node: public INode
{
public:
	Node(double mass)
		: fixed(false), mass(mass) {}

	Node(double mass, const glm::dvec3& initialPos)
		: fixed(false)
		, mass(mass)
		, position(initialPos) {}

	Node(double mass, const glm::dvec3& initialPos, const glm::dvec3& initialVel)
		: fixed(false)
		, mass(mass)
		, position(initialPos)
		, velocity(initialVel) {}

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
	bool fixed;
	double mass;
	glm::dvec3 totalForce;
	glm::dvec3 position, velocity, acceleration;
};


class NodeSystemImpl: public INodeSystem
{
public:
	void addConstraint(std::shared_ptr<IConstraint> constraint) {
		constraints.insert(constraint);
	}

    size_t getNumberOfNodes() const override {
    	return nodes.size();
    }

    std::shared_ptr<INode> getNode(size_t id) const override {
    	return nodes.at(id);
    }

public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<IConstraint>> constraints;
};


class SpaceFiller
{
public:
	virtual ~SpaceFiller() {}
	virtual std::list<glm::dvec3> fill(const glm::dvec3& from, const glm::dvec3& to) = 0;
};


class FillByLength: public SpaceFiller
{
};


class FillByPointNumber: public SpaceFiller
{
};


class NodeSystemBuilder::Implementation
{
public:
	std::vector<std::shared_ptr<INode>> nodes;
	std::set<std::shared_ptr<INode>> uniqueNodes;

	std::set<std::shared_ptr<IConstraint>> constraints;
	std::set<std::pair<size_t,size_t>> links;

	std::unique_ptr<SpaceFiller> spaceFiller;
};

NodeSystemBuilder::NodeSystemBuilder() {
	me = new Implementation;
}

NodeSystemBuilder::~NodeSystemBuilder() {
	delete me;
}

NodeSystemBuilder& NodeSystemBuilder::reserve(size_t count) {
	me->nodes.reserve(count);
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::linkNodes(size_t n1, size_t n2) {
	me->links.insert(std::make_pair(n1, n2));
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::fillSpaceByLength(double length) {
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::fillSpaceByNodeCount(size_t nodeCount) {
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::setConstantEqAngle(double equilibriumAngle) {
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::setCalculatedEqAngle() {
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::setConstantEqSpringLength(double equilibriumLength) {
	return *this;
}

NodeSystemBuilder& NodeSystemBuilder::setCalculatedEqSpringLength() {
	return *this;
}

std::shared_ptr<INodeSystem> NodeSystemBuilder::create() {
	auto system = std::make_shared<NodeSystemImpl>();
	return system;
}
