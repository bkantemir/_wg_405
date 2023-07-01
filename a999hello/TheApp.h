#pragma once
#include <string>

class TheApp
{
public:
	int screenSize[2];
	float screenRatio;

	bool bExitApp;
public:
	int run();
	int getReady();
	int drawFrame();
	int cleanUp();
	int onScreenResize(int width, int height);
};

