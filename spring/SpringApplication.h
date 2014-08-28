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
#include "ChainSystem.h"
#include "model/OdeFramedSolver.h"


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
	float chainWidth;
	std::shared_ptr<FontRenderer> systemFont;
	std::shared_ptr<ChainSystem> system;
	std::shared_ptr<OdeFramedSolver> solver;
	std::shared_ptr<Camera> camera;
	std::shared_ptr<IForce> appliedForce;

	bool InitializePointChain();
};

#endif /* SPRINGAPPLICATION_H_ */
