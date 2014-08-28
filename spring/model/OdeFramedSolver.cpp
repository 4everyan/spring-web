/*
 * OdeFramedSolver.cpp
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#include "OdeFramedSolver.h"


int derivativeFunction(double t, const double y[], double dydt[], void* params) {
    IOdeSystem* sys = static_cast<IOdeSystem*>(params);

    auto inputCoords = gsl::vector_const_view( gsl_vector_const_view_array(y, sys->getDimension()) );
    auto outputDerivs = gsl::vector_view( gsl_vector_view_array(dydt, sys->getDimension()) );

    return sys->getDerivative(t, inputCoords, outputDerivs);
}


OdeFramedSolver::OdeFramedSolver(std::shared_ptr<IOdeSystem> system, double startTime, gsl::base_const_vector& ics)
	: y(system->getDimension())
	, yerr(system->getDimension())
	, lastTime(0.0)
{
    size_t dim = system->getDimension();

    this->system = system;
	this->gslSystem = { &derivativeFunction, nullptr, dim, system.get() };
	this->gslStep.reset( gsl_odeiv_step_alloc(gsl_odeiv_step_rk4, dim), gsl_odeiv_step_free );

	this->y = ics;
	this->lastTime = startTime;
}

bool OdeFramedSolver::frame(double seconds) {

	int status = gsl_odeiv_step_apply(gslStep.get(), lastTime, seconds, y.data(), yerr.data(), nullptr, nullptr, &gslSystem);
	if (status != GSL_SUCCESS)
		return false;
	lastTime += seconds;
	return true;
}
