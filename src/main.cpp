
#include <iostream>
#include <SDL.h>
#include "config.hpp"
#include "bonus.hpp"
#include "map.hpp"
#include "snake.hpp"
#include "gui.hpp"
#include "scorebar.hpp"
#include "button.hpp"
#include "progress.hpp"
#include <boost/filesystem/path.hpp>

namespace fs = boost::filesystem;
void drawCol(SDL_Surface* dst, Map* map);

typedef void (Snake::*Move)();

int main(int argc, char *argv[])
{
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Surface* ecran = SDL_SetVideoMode(widthMap * sizeTile, heightMap * sizeTile + 25, 24, SDL_DOUBLEBUF);
	fs::path bonPath(rcdir);
	bonPath /= bonus_subdir;
	Bonus::loadAll(bonPath.c_str());

	srand(time(NULL));
	Map map;

	SDL_Rect pos;
	pos.x = 20;
	pos.y = 10;

	// Snakes
	Snake* snks[max_players];
	for(int i = 0; i < max_players; ++i)
		snks[i] = NULL;
	snks[0] = new Snake(&map, pos);
	if(!snks[0]->isLoaded())
		return 1;

	// Gui
	Gui gui;
	if(gui.getFont() == NULL)
		return 1;

	// ScoreBar
	ScoreBar bar(&gui, snks, ecran->w, 25);
	SDL_Rect bpos;
	bpos.x = 0;
	bpos.y = heightMap * sizeTile;

	// Boutton
	Button bquit("Quit", &gui);
	pos.x = widthMap * sizeTile - 50;
	pos.y = 50;
	bquit.setPos(pos);

	// ProgressBar
	Progress pb(&gui, 300, 100, 10);
	SDL_Rect ppos;
	ppos.x = ecran->w - 120;
	ppos.y = 100;

	Uint32 ltime = SDL_GetTicks();
	Move mv(&Snake::moveRight);

#ifndef NDEBUG
	Uint32 frameTime = SDL_GetTicks();
#endif

	SDL_Event ev;
	SDL_WaitEvent(&ev);
	bool continuer = true;
	while(continuer)
	{
		SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
		map.blitOn(ecran, NULL);
		snks[0]->blitOn(ecran, NULL);
		bar.blitOn(ecran, bpos);
		bquit.blitOn(ecran);
		pb.blitOn(ecran, ppos);
		SDL_Flip(ecran);

		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					continuer = false;
					break;
				case SDL_MOUSEBUTTONDOWN: 
					{
						unsigned int x = ev.button.x / sizeTile;
						unsigned int y = ev.button.y / sizeTile;
						if(map.testCase(x, y) == Map::EMPTY)
							std::cout << "Empty case." << std::endl;
						else if(map.testCase(x, y) == Map::BONUS)
						{
							Bonus* bon = map.getBonusAt(x, y);
							std::cout << "Bonus : \n"
								<< "\tPts : " << bon->getPts()
								<< "\n\tLength : " << bon->getLength()
								<< std::endl;
						}
					}
					pos.x = ev.button.x;
					pos.y = ev.button.y;
					if(bquit.clicked(pos))
						continuer = false;
					break;
				case SDL_KEYDOWN:
					switch(ev.key.keysym.sym)
					{
						case SDLK_ESCAPE:
						case SDLK_q:
							continuer = false;
							break;
						case SDLK_UP:
							mv = &Snake::moveUp;
							break;
						case SDLK_DOWN:
							mv = &Snake::moveDown;
							break;
						case SDLK_RIGHT:
							mv = &Snake::moveRight;
							break;
						case SDLK_LEFT:
							mv = &Snake::moveLeft;
							break;
						default:
							break;
					}
					break;
				case SDL_MOUSEMOTION:
					pos.x = ev.motion.x;
					pos.y = ev.motion.y;
					bquit.mouse(pos);
				default:
					break;
			}
		}

#ifndef NDEBUG
		std::cout << SDL_GetTicks() - frameTime << std::endl;
		frameTime = SDL_GetTicks();
#endif

		if(SDL_GetTicks() - ltime > 100)
		{
			(snks[0]->*mv)();
			pb.set(snks[0]->getScore());
			if(snks[0]->isDead())
				continuer = false;
			else
			{
				ltime = SDL_GetTicks();
				map.addBonus();
			}
		}
	}

	Bonus::freeAll();
	SDL_Quit();
	return 0;
}

void drawCol(SDL_Surface* dst, Map* map)
{
	for(int x = 0; x < widthMap; ++x)
	{
		for(int y = 0; y < heightMap; ++y)
		{
			if(map->testCase(x, y) == Map::WALL)
			{
				SDL_Rect r;
				r.x = x * sizeTile + sizeTile / 4;
				r.y = y * sizeTile + sizeTile / 4;
				r.w = r.h = sizeTile / 2;
				SDL_FillRect(dst, &r, SDL_MapRGB(dst->format, 0, 0, 255));
			}
		}
	}
}

