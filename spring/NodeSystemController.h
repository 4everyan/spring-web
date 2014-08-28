/*
 * NodeSystemController.h
 *
 *  Created on: 28 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEMCONTROLLER_H_
#define NODESYSTEMCONTROLLER_H_

#include <memory>
#include "model/INodeSystem.h"
#include "model/OdeFramedSolver.h"


class NodeSystemController
{
public:
	NodeSystemController(std::shared_ptr<INodeSystem> model);
	void frame(double seconds);

private:
	std::shared_ptr<INodeSystem> model;
	std::shared_ptr<OdeFramedSolver> solver;
};

#endif /* NODESYSTEMCONTROLLER_H_ */
