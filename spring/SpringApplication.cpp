/*
 * SpringApplication.cpp
 *
 *  Created on: 28 июня 2014 г.
 *      Author: yan
 */

#include <GL/glew.h>
#include <array>
#include <iostream>
#include "SpringApplication.h"


SpringApplication::SpringApplication()
{
#ifndef NDEBUG
	drawDebugInfo = true;
#else
	drawDebugInfo = false;
#endif
}

bool SpringApplication::InitializePointChain() {

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file("settings.xml");
	if( !res ) {
		SDL_Log("settings error: %s", res.description());
		return false;
	}
	pugi::xml_node root = doc.child("settings");

	// параметры системы
	size_t nodeCount = root.child("system").attribute("nodecount").as_double();
	double length = root.child("system").attribute("length").as_double();
	double mass = root.child("system").attribute("mass").as_double();
	double segmentLength = length/(nodeCount-1);
	chainWidth = root.child("system").attribute("width").as_float(0.1f);

	// параметры сил сопротивления и упругости
	double viscosity = root.child("resistance").attribute("viscosity").as_double();
	double stiffness = root.child("linearspring").attribute("stiffness").as_double();
	double torsionStiffness = root.child("torsionspring").attribute("stiffness").as_double();

	// параметры прикладываемой силы
	glm::dvec3 appliedForceAxis;
	appliedForceAxis.x = root.child("force").child("axis").attribute("x").as_double(0.0);
	appliedForceAxis.y = root.child("force").child("axis").attribute("y").as_double(0.0);
	appliedForceAxis.z = root.child("force").child("axis").attribute("z").as_double(0.0);
	double appliedForceValue = root.child("force").attribute("value").as_double(0.0);

	system = std::make_shared<ChainSystem>(nodeCount, mass, length);
	auto ics = system->icsBaseNodeAndDirection(0, glm::dvec3(0.0), glm::dvec3(1.0, 0.0, 0.0));

	for( size_t n=0; n<nodeCount-1; ++n )
		system->addForce( std::make_shared<SpringForce>(system.get(), n, n+1, segmentLength, stiffness) );

	for( size_t n=1; n<nodeCount-1; ++n )
		system->addForce( std::make_shared<TorsionSpringForce>(system.get(), n, torsionStiffness) );

	appliedForce = std::make_shared<BendingForce>(system.get(), appliedForceAxis, appliedForceValue);
	//appliedForce = std::make_shared<NodeToNodeForce>(system.get(), 8, 0, appliedForceValue);
	//appliedForce = std::make_shared<SingleNodeConstantForce>(system.get(), 5, glm::dvec3(-1.0, 0.2, 0.0) * appliedForceValue);

	system->addForce(appliedForce);
	system->addForce( std::make_shared<ResistanceForce>(system.get(), viscosity) );
	solver = std::make_shared<OdeFramedSolver>(system, 0.0, ics);

	system->fixNode(0, true);
	//system->fixNode(1, true);

	return true;
}

bool SpringApplication::OnCreate() {

	int width = Application::WindowWidth();
	int height = Application::WindowHeight();

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glViewport(0, 0, width, height);

	systemFont = FontRenderer::LoadFont("fonts/consolas14.fnt");
	if( !InitializePointChain() )
		return false;

	// камера
	glm::vec3 eye { 0.0f, 0.0f, 1.0f };
	glm::vec3 center { 0.0f, 0.0f, 0.0f };
	glm::vec3 up { 0.0f,1.0f,0.0f };

	camera = std::make_shared<Camera>( eye, center, up, (float)width, (float)height );
	camera->setAnchorNormalized(glm::vec2(0.5f, 0.5f));
	camera->setPixelsPerMetreRange(1, 2048);
	camera->setPixelsPerMetre(width / 2.f / (float)system->getLength());
	camera->setCenter( glm::vec3((float)system->getLength() / 2.0f, 0.0, 0.0) );

	return true;
}

void SpringApplication::OnDestroy() {
}

void SpringApplication::OnFrame() {
	double dt = Application::TimerGetDelta() / 1000.0;
	solver->frame(dt);
}

void SpringApplication::OnRender() {

	Application::GfxBeginScene();

	int width = Application::WindowWidth();
	int height = Application::WindowHeight();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glViewport(0, 0, width, height);
	glOrtho(0, width, 0, height, 1, -1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    size_t stride = system->getNumCoordsPerNode();
    size_t nodeCount = system->getNodeCount();

    auto result = solver->getLastResult();
    auto x = gsl::vector_const_view(result, ChainSystem::X, stride, nodeCount);
    auto y = gsl::vector_const_view(result, ChainSystem::Y, stride, nodeCount);
    auto z = gsl::vector_const_view(result, ChainSystem::Z, stride, nodeCount);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POINT_SMOOTH);

    glLineWidth(camera->toPixels(chainWidth));
    glBegin(GL_LINES);
    glColor4ub(0, 192, 227, 255);

    glm::vec3 w1, w2;
    glm::vec2 s1, s2;

    for( size_t i=0; i<system->getNodeCount()-1; ++i ) {
    	w1 = glm::vec3(x[i], y[i], z[i]);
    	w2 = glm::vec3(x[i+1], y[i+1], z[i+1]);
    	s1 = camera->toScreen(w1);
    	s2 = camera->toScreen(w2);
    	glVertex2f(s1.x, s1.y);
    	glVertex2f(s2.x, s2.y);
    }
    glEnd();

    glPointSize(camera->toPixels(chainWidth * 1.2));
    glBegin(GL_POINTS);
    glColor4ub(255, 186, 0, 255);
    for( size_t i=0; i<system->getNodeCount(); ++i ) {
    	w1 = glm::vec3(x[i], y[i], z[i]);
    	s1 = camera->toScreen(w1);
    	glVertex2f(s1.x, s1.y);
    }
    glEnd();

	//
	// прорисовка отладочной информации
	//
	if( drawDebugInfo )
	{
	    glMatrixMode(GL_PROJECTION);
	    glLoadIdentity();
	    glViewport(0, 0, width, height);
	    glOrtho(0, width, 0, height, 1, -1);

	    glMatrixMode(GL_MODELVIEW);
	    glLoadIdentity();

	    std::string info = "FPS: " + std::to_string(Application::TimerGetFps());
	    info += appliedForce->isEnabled() ? "\nForce ON" : "\nForce OFF";
	    systemFont->RenderText(20, (float)height - 40, info.c_str());
	}
	Application::GfxEndScene();
}

void SpringApplication::OnActivate(bool active) {
}

void SpringApplication::OnKeyDown(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) {

    if( keycode == SDLK_ESCAPE )
        Application::SystemExit();

	if( keycode == SDLK_F9 )
		drawDebugInfo = !drawDebugInfo;

	if( keycode == SDLK_SPACE )
		appliedForce->enable(true);
}

void SpringApplication::OnKeyUp(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) {

	if( keycode == SDLK_SPACE )
		appliedForce->enable(false);
}

void SpringApplication::OnMouseWheel(int dx, int dy) {
	//camera->zoom(0.9f);
}

void SpringApplication::OnMouseMove(int mx, int my, int relx, int rely, bool left, bool right, bool middle) {
    my = Application::WindowHeight() - my;
}

void SpringApplication::OnLButtonDown(int mx, int my) {
    my = Application::WindowHeight() - my;
}

void SpringApplication::OnLButtonUp(int mx, int my) {
    my = Application::WindowHeight() - my;
}

void SpringApplication::OnMouseDrag(int dx, int dy, Uint8 button) {
	if( button == SDL_BUTTON_LEFT ) {
		glm::vec3 drag;
		drag.x = camera->toMetres( float(dx) );
		drag.y = camera->toMetres( float(-dy) );
		camera->move( -drag );
	}
	else if( button == SDL_BUTTON_RIGHT ) {
		float drag = camera->toMetres( float(-dy) );
		camera->zoom( pow(1.1, drag) );
	}
}
