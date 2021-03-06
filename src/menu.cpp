
#include "menu.hpp"
#include "button.hpp"
#include "gui.hpp"
#include "server.hpp"
#include "music.hpp"
#include "selecter.hpp"
#include <boost/filesystem/path.hpp>
#include <SDL_image.h>
#include <iostream>

namespace fs = boost::filesystem;

	Menu::Menu(Gui* g, SDL_Surface* scr)
: m_gui(g), m_scr(scr),
	m_bret(NULL), m_bplay(NULL),
	m_title(NULL)
{
	SDL_Rect pos;
	pos.x = m_scr->w / 2;
	pos.y = m_scr->h / 2 + 50;

	m_bplay = new Button("New game", m_gui);
	m_bplay->setPos(pos);

	pos.y += 200;
	m_bret = new Button("Quit", m_gui);
	m_bret->setPos(pos);

	fs::path tpath(rcdir);
	tpath /= gui_subdir;
	tpath /= title_path;
	m_title = IMG_Load(tpath.string().c_str());
}

Menu::~Menu()
{
	if(m_bret != NULL)
		delete m_bret;
	if(m_bplay != NULL)
		delete m_bplay;
	if(m_title != NULL)
		SDL_FreeSurface(m_title);
}

bool Menu::run()
{
	bool continuer = true;
	SDL_Event ev;
	SDL_Rect m;

	while(continuer)
	{
		// Affichage
		SDL_FillRect(m_scr, NULL, SDL_MapRGB(m_scr->format, 0, 0, 0));
		SDL_BlitSurface(m_gui->background(), NULL, m_scr, NULL);
		m_bplay->blitOn(m_scr);
		m_bret->blitOn(m_scr);
		if(m_title != NULL)
		{
			m.x = m_scr->w / 2 - m_title->w / 2;
			m.y = 5;
			SDL_BlitSurface(m_title, NULL, m_scr, &m);
		}
		SDL_Flip(m_scr);

		// Évènements
		while(SDL_PollEvent(&ev))
		{
			switch(ev.type)
			{
				case SDL_QUIT:
					return false;
					break;
				case SDL_KEYDOWN:
			        if(ev.key.keysym.sym == SDLK_BACKSPACE
							|| ev.key.keysym.sym == SDLK_ESCAPE)
						continuer = false;
                    else if(ev.key.keysym.sym == SDLK_RETURN) {
						music->playSound(Music::OK);
						Selecter sel(m_gui, m_scr);
						if(!sel.run())
							return false;
                    }
					break;
				case SDL_MOUSEMOTION:
					m.x = ev.motion.x;
					m.y = ev.motion.y;
					m_bplay->mouse(m);
					m_bret->mouse(m);
					break;
				case SDL_MOUSEBUTTONDOWN:
					m.x = ev.button.x;
					m.y = ev.button.y;

					if(m_bplay->clicked(m))
					{
						music->playSound(Music::OK);
						Selecter sel(m_gui, m_scr);
						if(!sel.run())
							return false;
					}
					else if(m_bret->clicked(m))
						continuer = false;
					break;
				default:
					break;
			}
		}
	}

	return true;
}


