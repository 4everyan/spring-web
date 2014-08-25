/*
 * NodeSystemBuilder.h
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEMBUILDER_H_
#define NODESYSTEMBUILDER_H_

#include <set>
#include <vector>
#include <memory>
#include "INodeSystem.h"
#include "IConstraint.h"


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


class NodeSystemBuilder
{
public:
	enum EquilibriumAngle {
		Constant,
		Calculated,
	};

	NodeSystemBuilder& reserve(size_t count) {
		nodes.reserve(count);
		return *this;
	}

	NodeSystemBuilder& addNode(std::shared_ptr<Node> node) {
		if (uniqueNodes.find(node) != uniqueNodes.end())
			return *this;
		uniqueNodes.insert(node);
		nodes.push_back(node);
		return *this;
	}

	NodeSystemBuilder& linkNodes(size_t n1, size_t n2) {
		links.insert(std::make_pair(n1, n2));
		return *this;
	}

	NodeSystemBuilder& fillSpaceByLength(double length) {
		//TODO: add space filler here
		return *this;
	}

	NodeSystemBuilder& fillSpaceByNodeCount(size_t nodeCount) {
		//TODO: add space filler here
		return *this;
	}

	std::shared_ptr<INodeSystem> create() {
		return nullptr;
	}

private:
	std::vector<std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<Node>> uniqueNodes;
	std::set<std::shared_ptr<IConstraint>> constraints;
	std::set<std::pair<size_t,size_t>> links;
	std::unique_ptr<SpaceFiller> spaceFiller;
};

#endif /* NODESYSTEMBUILDER_H_ */
