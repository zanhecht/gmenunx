/***************************************************************************
 *   Copyright (C) 2006 by Massimiliano Torromeo   *
 *   massimiliano.torromeo@gmail.com   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "terminaldialog.h"
#include "messagebox.h"
#include "debug.h"

using namespace std;

#include <fstream>
#include <sstream>

TerminalDialog::TerminalDialog(GMenu2X *gmenu2x, const string &title, const string &description, const string &icon, const string &backdrop)
	: Dialog(gmenu2x), title(title), description(description), icon(icon), backdrop(backdrop)
{}

int TerminalDialog::drawText(vector<string> *text, int32_t firstCol, uint32_t firstRow, uint32_t rowsPerPage) {
	gmenu2x->s->setClipRect(gmenu2x->listRect);
	int mx = 0;

	for (uint32_t i = firstRow; i < firstRow + rowsPerPage && i < text->size(); i++) {
		int y = gmenu2x->listRect.y + (i - firstRow) * gmenu2x->font->getHeight();
		mx = max(mx, gmenu2x->font->getTextWidth(text->at(i)));
		gmenu2x->font->write(gmenu2x->s, text->at(i), 5 + firstCol, y);
	}

	gmenu2x->s->clearClipRect();
	gmenu2x->drawScrollBar(rowsPerPage, text->size(), firstRow, gmenu2x->listRect);
	return mx;
}

void TerminalDialog::exec(const string &cmd) {
	bool close = false, inputAction = false;

	drawTopBar(this->bg, title, description);

	//link icon
	if (gmenu2x->sc.skinRes(icon)==NULL)
		drawTitleIcon("icons/ebook.png", this->bg);
	else
		drawTitleIcon(icon, this->bg);

	drawBottomBar(this->bg);

	gmenu2x->drawButton(this->bg, "down", gmenu2x->tr["Scroll"],
	gmenu2x->drawButton(this->bg, "up", "",
	gmenu2x->drawButton(this->bg, "b", gmenu2x->tr["Exit"],
	5))-10);

	this->bg->box(gmenu2x->listRect, gmenu2x->skinConfColors[COLOR_LIST_BG]);

	FILE* pipe = popen((cmd + "2>&1").c_str(), "r");
	if (!pipe) return;
	char buffer[128];
	// string result = "";
	int32_t firstCol = 0, maxLine = 0;
	uint32_t firstRow = 0, rowsPerPage = gmenu2x->listRect.h/gmenu2x->font->getHeight();
	// while (!close) {

	rawText = "$ " + cmd + "\n";
	while (!close) {
		do {
			if(!feof(pipe) && fgets(buffer, 128, pipe) != NULL){
				rawText += buffer;
				inputAction = gmenu2x->input.update(false);
			} else {
				inputAction = gmenu2x->input.update();
			}
			split(text, rawText, "\n");

			this->bg->blit(gmenu2x->s,0,0);
			maxLine = drawText(&text, firstCol, firstRow, rowsPerPage);
			gmenu2x->s->flip();

			// inputAction = gmenu2x->input.update();
			if (gmenu2x->inputCommonActions(inputAction)) continue;

			if ( gmenu2x->input[UP  ] && firstRow > 0 ) firstRow--;
			else if ( gmenu2x->input[DOWN] && firstRow + rowsPerPage < text.size() ) firstRow++;
			else if ( gmenu2x->input[RIGHT]) {
				if (firstCol > -1 * (maxLine - gmenu2x->listRect.w) - 10)
					firstCol -= 30;
			}
			else if ( gmenu2x->input[LEFT]) {
				if (firstCol < 0)
					firstCol += 30;
			}
			else if ( gmenu2x->input[PAGEUP]) {
				if (firstRow >= rowsPerPage - 1)
					firstRow -= rowsPerPage - 1;
				else
					firstRow = 0;
			}
			else if ( gmenu2x->input[PAGEDOWN]) {
				if (firstRow + rowsPerPage * 2 - 1 < text.size())
					firstRow += rowsPerPage - 1;
				else
					firstRow = max(0,text.size()-rowsPerPage);
			}
			else if ( gmenu2x->input[SETTINGS] || gmenu2x->input[CANCEL] ) close = true;
		} while (!inputAction);
	}
	pclose(pipe);
}

// void TerminalDialog::appendText(const string &text) {
// 	this->rawText += text;
// }

// void TerminalDialog::appendFile(const string &file) {
// 	ifstream t(file);
// 	stringstream buf;
// 	buf << t.rdbuf();

// 	this->rawText += buf.str();
// }