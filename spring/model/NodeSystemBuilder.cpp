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
	Node(size_t id, double mass, glm::dvec3 pos, glm::dvec3 vel)
		: id(id)
		, fixed(false)
		, mass(mass)
		, position(pos)
		, velocity(vel) {}

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


struct BuildNode {

	std::set<size_t> next;
	std::set<size_t> prev;
	size_t id;

	BuildNode(size_t id): id(id) {}
};


class NodeSystemBuilder::Implementation
{
public:
	std::vector<std::shared_ptr<BuildNode>> buildNodes;
	std::set<std::pair<size_t,size_t>> links;
	std::shared_ptr<NodeSystemImpl> model;
	double subNodeMass = 1.0;
};


NodeSystemBuilder::NodeSystemBuilder() {
	me = new Implementation;
	me->model.reset(new NodeSystemImpl);
}

NodeSystemBuilder::~NodeSystemBuilder() {
	delete me;
}

void NodeSystemBuilder::linkNodes(size_t n1, size_t n2) {
	me->links.insert(std::make_pair(n1,n2));
}

size_t NodeSystemBuilder::addNode(double mass, glm::dvec3 pos, glm::dvec3 vel) {

	size_t newId = me->buildNodes.size();
	auto buildNode = std::make_shared<BuildNode>(newId);
	auto modelNode = std::make_shared<Node>(newId, mass, pos, vel);

	me->buildNodes.push_back(buildNode);
	me->model->nodes.push_back(modelNode);

	return newId;
}

size_t NodeSystemBuilder::addNode(double mass, glm::dvec3 pos) {
	return addNode(mass, pos, glm::dvec3(0.0));
}

std::list<glm::dvec3> fillSpace(glm::dvec3 from, glm::dvec3 to, size_t subNodes) {

	std::list<glm::dvec3> list;
	glm::dvec3 segment = (from - to) / (subNodes + 1.0);
	for (size_t i = 1; i <= subNodes; ++i)
		list.push_back(from + segment * (double) i);
	return list;
}

void NodeSystemBuilder::setSubNodeMass(double mass) {
	me->subNodeMass = mass;
}

std::shared_ptr<INodeSystem> NodeSystemBuilder::create() {

	auto system = std::make_shared<NodeSystemImpl>();
	for (auto& link: me->links) {

		const int subNodeCount = 2;

		auto nodeFrom = me->model->getNode(link.first);
		auto nodeTo = me->model->getNode(link.second);
		auto points = fillSpace(nodeFrom->getPosition(), nodeTo->getPosition(), subNodeCount);

		std::vector<size_t> subNodeIds;
		for (glm::dvec3& point: points) {
			subNodeIds.push_back(addNode(me->subNodeMass, point));
		}

		for (size_t i = 0; i < subNodeIds.size() - 1; ++i) {
			auto bnode = me->buildNodes[i];
			bnode->next.insert(subNodeIds[i + 1]);
		}
		for (size_t i = 1; i < subNodeIds.size(); ++i) {
			auto bnode = me->buildNodes[i];
			bnode->prev.insert(subNodeIds[i - 1]);
		}
		auto bnodeFrom = me->buildNodes[link.first];
		auto bnodeTo = me->buildNodes[link.second];

		bnodeFrom->next.insert(subNodeIds.front());
		bnodeTo->prev.insert(subNodeIds.back());
	}
	return system;
}
