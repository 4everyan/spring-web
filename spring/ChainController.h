/*
 * ChainController.h
 *
 *  Created on: 09 авг. 2014 г.
 *      Author: yan
 */

#ifndef CHAINCONTROLLER_H_
#define CHAINCONTROLLER_H_

#include <memory>
#include "ChainSystem.h"
#include "OdeSystem.h"


class ChainController
{
public:
	ChainController(StartConfig* config);

	void frame(double seconds);
	std::shared_ptr<ChainSystem> getSystem();

private:
	std::shared_ptr<ChainSystem> system;
	std::shared_ptr<OdeFramedSolver> solver;
};

#endif /* CHAINCONTROLLER_H_ */
