/*
 * INodeSystem.h
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#ifndef INODESYSTEM_H_
#define INODESYSTEM_H_

#include <glm/glm.hpp>
#include "OdeSystem.h"
#include "INode.h"


class INodeSystem: public IOdeSystem
{
public:
	enum CoordsLayout {
		X, Y, Z,
		U, V, W,
		CoordsNum,
	};

	virtual ~INodeSystem() {}

    virtual size_t getNumberOfNodes() const = 0;
    virtual std::shared_ptr<INode> getNode(size_t id) const = 0;
};

#endif /* INODESYSTEM_H_ */
