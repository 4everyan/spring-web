/*
 * OdeSystem.h
 *
 *  Created on: 28 июня 2014 г.
 *      Author: yan
 */

#ifndef ODESYSTEM_H_
#define ODESYSTEM_H_


#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_linalg.h>
#include "gsl_helper.h"


class IOdeSystem
{
public:
    virtual ~IOdeSystem() {}
    virtual int getDerivative(double t, gsl::base_const_vector& inputCoords, gsl::base_vector& outputDerivs) = 0;
    virtual size_t getDimension() const = 0;
};


/*class OdeSolver
{
public:
    OdeSolver(std::shared_ptr<IOdeSystem> _system)
		: system(_system) {}
private:
    void solve(gsl::base_const_vector& initial, double start, double stop, double step);
    void setCallback(std::function<void(gsl::base_const_vector&)> callback);

private:
    std::shared_ptr<IOdeSystem> system;
    std::function<void(gsl::base_const_vector&)> callback;
};*/


class OdeFramedSolver
{
private:
	OdeFramedSolver(const OdeFramedSolver&) = delete;
	OdeFramedSolver& operator = (const OdeFramedSolver&) = delete;

public:
	OdeFramedSolver(std::shared_ptr<IOdeSystem> system, double startTime, gsl::base_const_vector& ics);

	bool frame(double seconds);
	gsl::vector_const_view getLastResult() const { return y; }

private:
	std::shared_ptr<gsl_odeiv_step> gslStep;
	gsl_odeiv_system gslSystem;

	std::shared_ptr<IOdeSystem> system;
	gsl::vector y, yerr;
	double lastTime;
};

#endif /* ODESYSTEM_H_ */
