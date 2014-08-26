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

	void reserve(size_t count);
	void linkNodes(size_t n1, size_t n2);

	void fillSpaceByLength(double length);
	void fillSpaceByNodeCount(size_t nodeCount);

	void setConstantEqAngle(double equilibriumAngle);
	void setCalculatedEqAngle();
	void setConstantEqSpringLength(double equilibriumLength);
	void setCalculatedEqSpringLength();

	size_t addNode(double mass, const glm::dvec3& initialPosition);
	std::shared_ptr<INodeSystem> create();

private:
	class Implementation;
	Implementation* me;
};

#endif /* NODESYSTEMBUILDER_H_ */
