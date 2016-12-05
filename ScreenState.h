#pragma once

// Interface that any scene must implement
class ScreenState
{
public:
	// Function that gets called on active scene every frame
	void virtual execute() = 0;
};

