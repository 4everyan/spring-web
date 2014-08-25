/*
 * NodeSystem.h
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEM_H_
#define NODESYSTEM_H_

#include <set>
#include <vector>
#include <memory>
#include "INodeSystem.h"
#include "IConstraint.h"


class NodeSystemBuilder;


class NodeSystem: public INodeSystem {

    friend class NodeSystemBuilder;
	NodeSystem();

public:
	void addConstraint(std::shared_ptr<IConstraint> constraint) {
		constraints.insert(constraint);
	}

    size_t getNumberOfNodes() const override {
    	return nodes.size();
    }

    std::shared_ptr<Node> getNode(size_t id) const override {
    	return nodes.at(id);
    }

private:
	std::vector<std::shared_ptr<Node>> nodes;
	std::set<std::shared_ptr<IConstraint>> constraints;
};

#endif /* NODESYSTEM_H_ */
