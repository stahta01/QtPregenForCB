/***************************************************************
 * Name:      qtPrebuild.cpp
 * Purpose:   Code::Blocks plugin	'qtPregenForCB.cbp'  0.2.1
 * Author:    LETARTARE
 * Created:   2015-02-09
 * Copyright: LETARTARE
 * License:   GPL
 **************************************************************/
#include <sdk.h>
#include <manager.h>
#include "qtprebuild.h"
#include <cbproject.h>
#include <compiletargetbase.h>
#include <projectmanager.h>
#include <macrosmanager.h>
#include <editormanager.h>
#include <cbeditor.h>
#include <wx/datetime.h>
#include <projectfile.h>
// not change  !
#include "print.h"
///-----------------------------------------------------------------------------
/// called by :
///	1. QtPregen::OnAttach():1,
///
qtPrebuild::qtPrebuild(cbProject * prj)
	: qtPre(prj)
	, Dexeishere(true)
{
	// not use  + _T(Slash)   !!
	Dirgen = _T("qtprebuild") + wxString(Slash);
	Devoid = _T("__NULL__") ;
}
///-----------------------------------------------------------------------------
/// called by :
///	1. QtPregen::OnRelease():1,
///
qtPrebuild::~qtPrebuild()
{
	Project = nullptr;  Mam = nullptr;
}
///-----------------------------------------------------------------------------
/// called by :
///	1. buildQt():1,
///
void qtPrebuild::beginMesCreate() {
// begin '*.cre'
	if(!Project)
		return;
// base directory
	DirProject = Project->GetBasePath() ;
// directory changed
	wxFileName::SetCwd (DirProject);
	bool ok = createDir (Dirgen) ;
	if (!ok)
		return ;

	Nameproject = Project->GetTitle();
	wxString file = DirProject + Nameproject ;
	closedit(file + _T(".cre"));
	Fileswithstrings.Clear();
// tittle
	Mes = _("Begin ") +  Quote + Thename + _T(" ") + VERSION_QTP + Quote + _T(" : ") ;
	//print(Mes) ;
	Fileswithstrings.Add(Mes, 1) ;
	// Qt path
	wxString qtpath = findpathProjectQt() ;
	Mes = Lf + Tab + Quote + _T("Qt") + Quote + _(" Installation path") + _T(" : ") ;
	Mes +=  Quote + qtpath + Quote  ;
	//print(Mes) ;
	Fileswithstrings.Add(Mes, 1)  ;

	Mes = Lf + Tab + _("Eligibles files ") ;
	Mes += _T("(*.h, *.hpp, *.cpp, *.ui, *.qrc)")  ;
//	print(Mes) ;
	Fileswithstrings.Add(Mes, 1)  ;
	Mes = _T("");
    if (Project) {
		int nfp = Project->GetFilesCount();
		Mes = Lf + Tab + _("The active Project ") + Quote + Nameproject + Quote ;
		Mes += _T(", ") + (wxString() << nfp) + _(" files") ;
		Mes += Lf + Tab + _("Work path : ") + Quote + DirProject + Quote ;
    }
	// date
	wxString Datebegin = date() ;
	Mes +=  Lf +  Tab + _("Date : ")+ Datebegin  ;
	//print(Mes);
	Fileswithstrings.Add(Mes, 1);
	// for duration
	Start = clock();
}
///-----------------------------------------------------------------------------
/// called by :
///	1. buildQt():1,
///
void qtPrebuild::endMesCreate() {
// end '*.cre'
	Mes = Lf + _("End ") + Quote + Thename + Quote  ;
	Fileswithstrings.Add(Mes, 1) ;
	// date and duration
	Mes = date()  + _T(" -> ") + _("duration = ") + duration() ;
	//print (Mes) ;
	Fileswithstrings.Add(Mes, 1)  ;
// saving
	SaveFileswithstrings();
}
///-----------------------------------------------------------------------------
/// Generating the complements files...
///
/// called by :
///	1. QtPregen::OnPrebuild(CodeBlocksEvent& event):1
///
/// calls to :
///	1. findGoodfiles():1,
///	2. addRecording():1,
///	3. filesTocreate(allbuild):1,
///	4. createFiles():1,
///	5. validCreated():1,
///
bool qtPrebuild::buildQt(cbProject * prj, bool allbuild)
{
	Project = prj;
	bool ok = Project != nullptr;
	if (!ok) return ok;
    // begin banner
	beginMesCreate();

	ok = false;
	Mes = Tab + _("Generating the complements files ") ;
	Mes += _("for the project ") + Quote + Project->GetTitle() + Quote ;
	print(Mes);
	///***********************************************
	///1- find good target with eligible files
	///***********************************************
	wxString mes = _T("1- findGoodfiles() ...") ;
	// analyzing all project files
	int nelegible =  findGoodfiles() ;
	mes += Lf + Tab + _T("elegible files= ") + (wxString()<<nelegible) ;
	if (nelegible > 0) {
		///************************************************
		///2-  registered all files in 'Registered'
		///************************************************
		mes += Lf + _T("2- addRecording() ...");
		int nfiles = addRecording() ;
		mes += Lf + Tab + _T("recorded files = ") + (wxString()<<nfiles) ;
		if (nfiles > 0) {

			///************************************************
			///3-  files to create to 'Filestocreate'
			///************************************************
			mes += Lf + _T("3- filesTocreate(") ;
			mes += allbuild ? _T("true") : _T("false");
			mes += _T(") ...") ;
			uint16_t ntocreate = filesTocreate(allbuild) ;
			mes += Lf + Tab + _T("files to create = ") + (wxString()<<ntocreate) ;
			if (ntocreate == 0 )
				ok = !allbuild ;
			else
				ok = true;

			if (ok) {
				///**********************************************************
				///4- create files
				/// from 'FileCreator', 'Registered'
				/// 1- adds files created in 'Createdfile'
				/// 2- create additional files as needed  by 'create(qexe, fileori)'
				///***********************************************************
				mes +=  Lf + _T("4- createFiles() ...");
				bool good = createFiles() ;
				if (good) {

					///**********************************
					///5- register files created
					///**********************************
					mes += Lf + _T("5- validCreated() ...")  ;
					good = validCreated() ;
					if (good) {
                //  print(mes);
						///**********************************
						///6- ???
						///**********************************
						//print(_T("- empty ...") ) ;
						ok = true;

					}	// end (good)
				} // end (good)
			}	//end (ntocreate > 0)
		} // end (nfiles > 0)
	} // end (nelegible > 0)
    // end banner
	endMesCreate();

	return ok;
}
///-----------------------------------------------------------------------------
/// Delete build files by target project
///
/// called by :
///	1. QtPregen::OnPrebuild(CodeBlocksEvent& event):1,
///
/// calls to :
///	1. findwasCreated():1,
///
bool qtPrebuild::cleanCreated(cbProject * prj)  {
//print(_T("qtPrebuild::cleanCreated(...)"));
	//1-  active project and target
		Project = prj ;
		bool ok =  Project != nullptr ;
		if (!ok) return ok ;

		DirProject = Project->GetBasePath() ;
		Nameactivetarget = Project->GetActiveBuildTarget()  ;
		Mes = _("Begin ") +  Quote + Thename + _T(" ") + VERSION_QTP + Quote + _T(" : ") ;
		print(Mes) ;
		Fileswithstrings.Add(Mes, 1) ;
		// begin message
		Mes = Tab + _T("--> ") + _("Search created file into the active target ") ;
		Mes += Quote + Nameactivetarget + Quote  + _T(" ...") ;
		print(Mes) ;

	//3- search created file
		// find file   -> meet 'Filewascreated = wxArrayString'
		uint16_t ncase = findwasCreated() ;
		if (ncase <= 0) {
			Mes = Tab + _("no files to delete in the active target !!") ;
			Mes += Lf + _("End ") + Quote + Thename  + Quote  ;
			print (Mes) ;
			return true ;
		}
		// begin message
		Mes = Tab + (wxString()<<ncase) + _(" files had been finded into active target") ;
		print(Mes) ;

// ATTENTION =>  THIS MESSAGE MAY BE VERY LONG
		//wxString temp = Lf + GetStringFromArray(Filewascreated, Eol, false ) ;
		//cbMessageBox(_("The project has ") + wxString()<<ncase + _(" created files:") + temp) ;
// <--
		// acknowledgment user
		wxString title = _("WARNING for the target ") + Quote + Nameactivetarget + Quote ;
		Mes = _("Do you want delete ") +  wxString()<<ncase ;
		Mes += _(" files created (ui_*, moc_*, qrc_*) ?") ;
		int16_t choice = cbMessageBox(Mes, title, wxICON_QUESTION | wxYES_NO) ;
		bool res = choice == wxID_YES ;
		if (!res) {
			Mes = Tab + _T("==> ") + _("You canceled cleaning complement files ") + _T(" !!") ;
			print (Mes) ;
			ok = true;
			Mes = _("End ") + Quote + Thename  + Quote ;
			print (Mes) ;
			return ok;
		}

		if (res && ncase > 0 ) {
		//4- cleaning up
			Mes = Tab + _T("==> ") + _("Cleaning up") + _T(" ...");	print(Mes) ;
			// local variables
			wxString fout, file, longfile ;
			ProjectFile * prjfile = nullptr ;
			//wxFileName filewx ;
			bool good ;
			uint16_t nd=0 ;
			// all was created file from last
			for (uint16_t u = 0 ; u < ncase;  u++ ) {
				fout = Filewascreated.Item(u) ;
			// relative filename, not UnixFilename !!
				prjfile = Project->GetFileByFilename(fout, true, false)  ;
				if (!prjfile)
					continue ;

				file = prjfile->relativeFilename ;
				if (file.IsEmpty())
					 continue ;

			// delete file from the disk
				longfile = DirProject + file ;
				if (wxFileExists(longfile)) {
					// display
					Mes = Tab ;
					Mes += wxString()<<u ;
					Mes += _T("- ") + fout ;
					print (Mes) ;
				//1- unregister in the project
					good = Project->RemoveFile(prjfile) ;
					if (good) {
						Mes = Tab + _T(" -> ") +  _("was remove to project")  ;
						//2- delete disk file
						good = wxRemoveFile(longfile) ;
						if (good) {
							nd++ ;
							Mes += _T(", ") ;
							Mes += _("was delete to disk") ;
							print(Mes);
							Mes = _T("");
						}
						else {
							Mes = _("Can't to erase file to disk") + Lf  ;
							Mes += Quote + longfile + Quote ;
							cbMessageBox(Mes);
							ok = false;
							break;
						}
					}
					else {
						Mes = _("Can't to remove file to project") + Lf  ;
						Mes += Quote + longfile + Quote ;
						cbMessageBox(Mes);
						ok = false;
						break;
					}
				}
				else {
					Mes = Quote + longfile + Quote + Lf + _(" NOT FOUND") ;
					Mes += _T(" !!!") + Lf ;
					print(Mes) ;
					ok = false;
					break;
				}
			}
			if (ok) ; {
			// rebuild the tree
				// svn 9501 : CB 13.12
				Manager::Get()->GetProjectManager()->GetUI().RebuildTree() ;
				// message
				Mes =  Tab + wxString()<<nd ;
				Mes += _(" files had been deleted of project") ;
				print(Mes);
				Mes = Tab + _("-> ") + _("Rebuild of the tree and save project") ;
				Mes += _T(" ...");
				print(Mes) ;
			// save project
				ok =Manager::Get()->GetProjectManager()->SaveProject(Project) ;
			// end clean
				Mes = Tab + _T("==> ... ") + _("Cleaning end") ;
				print(Mes) ;
				Mes = _("End ") + Quote + Thename  + Quote ;
				print (Mes) ;
			}
		}

	return ok;
}
///-----------------------------------------------------------------------------
/// Find file into project : moc_*.cxx, ui_*.h, qrc_*.cpp
/// return files created number,  feed 'Filewascreated'
///
/// called by  :
///	1. cleanbuildCreated():1
///
uint16_t qtPrebuild::findwasCreated() {  // around project files
// no file created
	Filewascreated.Clear() ;
	Nameactivetarget = Project->GetActiveBuildTarget() ;

	ProjectFile * prjfile ;
	wxString file ;
	wxArrayString tabtargets ;
	uint16_t nt, nfprj = Project->GetFilesCount() ;
// all files project
	for (uint16_t nf =0 ; nf < nfprj; nf++ ) {
		prjfile = Project->GetFile(nf);
		if (!prjfile)
			continue ;
	//	file name
		file = prjfile->relativeFilename  ;
		if (file.IsEmpty() )
			continue ;

	// copy  all targets
		tabtargets = copyArray(prjfile->buildTargets) ;
		nt = tabtargets.GetCount() ;
		if (nt > 0) {
			wxString nametarget ;
			bool good ;
			// all file targets
			for (uint16_t t=0; t < nt; t++) {
				nametarget = tabtargets.Item(t)  ;
				good = nametarget.Matches(Nameactivetarget)  ;
				if (good )
					break  ;
			}
			if (good ) {
			// not eligible ?
				if (! wasCreated(file))
					continue ;

				// more one
				Filewascreated.Add(file, 1) ;
			}
		}
	}

	// files number
	return Filewascreated.GetCount() ;
}
///-----------------------------------------------------------------------------
/// 'file' was created ?  ->  moc_*.cxx, ui_*.h, qrc_*.cpp
///
/// called by  :
///	1. findwasCreated():1,
///
bool qtPrebuild:: wasCreated(const wxString&  file) {
// short name
	wxString last = file.AfterLast(Slash) ;
// find begin
	wxString begin = last.Mid(0,3)  ;
	bool ok = begin.Matches(Moc) || begin.Matches(Ui+ _T("_")) || begin.Matches(Qrc) ;
	if (ok)  {
	// find extension
		wxString ext = last.AfterLast('.') ;
		ok = (ext.Matches(EXT_H) || ext.Matches(EXT_CPP) ) ;
	}

	return ok ;
}
///-----------------------------------------------------------------------------
/// called by :
///	1. endMesCreate():1,
///
/// calls to :
///	1. saveArray(...):1,
///
bool qtPrebuild::SaveFileswithstrings() {
	bool good = false ;
	if (Project) {
		wxString nameprj = Project->GetTitle();
		good = saveArray(Fileswithstrings, nameprj + _T(".cre")) ;
	}
	return good;
}
///-----------------------------------------------------------------------------
/// Reference of libQt by target: "" or "4r" or "4d" or "5r" or "5d"
///  qt4 -> '4', qt5 -> '5', release -> 'r' and debug -> 'd'
///
/// called by  :
///	1. isGoodTargetQt():1,
///	2. findGoodfiles():1
///
wxString qtPrebuild::refTargetQt(ProjectBuildTarget * buildtarget) {
	wxString refqt = _T("") ;
	if (! buildtarget)
		return refqt  ;

	wxArrayString tablibs = buildtarget->GetLinkLibs();
	int nlib = tablibs.GetCount() ;
	if (!nlib)
		return refqt ;

	// search lib
	bool ok = false ;
	wxString str =  _T("") ;
	wxString namelib ;
	int u=0, index= -1, pos ;
	while (u < nlib && !ok ) {
		// lower, no extension
		namelib = tablibs.Item(u++).Lower().BeforeFirst('.') ;
		// no prefix "lib"
		pos = namelib.Find(_T("lib"));
		if (pos == 0)
			namelib.Remove(0, 3) ;
		// begin == "qt"
		pos = namelib.Find(_T("qt")) ;
		if (pos != 0)
			continue ;
		// compare
		index = TablibQt.Index(namelib);
		ok = index != -1 ;
		// first finded
		if (ok)  {
			if (namelib.GetChar(2) == '5') {
				refqt = _T("5") ;
				// the last
				str = namelib.Right(1) ;
				if ( str.Matches(_T("d") ) )
					refqt += _T("d") ;
				else
					refqt += _T("r") ;
			}
			else  {
				refqt = _T("4") ;
				str = namelib.Right(2);
				if (str.Matches(_T("d4") ) )
					refqt += _T("d") ;
				else
					refqt += _T("r") ;
			}
			break;
		}
	}
	if (refqt.IsEmpty() ) {
		Mes = _("It is not a Qt target") + Lf + _("or nothing library") + _T(" ! ") ;
		Mes += Lf + _("Cannot continue.") ;
		cbMessageBox(Mes, _("QtPregen information"), wxICON_INFORMATION | wxOK)  ;
		//Fileswithstrings.Add(Mes, 1)
	}

	return refqt ;
}
///-----------------------------------------------------------------------------
/// Find path Qt installation
///
/// called by  :
///	1. beginMesCreate():1,
///
wxString qtPrebuild::findpathProjectQt() {
	wxString path = _("");
	if (!Project)
		return path;

//1- analyze the Project	'cbproject* Project'
	path = pathQt(Project) ;
	if (path.IsEmpty()) {
//2- analyze all targets
		bool ok = false ;
		ProjectBuildTarget * buildtarget ;
		int ntargets = Project->GetBuildTargetsCount() ;
		int nt = 0 ;
		while (nt < ntargets && ! ok ) {
		// retrieve the target Qt libraries paths
			buildtarget = Project->GetBuildTarget(nt++) ;
			if (! buildtarget)
				continue;

			path = pathQt(buildtarget) ;
			ok = ! path.IsEmpty() ;
		}
	}
	return path ;
}
///-----------------------------------------------------------------------------
/// Find path Qt container  ->  'CompileOptionsBase * container'
///
/// called by  :
///	1. findpathprojetQt():2,
///	2. findTargetQtexe(buildtarget):1,
///
wxString qtPrebuild::pathQt(CompileTargetBase * container) {
	wxString path = _T("");
	if (!container)
		return path;

	wxArrayString tablibdirs = container->GetLibDirs() ;
	int npath = tablibdirs.GetCount() ;
	wxString  path_nomacro ;
	if (npath > 0 ) {
		wxString path;
		bool ok = false ;
		int u = 0  ;
		while (u < npath && !ok ) {
			path = tablibdirs.Item(u++).Lower();
			ok = path.Find(_T("qt")) != -1 ;
			if (ok) {
			// if macro :  "$(#qt?.lib)", "$(qt?)\lib",
			// else   "?:\path\dirqt\lib"
				Mam->ReplaceMacros(path) ;
				path_nomacro =  path ;
				// remove "\lib"
				path_nomacro = path_nomacro.BeforeLast(Slash) ;
				path_nomacro += wxString(Slash)  ;
			}
		}
	}

	return path_nomacro  ;
}
///-----------------------------------------------------------------------------
/// Find path Qt exe : 'ProjectBuildTarget* buildtarget'
///
/// called by  :
///	1. createFiles():1,
///
bool qtPrebuild::findTargetQtexe(cbProject * parentbuildtarget)
{
	if (!parentbuildtarget)
		return false  ;

	wxString qtpath = pathQt(parentbuildtarget) ;
	if(qtpath.IsEmpty()) {
		Mes = _("Nothing path 'qt'") ;
		Mes += Lf + _("or nothing library !") + Lf + _("Cannot continue.") ;
		cbMessageBox(Mes, _T(""), wxICON_ERROR) ;
		return false ;
	}
	wxString qtexe = qtpath + _T("bin") + wxFILE_SEP_PATH  ;
	if (Win) {
		Mexe = qtexe + _T("moc.exe") ;
		Uexe = qtexe + _T("uic.exe") ;
		Rexe = qtexe + _T("rcc.exe") ;
		// compile *_fr.ts -> *_fr.qm
		Lexe = qtexe + _T("lrelease.exe") ;
	}
	else
	if (Linux) {	// check !
		Mexe = qtexe + _T("moc") ;
		Uexe = qtexe + _T("uic") ;
		Rexe = qtexe + _T("rcc") ;
		Lexe = qtexe + _T("lrelease") ;
	}
	else
	if (Mac) { 	// ???
		Mexe = qtexe + _T("moc");
		Uexe = qtexe + _T("uic") ;
		Rexe = qtexe + _T("rcc") ;
		Lexe = qtexe + _T("lrelease") ;
	}
	bool Findqtexe = wxFileExists(Mexe) && wxFileExists(Uexe) ;
	Findqtexe = Findqtexe && wxFileExists(Rexe) && wxFileExists(Lexe) ;
	if (!Findqtexe) {
		Mes = _("Could not query the executable Qt") ;
		Mes +=  _T(" !") ;
		Mes += _("Cannot continue.") ;
		cbMessageBox(Mes, _T(""), wxICON_ERROR) ;
	//	Fileswithstrings.Add(Mes, 1)
	}
	IncPathQt = pathIncludeMoc() ;
	DefinesQt = definesMoc() ;

	return Findqtexe ;
}
///-----------------------------------------------------------------------------
/// Save messages array to file
///
/// called by  :
///	1. SaveFileswithstrings():1,
/// calls to :
///	1. none
///
bool qtPrebuild::saveArray(const wxArrayString& table, wxString namefile)
{
	int n = table.GetCount() ;
	bool ok = n > 0 ;
	if (ok)  {
	// full name
		namefile = DirProject + namefile ;
	// exists ?
		ok = wxFileExists(namefile)  ;
		if (ok)  {
			wxRemoveFile(namefile) ;
		}
	// data to file
		wxString shortname = namefile.AfterLast(Slash) ;
		wxString mes ;
		wxString temp = GetStringFromArray(table, Eol, false ) ;
		ok = WriteFileContents(namefile, temp ) ;
		if (!ok)  {
			mes =  Tab + _("Could not write file ") ;
			mes += Quote + _T("locale\\") + shortname + Quote + _T(" !!") ;
			mes += Eol + Tab + _("Check that the name not contain illegal characters ...") ;
			Fileswithstrings.Add(mes, 1)  ;
			return false ;
		}
		mes = Tab + _("Write file ") ;
		mes += Quote + shortname + Quote  + Lf  + Lf ;
		Fileswithstrings.Add(mes, 1) ;
	// open file into editor
		ok = openedit(namefile) ;
	}

	return ok ;
}
///-----------------------------------------------------------------------------
/// called by  :
///	1. q_object(...):1,hasIncluded(wxString):1,
///
/// calls to :
///		none
///
wxString qtPrebuild::ReadFileContents(const wxString& filename)
{
	wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(filename));
	NormalizePath(fname, wxEmptyString);
	wxFile f(fname.GetFullPath());

	return cbReadFileContents(f);
}
///-----------------------------------------------------------------------------
/// called by  :
///	1. saveArray():1,
/// calls to :
///		none
///
bool qtPrebuild::WriteFileContents(const wxString& filename, const wxString& contents)
{
	wxFileName fname(Manager::Get()->GetMacrosManager()->ReplaceMacros(filename));
	NormalizePath(fname, wxEmptyString);
	wxFile f(fname.GetFullPath(), wxFile::write);

	return cbWrite(f, contents);
}
///-----------------------------------------------------------------------------
/// Open file to editor
///
/// called by  :
///	1. saveArray():1,
/// calls to :
///     none
///
bool qtPrebuild::openedit(const wxString& filename)
{
	bool ok = wxFileExists(filename) ;
	if (ok) {
		cbEditor * ed_open = Manager::Get()->GetEditorManager()->IsBuiltinOpen(filename) ;
		if (ed_open )
			ed_open->Close() ;

		ed_open = Manager::Get()->GetEditorManager()->Open(filename) ;
		ok = ed_open != nullptr ;
		if (!ok ) {
			Mes = _("Couldn't open ") + Quote + filename + Quote + _T(" !!") ;
			cbMessageBox(Mes, _T(""), wxICON_ERROR)  ;
			//print (Mes)
			return ok ;
		}
	}
	return ok ;
}
///-----------------------------------------------------------------------------
/// Close file to editor
///
/// called by  :
///	1. saveArray():1,
///
/// calls to :
///     none
///
bool qtPrebuild::closedit(const wxString& filename)
{
//print(file);
	bool ok = wxFileExists(filename) ;
	if (ok) {
		cbEditor * ed_open = Manager::Get()->GetEditorManager()->IsBuiltinOpen(filename) ;
		if (ed_open ) {
			ok = ed_open->Close()  ;
			if (!ok ) {
				Mes = _("Couldn't close ") + Quote + filename + Quote + _T(" !!") ;
				cbMessageBox(Mes, _T(""), wxICON_ERROR) ;
				// print (Mes) ;
				return ok ;
			}
		}
	}
	return ok ;
}
///-----------------------------------------------------------------------------
/// Get date
///
/// called by  :
///	1. begimMesCre():1,
///	2. endMesCre():1
///
wxString qtPrebuild::date()
{
	wxDateTime now = wxDateTime::Now();
	//wxString str = now.FormatISODate() + _T("-") + now.FormatISOTime();
	wxString str = now.FormatDate() + _T("-") + now.FormatTime();
    return str ;
}
///-----------------------------------------------------------------------------
/// Execution time
///
/// called by  :
///	1. endMesCre():1
///
wxString qtPrebuild::duration()
{
	return wxString::Format(_T("%ld ms"), clock() - Start  );
}
///-----------------------------------------------------------------------------
/// Looking for eligible files to the active target,
/// 	meet to table 'Filecreator' : return eligible file number
///
/// called by  :
///	1. QtPregen::OnPrebuild( ...):1
///
/// calls to :
///	1. isVirtualTarget(Nameactivetarget):1,
///	2. isGoodTargetQt(Nameactivetarget):1,
///	3. isEligible(file):1,
///
uint16_t qtPrebuild::findGoodfiles()
{
	Filecreator.Clear();
// active target
	Nameactivetarget = Project->GetActiveBuildTarget() ;
	Mes = Lf + Tab + _("The active target ") ;
	Mes += Quote + Nameactivetarget + Quote  + _T(" :") ;
	//print(Mes) ;
	Fileswithstrings.Add(Mes, 1) ;
// virtual ?
	if (isVirtualTarget(Nameactivetarget) )
		return 0 ;

// no good target
	if (! isGoodTargetQt(Nameactivetarget))
		return 0 ;

// around project files
	ProjectFile * prjfile ;
	wxString file ;
	wxArrayString tabtargets ;
	int nt, nft=0 ;
	int nfprj = Project->GetFilesCount()  ;
// all files project
	for (int nf =0 ; nf < nfprj; nf++ ) {
		prjfile = Project->GetFile(nf);
		if (!prjfile)
			continue  ;

	//	file name
		file = prjfile->relativeFilename ;
		if (file.IsEmpty() )
			continue  ;

	// copy  all targets
	//	tabtargets = copyarray(prjfile->buildTargets) ;
		tabtargets = prjfile->buildTargets ;
		nt = tabtargets.GetCount()  ;
		if (nt > 0) {
			wxString nametarget ;
			bool good ;
			// all file targets
			for (int t=0; t < nt; t++) {
				nametarget = tabtargets.Item(t) ;
				good = nametarget.Matches(Nameactivetarget) ;
				if (good )
					break ;
			}
			if (good ) {
			// not eligible ?
				if (! isEligible(file))
					continue  ;

			// add the file and it target
				Filecreator.Add(file, 1) ;
			}
		}
	}

//  number file eligible
	return Filecreator.GetCount()  ;
}
///-----------------------------------------------------------------------------
/// Give the good type of target for Qt
///
/// called by  :
///	1. findGoodfiles():2,
///
/// calls to:
///	1. refTargetQt(buildtarget):1,
///
bool qtPrebuild::isGoodTargetQt(const wxString& nametarget)
{
	ProjectBuildTarget * buildtarget = Project->GetBuildTarget(nametarget) ;
	bool ok = buildtarget != nullptr;
	if (!ok) return ok ;

	ok = buildtarget->GetTargetType() != ::ttCommandsOnly  ;
	wxString str = refTargetQt(buildtarget)  ;
	bool qt = ! str.IsEmpty()  ;

	return ok && qt ;
}
///-----------------------------------------------------------------------------
/// Search virtual target
///
/// called by  :
///	1. findGoodfiles():1,
///
bool qtPrebuild::isVirtualTarget(const wxString& nametarget) {
	// search virtual target : vtargetsTable <- table::wxArrayString
	wxString namevirtualtarget = _T("") ;
	wxArrayString vtargetsTable = Project->GetVirtualBuildTargets () ;
	int nvirtual = vtargetsTable.GetCount() ;
	bool ok = nvirtual > 0 ;
	if (ok) {
		//PrintTable(_T("vtargetsTable"), vtargetsTable)
		ok = false  ;
	// virtual name
		for (int nv = 0; nv < nvirtual ; nv++ ) {
			namevirtualtarget = vtargetsTable.Item(nv) ;
			ok = nametarget.Matches(namevirtualtarget) ;
			if (ok)  {
				Mes = _T("'") + namevirtualtarget + _T("'") ;
				Mes += _(" is a virtual target !!") ;
				Mes += Lf + _("NOT YET IMPLEMENTED...") ;
				Mes += Lf + _("... you must use the real targets.")  ;
				Mes += Lf + _("Cannot continue.") ;
				cbMessageBox(Mes, _T(""), wxICON_ERROR)  ;
				Fileswithstrings.Add(Mes, 1)  ;
				break  ;
			}
		}
	}

	return ok  ;
}
///-----------------------------------------------------------------------------
/// For replace 'Targetsfile = prjfile.buildTargets'
/// copy a 'wxArrayString' to an another
///
/// called by  :
///	1. findwasCreated():1,
///	2. filesTocreate(bool):1,
///
wxArrayString qtPrebuild::copyArray (const wxArrayString& strarray) {
	wxArrayString tmp ;
	int nl = strarray.GetCount()  ;
	if (nl == 0)
		return  tmp ;
	// a line
	wxString line;
	for (int u = 0; u < nl; u++) {
	// read strarray line
		line = strarray.Item(u) ;
	// write line to tmp
		tmp.Add(line, 1) ;
	}

	return tmp ;
}
///-----------------------------------------------------------------------------
/// Search elegible files
/// 'file' was created ?  ->  moc_*.cxx, ui_*.h, qrc_*.cpp
///
/// called by  :
///	1. findGoodfiles():1,
///
/// calls to :
/// 1. q_object(file, _T("Q_OBJECT") ):1;
///
bool qtPrebuild::isEligible(const wxString& file)
{
	wxString name = file.AfterLast(Slash) ;
	// eliminates complements files
	int pos = name.Find(_T("_")) ;
	if (pos != -1) {
		wxString pre  = name.Mid(0, pos)  ;
		if (pre.Matches(Moc) || pre.Matches(Ui) || pre.Matches(Qrc) )
			return false  ;
	}
	wxString ext = name.AfterLast('.')  ;
	bool ok = ext.Matches(Ui) || ext.Matches(Qrc) ;
	if (ext.Matches(EXT_H) || ext.Matches(EXT_HPP) || ext.Matches(EXT_CPP) ) {
		bool good = q_object(file, _T("Q_OBJECT") ) > 0  ;
		ok = ok || good  ;
	}
	return ok ;
}
///-----------------------------------------------------------------------------
/// Find valid "Q_OBJECT" or "Q_GADGET"
///
/// Looks for occurrences of "Q_OBJECT" or "Q_GADGET" valid in the file.
///	Remarks:
///		Qt documentation indicates another keyword METATOBJETS -> "Q_GADGET"
///		like "Q_OBJECT" but when the class inherits from a subclass of QObject.
///	Algorithm:
///		1 - eliminates comments type C
///		2 - eliminates comments type Cpp  '// .... eol'
///		3 - one seeks "Q_OBJECT" then 'xQ_OBJECTy' to verify that 'x' in
///			[0xA, 0x9, 0x20] and 'y' in [0xd (or 0xA), 0x9, 0x20]
///		4 - one seeks "Q_GADGET" in the same way
///
///-----------------------------------------------------------------------------
/// Search 'qt_text' in 'tmp'
///
/// called by  :
///	1. q_object():2
///
int  qtPrebuild::qt_find (wxString tmp, const wxString& qt_text)
{
	//wxChar
	uint8_t tab = 0x9, espace = 0x20  ;
	int len_text = qt_text.length() ;
	int posq, number =0 ;
	bool ok, good, goodb, gooda ;

	do {
		posq = tmp.Find(qt_text) ;
		ok = posq != -1 ;
		if (ok) {
		// pred and next caracter
			//wxUChar
			uint8_t xcar = tmp.GetChar(posq-1) ;
			//wxUChar
			uint8_t carx = tmp.GetChar(posq + len_text)  ;
		/*
		Mes = _T(" char before '") ;
		Mes += wxString()<< xcar ;
		Mes +=_T("', char after '");
		Mes += wxString()<< carx ;
		Mes += _T("'");
		print (Mes);
		*/
		// only autorized caracters
			// before
			goodb = (xcar == espace || xcar == tab || xcar == Sepf) ;
			// next
			gooda = (carx == espace || carx == tab || carx == Sepd || carx == Sepf);
			gooda = gooda || carx == '(' || carx == '_' ;
			good = goodb && gooda  ;
			if (good)
				number++  ;
		// delete analyzed
			tmp.Remove(0, posq + len_text)  ;
		}
	} while (ok) ;

	return number ;
}
///-----------------------------------------------------------------------------
/// Return occurrences of "Q_OBJECT" + occurrences of "Q_GADGET"
///
/// called by  :
///	1. isEligible():1
///
int qtPrebuild::q_object(const wxString& filename, const wxString& qt_macro)
{
//1- the comments
	wxString CPP = _T("//") ;
	wxString DCC = _T("/*") ;
	wxString FCC = _T("*/") ;
	// end of line
	wxString SF, SD;
	SF.Append(Sepf); SD.Append(Sepd) ;
	// the result
	bool good= false ;
//2- verify exist
	wxString namefile = DirProject + filename ;
	if (! wxFileExists(namefile)) {
		Mes = Quote + namefile + Quote + _(" NOT FOUND") + _T(" !!!") + Lf ;
		print(Mes) ;
		Fileswithstrings.Add(Mes, 1);
		return -1  ;
	}
//3- read source 'namefile'
	wxString source = ReadFileContents(namefile) ;
	if (source.IsEmpty())  {
		Mes = Quote + namefile + Quote + _(" : file is empty") +  _T(" !!!") + Lf ;
		print(Mes) ;
		return -1 ;
	}
	// wxString
	int reste = source.length() ;
	// a string copy
	wxString temp = source.Mid(0, reste ) ;
	// pos
	int posdcc, posfcc, poscpp , moins ;
	// wxString
	wxString sup , after ;
	// boolean
	bool ok ;
//4- delete C comments
	do {
		reste = temp.length() ;
	// first DCC next FCC
		posdcc = temp.Find(DCC) ;
		if (posdcc != -1) {
			reste = reste - posdcc ;
			after = temp.Mid(posdcc, reste) ;
			posfcc = after.Find(FCC) ;
			// 'posfcc' relative to 'posdcc' !!
		}
		// comments exists ?
		ok = posdcc != -1 && posfcc != -1  ;
		if (ok) {
		// delete full comment
			moins = posfcc + FCC.length() ;
			temp.Remove(posdcc, moins) ;
		}
	} while (ok)  ;
//5- delete C++ comments
	do {
		reste = temp.length()  ;
	// start of a comment C
		poscpp = temp.Find(CPP) ;
		if (poscpp != -1) {
		// verify the caracter before : 'http://www.xxx' !!
			wxChar xcar = temp.GetChar(poscpp-1) ;
			if (xcar != ':' ) {
				reste = reste - poscpp  ;
			// string comment over begin
				after = temp.Mid(poscpp, reste) ;
			// end comment
				posfcc = after.Find(SF) ;
			}
			else
				poscpp = -1  ;
		}
		ok = poscpp != -1 && posfcc != -1 ;
		if (ok) {
		// delete full comment
			temp.Remove(poscpp, posfcc) ;
		}
	} while (ok) ;
//6- find "Q_OBJECT" and/or "Q_GADJECT"
	int number = 0  ;
	wxString findtext = _T("Q_OBJECT") ;
	if ( qt_macro.Matches(findtext) ) {
		number = qt_find(temp, findtext)  ;
		findtext = _T("Q_GADGET") ;
		number += qt_find(temp, findtext)  ;
	}
	else  {
      // TODO ...
	}

	return number ;
}
///-----------------------------------------------------------------------------
/// To register all new files in project : 'Registered' is meet
///	return 'true' when right
///
/// called by  :
///	1. QtPregen::OnPrebuild():1,
///
/// calls to :
///	1. indexTarget(Nameactivetarget):1,
///	2. nameCreated(fcreator):1,
///	3. inProjectFile(fout):1,
///	4. hasIncluded(fcreator):1,
///

uint16_t qtPrebuild::addRecording()
{
	Registered.Clear();
// local variables
	bool valid = false , inproject = false  ;
	wxString fout, fcreator, strindex = _T("0") ;
	uint16_t n=0, index, indextarget = indexTarget(Nameactivetarget) ;
	ProjectFile *  prjfile;
// read file list to 'Filecreator'  (*.h, *.cpp, *.qrc, *.ui)
	uint16_t nfiles = Filecreator.GetCount() ;
	for (uint16_t u=0; u < nfiles ; u++ ) {
		// creator
		fcreator = Filecreator.Item(u)  ;
		//  this is where you add the name of the target
		fout = Dirgen + Nameactivetarget + wxString(Slash)  ;
		fout += nameCreated(fcreator) ;
		// checks if the file exists in the project
		inproject = inProjectFile(fout) ;
		if (!inproject) {
			// has included ?
			wxString extin = fcreator.AfterLast('.') ;
			wxString extout = fout.AfterLast('.')  ;
			bool include = false ;
			if ( extin.Matches(EXT_H) && extout.Matches(EXT_CPP) )
				include =  hasIncluded(fcreator) ;

		// AddFile(Nametarget, file, compile, link, weight)
			prjfile = Project->AddFile(Nameactivetarget, fout, !include, !include, 50);
			valid = true;
			if (!prjfile) {
				valid = false;
			// display message
				Mes  = _T("===> ") ;
				Mes += _("can not add this file ");
				Mes += Quote + fout + Quote + _(" to target ") + Nameactivetarget ;
				printErr (Mes) ;
				cbMessageBox(Mes, _T("AddFile(...)"), wxICON_ERROR) ;
			}
			// display
			n++ ;
			Mes = Tab + (wxString()<<n) + _T("- ") ;
			Mes += Quote + fout + Quote ;
			if (include) {
				Mes += Lf + Tab + Tab + _T("*** ") + _("This file is included, ") ;
				Mes += _("attributes 'compile' and 'link' will be set to 'false'") ;
			}
			//print(Mes);
			Fileswithstrings.Add(Mes, 1) ;
		}
		// valid
		if ( valid || inproject){
		// registered in table
			Registered.Add(fout, 1) ;
		}
	}
// tree refresh
	// svn 9501 : CB 13.12
	Manager::Get()->GetProjectManager()->GetUI().RebuildTree() ;
	// svn 8629 : CB 12.11
	//Manager::Get()->GetProjectManager()->RebuildTree() ;
//  end registering
	uint16_t nf = Registered.GetCount() ;
	Mes = Tab + Tab + _T("- ") + (wxString()<<nf) + _T(" ") ;
	Mes += _("supplements files are registered in the plugin") ;
	//print(Mes) ;
	Fileswithstrings.Add(Mes, 1) ;

	return  nf;
}
///-----------------------------------------------------------------------------
/// Returns the index of a target in the project
///
/// called by  :
///	1. addRecording():1,
///
int qtPrebuild::indexTarget(const wxString& nametarget)
{
	ProjectBuildTarget *  buildtarget ;
	uint16_t ntargets = Project->GetBuildTargetsCount() ;
	if (ntargets < 1) return -1 ;

	for (uint16_t nt = 0; nt < ntargets; nt++) {
		buildtarget = Project->GetBuildTarget(nt) ;
		if (!buildtarget)
			continue ;

		if (buildtarget->GetTitle().Matches(nametarget) ) ;
			return nt  ;
	}
	return -1 ;
}
///-----------------------------------------------------------------------------
/// Gives the name of the file to create on
///
/// called by  :
///	1. addRecording():1,
///
wxString qtPrebuild::nameCreated(const wxString& file)
{
	wxString name  = file.BeforeLast('.') ;
	wxString fout ;
	if (name.Find(Slash) > 0) {
	// short name
		name = name.AfterLast(Slash) ;
	// before path
		fout += file.BeforeLast(Slash) + wxString(Slash) ;
	}
	wxString ext  = file.AfterLast('.')  ;
//1- file *.ui  (forms)
	if ( ext.Matches(Ui) )
		fout += Ui + _T("_") + name + DOT_EXT_H  ;
	else
//2- file *.qrc  (resource)
	if (ext.Matches(Qrc) )
		fout += Qrc + _T("_") + name + DOT_EXT_CPP  ;
	else
//3- file *.h or *hpp with 'Q_OBJECT' and/or 'Q_GADGET'
	if (ext.Matches(EXT_H) || ext.Matches(EXT_HPP)  )
		fout +=  Moc + _T("_") + name + DOT_EXT_CPP ;
	else
//4- file *.cpp with 'Q_OBJECT' and/or 'Q_GADGET'
	if (ext.Matches(EXT_CPP) )
		fout +=  name + _T(".moc") ;

	fout = fout.AfterLast(Slash) ;

	return fout  ;
}
///-----------------------------------------------------------------------------
/// Indicates whether a file is registered in the project
///
/// called by  :
///	1. addRecording():1,
///
bool qtPrebuild::inProjectFile(const wxString& file)
{
    // relative filename !!!
	ProjectFile * prjfile = Project->GetFileByFilename (file, true, false) ;
	return prjfile != nullptr;
}
///-----------------------------------------------------------------------------
///	Search included file
// *.cpp contains  (#include "moc_*.cpp") or (#include "*.moc")
///
/// called by  :
///	1. addRecording():1,
///
bool qtPrebuild::hasIncluded(const wxString& file)
{
// searcch into '*.cpp' -> "#include "moc_*.cpp" or "#include "*.moc"
	wxString namefile = file.BeforeLast('.') + DOT_EXT_CPP;
// load file  xxxx.cpp
	if (! wxFileExists(namefile) )
		 return false  ;

	wxString source = ReadFileContents(namefile) ;
	if (source.IsEmpty())
		return false ;

// search >"moc_namefile.cpp"<
	namefile = namefile.AfterLast(Slash) ;
	wxString txt = _T("\"moc_") + namefile + _T("\"") ;
	bool include =  source.Find(txt) != -1 ;
	if (!include) {
// search >"namefile.moc"<
		txt = _T("\"") + namefile + _T(".moc\"") ;
		include  = source.Find(txt) != -1 ;
	}

	return  include ;
}
///-----------------------------------------------------------------------------
///  Create directory for "qtbrebuilt\name-target\"
///
///  called by :
///	1. createComplement():1,
///
bool qtPrebuild::createDir (const wxString&  dirgen)
{
	bool ok = true  ;
	if (! wxDirExists(dirgen)) {
		ok = wxMkdir(dirgen, 0) ;
		if (!ok) {
			Mes = _("Can't create directory ")  ;
			Mes += Quote + dirgen + Quote + _T(" !!") ;
			cbMessageBox(Mes, _T("createDir()"), wxICON_ERROR) ;
		}
	}

	return ok ;
}
///-----------------------------------------------------------------------------
///  Remove directory for "qtbrebuilt\name-target\"
///
///  called by :
///		none
///
bool qtPrebuild::removeDir(const wxString&  dirgen )
{
	bool ok = wxDirExists(dirgen) ;
	if (ok) {
		ok = wxRmdir(dirgen)  ;
		if (!ok) {
			Mes = _("Can't remove directory ") ;
			Mes += Quote + dirgen + Quote + _T(" !!") ;
			cbMessageBox(Mes, _T(""), wxICON_ERROR) ;
		}
	}

	return ok ;
}
///-----------------------------------------------------------------------------
/// Must complete the table 'Filestocreate'
///
/// called by
///    1. QtPregen::prebuild (...)
///
uint16_t qtPrebuild::filesTocreate(bool allrebuild)
{
	Filestocreate.Clear();
// save all open files
	bool ok = Manager::Get()->GetEditorManager()->SaveAll();
	if (!ok ) {
		Mes = _("Couldn't save all files ") ;
		Mes += _T(" !!")  ;
		cbMessageBox(Mes, _T(""), wxICON_ERROR)  ;
		print (Mes);
		return ok  ;
	}
    uint16_t  ntocreate = 0;
// all rebuild : copy table
	if (allrebuild) {
		Filestocreate = copyArray(Registered) ;
		ntocreate = Filestocreate.GetCount();
	}
// build or rebuild not identical
	else {
	// local variables
		wxString inputfile, nameout ;				// wxString
		bool inproject ;						// bool
	// analyze all eligible files/targets
		uint16_t nfiles = Filecreator.GetCount() ; 	// integer
		for (uint16_t i=0; i < nfiles ; i++ ) {
		// relative name inputfile
			inputfile = Filecreator.Item(i) ;
		// file registered to create
			nameout = Registered.Item(i) ;
		// in project ?
			inproject =  inProjectFile(nameout) ;
			Identical = false ;
		//- verify datation on disk
			if (inproject && Dexeishere) {
			// verify if exists file
				if (wxFileExists(nameout)) {
				// test his modification date
					Identical = CompareDate(inputfile, nameout) ;
				}
			}
			// identical date -> no create
			if (Identical)
			// add 'Devoid' to 'Filetocreate' and continue
				Filestocreate.Add(Devoid, 1) ;
			else  {
			// to create
				Filestocreate.Add(nameout, 1)  ;
				ntocreate++;
			}
		} // end for
	}

	return ntocreate;
}
///-----------------------------------------------------------------------------
/// Compare file date
///
/// called by  :
///	1. createFiles():1,
///
bool qtPrebuild::CompareDate(const wxString& fileref, const wxString&  filetarget)
{
	if ( !(wxFileExists(fileref) && wxFileExists(filetarget) ) )
		return false;

	wxFileName refname(fileref);
	wxDateTime refdate = refname.GetModificationTime();

	wxFileName target(filetarget);
	wxDateTime targetdate = target.GetModificationTime();

	bool ok = refdate.IsEqualTo(targetdate);

	return ok ;
}
///-----------------------------------------------------------------------------
/// Set date to target
///
/// called by  :
///	1. createComplement(qexe, index):1
///
bool qtPrebuild::ModifyDate(const wxString&  fileref, const wxString& filetarget)
{
	if ( !(wxFileExists(fileref) && wxFileExists(filetarget) ) )
		return false;

	wxFileName refname(fileref);
	wxDateTime refdate = refname.GetModificationTime();

	wxFileName target(filetarget);
	wxDateTime targetdate = target.GetModificationTime();

	bool ok  = target.SetTimes(0, &refdate, 0);

	return ok ;
}
///-----------------------------------------------------------------------------
/// Create files before build project :
///	- additions built files before generation
///	1. 'Filecreator' is meet by 'findGoodfiles()'
///	2. 'Registered is filled by 'addRecording()'
///	3. 'Filestocreate' filled by 'filesTocreate(allrebuild)'
///	4. meet 'Createdfile'
///
/// return true if all right
///
/// called by  :
///	1. buildQt(cbProject*, bool) :1,
///
///	calls to :
///	1. isEmpty(...):1,
///	2. findTargetQtexe(buildtarget->GetParentProject()):1,
///	3. createComplement(...):3,
///
bool qtPrebuild::createFiles()
{
	Createdfile.Clear();
// nothing to do ?
	bool emptiness = isEmpty (Filestocreate) ;
	if (emptiness) {
	// already created
		Mes = Tab + _T("- ") + _("thing to be done (all items are up-to-date)") ;
		print(Mes) ;
		Fileswithstrings.Add(Mes, 1) ;
		return true  ;
	}

//  message "begin create"
	Mes = Lf + Tab + _T("=> ") + _("Begin create file to target")  ;
	Fileswithstrings.Add(Mes, 1) ;

// search target qt path for moc, ...
	ProjectBuildTarget * buildtarget = Project->GetBuildTarget(Nameactivetarget)  ;
	bool ok = buildtarget != nullptr ;
	if (!ok) return ok ;

	ok = findTargetQtexe(buildtarget->GetParentProject()) ;
	if (!ok) return ok ;

// local variables
	wxString nameout, ext ; 	// wxString
	bool created = false;	// bool
	// used by 'createComplement()'
	Nfilescreated = 0 ;
// analyze all eligible files/target
	uint16_t  nfiles = Filestocreate.GetCount() ;
	for (uint16_t   i=0; i < nfiles ; i++ ) {
		nameout = Filestocreate.Item(i) ;
		if (nameout.Matches(Devoid) )
			continue  ;

	// extension creator
		ext = Filecreator.Item(i).AfterLast('.') ;
	// create file
		//1- file *.ui : launch 'uic.exe'
		if ( ext.Matches(Ui) )
			created = createComplement(Uexe, i) ;
		else
		//2- file *.qrc : launch 'rcc.exe'
		if (ext.Matches(Qrc) )
			created = createComplement(Rexe, i)  ;
		else
		//3- file with 'Q_OBJECT' and/or 'Q_GADGET' : launch 'moc.exe'
		if (ext.Matches(EXT_H) || ext.Matches(EXT_HPP) || ext.Matches(EXT_CPP) )
			created = createComplement(Mexe, i)  ;

	// more one
		if (created)
			Createdfile.Add(nameout, 1)  ;
		else
			Createdfile.Add(Devoid, 1)  ;

	}  // end forManageprj.RebuildTree()

	uint16_t nf = Createdfile.GetCount() ;
	Mes =  Tab + _T("- ") + (wxString()<<nf) + _T(" ")  ;
	Mes += _("supplements files are created in the target") ;
	print(Mes)  ;

// message "end create"
	Mes = Tab + _T("<= ") + _("End create file") ;
	Fileswithstrings.Add(Mes, 1)  ;

	return true   ;
}
///-----------------------------------------------------------------------------
/// Search all 'Devoid' to an wxArrayString
///
/// called by  :
///	1. createFiles()!1,
///
bool qtPrebuild::isEmpty (const wxArrayString& arraystr)
{
	uint16_t nf = arraystr.GetCount() ;
	bool devoid = false ;
	if (nf > 0) {
		for (uint16_t u= 0; u < nf ; u++ ) {
			devoid  = arraystr.Item(u).Matches(Devoid) ;
			if (!devoid)
				break  ;
		}
	}

	return devoid ;
}
///-----------------------------------------------------------------------------
///	Execute commands 'moc', 'uic', 'rcc' : return 'true' if file created
///
/// called by  :
///	1. createFiles():6,
///
bool qtPrebuild::createComplement(const wxString& qexe, const uint16_t index)
{
	bool created = false ;
//1- name relative input file
	wxString inputfile = Filecreator.Item(index) ;
	// create directory for Nameactivetarget
	wxString dirgen = Registered.Item(index).BeforeLast(Slash)  ;
	dirgen +=  wxFILE_SEP_PATH ;
	bool ok = createDir(dirgen) ;
	if (!ok) return ok ;

	// add search path for compiler
	ProjectBuildTarget * buildtarget = Project->GetBuildTarget(Nameactivetarget) ;
	if (!buildtarget)
		return created ;

	// add include directory
	buildtarget->AddIncludeDir(dirgen) ;
//2- full path name  output file
	wxString Outputfile = Registered.Item(index) ;
//3- build command
	wxString command = qexe  ;
	// add file name whithout extension
	if (qexe.Matches(Rexe)) {
		wxString name = inputfile.BeforeLast('.')  ;
		command +=  _T(" -name ") + name  ;
	}
	if (qexe.Matches(Mexe)) {
		command +=  DefinesQt + IncPathQt  + _T(" -D__GNUC__ -DWIN32") ;
	}
	// add input file
	command += _T(" \"") + inputfile + _T("\"") ;
	// add output file
	command += _T(" -o \"") + Outputfile + _T("\" ") ;

//4- execute command line : use short file name
	wxString strerror = ExecuteAndGetOutputAndError(command, true)  ;
	created =  strerror.IsEmpty() ;
	Nfilescreated++ ;
	Mes = Tab + (wxString()<<Nfilescreated) + _T("- ") + Quote + inputfile + Quote  ;
	Mes +=  _T(" ---> ") + Quote + Outputfile + Quote ;
	Fileswithstrings.Add(Mes, 1)  ;
	Mes = _T(">>") + command + _T("<<") ;
// log to debug
	//printMes) ;
	Fileswithstrings.Add(Mes, 1)  ;
	// create error
	if (! created) {
		Mes = _("Creating ") + Quote + Outputfile + Quote + _(" failed :") ;
		Mes +=  Lf + _T("=> ") ;
		Mes += strerror.BeforeLast(Lf.GetChar(0)) ;
		printErr (Mes) ;
		cbMessageBox(Mes) ;
		Fileswithstrings.Add(Mes, 1) ;
	// unregister the file 'Outputfile' in the project
		ProjectFile * prjfile =  Project->GetFileByFilename(Outputfile, true, false) ;
		if (!prjfile) {
			Project->RemoveFile(prjfile) ;
			// svn 9501 : CB 13.12
			Manager::Get()->GetProjectManager()->GetUI().RebuildTree() ;
			// svn 8629 : CB 12.11
			//Manager::Get()->GetProjectManager()->RebuildTree() ;
		}
	}
	else {
		if (Dexeishere)
		// modify date of created file
			Identical = ModifyDate(inputfile ,Outputfile) ;
	}

//5- return 'true' if created file, 'false' if error
	return created ;
}
///-----------------------------------------------------------------------------
/// Give include path qt for 'Moc'
///
/// called by :
/// 1.findTargetQtexe(cbProject *):1,
///
wxString qtPrebuild::pathIncludeMoc()
{
	wxArrayString tabincludedirs = Project->GetIncludeDirs() ;
//	PrintTable(_T("tabincludedirs"),tabincludedirs)
	wxArrayString tabpath ;
	wxString line ;
	uint16_t ncase = tabincludedirs.GetCount()  ;
	for (uint16_t u = 0; u < ncase ; u++) {
		line = tabincludedirs.Item(u) ;
	//	if (line.Find(_T("#qt")) != -1 ) {
			Mam-> ReplaceMacros(line) ;
			line = 	_T("-I") + Quote + line + Quote  ;
			tabpath.Add(line, 1) ;
	//	}
	}
	//PrintTable(_T("tabpath"),tabpath)
	// build 'incpath'
	wxString incpath = _T(" ") + GetStringFromArray(tabpath, _T(" "), true) + _T(" ");
//	print(incpath)

	return incpath  ;
}
///-----------------------------------------------------------------------------
/// Give 'defines' qt for 'Moc'
///
/// called by :
///	1. findTargetQtexe(buildtarget):1,
///
wxString qtPrebuild::definesMoc()
{
	wxArrayString tabdefines = Project->GetCompilerOptions() ;
	//PrintTable(_T("tabdefines"),tabdefines)
	wxArrayString tabdef ;
	wxString  line ;
	uint16_t ncase = tabdefines.GetCount()  ;
	for (uint16_t u = 0; u < ncase ; u++) {
		line = tabdefines.Item(u) ;
		if (line.Find(_T("-D")) != -1 )
			tabdef.Add(line, 1) ;
	}
	//PrintTable(_T("tabdef"),tabdef)
// build 'def'
	wxString def = _T(" ") + GetStringFromArray(tabdef, _T(" "), true) + _T(" ") ;

	return def ;
}
///-----------------------------------------------------------------------------
/// Verify if at least one valid file is saved in 'Createdfile'
///
/// called by  :
/// 1. buildQt(cbProject *, bool):1,
///
bool qtPrebuild::validCreated()
{
	bool ok = false  ;
	uint16_t ncase = Createdfile.GetCount()  ;
	while (ncase > 0 && !ok) {
		ok = ! Createdfile.Item(--ncase).Matches(Devoid)  ;
	}
	if (ok) {
		// svn 9501 : CB 13.12
		Manager::Get()->GetProjectManager()->GetUI().RebuildTree()  ;
		// svn 8629 : CB 12.11
		// Manager::Get()->GetProjectManager()->RebuildTree()  ;
		Mes = Tab + _("-> ") + _("Save project and rebuild of the tree") ;
		Mes += _T(" ...") ;
		print(Mes) ;
		Fileswithstrings.Add(Mes + Lf, 1) ;
	// save project
		ok = Manager::Get()->GetProjectManager()->SaveProject(Project)  ;
	}
	return ok ;
}
///-----------------------------------------------------------------------------
/// Execute a command
///
/// called by :
///	1. createComplement(wxString, uint16_t)
///
wxString qtPrebuild::ExecuteAndGetOutputAndError(const wxString& command, bool prepend_error)
{
	wxArrayString output;
	wxArrayString error;
	wxExecute(command, output, error, wxEXEC_NODISABLE);

	wxString str_out;

	if ( prepend_error && !error.IsEmpty())
		str_out += GetStringFromArray(error,  _T("\n"));

	if (!output.IsEmpty())
		str_out += GetStringFromArray(output, _T("\n"));

	if (!prepend_error && !error.IsEmpty())
		str_out += GetStringFromArray(error,  _T("\n"));

	return  str_out;
}
///-----------------------------------------------------------------------------
/// Check that the filename not contain illegal characters for windows !
///
/// called by :
///		 none
///
wxString qtPrebuild::filenameOk(const wxString& namefile)
{
	wxString banned = _T("") ;
	if (Win) {
		wxString illegal = _T("[:*?\"<>|]") ;
		uint16_t pos, ncar = illegal.length() ;
	// all illegal characters
		for (uint16_t u = 0; u < ncar; u++) {
			banned += illegal.GetChar(u) ;
			pos = namefile.Find(banned) ;
			if (pos != -1)
				break ;
			banned = _T("")  ;
		}
	}

	return banned  ;
}
///-----------------------------------------------------------------------------