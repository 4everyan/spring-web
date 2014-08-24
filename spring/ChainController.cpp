/*
 * ChainController.cpp
 *
 *  Created on: 09 авг. 2014 г.
 *      Author: yan
 */

#include "ChainController.h"


ChainController::ChainController(StartConfig* config)
{
}

void ChainController::frame(double seconds) {
	solver->frame(seconds);
}

std::shared_ptr<ChainSystem> ChainController::getSystem() {
	return system;
}
