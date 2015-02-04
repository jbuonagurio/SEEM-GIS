///////////////////////////////////////////////////////////
//                       seem.h                          //
//                                                       //
//                 Copyright (C) 2012 by                 //
//                    Exponent, Inc.                     //
//-------------------------------------------------------//
//    e-mail:     jbuonagurio@exponent.com               //
//                                                       //
//    contact:    Exponent, Inc.                         //
//                Attn: John Buonagurio                  //
//                Ecological & Biological Sciences       //
//                1800 Diagonal Road, Suite 500          //
//                Alexandria, VA 22314                   //
//-------------------------------------------------------//

#ifndef HEADER_INCLUDED__seem_H
#define HEADER_INCLUDED__seem_H

#include "MLB_Interface.h"

///////////////////////////////////////////////////////////
//														 //
//		CLASS Cseem										 //
//														 //
///////////////////////////////////////////////////////////

class Cseem : public CSG_Module_Grid
{
public:

	// constructor
	Cseem(void);

	// destructor
	virtual ~Cseem(void);

protected:

	virtual bool	On_Execute(void);
	virtual int		On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

private:

	//-----------------------------------
	// module parameters
	//-----------------------------------

	int				m_Days;				// simulation time
	int				m_popSize;			// population size
	int				m_fType;			// foraging type
	double			m_fRadius;			// foraging radius
	int				m_fEvents;			// daily exposure events
	double			m_irSoil;			// soil ingestion rate
	double			m_irFood;			// food ingestion rate
	int				m_nDiets;			// number of diet items
	double			m_TRV;				// toxicity reference value

	//-----------------------------------
	// private member functions
	//-----------------------------------

	void			_Set_Diets_Count(CSG_Parameters *pDiets, int nDiets);
	double			_Rand(double min, double max);
};

#endif
