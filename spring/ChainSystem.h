/*
 *  Created on: 28 июня 2014 г.
 *      Author: yan
 */

#ifndef CHAINSYSTEM_H_
#define CHAINSYSTEM_H_

#include <vector>
#include <list>
#include <glm/glm.hpp>
#include "gsl_helper.h"
#include "OdeSystem.h"



class IForce
{
public:
    virtual ~IForce() {}

    using SaveForceFunction = std::function<void(size_t,glm::dvec3)>;

    virtual void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) = 0;
    virtual void enable(bool enabled) {}
    virtual bool isEnabled() const { return false; }
};


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


class ChainSystem: public IOdeSystem
{
public:
	enum CoordsLayout {
		X, Y, Z, U, V, W,
	};

	ChainSystem(size_t nodeCount, double mass, double length)
	{
        nodes.resize(nodeCount);
        mass /= nodeCount;
        for( auto& n: nodes ) {
        	n.fixed = false;
        	n.mass = mass;
        	n.totalForce = glm::dvec3(0.0);
        }
    	chainLength = length;
    }

    void addForce(std::shared_ptr<IForce> force) {
    	forces.push_back(force);
    }

    void fixNode(size_t node, bool fix) {
    	if( node < 0 || node >= nodes.size() )
    		return;
    	nodes[node].fixed = fix;
    }

    gsl::vector icsBaseNodeAndDirection(size_t baseNode, glm::dvec3 baseNodePosition, glm::dvec3 direction)
    {
    	if( baseNode < 0 || baseNode >= nodes.size() )
    		baseNode = 0;
    	gsl::vector ics(getDimension());
    	direction = glm::normalize(direction);

    	size_t stride = getNumCoordsPerNode();
    	size_t size = getNodeCount();

        auto x = gsl::vector_view(ics, X, stride, size);
        auto y = gsl::vector_view(ics, Y, stride, size);
    	auto z = gsl::vector_view(ics, Z, stride, size);
        double segmentLength = chainLength / (nodes.size() - 1);

    	for( size_t i=0; i<nodes.size(); ++i ) {
    		glm::dvec3 pos = baseNodePosition + direction * ( segmentLength * ((long)i - (long)baseNode) );
    		x[i] = pos.x;
    		y[i] = pos.y;
    		z[i] = pos.z;
    	}
    	return ics;
    }

    int getDerivative(double t, gsl::base_const_vector& inputCoords, gsl::base_vector& outputDerivs) override {

    	//
        // nullifying node forces
    	//
        for( auto& node: nodes ) {
            node.totalForce = glm::dvec3(0.0);
        }
        //
        // applying node forces
        //
        for( auto force: forces ) {
            force->apply(t, inputCoords, [&](size_t node, glm::dvec3 nodeForce) {
                nodes[node].totalForce += nodeForce;
            });
        }
        //
        // calculate derivatives
        //
        glm::dvec3 position;
        glm::dvec3 velocity;

        size_t stride = getNumCoordsPerNode();
        size_t size = getNodeCount();

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

        	if( nodes[n].fixed ) {
        		dx[n] =	dy[n] =	dz[n] = 0.0;
        		du[n] = dv[n] = dw[n] = 0.0;
        		continue;
        	}
        	derivs = nodes[n].totalForce/nodes[n].mass;
            dx[n] = u[n];
            dy[n] = v[n];
            dz[n] = w[n];
            du[n] = derivs[0];
            dv[n] = derivs[1];
            dw[n] = derivs[2];
        }
        return GSL_SUCCESS;
    }

    size_t getDimension() const override {
        return nodes.size() * getNumCoordsPerNode();
    }

    size_t getNumCoordsPerNode() const {
        return 6;
    }

    size_t getNodeCount() const {
        return nodes.size();
    }

    double getMass(size_t node) const {
        return nodes[node].mass;
    }

    double getLength() const {
    	return chainLength;
    }

private:
    struct Node {
    	bool fixed;
        double mass;
        glm::dvec3 totalForce;
    };
    std::vector<Node> nodes;
    std::list<std::shared_ptr<IForce>> forces;
    double chainLength;
};


class ResistanceForce: public IForce
{
public:
    ResistanceForce(ChainSystem* _system, double _viscosity)
        : system(_system), viscosity(_viscosity) {}

    void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {
        size_t nodeCount = system->getNodeCount();

        size_t stride = system->getNumCoordsPerNode();
        size_t size = system->getNodeCount();

        auto u = gsl::vector_const_view(coords, ChainSystem::U, stride, size);
        auto v = gsl::vector_const_view(coords, ChainSystem::V, stride, size);
        auto w = gsl::vector_const_view(coords, ChainSystem::W, stride, size);

        glm::dvec3 vel;

        for( size_t n = 0; n < nodeCount; ++n ) {
        	vel = glm::dvec3{ u[n], v[n], w[n] };
        	saveForceValue(n, -viscosity * vel);
        }
    }
private:
    ChainSystem* system;
    double viscosity;
};


class SpringForce: public IForce
{
public:
    SpringForce(ChainSystem* _system, size_t _n1, size_t _n2, double _length, double _stiffness)
        : system(_system)
		, firstNode(_n1)
		, secondNode(_n2)
		, length(_length)
		, stiffness(_stiffness) {}

    void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {

        if( firstNode == secondNode )
            return;

        size_t nodeCount = system->getNodeCount();
        if( firstNode < 0 || firstNode >= nodeCount )
            return;

        if( secondNode < 0 || secondNode >= nodeCount )
            return;

        size_t stride = system->getNumCoordsPerNode();
        size_t size = system->getNodeCount();

        auto x = gsl::vector_const_view(coords, ChainSystem::X, stride, size);
        auto y = gsl::vector_const_view(coords, ChainSystem::Y, stride, size);
        auto z = gsl::vector_const_view(coords, ChainSystem::Z, stride, size);

        glm::dvec3 r1{ x[firstNode], y[firstNode], z[firstNode] };
        glm::dvec3 r2{ x[secondNode], y[secondNode], z[secondNode] };
        glm::dvec3 p = r1 - r2;
        glm::dvec3 f = stiffness * (glm::length(p) - length) * p/glm::length(p);

        saveForceValue(firstNode, -f);
        saveForceValue(secondNode, f);
    }

private:
    ChainSystem* system;
    size_t firstNode;
    size_t secondNode;
    double length;
    double stiffness;
};


class TorsionSpringForce: public IForce
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

    TorsionSpringForce(ChainSystem* _system, size_t _node, double _stiffness)
        : system(_system), node(_node), stiffness(_stiffness) {}

    void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {
        size_t nodeCount = system->getNodeCount();
        if( node-1 < 0 || node+1 >= nodeCount )
        	return;

        size_t stride = system->getNumCoordsPerNode();
        size_t size = system->getNodeCount();

        auto x = gsl::vector_const_view(coords, ChainSystem::X, stride, size);
        auto y = gsl::vector_const_view(coords, ChainSystem::Y, stride, size);
        auto z = gsl::vector_const_view(coords, ChainSystem::Z, stride, size);

        glm::dvec3 r1 = glm::dvec3( x[node-1], y[node-1], z[node-1] );
        glm::dvec3 r3 = glm::dvec3( x[node+1], y[node+1], z[node+1] );
        glm::dvec3 r2 = glm::dvec3( x[node], y[node], z[node] );
        glm::dvec3 p1 = r1 - r2;
        glm::dvec3 p2 = r3 - r2;

        double cos_theta = glm::dot(p1,p2) / glm::length(p1) / glm::length(p2);
        double theta = ::acos(cos_theta); // всегда в пределах [0;pi]
        glm::dvec3 a;
        glm::dvec3 b;

        if( fabs(theta - M_PI) > angleCompareEpsilon ) {
        	double k = stiffness * (theta - M_PI)/sin(theta);
        	a = k/glm::length(p1) * (p2/glm::length(p2) - p1/glm::length(p1) * cos_theta);
        	b = k/glm::length(p2) * (p1/glm::length(p1) - p2/glm::length(p2) * cos_theta);
        }
        else {
        	// используя разложение синуса и косинуса в ряд Тейлора в окрестности точки theta=pi
        	// sin(x) = pi - x, при x -> pi
        	// cos(x) = -1,     при x -> pi
        	// запишем упрощённые формулы вычисления векторов a и b, лишённые проблемы деления на 0
        	glm::dvec3 p12norm = glm::normalize(p1) + glm::normalize(p2);
        	a = -stiffness/glm::length(p1) * p12norm;
        	b = -stiffness/glm::length(p2) * p12norm;
        }
        saveForceValue(node-1, a);
        saveForceValue(node+1, b);
        saveForceValue(node, -a-b);
    }
private:
    ChainSystem* system;
    size_t node;
    double stiffness;
};


class NodeToNodeForce: public IForce
{
public:
	NodeToNodeForce(ChainSystem* _system, size_t _nodeFrom, size_t _nodeTo, double _value) {
		system = _system;
		enabled = false;
		value = _value;
		from = _nodeFrom;
		to = _nodeTo;
	}
	void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {

		if( !enabled )
			return;

        size_t stride = system->getNumCoordsPerNode();
        size_t size = system->getNodeCount();

        auto x = gsl::vector_const_view(coords, ChainSystem::X, stride, size);
        auto y = gsl::vector_const_view(coords, ChainSystem::Y, stride, size);
        auto z = gsl::vector_const_view(coords, ChainSystem::Z, stride, size);

        glm::dvec3 p1( x[from], y[from], z[from] );
        glm::dvec3 p2( x[to], y[to], z[to] );
        glm::dvec3 f = p2 - p1;

        saveForceValue(from, glm::normalize(f) * value);
	}
	void enable(bool e) override {
		enabled = e;
	}
    bool isEnabled() const override {
    	return enabled;
    }
private:
    ChainSystem* system;
    bool enabled;
    double value;
	size_t from;
	size_t to;
};


class BendingForce: public IForce
{
public:
	const double angleCompareEpsilon = 0.001 * M_PI;

	BendingForce(ChainSystem* _system, glm::dvec3 axis, double value) {
		system = _system;
		zaxis = glm::normalize(axis);
		forceValue = value;
		enabled = false;
	}

	void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {

		if( system->getNodeCount() < 3 )
			return;

		if( !enabled )
			return;

        size_t stride = system->getNumCoordsPerNode();
        size_t size = system->getNodeCount();

        auto x = gsl::vector_const_view(coords, ChainSystem::X, stride, size);
        auto y = gsl::vector_const_view(coords, ChainSystem::Y, stride, size);
        auto z = gsl::vector_const_view(coords, ChainSystem::Z, stride, size);

        size_t nc = system->getNodeCount();
        glm::dvec3 p1( x[nc-2], y[nc-2], z[nc-2] );
        glm::dvec3 p2( x[nc-1], y[nc-1], z[nc-1] );

        glm::dvec3 xaxis = glm::normalize( p2 - p1 );
        glm::dvec3 yaxis = glm::normalize( glm::cross(zaxis,xaxis) );

        saveForceValue(nc-1, yaxis * forceValue);
	}
	void enable(bool e) override {
		enabled = e;
	}
    bool isEnabled() const override {
    	return enabled;
    }
	void setValue(double value) {
		forceValue = value;
	}
private:
	ChainSystem* system;
    glm::dvec3 zaxis;
    double forceValue;
    bool enabled;
};


class SingleNodeConstantForce: public IForce
{
public:
	SingleNodeConstantForce(ChainSystem* _system, size_t _node, glm::dvec3 _force)
		: system(_system), node(_node), force(_force), enabled(false) {}

	void apply(double t, gsl::base_const_vector& coords, SaveForceFunction saveForceValue) override {
		if( enabled )
			saveForceValue(node, force);
	}
	void enable(bool e) override {
		enabled = e;
	}
    bool isEnabled() const override {
    	return enabled;
    }
private:
    ChainSystem* system;
    size_t node;
    glm::dvec3 force;
    bool enabled;
};

#endif /* CHAINSYSTEM_H_ */
