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
#include "Camera.h"


class NodeSystemView
{
public:
	NodeSystemView(std::shared_ptr<INodeSystem> model, float chainWidth);
	void render(std::shared_ptr<Camera> camera);

private:
	std::shared_ptr<INodeSystem> model;
	float chainWidth;
};

#endif /* NODESYSTEMVIEW_H_ */
