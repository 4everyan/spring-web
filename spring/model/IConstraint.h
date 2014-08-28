/*
 * IConstraint.h
 *
 *  Created on: 24 авг. 2014 г.
 *      Author: yan
 */

#ifndef ICONSTRAINT_H_
#define ICONSTRAINT_H_

#include <memory>
#include "INodeSystem.h"


class IConstraint
{
public:
	virtual ~IConstraint() {}

	virtual void assignToSystem(std::weak_ptr<INodeSystem> system) {
		this->system = system;
	}
	virtual void enable(bool enabled) {}
	virtual bool isEnabled() const { return true; }
	virtual void solve() = 0;

protected:
	std::weak_ptr<INodeSystem> system;
};


class IActivatableConstraint: public IConstraint
{
public:
	virtual void enable(bool e) override {
		enabled = e;
	}
	virtual bool isEnabled() const override {
		return enabled;
	}

protected:
	bool enabled = false;
};

#endif /* ICONSTRAINT_H_ */
