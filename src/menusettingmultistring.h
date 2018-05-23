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
#ifndef MENUSETTINGMULTISTRING_H
#define MENUSETTINGMULTISTRING_H

#include "menusettingstringbase.h"

#include <vector>
#include "debug.h"

#include "FastDelegate.h"
using namespace fastdelegate;
using fastdelegate::MakeDelegate;

typedef FastDelegate0<uint> cbAction;

class MenuSettingMultiString : public MenuSettingStringBase {
private:
	virtual void edit() {
		/* never called because manageInput() is overridden */
	}

	const std::vector<std::string> *choices;
	int selected;

	void incSel();
	void decSel();
	void setSel(int sel);

	cbAction onChange; // variable to store function pointer type

public:
	MenuSettingMultiString(
			GMenu2X *gmenu2x, const std::string &name,
			const std::string &description, std::string *value,
			const std::vector<std::string> *choices);

	MenuSettingMultiString(
			GMenu2X *gmenu2x, const std::string &name,
			const std::string &description, std::string *value,
			const std::vector<std::string> *choices, cbAction cbOnChange);

	uint voidAction() { return 0; };


	virtual ~MenuSettingMultiString() {};
	virtual uint manageInput();
};

#endif
