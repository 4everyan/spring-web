/*
 * NodeSystemBuilder.h
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEMBUILDER_H_
#define NODESYSTEMBUILDER_H_

#include <glm/glm.hpp>
#include "INodeSystem.h"


class NodeSystemBuilder
{
public:
	NodeSystemBuilder();
	~NodeSystemBuilder();

	size_t addNode(double mass, glm::dvec3 pos, glm::dvec3 vel);
	size_t addNode(double mass, glm::dvec3 pos);

	void setSubNodeMass(double mass);
	void linkNodes(size_t n1, size_t n2);

	std::shared_ptr<INodeSystem> create();

private:
	class Implementation;
	Implementation* me;
};

#endif /* NODESYSTEMBUILDER_H_ */
