/*
 * SpringApplication.h
 *
 *  Created on: 28 июня 2014 г.
 *      Author: yan
 */

#ifndef SPRINGAPPLICATION_H_
#define SPRINGAPPLICATION_H_

#include "Application.h"
#include "FontRenderer.h"
#include "Camera.h"
#include "model/INodeSystem.h"
#include "NodeSystemView.h"
#include "NodeSystemController.h"


class SpringApplication: public Application
{
public:
	SpringApplication();

	bool OnCreate() override;
	void OnDestroy() override;
	void OnFrame() override;
	void OnRender() override;
	void OnActivate(bool active) override;

	void OnKeyDown(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) override;
	void OnKeyUp(SDL_Keycode keycode, SDL_Scancode scancode, Uint16 mod) override;
	void OnMouseMove(int mx, int my, int relx, int rely, bool left, bool right, bool middle) override;
	void OnLButtonDown(int mx, int my) override;
	void OnLButtonUp(int mx, int my) override;
	void OnMouseWheel(int dx, int dy) override;
    void OnMouseDrag(int dx, int dy, Uint8 button) override;

private:
	bool drawDebugInfo;
	std::shared_ptr<FontRenderer> systemFont;
	std::shared_ptr<Camera> camera;

	std::shared_ptr<INodeSystem> model;
	std::shared_ptr<NodeSystemView> view;
	std::shared_ptr<NodeSystemController> controller;

	bool InitializeSystem();
};

#endif /* SPRINGAPPLICATION_H_ */
