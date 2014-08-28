/*
 * NodeSystemView.cpp
 *
 *  Created on: 28 авг. 2014 г.
 *      Author: yan
 */

#include <GL/glew.h>
#include "NodeSystemView.h"


NodeSystemView::NodeSystemView(std::shared_ptr<INodeSystem> model, float chainWidth)
	: model(model), chainWidth(chainWidth) {

	chainWidth = 1.0;
}

void NodeSystemView::render(std::shared_ptr<Camera> camera) {

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

    glm::vec3 w1, w2;
    glm::vec2 s1, s2;

    glLineWidth(camera->toPixels(chainWidth));
    glBegin(GL_LINES);
    glColor4ub(0, 192, 227, 255);

    for (size_t i = 0; i < model->getNumberOfLinks(); ++i) {

    	auto link = model->getLink(i);
    	w1 = model->getNode(link.first)->getPosition();
    	w2 = model->getNode(link.second)->getPosition();

    	s1 = camera->toScreen(w1);
    	s2 = camera->toScreen(w2);

    	glVertex2f(s1.x, s1.y);
    	glVertex2f(s2.x, s2.y);
    }
    glEnd();

    glPointSize(camera->toPixels(chainWidth * 1.2));
    glBegin(GL_POINTS);
    glColor4ub(255, 186, 0, 255);

    for (size_t i = 0; i < model->getNumberOfNodes(); ++i) {
    	w1 = model->getNode(i)->getPosition();
    	s1 = camera->toScreen(w1);
    	glVertex2f(s1.x, s1.y);
    }
    glEnd();
}
