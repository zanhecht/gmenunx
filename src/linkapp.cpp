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
#include <sys/stat.h>
#include <unistd.h>
#include <fstream>

#include "linkapp.h"
#include "menu.h"
#include "selector.h"
#include "messagebox.h"
#include "debug.h"

using namespace std;

extern char** environ;

LinkApp::LinkApp(GMenu2X *gmenu2x_, InputManager &inputMgr_, const char* linkfile):
	Link(gmenu2x_, MakeDelegate(this, &LinkApp::run)),
	inputMgr(inputMgr_)
{
	manual = manualPath = "";
	file = linkfile;
	setCPU(gmenu2x->confInt["cpuMenu"]);

#if defined(TARGET_GP2X)
	//G
	setGamma(0);
	// wrapper = false;
	// dontleave = false;
	// setVolume(-1);
	// useRamTimings = false;
	// useGinge = false;
#endif

	selectordir = "";
	selectorfilter = "";
	icon = iconPath = "";
	selectorbrowser = true;
	if (gmenu2x->w/gmenu2x->h > 1.4) _scalemode = 1;
	scalemode = _scalemode;
	workdir = "";
	backdrop = backdropPath = "";

	string line;
	ifstream infile (linkfile, ios_base::in);
	while (getline(infile, line, '\n')) {
		line = trim(line);
		if (line == "") continue;
		if (line[0] == '#') continue;

		string::size_type position = line.find("=");
		string name = trim(line.substr(0,position));
		string value = trim(line.substr(position+1));

		if (name == "title") title = value;
		else if (name == "description") description = value;
		else if (name == "icon") setIcon(value);
		else if (name == "exec") exec = value;
		else if (name == "params") params = value;
		else if (name == "workdir") workdir = value;
		else if (name == "manual") setManual(value);
		else if (name == "clock") setCPU(atoi(value.c_str()));

#if defined(TARGET_GP2X)
		// else if (name == "wrapper" && value == "true") // wrapper = true;
		// else if (name == "dontleave" && value == "true") // dontleave = true;
		// else if (name == "volume") // setVolume(atoi(value.c_str()));
		// else if (name == "useramtimings" && value == "true") // useRamTimings = true;
		// else if (name == "useginge" && value == "true") // useGinge = true;
		else if (name == "gamma") setGamma(atoi(value.c_str()));
#endif
		else if (name == "selectordir") setSelectorDir(value);
		else if (name == "selectorbrowser" && value == "false") selectorbrowser = false;
		else if (name == "scalemode") scalemode = atoi(value.c_str());
		else if (name == "selectorfilter") setSelectorFilter(value);
		else if (name == "selectorscreens") setSelectorScreens(value);
		else if (name == "selectoraliases") setAliasFile(value);
		else if (name == "selectorelement") setSelectorElement(atoi(value.c_str()));
		else if (name == "backdrop") setBackdrop(value);
		// else WARNING("Unrecognized option: '%s'", name.c_str());
	}
	infile.close();

	if (iconPath.empty()) searchIcon();
	if (manualPath.empty()) searchManual();
	if (backdropPath.empty()) searchBackdrop();

	edited = false;
}

const string &LinkApp::searchManual() {
	if (!manualPath.empty()) return manualPath;
	string filename = exec;
	string::size_type pos = exec.rfind(".");
	if (pos != string::npos) filename = exec.substr(0, pos);
	filename += ".man.txt";

	string dname = dir_name(exec) + "/";

	string dirtitle = dname + base_name(dir_name(exec)) + ".man.txt";
	string linktitle = base_name(file);
	pos = linktitle.rfind(".");
	if (pos != string::npos) linktitle = linktitle.substr(0, pos);
	linktitle = dname + linktitle + ".man.txt";

	if (file_exists(linktitle))
		manualPath = linktitle;
	else if (file_exists(filename))
		manualPath = filename;
	else if (file_exists(dirtitle))
		manualPath = dirtitle;

	return manualPath;
}

const string &LinkApp::searchBackdrop() {
	if (!backdropPath.empty() || !gmenu2x->confInt["skinBackdrops"]) return backdropPath;
	string execicon = exec;
	string::size_type pos = exec.rfind(".");
	if (pos != string::npos) execicon = exec.substr(0, pos);
	string exectitle = base_name(execicon);
	string dirtitle = base_name(dir_name(exec));
	string linktitle = base_name(file);
	string sublinktitle = base_name(file);

	pos = linktitle.find(".");
	if (pos != string::npos) sublinktitle = linktitle.substr(0, pos);

	pos = linktitle.rfind(".");
	if (pos != string::npos) linktitle = linktitle.substr(0, pos);

// auto backdrop
	if (!gmenu2x->sc.getSkinFilePath("backdrops/" + sublinktitle + ".png").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + sublinktitle + ".png");
	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + sublinktitle + ".jpg").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + sublinktitle + ".jpg");

	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + linktitle + ".png").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + linktitle + ".png");
	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + linktitle + ".jpg").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + linktitle + ".jpg");

	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + exectitle + ".png").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + exectitle + ".png");
	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + exectitle + ".jpg").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + exectitle + ".jpg");

	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + dirtitle + ".png").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + dirtitle + ".png");
	else if (!gmenu2x->sc.getSkinFilePath("backdrops/" + dirtitle + ".jpg").empty())
		backdropPath = gmenu2x->sc.getSkinFilePath("backdrops/" + dirtitle + ".jpg");

	else if (file_exists(dir_name(exec) + "/backdrop.png"))
		backdropPath = dir_name(exec) + "/backdrop.png";

	return backdropPath;
}

const string &LinkApp::searchIcon() {
	string execicon = exec;
	string::size_type pos = exec.rfind(".");
	if (pos != string::npos) execicon = exec.substr(0, pos);
	execicon += ".png";
	string exectitle = base_name(execicon);
	string dirtitle = base_name(dir_name(exec)) + ".png";
	string linktitle = base_name(file);
	string sublinktitle = base_name(file);

	pos = linktitle.find(".");
	if (pos != string::npos) sublinktitle = linktitle.substr(0, pos);
	sublinktitle = sublinktitle + ".png";

	pos = linktitle.rfind(".");

	if (pos != string::npos) linktitle = linktitle.substr(0, pos);

	linktitle += ".png";

	if (!gmenu2x->sc.getSkinFilePath("icons/" + sublinktitle).empty())
		iconPath = gmenu2x->sc.getSkinFilePath("icons/" + sublinktitle);

	else if (!gmenu2x->sc.getSkinFilePath("icons/" + linktitle).empty())
		iconPath = gmenu2x->sc.getSkinFilePath("icons/" + linktitle);

	else if (!gmenu2x->sc.getSkinFilePath("icons/" + exectitle).empty())
		iconPath = gmenu2x->sc.getSkinFilePath("icons/" + exectitle);

	else if (!gmenu2x->sc.getSkinFilePath("icons/" + dirtitle).empty())
		iconPath = gmenu2x->sc.getSkinFilePath("icons/" + dirtitle);

	else if (file_exists(dir_name(exec) + "/" + sublinktitle))
		iconPath = dir_name(exec) + "/" + sublinktitle;
	else if (file_exists(execicon))
		iconPath = execicon;
	else if (file_ext(exec, true) == ".opk" && file_exists((string)getenv("HOME") + (string)"/.cache/" + sublinktitle))
		iconPath = (string)getenv("HOME") + (string)"/.cache/" + sublinktitle;
	else
		iconPath = gmenu2x->sc.getSkinFilePath("icons/generic.png");

	return iconPath;
}

int LinkApp::getCPU() {
	return iclock;
}

void LinkApp::setCPU(int mhz) {
	iclock = mhz;
	// if (iclock != 0) iclock = constrain(iclock, gmenu2x->confInt["cpuMin"], gmenu2x->confInt["cpuMax"]);
	if (iclock != 0) iclock = constrain(iclock, gmenu2x->confInt["cpuMenu"], gmenu2x->confInt["cpuMax"]);
	edited = true;
}

#if defined(TARGET_GP2X)
int LinkApp::gamma() {
	return igamma;
}

void LinkApp::setGamma(int gamma) {
	igamma = constrain(gamma, 0, 100);
	edited = true;
}
#endif

bool LinkApp::targetExists() {
#if defined(TARGET_PC)
	return true; //For displaying elements during testing on pc
#endif

	string target = exec;
	if (!exec.empty() && exec[0] != '/' && !workdir.empty())
		target = workdir + "/" + exec;

	return file_exists(target);
}

bool LinkApp::save() {
	if (!edited) return false;

	ofstream f(file.c_str());
	if (f.is_open()) {
		if (title != "")			f << "title="			<< title			<< endl;
		if (description != "")		f << "description="		<< description		<< endl;
		if (icon != "")				f << "icon="			<< icon				<< endl;
		if (exec != "")				f << "exec="			<< exec				<< endl;
		if (params != "")			f << "params="			<< params			<< endl;
		if (workdir != "")			f << "workdir="			<< workdir			<< endl;
		if (manual != "")			f << "manual="			<< manual			<< endl;
		if (iclock != 0)			f << "clock="			<< iclock			<< endl;

#if defined(TARGET_GP2X)
		// if (useRamTimings)		f << "useramtimings=true"					<< endl;
		// if (useGinge)			f << "useginge=true"						<< endl;
		// if (ivolume > 0)			f << "volume="			<< ivolume			<< endl;
		if (igamma != 0)			f << "gamma="			<< igamma			<< endl;
#endif

		if (selectordir != "")		f << "selectordir="		<< selectordir		<< endl;
		if (!selectorbrowser)		f << "selectorbrowser=false"				<< endl; // selectorbrowser = true by default
		if (scalemode != _scalemode)	f << "scalemode="	<< scalemode		<< endl; // scalemode = 1 by default
		if (selectorfilter != "")	f << "selectorfilter="	<< selectorfilter	<< endl;
		if (selectorscreens != "")	f << "selectorscreens="	<< selectorscreens	<< endl;
		if (selectorelement > 0)	f << "selectorelement="	<< selectorelement	<< endl;
		if (aliasfile != "")		f << "selectoraliases="	<< aliasfile		<< endl;
		if (backdrop != "")			f << "backdrop="		<< backdrop			<< endl;
		f.close();
		return true;
	} else
		ERROR("Error while opening the file '%s' for write.", file.c_str());
	return false;
}

void LinkApp::run() {
	if (selectordir != "") {
		selector();
	} else {
		launch();
	}
}

void LinkApp::selector(int startSelection, const string &selectorDir) {
	//Run selector interface
	Selector bd(gmenu2x, this->getTitle(), this->getDescription(), this->getIconPath(), this);
	bd.showDirectories = this->getSelectorBrowser();

	if (selectorDir != "") bd.setPath(selectorDir);
	else bd.setPath(this->getSelectorDir());

	bd.setFilter(this->getSelectorFilter());

	if (startSelection > 0) bd.selected = startSelection;
	else bd.selected = this->getSelectorElement();

	if (bd.exec()) {
		gmenu2x->writeTmp(bd.selected, bd.getPath());

		if (gmenu2x->confInt["saveSelection"]) {
			setSelectorDir(bd.getPath());
			setSelectorElement(bd.selected);
			save();
		}

		params = trim(params + " " + bd.getParams(bd.selected));

		launch(bd.getFile(bd.selected), bd.getPath());
	}
}

void LinkApp::launch(const string &selectedFile, string dir) {
	MessageBox mb(gmenu2x, gmenu2x->tr["Launching "] + this->getTitle().c_str(), this->getIconPath());
	mb.setAutoHide(-1);
	mb.exec();

	gmenu2x->setVolume(gmenu2x->confInt["globalVolume"]);

	// Set correct working directory
	string wd = getRealWorkdir();
	if (!wd.empty())
		chdir(wd.c_str());

	if (!selectedFile.empty()) {
		string selectedFileExtension;
		string selectedFileName;
		string::size_type i = selectedFile.rfind(".");
		if (i != string::npos) {
			selectedFileExtension = selectedFile.substr(i,selectedFile.length());
			selectedFileName = selectedFile.substr(0,i);
		}

		if (dir.empty()) {
			dir = getSelectorDir();
		}

		if (params.empty()) {
			params = cmdclean(dir + "/" + selectedFile);
		} else {
			string origParams = params;
			params = strreplace(params, "[selFullPath]", cmdclean(dir + "/" + selectedFile));
			params = strreplace(params, "[selPath]", cmdclean(dir));
			params = strreplace(params, "[selFile]", cmdclean(selectedFileName));
			params = strreplace(params, "\%f", cmdclean(selectedFileName));
			params = strreplace(params, "[selExt]", cmdclean(selectedFileExtension));
			if (params == origParams) params += " " + cmdclean(dir + "/" + selectedFile);
		}
	}

	INFO("Executing '%s' (%s %s)", title.c_str(), exec.c_str(), params.c_str());

	string command = cmdclean(exec);
	// string command = exec;

	if (file_ext(command, true) == ".opk") {
		command = "retrofw opkrun " + command;
	}

	// Check to see if permissions are desirable
	struct stat fstat;
	if (!stat(command.c_str(), &fstat)) {
		struct stat newstat = fstat;
		if (S_IRUSR != (fstat.st_mode & S_IRUSR)) newstat.st_mode |= S_IRUSR;
		if (S_IXUSR != (fstat.st_mode & S_IXUSR)) newstat.st_mode |= S_IXUSR;
		if (fstat.st_mode != newstat.st_mode) chmod(command.c_str(), newstat.st_mode);
	} // else, well.. we are no worse off :)

	if (params != "") command += " " + params;

	if (gmenu2x->confInt["saveSelection"] && (gmenu2x->confInt["section"] != gmenu2x->menu->selSectionIndex() || gmenu2x->confInt["link"] != gmenu2x->menu->selLinkIndex())) {
		gmenu2x->writeConfig();
	}

	if (getCPU() != gmenu2x->confInt["cpuMenu"]) gmenu2x->setCPU(getCPU());

#if defined(TARGET_GP2X)
	//if (useRamTimings) gmenu2x->applyRamTimings();
	// if (useGinge) {
		// string ginge_prep = gmenu2x->getExePath() + "/ginge/ginge_prep";
		// if (file_exists(ginge_prep)) command = cmdclean(ginge_prep) + " " + command;
	// }
	if (fwType == "open2x") gmenu2x->writeConfigOpen2x();
	if (gamma() != 0 && gamma() != gmenu2x->confInt["gamma"]) gmenu2x->setGamma(gamma());
#endif

	gmenu2x->setScaleMode(scalemode);

	command = gmenu2x->hwPreLinkLaunch() + command;

	if (gmenu2x->confInt["outputLogs"]) command += " &> " + cmdclean(gmenu2x->getExePath()) + "/log.txt";

	if (!gmenu2x->confStr["homePath"].empty() && dir_exists(gmenu2x->confStr["homePath"])) {
		gmenu2x->confStr["homePath"] = "HOME=" + gmenu2x->confStr["homePath"];
		// populate environ
		environ[0] = (char*)gmenu2x->confStr["homePath"].c_str();
		environ[1] = (char*)"PATH=/bin:/sbin:/usr/bin:/usr/sbin",
		environ[2] = (char*)"TERM=vt100",
		environ[3] = (char*)"SDL_NOMOUSE=1",
		environ[4] = NULL;
	}

	gmenu2x->quit();

	execle("/bin/sh", "/bin/sh", "-c", command.c_str(), NULL, environ);

	//if execution continues then something went wrong and as we already called SDL_Quit we cannot continue
	//try relaunching gmenu2x
	chdir(gmenu2x->getExePath().c_str());
	execlp("./gmenu2x", "./gmenu2x", NULL);
	chdir(gmenu2x->getExePath().c_str());
}

const string &LinkApp::getExec() {
	return exec;
}

void LinkApp::setExec(const string &exec) {
	this->exec = exec;
	edited = true;
}

const string &LinkApp::getParams() {
	return params;
}

void LinkApp::setParams(const string &params) {
	this->params = params;
	edited = true;
}

const string &LinkApp::getWorkdir() {
	return workdir;
}

const string LinkApp::getRealWorkdir() {
	string wd = workdir;
	if (wd.empty()) {
		if (exec[0] != '/') {
			wd = gmenu2x->getExePath();
		} else {
			string::size_type pos = exec.rfind("/");
			if (pos != string::npos)
				wd = exec.substr(0,pos);
		}
	}
	return wd;
}

void LinkApp::setWorkdir(const string &workdir) {
	this->workdir = workdir;
	edited = true;
}

const string &LinkApp::getManual() {
	return manual;
}

void LinkApp::setManual(const string &manual) {
	this->manual = manualPath = manual;
	edited = true;
}

const string &LinkApp::getSelectorDir() {
	return selectordir;
}

void LinkApp::setSelectorDir(const string &selectordir) {
	edited = this->selectordir != selectordir;
	this->selectordir = selectordir;
	if (this->selectordir != "") this->selectordir = real_path(this->selectordir);
}

bool LinkApp::getSelectorBrowser() {
	return selectorbrowser;
}

void LinkApp::setSelectorBrowser(bool value) {
	selectorbrowser = value;
	edited = true;
}

int LinkApp::getScaleMode() {
	return scalemode;
}

void LinkApp::setScaleMode(int value) {
	scalemode = value;
	edited = true;
}

const string &LinkApp::getSelectorFilter() {
	return selectorfilter;
}

void LinkApp::setSelectorFilter(const string &selectorfilter) {
	this->selectorfilter = selectorfilter;
	edited = true;
}

const string &LinkApp::getSelectorScreens() {
	return selectorscreens;
}

void LinkApp::setSelectorScreens(const string &selectorscreens) {
	this->selectorscreens = selectorscreens;
	edited = true;
}

int LinkApp::getSelectorElement() {
	return selectorelement;
}

void LinkApp::setSelectorElement(int i) {
	edited = selectorelement != i;
	selectorelement = i;
}

const string &LinkApp::getAliasFile() {
	return aliasfile;
}

void LinkApp::setAliasFile(const string &aliasfile) {
	if (aliasfile == "" || file_exists(aliasfile)) {
		this->aliasfile = aliasfile;
		edited = true;
	}
}

void LinkApp::renameFile(const string &name) {
	file = name;
}

// bool LinkApp::getUseRamTimings() {
// 	return useRamTimings;
// }

// void LinkApp::setUseRamTimings(bool value) {
// 	useRamTimings = value;
// 	edited = true;
// }

// bool LinkApp::getUseGinge() {
// 	return useGinge;
// }

// void LinkApp::setUseGinge(bool value) {
// 	useGinge = value;
// 	edited = true;
// }
