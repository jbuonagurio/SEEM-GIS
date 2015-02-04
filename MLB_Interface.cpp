///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                         SEEM                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     jbuonagurio@exponent.com               //
//                                                       //
//    contact:    John Buonagurio                        //
//                Exponent                               //
//                1800 Diagonal Road, Suite 500          //
//                Alexandria, VA 22314                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"

//---------------------------------------------------------
// 2. Place general module library informations here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Simulation - Ecology: SEEM") );

	case MLB_INFO_Author:
		return( SG_T("Copyright (c) 2012 Exponent, Inc.") );

	case MLB_INFO_Description:
		return( _TW(
			"Drawing from user-defined input parameters, SEEM is a tool by "
			"which to complete a wildlife exposure assessment guided by "
			"wildlife habitat preferences and foraging behaviors. SEEM "
			"incorporates habitat suitability indices and two foraging "
			"strategies to capture exposures that are more realistic than "
			"assuming uniform exposure across a heterogeneous site (i.e. "
			"site-wide averages). Building off of earlier work (Freshman and "
			"Menzie 1996; Hope 2000, 2001), SEEM offers an accessible and "
			"flexible tool by which to capture the foraging activities of all "
			"individuals in a local population. The modeling approach and "
			"level of complexity are user defined. As a result, SEEM can be "
			"employed to assess a variety of wildlife exposure problems. "
			"Ultimately, SEEM can assist risk managers and remedial engineers "
			"to better understand the costs and benefits of remediating "
			"certain parcels based on the HSI, potential exposures and acreages."
			"\n\n"
			"<b>References</b>\n"
			"<ul>"
			"<li>Freshman, J.S., Menzie, C.A. Two wildlife exposure models to assess impacts at the individual and population levels and the efficacy of remedial actions. Human and Ecological Risk Assessment 1996; 2(3):481-496.</li>"
			"<li>Hope, B.K. Generating probabilistic spatially-explicit individual and population exposure estimates for ecological risk assessments. Risk Analysis 2000; 20(5):573-590.</li>"
			"<li>Hope, B.K. A case study comparing static and spatially explicit ecological exposure analysis methods. Risk Analysis 2001: 21(6):1001-1010.</li>"
			"<li>Wickwire, W.T., Menzie, C.A., Burmistrov, D., Hope, B.K. Incorporating spatial data into ecological risk assessments: The Spatially Explicit Exposure Module (SEEM) for ARAMS. In: Landscape Ecology and Wildlife Habitat Evaluation: Critical Information for Ecological Risk Assessment, Land-Use Management Activities, and Biodiversity Enhancement Practices. ASTM STP 1458. Kapustka LA, Galbraith H, Luxon M, Biddinger GR (eds). ASTM International, West Conshohocken, PA, 2004.</li>"
			"<li>von Stackelberg, K., Wickwire W.T., Burmistrov D. Spatially-explicit exposure modeling tools for use in human health and ecological risk assessment: SEEM and FISHRAND-Migration. pp. 279–288. In: Environmental Exposure and Health, 2005. Aral MM, Brebbia CA, Maslia MLand Sinks T (eds), United Kingdom: WIT Press.</li>"
			"<li>Wickwire, W.T., Johnson, M.S., Hope, B.K., Greenberg, M.S. Spatially explicit exposure models: a rationale for and path toward their increased acceptance and use. Integrated Environmental Assessment and Management 2011: 7(2):158-168.</li>"
			"</ul>"
		));

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Simulation|Ecology|SEEM") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "seem.h"

//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Module *		Create_Module(int i)
{
	// Don't forget to continuously enumerate the case switches
	// when adding new modules! Also bear in mind that the
	// enumeration always has to start with [case 0:] and
	// that [default:] must return NULL!...

	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule = new Cseem;
		break;

	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA
