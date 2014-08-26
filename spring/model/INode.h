/*
 * INode.h
 *
 *  Created on: 26 авг. 2014 г.
 *      Author: yan
 */

#ifndef INODE_H_
#define INODE_H_

#include <glm/glm.hpp>


class INode
{
public:
	virtual ~INode() {}

	virtual void applyForce(const glm::dvec3& force) = 0;
	virtual void fix(bool fixed) = 0;
	virtual bool isFixed() const = 0;

	virtual glm::dvec3 getPosition() const = 0;
	virtual glm::dvec3 getVelocity() const = 0;
	virtual glm::dvec3 getAcceleration() const = 0;
};

#endif /* INODE_H_ */