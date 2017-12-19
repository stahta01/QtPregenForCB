/***************************************************************
 * Name:      print.h
 * Purpose:   Code::Blocks plugin	'qtPregen.cbp'  1.1
 * Author:    LETARTARE
 * Created:   2015-10-17
 * Modified:  2017-11-29
 * Copyright: LETARTARE
 * License:   GPL
 **************************************************************/

///-----------------------------------------------------------------------------
#ifndef _PRINT_H_
	#define _PRINT_H_

///-----------------------------------------------------------------------------
/** @brief end of line for Win/Linux/OX
 */
#define 	Cr 		wxString(_T("\r"))
#define 	Lf 		wxString(_T("\n"))
#define 	CrLf 	wxString(_T("\r\n"))
#define 	Eol 	wxString(_T("\r\n"))
#define 	Quote 	wxString(_T("'"))
#define		Dquote  wxString(_T("\""))
#define 	Tab		wxString(_T("\t"))
#define 	Space	wxString(_T(" "))
/** text separator
 */
#define 	SepD 	13 	// 0xD
#define 	SepA 	10	// 0xA
#define 	Sizesep 2

#include <wx/filefn.h>
/** @brief directory separator
 *  use _T("....")  +  wxString (Slash ) : not  _T("....")  +  Slash !!
 */
#define 	Slash 	wxFILE_SEP_PATH
///-----------------------------------------------------------------------------
#include <logmanager.h>
/** @brief debug messages  -> 'Code::Blocks log'
 */
#define lm			Manager::Get()->GetLogManager()
#define Print		lm->Log
#define PrintLn		lm->Log(_T(""))
#define PrintWarn	lm->LogWarning
#define PrintErr	lm->LogError
/** @brief messages  -> 'PreBuild log'
 */
#define p               m_LogPageIndex
#define print(a)	    lm->Log(a, p)
#define printLn		    lm->Log(_T(""), p)
#define printWarn(a)	lm->LogWarning(a, p)
#define printErr(a)	    lm->LogError(a, p)
///-----------------------------------------------------------------------------
/** @brief news extensions
 */
#define		EXT_UI			_T("ui")
#define		EXT_MOC			_T("moc")
#define		EXT_QRC			_T("qrc")
#define		DOT_EXT_UI		_T(".ui")
#define		DOT_EXT_MOC		_T(".moc")
#define		DOT_EXT_QRC		_T(".qrc")

#include <filefilters.h>
#define  	EXT_H 			FileFilters::H_EXT
#define  	EXT_HPP 		FileFilters::HPP_EXT
#define  	EXT_CPP 		FileFilters::CPP_EXT
#define 	DOT_EXT_H 		FileFilters::H_DOT_EXT
#define 	DOT_EXT_CPP 	FileFilters::CPP_DOT_EXT
///-----------------------------------------------------------------------------
#endif // PRINT_H_INCLUDED
