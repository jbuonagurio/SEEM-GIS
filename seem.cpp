///////////////////////////////////////////////////////////
//                      seem.cpp                         //
//                                                       //
//-------------------------------------------------------//
//    e-mail:     jbuonagurio@exponent.com               //
//                                                       //
//    contact:    John Buonagurio                        //
//                Exponent                               //
//                1800 Diagonal Road, Suite 500          //
//                Alexandria, VA 22314                   //
//-------------------------------------------------------//

#include "seem.h"
#include <algorithm>
#include <ctime>
#include <numeric>
#include <random>
#include <vector>

#define NULL 0

//---------------------------------------------------------------------
Cseem::Cseem()
{
	Set_Name(_TL("SEEM Version 1.0"));

	Set_Author(SG_T("Copyright (c) 2012 Exponent, Inc."));

	Set_Description(_TW(
		"Spatially Explicit Exposure Model"
	));

	CSG_Parameter	*pNode;
	CSG_String		s;

	//-----------------------------------------------------------------
	// DATA OBJECTS
	//-----------------------------------------------------------------

	Parameters.Add_Grid(
		NULL	, "HSGRID"		, _TL("Habitat Suitability"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid(
		NULL	, "CCGRID"		, _TL("Soil Chemical Concentrations"),
		_TL(""),
		PARAMETER_INPUT
	);
	
	Parameters.Add_Grid(
		NULL	, "EHGRID"		, _TL("Ecological Hazard Quotient"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	Parameters.Add_Shapes(
		NULL	, "POINTS"		, _TL("Foraging Positions"), 
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table(
		NULL	, "TABLE"		, _TL("Results"),
		_TL(""),
		PARAMETER_OUTPUT
	);
	
	//-----------------------------------------------------------------
	// SIMULATION
	//-----------------------------------------------------------------

	s.Printf(SG_T("NODE1"), 1);
	pNode = Parameters.Add_Node(NULL,s,_TL("Simulation"),_TL(""));

	Parameters.Add_Value(
		pNode	, "DAYS"		, _TL("Days"),
		_TL(""),
		PARAMETER_TYPE_Int, 365, 1, true
	);

	//-----------------------------------------------------------------
	// RECEPTOR
	//-----------------------------------------------------------------

	s.Printf(SG_T("NODE2"), 2);
	pNode = Parameters.Add_Node(NULL,s,_TL("Receptor"),_TL(""));

	Parameters.Add_Value(
		pNode	, "POPSIZE"		, _TL("Population Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
	
	Parameters.Add_Choice(
		pNode	, "FTYPE"		, _TL("Foraging Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Free Ranging"),	//  0
			_TL("Static Homerange")	//  1
		)
	);
	
	Parameters.Add_Value(
		pNode	, "FRADIUS"		, _TL("Foraging Radius"),
		_TL(""),
		PARAMETER_TYPE_Double, 10
	);

	//-----------------------------------------------------------------
	// INTAKE
	//-----------------------------------------------------------------

	s.Printf(SG_T("NODE3"), 3);
	pNode = Parameters.Add_Node(NULL,s,_TL("Intake"),_TL(""));

	Parameters.Add_Value(
		pNode	, "IRSOIL"		, _TL("Soil Ingestion Rate"),
		_TL("kg-soil/kg-body wt per day"),
		PARAMETER_TYPE_Double, 0
	);

	Parameters.Add_Value(
		pNode	, "IRFOOD"		, _TL("Food Ingestion Rate"),
		_TL("kg-food/kg-body wt per day"),
		PARAMETER_TYPE_Double, 0
	);
	
	Parameters.Add_Value(
		pNode	, "NDIETS"		, _TL("Number of Diet Items"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Parameters(
		pNode	, "DIETS"		, _TL("Diet Items"),
		_TL("")
	);
	
	//-----------------------------------------------------------------
	// EXPOSURE
	//-----------------------------------------------------------------

	s.Printf(SG_T("NODE4"), 4);
	pNode = Parameters.Add_Node(NULL,s,_TL("Exposure"),_TL(""));

	Parameters.Add_Value(
		pNode	, "FEVENTS"		, _TL("Daily Exposure Events"),
		_TL(""),
		PARAMETER_TYPE_Int, 15, 1, true
	);

	Parameters.Add_Value(
		pNode	, "TRV"			, _TL("Toxicity Reference Value"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.01
	);
	
	//-----------------------------------------------------------------
	CSG_Parameters *pDiets = Parameters("DIETS")->asParameters();
	_Set_Diets_Count(pDiets, Parameters("NDIETS")->asInt());
}

//---------------------------------------------------------------------
Cseem::~Cseem()
{}

///////////////////////////////////////////////////////////////////////
//
//		                          UPDATE
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
int Cseem::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("NDIETS")))
	{
		_Set_Diets_Count(pParameters->Get_Parameter("DIETS")->asParameters(), pParameter->asInt());
		return(true);
	}
	return(false);
}

//---------------------------------------------------------------------
void Cseem::_Set_Diets_Count(CSG_Parameters *pDiets, int nDiets)
{
	if( pDiets && nDiets > 0 )
	{
		int nCurrent = pDiets->Get_Count() / 4;

		if(nCurrent < nDiets)
		{
			for(int i=nCurrent; i<nDiets; i++)
			{
				CSG_Parameter *pNode = pDiets->Add_Node(
					NULL,
					CSG_String::Format(SG_T("NODE%03d"), i),
					CSG_String::Format(SG_T("%s %i"), _TL("Diet Item"), i + 1),
					_TL("")
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("TCONC%03d"), i),
					_TL("Tissue Concentration"),
					_TL("mg/kg-body wt"),
					PARAMETER_TYPE_Double, 0
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("SCONC%03d"), i),
					_TL("Soil Concentration"),
					_TL("mg/kg-soil dw"),
					PARAMETER_TYPE_Double, 0
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("PROP%03d"), i),
					_TL("Percent of Total Diet"),
					_TL(""),
					PARAMETER_TYPE_Double, 100, 0, true, 100, true
				);
			}
		}
		else if(nCurrent > nDiets)
		{
			CSG_Parameters Temp;
			Temp.Assign(pDiets);
			pDiets->Destroy();
			pDiets->Set_Name(Temp.Get_Name());

			for(int i=0; i<nDiets; i++)
			{
				CSG_Parameter *pNode = pDiets->Add_Node(
					NULL,
					CSG_String::Format(SG_T("NODE%03d"), i),
					CSG_String::Format(SG_T("%s %i"), _TL("Diet Item"), i + 1),
					_TL("")
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("TCONC%03d"), i),
					_TL("Tissue Concentration"),
					_TL("mg/kg-body wt"),
					PARAMETER_TYPE_Double,
					Temp(CSG_String::Format(SG_T("TCONC%03d"), i)) ? Temp(CSG_String::Format(SG_T("TCONC%03d"), i))->asDouble() : 0
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("SCONC%03d"), i),
					_TL("Soil Concentration"),
					_TL("mg/kg-soil dw"),
					PARAMETER_TYPE_Double,
					Temp(CSG_String::Format(SG_T("SCONC%03d"), i)) ? Temp(CSG_String::Format(SG_T("SCONC%03d"), i))->asDouble() : 0
				);

				pDiets->Add_Value(
					pNode,
					CSG_String::Format(SG_T("PROP%03d"), i),
					_TL("Percent of Total Diet"),
					_TL(""),
					PARAMETER_TYPE_Double,
					Temp(CSG_String::Format(SG_T("PROP%03d"), i)) ? Temp(CSG_String::Format(SG_T("PROP%03d"), i))->asDouble() : 0,
					100, true, 100, true
				);
			}
		}
	}
}

///////////////////////////////////////////////////////////////////////
//
//		                          SIMULATION
//
///////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------
// ON_EXECUTE
//---------------------------------------------------------------------
bool Cseem::On_Execute()
{
	Message_Add(CSG_String::Format(SG_T("%s: %i"), _TL("NY"), Get_NY()));
	Message_Add(CSG_String::Format(SG_T("%s: %i"), _TL("NX"), Get_NX()));
	
	//-----------------------------------------------------------
	m_Days			= Parameters("DAYS")			->asInt();
	m_popSize		= Parameters("POPSIZE")			->asInt();
	m_fType			= Parameters("FTYPE")			->asInt();
	m_fRadius		= Parameters("FRADIUS")			->asDouble();
	m_fEvents		= Parameters("FEVENTS")			->asInt();
	m_irSoil		= Parameters("IRSOIL")			->asDouble();
	m_irFood		= Parameters("IRFOOD")			->asDouble();
	m_nDiets		= Parameters("NDIETS")			->asInt();
	m_TRV			= Parameters("TRV")				->asDouble();
	//-----------------------------------------------------------

	CSG_Parameters *pDiets = Parameters("DIETS")->asParameters();

	CSG_Grid *hsGrid = Parameters("HSGRID")->asGrid();
	CSG_Grid *ccGrid = Parameters("CCGRID")->asGrid();
	CSG_Grid *ehGrid = Parameters("EHGRID")->asGrid();

	CSG_Shape *pShape;
	CSG_Shapes *pShapes = Parameters("POINTS")->asShapes();

	CSG_Table_Record *pRecord;
	CSG_Table *pTable = Parameters("TABLE")->asTable();

	// Validate TRV
	if(m_TRV <= 0)
	{
		Error_Set(_TL("TRV must be greater than zero."));
		return(false);
	}

	// Calculate Food Exposure Dose
	double foodExposure=0, sumProportion=0;
	for(int i=0; i<m_nDiets; i++)
	{
		double tissueConc = pDiets->Get_Parameter(CSG_String::Format(SG_T("TCONC%03d"), i))->asDouble();
		double soilConc = pDiets->Get_Parameter(CSG_String::Format(SG_T("SCONC%03d"), i))->asDouble();
		double dietProportion = pDiets->Get_Parameter(CSG_String::Format(SG_T("PROP%03d"), i))->asDouble();
		sumProportion += dietProportion;
		if(tissueConc > 0 && soilConc > 0)
			foodExposure += (tissueConc/soilConc) * m_irFood * (dietProportion/100);
	}

	// Validate Diet Proportions
	if((int)(sumProportion+0.5) != 100)
	{
		Error_Set(_TL("Diet proportions must sum to 100%."));
		return(false);
	}

	// Create EHQ Grid
	Message_Add(CSG_String::Format(SG_T("%s"), _TL("Building EHQ Grid")));
	for(int j=0; j<Get_NY() && Set_Progress(j); j++)
	{
		for(int i=0; i<Get_NX(); i++)
		{
			ehGrid->Set_Value(i, j, (ccGrid->asDouble(i, j) * (m_irSoil + foodExposure)) / m_TRV);
		}
	}

	// Get HSI Extents
	double xmin, xmax, ymin, ymax;
	xmin = hsGrid->Get_XMin();
	xmax = hsGrid->Get_XMax();
	ymin = hsGrid->Get_YMin();
	ymax = hsGrid->Get_YMax();

	// Initialize Points Output
	pShapes->Create(SHAPE_TYPE_Point, _TL("Foraging Positions"));
	pShapes->Add_Field("ID", SG_DATATYPE_Int);
	pShapes->Add_Field("Time", SG_DATATYPE_Int);
	pShapes->Add_Field("X", SG_DATATYPE_Double);
	pShapes->Add_Field("Y", SG_DATATYPE_Double);
	pShapes->Add_Field("Dose", SG_DATATYPE_Double);

	// Initialize Table Output
	pTable->Destroy();
	pTable->Set_Name(_TL("Results"));
	pTable->Add_Field("ID", SG_DATATYPE_Int);
	pTable->Add_Field("Min", SG_DATATYPE_Double);
	pTable->Add_Field("Max", SG_DATATYPE_Double);
	pTable->Add_Field("Mean", SG_DATATYPE_Double);
	pTable->Add_Field("StdDev", SG_DATATYPE_Double);
	pTable->Add_Field("P05", SG_DATATYPE_Double);
	pTable->Add_Field("P50", SG_DATATYPE_Double);
	pTable->Add_Field("P95", SG_DATATYPE_Double);

	double x, y, x1, y1, r1, r2;
	double hsi=0, maxhsi=0;
	std::vector<double> v; // vector of daily average exposures

	Message_Add(CSG_String::Format(SG_T("%s"), _TL("Starting Simulation")));
	for(int n=0; n<m_popSize && Set_Progress(n, m_popSize); n++)
	{
		while(true)
		{
			x = _Rand(hsGrid->Get_XMin(), hsGrid->Get_XMax());
			y = _Rand(hsGrid->Get_YMin(), hsGrid->Get_YMax());
			hsGrid->Get_Value(x, y, hsi, GRID_INTERPOLATION_NearestNeighbour, true);
			if (_Rand(0,1) <= hsi)
				break;
		}

		for(int i=0; i<m_Days; i++)
		{
			// *** Movement Simulation ***

			if(m_fType == 0)
			{
				while(true)
				{
					r1 = _Rand(0,1);
					r2 = _Rand(0,M_PI_360);
					x1 = x + m_fRadius*sqrt(r1)*cos(r2);
					y1 = y + m_fRadius*sqrt(r1)*sin(r2);

					if (x1 > xmin && x1 < xmax && y1 > ymin && y1 < ymax)
					{
						hsGrid->Get_Value(x1, y1, hsi, GRID_INTERPOLATION_NearestNeighbour, true);
						if (hsi > maxhsi)
							maxhsi = hsi;
						if (hsi > 0 && maxhsi > 0)
							if (_Rand(0,1) <= hsi/maxhsi)
								break;
					}
				}
				x = x1; // Foraging Event X Position
				y = y1; // Foraging Event Y Position
			}
		
			// *** Exposure Calculation ***

			double cc=0, sumExposure=0, avgExposure=0;
			for(int j=0; j<m_fEvents; j++)
			{
				while(true)
				{
					r1 = _Rand(0,1);
					r2 = _Rand(0,M_PI_360);
					x1 = x + m_fRadius*sqrt(r1)*cos(r2);
					y1 = y + m_fRadius*sqrt(r1)*sin(r2);

					if (x1 > xmin && x1 < xmax && y1 > ymin && y1 < ymax)
						break;
				}

				ccGrid->Get_Value(x1, y1, cc, GRID_INTERPOLATION_NearestNeighbour, true);
				sumExposure += cc * (m_irSoil + foodExposure);
			}

			avgExposure = sumExposure / m_fEvents;

			pShape = pShapes->Add_Shape();
			pShape->Add_Point(x, y);
			pShape->Set_Value(0, n);
			pShape->Set_Value(1, i);
			pShape->Set_Value(2, x);
			pShape->Set_Value(3, y);
			pShape->Set_Value(4, avgExposure);

			v.push_back(avgExposure);

			//DataObject_Update(pShapes, SG_UI_DATAOBJECT_SHOW);
		}

		// *** Statistics ***
		
		std::sort(v.begin(), v.end());
		double minDose = *std::min_element(v.begin(), v.end());
		double maxDose = *std::max_element(v.begin(), v.end());
		double avgDose = std::accumulate(v.begin(), v.end(), 0) / double(v.size());
		double ssqDose = std::inner_product(v.begin(), v.end(), v.begin(), 0.0);
		double stdDose = std::sqrt(ssqDose / v.size() - avgDose * avgDose);
		double p05Dose = *(v.begin()+int(v.size()*0.05));
		double p50Dose = *(v.begin()+int(v.size()*0.5));
		double p95Dose = *(v.begin()+int(v.size()*0.95));
		v.clear();

		pRecord	= pTable->Add_Record();
		pRecord->Set_Value(0, n);
		pRecord->Set_Value(1, minDose);
		pRecord->Set_Value(2, maxDose);
		pRecord->Set_Value(3, avgDose);
		pRecord->Set_Value(4, stdDose);
		pRecord->Set_Value(5, p05Dose);
		pRecord->Set_Value(6, p50Dose);
		pRecord->Set_Value(7, p95Dose);
	}
	
	Message_Add(CSG_String::Format(SG_T("%s"), _TL("Simulation Complete")));
	return(true);
}

//---------------------------------------------------------------------
// PRNG
//---------------------------------------------------------------------
double Cseem::_Rand(double min, double max)
{
	static std::mt19937 mt(time(NULL));
	std::uniform_real_distribution<double> uniform(min,max);
	return uniform(mt);
}
