
#ifndef DEF_GUI
#define DEF_GUI

#include <SDL_ttf.h>
#include "config.hpp"

class Gui
{
	public:
		Gui();
		~Gui();

		TTF_Font* getFont() const; // Si NULL, erreur d'initialisation
		SDL_Color getPlayerColor(int p) const; // La couleur du joueur

	private:
		TTF_Font* m_font;
		SDL_Color m_colors[max_players];
};

#endif

