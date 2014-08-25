/*
 * XxxConfig.h
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#ifndef XXXCONFIG_H_
#define XXXCONFIG_H_


class IChainSystem
{
public:
	enum CoordsLayout {
		X, Y, Z,
		U, V, W,
		CoordsNum,
	};

	virtual ~IChainSystem() {}

    // returns number of nodes
    virtual size_t getNumberOfNodes() const = 0;

    // returns the chain length in normal state
    virtual double getChainLength() const = 0;

    // returns node mass
    virtual double getNodeMass(size_t node) const = 0;

    // returns current total force affecting node
    virtual glm::dvec3 getNodeForce(size_t node) const = 0;
};



class StartConfig
{
public:
	virtual ~StartConfig() {}
	virtual gsl::vector startCoordinates(size_t nodeCount, double freeChainLength) = 0;
};


class DirectLineConfig: public StartConfig
{
public:
	DirectLineConfig()
		: baseNode(0)
		, direction(1.0, 0.0, 0.0) {}

	DirectLineConfig(size_t baseNode, const glm::dvec3& baseNodePosition, const glm::dvec3& direction)
		: baseNode(baseNode)
		, baseNodePosition(baseNodePosition)
		, direction(direction) {}

	void setBaseNode(size_t n) {
		baseNode = n;
	}

	void setBaseNodePosition(const glm::dvec3& pos) {
		baseNodePosition = pos;
	}

	void setDirection(const glm::dvec3& dir) {
		direction = dir;
	}

	gsl::vector startCoordinates(size_t nodeCount, double freeChainLength) override
	{
    	if (baseNode < 0 || baseNode >= nodeCount)
    		baseNode = 0;

    	size_t stride = IChainSystem::CoordsNum;
    	gsl::vector ics(nodeCount * stride);
    	direction = glm::normalize(direction);

        auto x = gsl::vector_view(ics, IChainSystem::X, stride, nodeCount);
        auto y = gsl::vector_view(ics, IChainSystem::Y, stride, nodeCount);
    	auto z = gsl::vector_view(ics, IChainSystem::Z, stride, nodeCount);
        double segmentLength = freeChainLength / (nodeCount - 1);

        for (int i = 0; i < (int) nodeCount; ++i) {
        	glm::dvec3 pos = baseNodePosition + direction * (segmentLength * (i - (int) baseNode));
        	x[i] = pos.x;
        	y[i] = pos.y;
        	z[i] = pos.z;
        }
    	return ics;
	}

private:
	size_t baseNode;
	glm::dvec3 baseNodePosition;
	glm::dvec3 direction;
};


class PointToPointConfig: public StartConfig
{
public:
	PointToPointConfig()
		: direction(1.0, 0.0, 0.0) {}

	PointToPointConfig(const glm::dvec3& start, const glm::dvec3& end)
		: start(start)
		, direction(end - start) {}

	gsl::vector startCoordinates(size_t nodeCount, double /*unused*/) override
	{
		size_t stride = IChainSystem::CoordsNum;
		gsl::vector ics(nodeCount * stride);

		auto x = gsl::vector_view(ics, IChainSystem::X, stride, nodeCount);
		auto y = gsl::vector_view(ics, IChainSystem::Y, stride, nodeCount);
		auto z = gsl::vector_view(ics, IChainSystem::Z, stride, nodeCount);

		glm::dvec3 segment = direction / (double)(nodeCount - 1);

        for (int i = 0; i < (int) nodeCount; ++i) {
        	glm::dvec3 pos = start + segment * (double)i;
        	x[i] = pos.x;
        	y[i] = pos.y;
        	z[i] = pos.z;
        }
        return ics;
	}

private:
	glm::dvec3 start;
	glm::dvec3 direction;
};


#endif /* XXXCONFIG_H_ */
