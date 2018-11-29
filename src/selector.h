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

#ifndef SELECTOR_H_
#define SELECTOR_H_

// #include <string>
// #include "gmenu2x.h"
// #include "utilities.h"
// #include "filelister.h"
// #include "dialog.h"
#include "browsedialog.h"

class LinkApp;
// class FileLister;

using std::string;
// using std::vector;

// class Selector : protected Dialog {
class Selector : public BrowseDialog {
protected:
	// FileLister fl;

private:
	LinkApp *link;

	// string file, dir;
	unordered_map<string, string> aliases;
	void loadAliases();
	// void setPath(const string &path);
	// string getAlias(const string &fname);
	// string getScreenshot(const string &path, const string &fname, const string &screendir = "");
	// void freeScreenshots(vector<string> *screens);
	const std::string getPreview(uint32_t i = 0);
	
public:
	Selector(GMenu2X *gmenu2x, const string &title, const string &description, const string &icon, LinkApp *link);
	const std::string getFile(uint32_t i = 0);
	// Selector(GMenu2X *gmenu2x, LinkApp *link, const string &selectorDir = "");
	// int exec(int startSelection = 0);
	// const string &getFile() { return file; }
	// const string &getDir() { return dir; }
};

#endif /*SELECTOR_H_*/

















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

// #ifndef SELECTOR_H_
// #define SELECTOR_H_

// #include <string>
// #include "gmenu2x.h"
// #include "utilities.h"
// #include "dialog.h"
// #include "filelister.h"

// class LinkApp;
// class FileLister;

// using std::string;
// using std::vector;

// class Selector : protected Dialog {
// protected:
// 	FileLister fl;

// private:
// 	LinkApp *link;

// 	string file, dir;
// 	unordered_map<string, string> aliases;
// 	void loadAliases();
// 	void setPath(const string &path);
// 	string getAlias(const string &fname);
// 	string getScreenshot(const string &path, const string &fname, const string &screendir = "");
// 	void freeScreenshots(vector<string> *screens);
	
// public:
// 	Selector(GMenu2X *gmenu2x, LinkApp *link, const string &selectorDir = "");
// 	int exec(int startSelection = 0);
// 	const string &getFile() { return file; }
// 	const string &getDir() { return dir; }
// };

// #endif /*SELECTOR_H_*/
