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
#include "OdeFramedSolver.h"


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
	virtual size_t getNumberOfNodes() const override {
    	return nodes.size();
    }

    virtual std::shared_ptr<INode> getNode(size_t id) const override {
    	return nodes.at(id);
    }

    virtual size_t getNumberOfLinks() const override {
    	return links.size();
    }

    virtual std::pair<size_t,size_t> getLink(size_t id) const override {
    	return links.at(id);
    }

    // calculate derivative vector and store it in outputDerivs
    virtual int getDerivative(double t, gsl::base_const_vector& inputCoords, gsl::base_vector& outputDerivs) {

    	//std::cout << inputCoords;
    	//throw 0;

        // nullifying node forces
        for (auto& node: nodes) {
        	node->resetForce();
        }
        // applying forces
        for (auto& constraint: constraints) {
        	constraint->solve();
        }
        glm::dvec3 position;
        glm::dvec3 velocity;

        size_t stride = INodeSystem::CoordsNum;
        size_t size = nodes.size();

        auto x = gsl::vector_const_view(inputCoords, X, stride, size);
        auto y = gsl::vector_const_view(inputCoords, Y, stride, size);
        auto z = gsl::vector_const_view(inputCoords, Z, stride, size);
        auto u = gsl::vector_const_view(inputCoords, U, stride, size);
        auto v = gsl::vector_const_view(inputCoords, V, stride, size);
        auto w = gsl::vector_const_view(inputCoords, W, stride, size);

        auto dx = gsl::vector_view(outputDerivs, X, stride, size);
        auto dy = gsl::vector_view(outputDerivs, Y, stride, size);
        auto dz = gsl::vector_view(outputDerivs, Z, stride, size);
        auto du = gsl::vector_view(outputDerivs, U, stride, size);
        auto dv = gsl::vector_view(outputDerivs, V, stride, size);
        auto dw = gsl::vector_view(outputDerivs, W, stride, size);

        glm::dvec3 derivs;

        for (size_t n = 0; n < nodes.size(); ++n) {

            nodes[n]->updateWithNewState(glm::dvec3(x[n], y[n], z[n]), glm::dvec3(u[n], v[n], w[n]));

        	auto vel = nodes[n]->getVelocity();
        	auto acc = nodes[n]->getAcceleration();

            dx[n] = vel.x;
            dy[n] = vel.y;
            dz[n] = vel.z;

            du[n] = acc.x;
            dv[n] = acc.y;
            dw[n] = acc.z;
        }
        return GSL_SUCCESS;
    }

    // returns the dimension of the system
    virtual size_t getDimension() const {
    	return nodes.size() * CoordsNum;
    }

public:
	std::vector<std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<IConstraint>> constraints;
	std::vector<std::pair<size_t,size_t>> links;
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

	double defaultSubNodeMass = 1.0;
	double defaultSpringStiffness = 1.0;
	double defaultTorsionSpringStiffness = 1.0;

public:
	void linkBuildNodes(size_t n1, size_t n2) {
		buildNodes[n1]->next.insert(n2);
		buildNodes[n2]->prev.insert(n1);
		model->links.push_back(std::make_pair(n1,n2));
	}

	void putSpring(size_t n1, size_t n2) {
		auto p1 = model->getNode(n1);
		auto p2 = model->getNode(n2);
		double length = glm::length(p1->getPosition() - p2->getPosition());
		auto spring = std::make_shared<SpringForce>(n1, n2, length, defaultSpringStiffness);
		model->constraints.insert(spring);
	}

	void putTorsionSpring(size_t n1, size_t n2, size_t n3) {
		auto torsionSpring = std::make_shared<TorsionSpringForce>(n1, n2, n3, defaultTorsionSpringStiffness);
		model->constraints.insert(torsionSpring);
	}
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
	glm::dvec3 segment = (to - from) / (subNodes + 1.0);
	for (size_t i = 1; i <= subNodes; ++i)
		list.push_back(from + segment * (double) i);
	return list;
}

void NodeSystemBuilder::setSubNodeMass(double mass) {
	me->defaultSubNodeMass = mass;
}

void NodeSystemBuilder::addConstraint(std::shared_ptr<IConstraint> constraint) {
	me->model->constraints.insert(constraint);
}

std::shared_ptr<INodeSystem> NodeSystemBuilder::create() {

	for (auto& link: me->links) {

		const int subNodeCount = 2;

		// both linked nodes
		auto nodeFrom = me->model->getNode(link.first);
		auto nodeTo = me->model->getNode(link.second);

		// fill space between two nodes by adding a number of extra nodes
		auto points = fillSpace(nodeFrom->getPosition(), nodeTo->getPosition(), subNodeCount);
		std::vector<size_t> subNodeIds;

		for (glm::dvec3& point: points) {
			subNodeIds.push_back(addNode(me->defaultSubNodeMass, point));
		}
		// link all nodes to make a chain
		for (size_t i = 0; i < subNodeIds.size() - 1; ++i) {
			me->linkBuildNodes(subNodeIds[i], subNodeIds[i + 1]);
			me->putSpring(subNodeIds[i], subNodeIds[i + 1]);
		}
		// link main nodes with head and tail of list of sub-nodes
		me->linkBuildNodes(link.first, subNodeIds.front());
		me->linkBuildNodes(subNodeIds.back(), link.second);

		me->putSpring(link.first, subNodeIds.front());
		me->putSpring(subNodeIds.back(), link.second);
	}
	for (auto& center: me->buildNodes) {
		for (auto& left: center->prev) {
			for (auto& right: center->next) {
				me->putTorsionSpring(left, center->id, right);
			}
		}
	}
	for (auto& constraint: me->model->constraints)
		constraint->assignToSystem(me->model);

	return me->model;
}
