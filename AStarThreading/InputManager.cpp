#include "stdafx.h"
#include "InputManager.h"
#include <iostream>


InputManager::InputManager()
{
}


InputManager::~InputManager()
{
}

void InputManager::AddListener(EventListener::Event evt,EventListener *listener)
{
	if (listeners.find(evt) == listeners.end()) { //event not in map add it.
		listeners[evt] = new std::vector<EventListener*>();
	}

	listeners[evt]->push_back(listener);

}

//Send the events to interested objects
void InputManager::Dispatch(EventListener::Event evt)
{

	if (listeners.find(evt) != listeners.end()) {
		
		for (auto const &listener : *listeners[evt]) { //go through all listeners for this event
			listener->onEvent(evt); //Call on event for the listener

		}
	}
	
}


//Gnereate events
void InputManager::ProcessInput()
{
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0)
	{
		switch (e.type) {
			/* Keyboard event */
			case SDL_KEYDOWN:
				switch (e.key.keysym.sym) {
					case SDLK_SPACE:
						Dispatch(EventListener::Event::SPACE);
						break;
					case SDLK_f:
						//Dispatch(EventListener::Event::FOLLOW);
						break;
					case SDLK_RIGHT:
						Dispatch(EventListener::Event::RIGHTARROW);
						break;
					case SDLK_LEFT:
						Dispatch(EventListener::Event::LEFTARROW);
						break;
					case SDLK_UP:
						Dispatch(EventListener::Event::UPARROW);
						break;
					case SDLK_DOWN:
						Dispatch(EventListener::Event::DOWNARROW);
						break;
					case SDLK_ESCAPE:
						Dispatch(EventListener::Event::QUIT);
						break;
				}
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
				case SDLK_f:
					Dispatch(EventListener::Event::FOLLOW);
					break;
				}
				break;
			/* SDL_QUIT event (window close) */
			case SDL_QUIT:
				Dispatch(EventListener::Event::QUIT);
				break;

			default:
				break;
			}
	
	}
	//check for exit
}
