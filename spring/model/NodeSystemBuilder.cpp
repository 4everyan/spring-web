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
	Node(size_t id, double mass, const glm::dvec3& initialPos)
		: id(id)
		, fixed(false)
		, mass(mass)
		, position(initialPos) {}

	size_t getId() const override {
		return id;
	}

	double getMass() const override {
		return mass;
	}

	glm::dvec3 getPosition() const override {
		return position;
	}

	glm::dvec3 getVelocity() const override {
		if (fixed)
			return {0.0, 0.0, 0.0};
		return velocity;
	}

	glm::dvec3 getAcceleration() const override {
		if (fixed)
			return {0.0, 0.0, 0.0};
		return acceleration;
	}

	void fix(bool fixed) override {
		this->fixed = fixed;
	}

	bool isFixed() const override {
		return fixed;
	}

	void applyForce(const glm::dvec3& force) override {
		totalForce += force;
	}

	void resetForce() {
		totalForce = glm::dvec3(0.0);
	}

	void updateWithNewState(const glm::dvec3& _position, const glm::dvec3& _velocity) {
		if (fixed)
			return;
		position = _position;
		velocity = _velocity;
		acceleration = totalForce / mass;
	}

private:
	size_t id;
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
	std::vector<std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<IConstraint>> constraints;
	std::set<std::pair<size_t,size_t>> links;

	std::unique_ptr<SpaceFiller> spaceFiller;

	std::shared_ptr<INodeSystem> create() {
		return nullptr;
	}
};

NodeSystemBuilder::NodeSystemBuilder() {
	me = new Implementation;
}

NodeSystemBuilder::~NodeSystemBuilder() {
	delete me;
}

void NodeSystemBuilder::reserve(size_t count) {
	me->nodes.reserve(count);
}

void NodeSystemBuilder::linkNodes(size_t n1, size_t n2) {
	me->links.insert(std::make_pair(n1, n2));
}

void NodeSystemBuilder::fillSpaceByLength(double length) {
}

void NodeSystemBuilder::fillSpaceByNodeCount(size_t nodeCount) {
}

void NodeSystemBuilder::setConstantEqAngle(double equilibriumAngle) {
}

void NodeSystemBuilder::setCalculatedEqAngle() {
}

void NodeSystemBuilder::setConstantEqSpringLength(double equilibriumLength) {
}

void NodeSystemBuilder::setCalculatedEqSpringLength() {
}

size_t NodeSystemBuilder::addNode(double mass, const glm::dvec3& initialPosition) {
	size_t newId = me->nodes.size();
	auto node = std::make_shared<Node>(newId, mass, initialPosition);
	me->nodes.push_back(node);
	return newId;
}

std::shared_ptr<INodeSystem> NodeSystemBuilder::create() {
	auto system = std::make_shared<NodeSystemImpl>();
	return system;
}
