#include "Game.h"
#include "Modules/ModuleManager.h"

IMPLEMENT_PRIMARY_GAME_MODULE(FDefaultGameModuleImpl, Game, "Game");

// Global helper methods

inline int sgn(float val)
{
	return (0.f < val) - (val < 0.f);
}