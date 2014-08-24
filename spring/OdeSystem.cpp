/*
 * OdeSystem.cpp
 *
 *  Created on: 30 июня 2014 г.
 *      Author: yan
 */

#include "OdeSystem.h"


int derivativeFunction(double t, const double y[], double dydt[], void* params) {
    IOdeSystem* sys = static_cast<IOdeSystem*>(params);

    auto inputCoords = gsl::vector_const_view( gsl_vector_const_view_array(y, sys->getDimension()) );
    auto outputDerivs = gsl::vector_view( gsl_vector_view_array(dydt, sys->getDimension()) );

    return sys->getDerivative(t, inputCoords, outputDerivs);
}


/*void OdeSolver::solve(gsl::base_const_vector& initial, double start, double stop, double step) {

    size_t dim = system->getDimension();

    const gsl_odeiv_step_type* gslStepType = gsl_odeiv_step_rk4;
    std::shared_ptr<gsl_odeiv_step> gslStep( gsl_odeiv_step_alloc(gslStepType, dim), gsl_odeiv_step_free );
    gsl_odeiv_system gslSystem = { &derivativeFunction, nullptr, dim, system.get() };

    gsl::vector y = initial;
    gsl::vector yerr(dim);
    gsl::vector dyout(dim);

    while( start < stop ) {
        int status = gsl_odeiv_step_apply(gslStep.get(), start, step, y.data(), yerr.data(), nullptr, dyout.data(), &gslSystem);
        if (status != GSL_SUCCESS)
            break;
        if (callback)
        	callback(y);
        start += step;
    }
}


void OdeSolver::setCallback(std::function<void(gsl::base_const_vector&)> callback) {
	this->callback = callback;
}*/


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
