/*
 * OdeFramedSolver.h
 *
 *  Created on: 25 авг. 2014 г.
 *      Author: yan
 */

#ifndef ODEFRAMEDSOLVER_H_
#define ODEFRAMEDSOLVER_H_

#include <memory>
#include <vector>
#include <gsl/gsl_odeiv.h>
#include <gsl/gsl_linalg.h>
#include "gsl_helper.h"
#include "OdeSystem.h"



class OdeFramedSolver
{
public:
	OdeFramedSolver(const OdeFramedSolver&) = delete;
	void operator = (const OdeFramedSolver&) = delete;

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

#endif /* ODEFRAMEDSOLVER_H_ */
