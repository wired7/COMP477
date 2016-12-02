#include "InputState.h"

bool InputState::mouseDragged = false;
bool InputState::shiftPressed = false;
bool InputState::controlPressed = false;
bool InputState::altPressed = false;
bool InputState::wireframeModeOn = true;
bool InputState::surfaceModeOn = true;
bool InputState::mouseLeftButtonPrevState = false;
bool InputState::mouseButtonLeftPressed = false;
bool InputState::wPressed = false;
bool InputState::sPressed = false;
double InputState::previousMousePosX = 0;
double InputState::previousMousePosY = 0;
glm::vec2 InputState::mouseCoords = glm::vec2(0.0f);
glm::vec2 InputState::mouseGuiCoords = glm::vec2(0.0f);

