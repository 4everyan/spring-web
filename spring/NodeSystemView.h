/*
 * NodeSystemView.h
 *
 *  Created on: 28 авг. 2014 г.
 *      Author: yan
 */

#ifndef NODESYSTEMVIEW_H_
#define NODESYSTEMVIEW_H_

#include <memory>
#include "model/INodeSystem.h"


class NodeSystemView
{
public:
	NodeSystemView(std::shared_ptr<INodeSystem> model);
	void render();

private:
	std::shared_ptr<INodeSystem> model;
};

#endif /* NODESYSTEMVIEW_H_ */
