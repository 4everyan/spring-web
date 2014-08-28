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
#include "model/NodeSystemBuilder.h"
#include "model/Constraints.h"


SpringApplication::SpringApplication()
{
#ifndef NDEBUG
	drawDebugInfo = true;
#else
	drawDebugInfo = false;
#endif
}

bool SpringApplication::InitializeSystem() {

	pugi::xml_document doc;
	pugi::xml_parse_result res = doc.load_file("settings.xml");
	if( !res ) {
		SDL_Log("settings error: %s", res.description());
		return false;
	}
	pugi::xml_node root = doc.child("settings");
	double mass = root.child("system").attribute("mass").as_double(1.0);
	float chainWidth = root.child("system").attribute("width").as_float(1.0);

	NodeSystemBuilder builder;
	builder.addNode(mass, glm::dvec3(-3.0, 0.0, 0.0));
	builder.addNode(mass, glm::dvec3( 3.0, 0.0, 0.0));
	builder.addNode(mass, glm::dvec3( 0.0, 5.0, 0.0));
	builder.linkNodes(0, 1);
	builder.linkNodes(1, 2);
	builder.linkNodes(2, 0);
	builder.addConstraint(std::make_shared<ResistanceConstraint>(model, 0.4));

	model = builder.create();
	view = std::make_shared<NodeSystemView>(model, chainWidth);
	controller = std::make_shared<NodeSystemController>(model);

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
	if( !InitializeSystem() )
		return false;

	// камера
	glm::vec3 eye { 0.0f, 0.0f, 1.0f };
	glm::vec3 center { 0.0f, 0.0f, 0.0f };
	glm::vec3 up { 0.0f,1.0f,0.0f };

	camera = std::make_shared<Camera>( eye, center, up, (float)width, (float)height );
	camera->setAnchorNormalized(glm::vec2(0.5f, 0.5f));
	camera->setPixelsPerMetreRange(1, 2048);

	return true;
}

void SpringApplication::OnDestroy() {
}

void SpringApplication::OnFrame() {
	double dt = Application::TimerGetDelta() / 1000.0;
	controller->frame(dt);
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

	view->render(camera);

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
	    //info += appliedForce->isEnabled() ? "\nForce ON" : "\nForce OFF";
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
		;
}

void SpringApplication::OnKeyUp(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) {

	if( keycode == SDLK_SPACE )
		;
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
