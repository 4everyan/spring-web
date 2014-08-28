/*
 * NodeSystemController.cpp
 *
 *  Created on: 28 авг. 2014 г.
 *      Author: yan
 */

#include "NodeSystemController.h"


NodeSystemController::NodeSystemController(std::shared_ptr<INodeSystem> model)
	:model(model) {

	gsl::vector ics(model->getDimension());
	for (size_t i = 0; i < model->getNumberOfNodes(); ++i) {

		auto pos = model->getNode(i)->getPosition();
		auto vel = model->getNode(i)->getVelocity();

		ics[INodeSystem::CoordsNum * i + INodeSystem::X] = pos.x;
		ics[INodeSystem::CoordsNum * i + INodeSystem::Y] = pos.y;
		ics[INodeSystem::CoordsNum * i + INodeSystem::Z] = pos.z;

		ics[INodeSystem::CoordsNum * i + INodeSystem::U] = vel.x;
		ics[INodeSystem::CoordsNum * i + INodeSystem::V] = vel.y;
		ics[INodeSystem::CoordsNum * i + INodeSystem::W] = vel.z;
	}
	solver.reset(new OdeFramedSolver(model, 0.0, ics));
}

void NodeSystemController::frame(double seconds) {
	solver->frame(seconds);
}
