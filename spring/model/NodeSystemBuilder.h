/*
 * NodeSystemBuilder.h
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEMBUILDER_H_
#define NODESYSTEMBUILDER_H_

#include "INodeSystem.h"


class NodeSystemBuilder
{
public:
	NodeSystemBuilder();
	~NodeSystemBuilder();

	NodeSystemBuilder& reserve(size_t count);
	NodeSystemBuilder& linkNodes(size_t n1, size_t n2);

	NodeSystemBuilder& fillSpaceByLength(double length);
	NodeSystemBuilder& fillSpaceByNodeCount(size_t nodeCount);

	NodeSystemBuilder& setConstantEqAngle(double equilibriumAngle);
	NodeSystemBuilder& setCalculatedEqAngle();
	NodeSystemBuilder& setConstantEqSpringLength(double equilibriumLength);
	NodeSystemBuilder& setCalculatedEqSpringLength();

	std::shared_ptr<INodeSystem> create();

private:
	class Implementation;
	Implementation* me;
};

#endif /* NODESYSTEMBUILDER_H_ */
