//******** PRACTICA VISUALITZACIÓ 3D (Facultat de Ciències / Escola d'Enginyeria - UAB)
//******** Entorn bàsic VS2026 MULTIFINESTRA amb OpenGL, interfície MFC i Status Bar
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2026)
// EntornV3DView.cpp: implementación de la clase CEntornV3DView
// FUNCIONS:		- Control del bucle principal (OnPaint)
//					- Control teclat (OnKeyDown)
//					- Control mouse interactiu i botons mouse 
//							(OnLButtomDown, OnRButtomDown, OnMouseMove)
//					- Control opcions de menú (On*, OnUpdate*)
//					- Control de color de fons per teclat (FONS)
//					- Transformacions Geomètriques Interactives via mouse
//
//    Versió 2.0:	- Canvi de color de l'objecte per teclat (OBJ)
//					- Fixar Transformacions Geomètriques desactivant opcions Transforma (INSERT dins opcions Transforma)
//					- Esborrar Transformacions Geomètriques fixades (DELETE dins opcions Transforma)
//					- Fixar Traslació pantalla fora de l'opció Vista->Pan? (INSERT dins opció Vista->Pan?)
//					- Esborrar Traslació pantalla fixada (DELETE dins opció Vista->Pan?)
//
//	  Versió 2.2:	- Opció VISTA --> Satèl.lit: Punt de Vista en moviment segons moviment mouse
//
//	  Versió 2.5:	- Afegits objectes cubRGB i Tie (nau Star Wars fet per alumnes)
//					- Opció VISTA --> FullScreen?: Opció FullScreen per menú i per PopUp menu enmig pantalla. Té un bug al restaurar pantalla 
//													normal (inclos menú IDR_MENU_WINDOW)
//

#include "pch.h"
#include "framework.h"
// Se pueden definir SHARED_HANDLERS en un proyecto ATL implementando controladores de vista previa, miniatura
// y filtro de búsqueda, y permiten compartir código de documentos con ese proyecto.
#ifndef SHARED_HANDLERS
#include "EntornV3D.h"
#endif

#include "EntornV3DDoc.h"
#include "EntornV3DView.h"
#include "visualitzacio.h"	// Include funcions de projeció i il.luminació
#include "escena.h"			// Include funcions d'objectes OpenGL

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CEntornV3DView

IMPLEMENT_DYNCREATE(CEntornV3DView, CView)

BEGIN_MESSAGE_MAP(CEntornV3DView, CView)
	// Comandos de impresión estándar
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CEntornV3DView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KEYDOWN()
	ON_WM_KEYUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_PAINT()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_ARXIU_OBRIR_FRACTAL, &CEntornV3DView::OnArxiuObrirFractal)
	ON_COMMAND(ID_ARXIU_OBRIR_FITXER_OBJ, &CEntornV3DView::OnArxiuObrirFitxerOBJ)
	ON_COMMAND(ID_ARXIU_OBRIR_FITXER_3DS, &CEntornV3DView::OnArxiuObrirFitxer3DS)
	ON_COMMAND(ID_CAMERA_ESFERICA, &CEntornV3DView::OnCameraEsferica)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_ESFERICA, &CEntornV3DView::OnUpdateCameraEsferica)
	ON_COMMAND(ID_CAMERA_MOBIL, &CEntornV3DView::OnCameraMobil)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_MOBIL, &CEntornV3DView::OnUpdateCameraMobil)
	ON_COMMAND(ID_CAMERA_ZOOM, &CEntornV3DView::OnCameraZoom)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_ZOOM, &CEntornV3DView::OnUpdateCameraZoom)
	ON_COMMAND(ID_CAMERA_SATELIT, &CEntornV3DView::OnCameraSatelit)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_SATELIT, &CEntornV3DView::OnUpdateCameraSatelit)
	ON_COMMAND(ID_CAMERA_POLARS_EIX_X, &CEntornV3DView::OnCameraPolarsEixX)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_POLARS_EIX_X, &CEntornV3DView::OnUpdateCameraPolarsEixX)
	ON_COMMAND(ID_CAMERA_POLARS_EIX_Y, &CEntornV3DView::OnCameraPolarsEixY)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_POLARS_EIX_Y, &CEntornV3DView::OnUpdateCameraPolarsEixY)
	ON_COMMAND(ID_CAMERA_POLARS_EIX_Z, &CEntornV3DView::OnCameraPolarsEixZ)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_POLARS_EIX_Z, &CEntornV3DView::OnUpdateCameraPolarsEixZ)
	ON_COMMAND(ID_CAMERA_GEODE, &CEntornV3DView::OnCameraGeode)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_GEODE, &CEntornV3DView::OnUpdateCameraGeode)
	ON_COMMAND(ID_CAMERA_ORIGEN_GEODE, &CEntornV3DView::OnCameraOrigenGeode)
	ON_COMMAND(ID_CAMERA_NAVEGA, &CEntornV3DView::OnCameraNavega)
	ON_UPDATE_COMMAND_UI(ID_CAMERA_NAVEGA, &CEntornV3DView::OnUpdateCameraNavega)
	ON_COMMAND(ID_CAMERA_ORIGEN_NAVEGA, &CEntornV3DView::OnCameraOrigenNavega)
	ON_COMMAND(ID_VISTA_FULLSCREEN, &CEntornV3DView::OnVistaFullScreen)
	ON_UPDATE_COMMAND_UI(ID_VISTA_FULLSCREEN, &CEntornV3DView::OnUpdateVistaFullScreen)
	ON_COMMAND(ID_VISTA_PAN, &CEntornV3DView::OnVistaPan)
	ON_UPDATE_COMMAND_UI(ID_VISTA_PAN, &CEntornV3DView::OnUpdateVistaPan)
	ON_COMMAND(ID_VISTA_ORIGENPAN, &CEntornV3DView::OnVistaOrigenPan)
	ON_COMMAND(ID_VISTA_EIXOS, &CEntornV3DView::OnVistaEixos)
	ON_UPDATE_COMMAND_UI(ID_VISTA_EIXOS, &CEntornV3DView::OnUpdateVistaEixos)
	ON_COMMAND(ID_VISTA_GRIDXY, &CEntornV3DView::OnVistaGridXY)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXY, &CEntornV3DView::OnUpdateVistaGridXY)
	ON_COMMAND(ID_VISTA_GRIDXZ, &CEntornV3DView::OnVistaGridXZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXZ, &CEntornV3DView::OnUpdateVistaGridXZ)
	ON_COMMAND(ID_VISTA_GRIDYZ, &CEntornV3DView::OnVistaGridYZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDYZ, &CEntornV3DView::OnUpdateVistaGridYZ)
	ON_COMMAND(ID_VISTA_GRIDXYZ, &CEntornV3DView::OnVistaGridXYZ)
	ON_UPDATE_COMMAND_UI(ID_VISTA_GRIDXYZ, &CEntornV3DView::OnUpdateVistaGridXYZ)
	ON_COMMAND(ID_PROJECCIO_PERSPECTIVA, &CEntornV3DView::OnProjeccioPerspectiva)
	ON_UPDATE_COMMAND_UI(ID_PROJECCIO_PERSPECTIVA, &CEntornV3DView::OnUpdateProjeccioPerspectiva)
	ON_COMMAND(ID_OBJECTE_CAP, &CEntornV3DView::OnObjecteCap)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CAP, &CEntornV3DView::OnUpdateObjecteCap)
	ON_COMMAND(ID_OBJECTE_CUB, &CEntornV3DView::OnObjecteCub)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB, &CEntornV3DView::OnUpdateObjecteCub)
	ON_COMMAND(ID_OBJECTE_CUB_RGB, &CEntornV3DView::OnObjecteCubRGB)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CUB_RGB, &CEntornV3DView::OnUpdateObjecteCubRGB)
	ON_COMMAND(ID_OBJECTE_ESFERA, &CEntornV3DView::OnObjecteEsfera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_ESFERA, &CEntornV3DView::OnUpdateObjecteEsfera)
	ON_COMMAND(ID_OBJECTE_TETERA, &CEntornV3DView::OnObjecteTetera)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TETERA, &CEntornV3DView::OnUpdateObjecteTetera)
	ON_COMMAND(ID_OBJECTE_TIE, &CEntornV3DView::OnObjecteTie)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TIE, &CEntornV3DView::OnUpdateObjecteTie)
	ON_COMMAND(ID_OBJECTE_TRUCK, &CEntornV3DView::OnObjecteTruck)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_TRUCK, &CEntornV3DView::OnUpdateObjecteTruck)
	ON_COMMAND(ID_OBJECTE_CORBA_BEZIER, &CEntornV3DView::OnObjecteCorbaBezier)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_BEZIER, &CEntornV3DView::OnUpdateObjecteCorbaBezier)
	ON_COMMAND(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornV3DView::OnObjecteCorbaLemniscata)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_LEMNISCATA, &CEntornV3DView::OnUpdateObjecteCorbaLemniscata)
	ON_COMMAND(ID_OBJECTE_CORBA_BSPLINE, &CEntornV3DView::OnObjecteCorbaBSpline)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_BSPLINE, &CEntornV3DView::OnUpdateObjecteCorbaBSpline)
	ON_COMMAND(ID_OBJECTE_CORBA_CATMULL_ROM, &CEntornV3DView::OnObjecteCorbaCatmullRom)
	ON_UPDATE_COMMAND_UI(ID_OBJECTE_CORBA_CATMULL_ROM, &CEntornV3DView::OnUpdateObjecteCorbaCatmullRom)
	ON_COMMAND(ID_CORBES_PUNTS_CONTROL, &CEntornV3DView::OnCorbesPuntsControl)
	ON_UPDATE_COMMAND_UI(ID_CORBES_PUNTS_CONTROL, &CEntornV3DView::OnUpdateCorbesPuntsControl)
	ON_COMMAND(ID_CORBES_TRIEDRE_FRENET, &CEntornV3DView::OnCorbesTriedreFrenet)
	ON_UPDATE_COMMAND_UI(ID_CORBES_TRIEDRE_FRENET, &CEntornV3DView::OnUpdateCorbesTriedreFrenet)
	ON_COMMAND(ID_TRANSFORMA_TRASLACIO, &CEntornV3DView::OnTransformaTraslacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_TRASLACIO, &CEntornV3DView::OnUpdateTransformaTraslacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGEN_TRASLACIO, &CEntornV3DView::OnTransformaOrigenTraslacio)
	ON_COMMAND(ID_TRANSFORMA_ROTACIO, &CEntornV3DView::OnTransformaRotacio)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ROTACIO, &CEntornV3DView::OnUpdateTransformaRotacio)
	ON_COMMAND(ID_TRANSFORMA_ORIGEN_ROTACIO, &CEntornV3DView::OnTransformaOrigenRotacio)
	ON_COMMAND(ID_TRANSFORMA_ESCALAT, &CEntornV3DView::OnTransformaEscalat)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_ESCALAT, &CEntornV3DView::OnUpdateTransformaEscalat)
	ON_COMMAND(ID_TRANSFORMA_ORIGEN_ESCALAT, &CEntornV3DView::OnTransformaOrigenEscalat)
	ON_COMMAND(ID_TRANSFORMA_MOBIL_EIXX, &CEntornV3DView::OnTransformaMobilEixX)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBIL_EIXX, &CEntornV3DView::OnUpdateTransformaMobilEixX)
	ON_COMMAND(ID_TRANSFORMA_MOBIL_EIXY, &CEntornV3DView::OnTransformaMobilEixY)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBIL_EIXY, &CEntornV3DView::OnUpdateTransformaMobilEixY)
	ON_COMMAND(ID_TRANSFORMA_MOBIL_EIXZ, &CEntornV3DView::OnTransformaMobilEixZ)
	ON_UPDATE_COMMAND_UI(ID_TRANSFORMA_MOBIL_EIXZ, &CEntornV3DView::OnUpdateTransformaMobilEixZ)
	ON_COMMAND(ID_OCULTACIONS_FRONT_FACES, &CEntornV3DView::OnOcultacionsFrontFaces)
		ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_FRONT_FACES, &CEntornV3DView::OnUpdateOcultacionsFrontFaces)
		ON_COMMAND(ID_OCULTACIONS_TEST_VIS, &CEntornV3DView::OnOcultacionsTestVis)
		ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_TEST_VIS, &CEntornV3DView::OnUpdateOcultacionsTestVis)
		ON_COMMAND(ID_OCULTACIONS_ZBUFFER, &CEntornV3DView::OnOcultacionsZBuffer)
		ON_UPDATE_COMMAND_UI(ID_OCULTACIONS_ZBUFFER, &CEntornV3DView::OnUpdateOcultacionsZBuffer)
		ON_COMMAND(ID_ILUMINACIO_LLUM_FIXE, &CEntornV3DView::OnIluminacioLlumFixe)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_LLUM_FIXE, &CEntornV3DView::OnUpdateIluminacioLlumFixe)
		ON_COMMAND(ID_ILUMINACIO_BOTH_SIDES, &CEntornV3DView::OnIluminacioBothSides)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_BOTH_SIDES, &CEntornV3DView::OnUpdateIluminacioBothSides)
		ON_COMMAND(ID_ILUMINACIO_PUNTS, &CEntornV3DView::OnIluminacioPunts)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PUNTS, &CEntornV3DView::OnUpdateIluminacioPunts)
		ON_COMMAND(ID_ILUMINACIO_FILFERROS, &CEntornV3DView::OnIluminacioFilferros)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_FILFERROS, &CEntornV3DView::OnUpdateIluminacioFilferros)
		ON_COMMAND(ID_ILUMINACIO_PLANA, &CEntornV3DView::OnIluminacioPlana)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_PLANA, &CEntornV3DView::OnUpdateIluminacioPlana)
		ON_COMMAND(ID_ILUMINACIO_SUAU, &CEntornV3DView::OnIluminacioSuau)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_SUAU, &CEntornV3DView::OnUpdateIluminacioSuau)
		ON_COMMAND(ID_MATERIAL_EMISSIO, &CEntornV3DView::OnMaterialEmissio)
		ON_UPDATE_COMMAND_UI(ID_MATERIAL_EMISSIO, &CEntornV3DView::OnUpdateMaterialEmissio)
		ON_COMMAND(ID_MATERIAL_AMBIENT, &CEntornV3DView::OnMaterialAmbient)
		ON_UPDATE_COMMAND_UI(ID_MATERIAL_AMBIENT, &CEntornV3DView::OnUpdateMaterialAmbient)
		ON_COMMAND(ID_MATERIAL_DIFUSA, &CEntornV3DView::OnMaterialDifusa)
		ON_UPDATE_COMMAND_UI(ID_MATERIAL_DIFUSA, &CEntornV3DView::OnUpdateMaterialDifusa)
		ON_COMMAND(ID_MATERIAL_ESPECULAR, &CEntornV3DView::OnMaterialEspecular)
		ON_UPDATE_COMMAND_UI(ID_MATERIAL_ESPECULAR, &CEntornV3DView::OnUpdateMaterialEspecular)
		ON_COMMAND(ID_ILUMINACIO_TEXTURES, &CEntornV3DView::OnIluminacioTextures)
		ON_UPDATE_COMMAND_UI(ID_ILUMINACIO_TEXTURES, &CEntornV3DView::OnUpdateIluminacioTextures)
		ON_COMMAND(ID_TEXTURA_FITXER_IMATGE, &CEntornV3DView::OnTexturaFitxerImatge)
		ON_UPDATE_COMMAND_UI(ID_TEXTURA_FITXER_IMATGE, &CEntornV3DView::OnUpdateTexturaFitxerImatge)
		ON_COMMAND(ID_LLUMS_LLUM_AMBIENT, &CEntornV3DView::OnLlumsLlumAmbient)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM_AMBIENT, &CEntornV3DView::OnUpdateLlumsLlumAmbient)
		ON_COMMAND(ID_LLUMS_LLUM0, &CEntornV3DView::OnLlumsLlum0)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM0, &CEntornV3DView::OnUpdateLlumsLlum0)
		ON_COMMAND(ID_LLUMS_LLUM1, &CEntornV3DView::OnLlumsLlum1)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM1, &CEntornV3DView::OnUpdateLlumsLlum1)
		ON_COMMAND(ID_LLUMS_LLUM2, &CEntornV3DView::OnLlumsLlum2)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM2, &CEntornV3DView::OnUpdateLlumsLlum2)
		ON_COMMAND(ID_LLUMS_LLUM3, &CEntornV3DView::OnLlumsLlum3)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM3, &CEntornV3DView::OnUpdateLlumsLlum3)
		ON_COMMAND(ID_LLUMS_LLUM4, &CEntornV3DView::OnLlumsLlum4)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM4, &CEntornV3DView::OnUpdateLlumsLlum4)
		ON_COMMAND(ID_LLUMS_LLUM5, &CEntornV3DView::OnLlumsLlum5)
		ON_UPDATE_COMMAND_UI(ID_LLUMS_LLUM5, &CEntornV3DView::OnUpdateLlumsLlum5)
		ON_COMMAND(ID_SHADERS_SENSE, &CEntornV3DView::OnShadersSense)
		ON_UPDATE_COMMAND_UI(ID_SHADERS_SENSE, &CEntornV3DView::OnUpdateShadersSense)
		ON_COMMAND(ID_SHADER_GOURAUD, &CEntornV3DView::OnShaderGouraud)
		ON_UPDATE_COMMAND_UI(ID_SHADER_GOURAUD, &CEntornV3DView::OnUpdateShaderGouraud)
		ON_COMMAND(ID_SHADER_PHONG, &CEntornV3DView::OnShaderPhong)
		ON_UPDATE_COMMAND_UI(ID_SHADER_PHONG, &CEntornV3DView::OnUpdateShaderPhong)
		ON_COMMAND(ID_SHADER_LOAD_FILES, &CEntornV3DView::OnShaderLoadFiles)
		ON_UPDATE_COMMAND_UI(ID_SHADER_LOAD_FILES, &CEntornV3DView::OnUpdateShaderLoadFiles)
		ON_COMMAND(ID_OBJECTE_ESCENA_LOXODROMA, &CEntornV3DView::OnObjecteEscenaLoxodroma)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_ESCENA_LOXODROMA, &CEntornV3DView::OnUpdateObjecteEscenaLoxodroma)
		ON_COMMAND(ID_OBJECTE_PAISATGE, &CEntornV3DView::OnObjectePaisatge)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_PAISATGE, &CEntornV3DView::OnUpdateObjectePaisatge)
		ON_COMMAND(ID_PROJECCIO_ORTOGRAFICA, &CEntornV3DView::OnProjeccioOrtografica)
		ON_UPDATE_COMMAND_UI(ID_PROJECCIO_ORTOGRAFICA, &CEntornV3DView::OnUpdateProjeccioOrtografica)
		ON_COMMAND(ID_PROJECCIO_AXONOMETRICA, &CEntornV3DView::OnProjeccioAxonometrica)
		ON_UPDATE_COMMAND_UI(ID_PROJECCIO_AXONOMETRICA, &CEntornV3DView::OnUpdateProjeccioAxonometrica)
		ON_COMMAND(ID_OBJECTE_RESPALL, &CEntornV3DView::OnObjecteRespall)
		ON_UPDATE_COMMAND_UI(ID_OBJECTE_RESPALL, &CEntornV3DView::OnUpdateObjecteRespall)
		END_MESSAGE_MAP()

// Construcción o destrucción de CEntornV3DView

CEntornV3DView::CEntornV3DView() noexcept
{
// TODO: agregar aquí el código de construcción
//------ Entorn V3D: Inicialització de les variables globals de CEntornV3DView
	int i;

	//saveParent = NULL;	//ContextMenu = NULL;
	//m_hrc = NULL;		m_hRC = NULL;		m_pDC = NULL;

// Entorn V3D: Variables de control per Menú Càmera: Esfèrica, Navega, Mòbil, Zoom, Satelit, Polars... 
	camera = CAM_ESFERICA;
	mobil = true;	zzoom = true;	zzoomO = false;	 satelit = false;
	//navega = false;

// Entorn V3D: Variables de control de l'opció CAMERA->ESFÈRICA?
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;	// Origen PV en esfèriques
	Vis_Polar = POLARZ;

// Entorn V3D: Variables de control de l'opció CÀMERA->NAVEGA?
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;

// Entorn V3D: Variables de control de l'opció CÀMERA->GEODE?
	OPV_G.R = 15.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;	// Origen PV en esfèriques per a Camera_Geode

// Entorn V3D: Variables de control de l'opció CÀMERA->LOXOSROMA_T*?
	cam_t = 0.0;
	nCam.x = 0.0;		nCam.y = 0.0;		nCam.z = 0.0;
	vCam.x = 10.0;		vCam.y = 0.0;		vCam.z = 1.0;
	animaCamLxd = false;					camL_left = false;

// Entorn V3D: Variables de control per Menú VISTA: Pantalla Completa, Pan, dibuixar eixos i grids
	fullscreen = false;
	pan = false;	eixos = true;
	sw_grid = false;
	grid.x = false;	grid.y = false; grid.z = false;		grid.w = false;
	hgrid.x = 0.0;	hgrid.y = 0.0;	hgrid.z = 0.0;		hgrid.w = 0.0;

// Entorn V3D: Variables opció VISTA->PAN?
	fact_pan = 1;
	tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;		tr_cpvF.x = 0;	tr_cpvF.y = 0;	tr_cpvF.z = 0;

// Entorn V3D: Variables de control per les opcions de menú PROJECCIÓ, OBJECTE
	projeccio = CAP;			objecte = CAP;

// Entorn V3D: Variables de control del menú TRANSFORMA
	transf = false;		trasl = false;		rota = false;		escal = false;
	fact_Tras = 1;		fact_Rota = 90;
	TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;	TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0;
	TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;	TGF.VRota.x = 0;	TGF.VRota.y = 0;	TGF.VRota.z = 0;
	TG.VScal.x = 1;		TG.VScal.y = 1;		TG.VScal.z = 1;	TGF.VScal.x = 1;	TGF.VScal.y = 1;	TGF.VScal.z = 1;
	transX = false;		transY = false;		transZ = false;

// Entorn V3D: Variables de control per les opcions de menú OCULTACIONS
	front_faces = true;	test_vis = false;	oculta = false;		back_line = false;

// Entorn V3D: Variables de control del menú ILUMINACIÓ
	ifixe = false;					ilum2sides = false;				ilumina = FILFERROS;
// Reflexions actives: Ambient [1], Difusa [2] i Especular [3]. No actives: Emission [0]. 
	sw_material[0] = false;			sw_material[1] = true;			sw_material[2] = true;			sw_material[3] = true;
	sw_material_old[0] = false;		sw_material_old[1] = true;		sw_material_old[2] = true;		sw_material_old[3] = true;
	textura = false;				t_textura = CAP;
	for (i = 0; i < NUM_MAX_TEXTURES; i++) texturesID[i] = -1;

// ----------------- Entorn V3D: Variables de control del menú LLUMS
// Entorn V3D: Inicialització variables Llums
	llum_ambient = true;
	for (i = 0; i < NUM_MAX_LLUMS; i++) llumGL[i].encesa = false;
	for (i = 0; i < NUM_MAX_LLUMS; i++) {
		llumGL[i].posicio[0] = 0.0;		llumGL[i].posicio[1] = 0.0;		llumGL[i].posicio[2] = 0.0;		llumGL[i].posicio[3] = 1.0;
		llumGL[i].difusa[0] = 1.0f;	llumGL[i].difusa[1] = 1.0f;	llumGL[i].difusa[2] = 1.0f;	llumGL[i].difusa[3] = 1.0f;
		llumGL[i].especular[0] = 1.0f; llumGL[i].especular[1] = 1.0f; llumGL[i].especular[2] = 1.0f; llumGL[i].especular[3] = 1.0f;
		llumGL[i].restringida = false;
		llumGL[i].spotdirection[0] = 0.0;	llumGL[i].spotdirection[1] = 0.0;	llumGL[i].spotdirection[2] = 0.0;
		llumGL[i].spotcoscutoff = 0.0;		llumGL[i].spotexponent = 0.0;
	}
	sw_llambient = false;
	col_llambient.r = 1.0;	col_llambient.g = 1.0;	col_llambient.b = 1.0;	col_llambient.a = 1.0;

// ---------------- DEFINICIÓ LLUMS
// ---------------- LLUM #0 - (+Z) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(0,0,200):
	llumGL[0].posicio[0] = 0.0;			llumGL[0].posicio[1] = 0.0;			llumGL[0].posicio[2] = 200.0;	llumGL[0].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[0].difusa[0] = 1.0f;			llumGL[0].difusa[1] = 1.0f;			llumGL[0].difusa[2] = 1.0f;		llumGL[0].difusa[3] = 1.0f;
	llumGL[0].especular[0] = 1.0f;		llumGL[0].especular[1] = 1.0f;		llumGL[0].especular[2] = 1.0f;	llumGL[0].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[0].atenuacio.a = 0.0;		llumGL[0].atenuacio.b = 0.0;		llumGL[0].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[0].restringida = false;
	llumGL[0].spotdirection[0] = 0.0;	llumGL[0].spotdirection[1] = 0.0;	llumGL[0].spotdirection[2] = -1.0;
	llumGL[0].spotcoscutoff = cos(25.0 * PI / 180);		llumGL[0].spotexponent = 1.0;		// llumGL[0].spotexponent = 45.0; Model de Warn (10, 500)

// Activació font de llum: ENCESA
	llumGL[0].encesa = true;

// ---------------- LLUM #1 - (+X) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(75,0,0):
	llumGL[1].posicio[0] = 75.0;		llumGL[1].posicio[1] = 0.0;			llumGL[1].posicio[2] = 0.0;		llumGL[1].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[1].difusa[0] = 1.0f;			llumGL[1].difusa[1] = 1.0f;			llumGL[1].difusa[2] = 1.0f;		llumGL[1].difusa[3] = 1.0f;
	llumGL[1].especular[0] = 1.0f;		llumGL[1].especular[1] = 1.0f;		llumGL[1].especular[2] = 1.0f;	llumGL[1].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[1].atenuacio.a = 0.0;		llumGL[1].atenuacio.b = 0.0;		llumGL[1].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[1].restringida = false;
	llumGL[1].spotdirection[0] = 0.0;	llumGL[1].spotdirection[1] = 0.0;	llumGL[1].spotdirection[2] = 0.0;
	llumGL[1].spotcoscutoff = 0.0;		llumGL[1].spotexponent = 0.0;

// Activació font de llum: APAGADA
	llumGL[1].encesa = false;

// ---------------- LLUM #2 - (+Y) no restringida, amb atenuació constant (a,b,c) = (0,0,1)
// Posició de la font de llum (x,y,z)=(0,75,0):
	llumGL[2].posicio[0] = 0.0;			llumGL[2].posicio[1] = 75.0;		llumGL[2].posicio[2] = 0.0;		llumGL[2].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[2].difusa[0] = 1.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[2].difusa[2] = 1.0f;		llumGL[2].difusa[3] = 1.0f;
	llumGL[2].especular[0] = 1.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[2].especular[2] = 1.0f;	llumGL[2].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum amb atenuació per distància (a,b,c)=(0,0.025,1):
	llumGL[2].atenuacio.a = 0.0;		llumGL[2].atenuacio.b = 0.0;		llumGL[2].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[2].restringida = false;
	llumGL[2].spotdirection[0] = 0.0;	llumGL[2].spotdirection[1] = -1.0;	llumGL[2].spotdirection[2] = 0.0;
	llumGL[2].spotcoscutoff = cos(2.5 * PI / 180);							llumGL[2].spotexponent = 1.0;

	// Activació font de llum: APAGADA
	llumGL[2].encesa = false;

// ---------------- LLUM #3 - (Z=Y=X), restringida amb 25 graus obertura i exponent 45, amb atenuació constant (a,b,c) = (0,0,1)
	// Posició de la font de llum (x,y,z)=(75,75,75):
	llumGL[3].posicio[0] = 75.0;		llumGL[3].posicio[1] = 75.0;		llumGL[3].posicio[2] = 75.0;		llumGL[3].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (0,1,0):
	llumGL[3].difusa[0] = 0.0f;			llumGL[2].difusa[1] = 1.0f;			llumGL[3].difusa[2] = 0.0f;		llumGL[3].difusa[3] = 1.0f;
	llumGL[3].especular[0] = 0.0f;		llumGL[2].especular[1] = 1.0f;		llumGL[3].especular[2] = 0.0f;	llumGL[3].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[3].atenuacio.a = 0.0;		llumGL[3].atenuacio.b = 0.0;		llumGL[3].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[3].restringida = true;
	llumGL[3].spotdirection[0] = -1.0;	llumGL[3].spotdirection[1] = -1.0;	llumGL[3].spotdirection[2] = -1.0;
	llumGL[3].spotcoscutoff = cos(25.0 * PI / 180);							llumGL[3].spotexponent = 45.0;

// Activació font de llum: APAGADA
	llumGL[3].encesa = false;

// ---------------- LLUM #4 - (-Z), no restringida, amb atenuació constant (a,b,c) = (0,0,1)
	// Posició de la font de llum (x,y,z)=(0,0,-75):
	llumGL[4].posicio[0] = 0.0;			llumGL[4].posicio[1] = 0.0;			llumGL[4].posicio[2] = -75.0;	llumGL[4].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,1,1):
	llumGL[4].difusa[0] = 1.0f;			llumGL[4].difusa[1] = 1.0f;			llumGL[4].difusa[2] = 1.0f;		llumGL[4].difusa[3] = 1.0f;
	llumGL[4].especular[0] = 1.0f;		llumGL[4].especular[1] = 1.0f;		llumGL[4].especular[2] = 1.0f;	llumGL[4].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[4].atenuacio.a = 0.0;		llumGL[4].atenuacio.b = 0.0;		llumGL[4].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[4].restringida = false;
	llumGL[4].spotdirection[0] = 0.0;	llumGL[4].spotdirection[1] = 0.0;	llumGL[4].spotdirection[2] = -1.0;
	llumGL[4].spotcoscutoff = cos(5 * PI / 180);							llumGL[4].spotexponent = 30.0;

// Activació font de llum: APAGADA
	llumGL[4].encesa = false;

// ---------------- LLUM #5 - (-X,-Y,-Z), direccional, no restringida, amb atenuació constant (a,b,c) = (0,0,1)
	// Vector de la font de llum direccional (x,y,z)=(-1,-1,-1):
	llumGL[5].posicio[0] = -1.0;		llumGL[5].posicio[1] = -1.0;		llumGL[5].posicio[2] = -1.0;		llumGL[5].posicio[3] = 0.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,0,1):
	llumGL[5].difusa[0] = 1.0f;			llumGL[5].difusa[1] = 0.0f;			llumGL[5].difusa[2] = 1.0f;		llumGL[5].difusa[3] = 1.0f;
	llumGL[5].especular[0] = 1.0f;		llumGL[5].especular[1] = 0.0f;		llumGL[5].especular[2] = 1.0f;	llumGL[5].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[5].atenuacio.a = 0.0;		llumGL[5].atenuacio.b = 0.0;		llumGL[5].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[5].restringida = false;
	llumGL[5].spotdirection[0] = 0.0;	llumGL[5].spotdirection[1] = 0.0;	llumGL[5].spotdirection[2] = 0.0;
	llumGL[5].spotcoscutoff = 0.0;		llumGL[5].spotexponent = 0.0;

// Activació font de llum: APAGADA
	llumGL[5].encesa = false;

// ---------------- LLUM #6 - Llum Vaixell, configurada a la funció vaixell() en escena.cpp.
// Posició de la font de llum (x,y,z)=(-75,75,75):
	llumGL[6].posicio[0] = -75.0;		llumGL[6].posicio[1] = 75.0;		llumGL[6].posicio[2] = 75.0;	llumGL[6].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (1,0,0):
	llumGL[6].difusa[0] = 1.0f;			llumGL[6].difusa[1] = 0.0f;			llumGL[6].difusa[2] = 0.0f;		llumGL[6].difusa[3] = 1.0f;
	llumGL[6].especular[0] = 1.0f;		llumGL[6].especular[1] = 0.0f;		llumGL[6].especular[2] = 0.0f;	llumGL[6].especular[3] = 1;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[6].atenuacio.a = 0.0;		llumGL[6].atenuacio.b = 0.0;		llumGL[6].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[6].restringida = true;
	llumGL[6].spotdirection[0] = 1.0;	llumGL[6].spotdirection[1] = -1.0;	llumGL[6].spotdirection[2] = -1.0;
	llumGL[6].spotcoscutoff = sqrt(2.0) / 2.0;		llumGL[6].spotexponent = 10.0;

// Activació font de llum: APAGADA
	llumGL[6].encesa = false;

// ---------------- LLUM #7 - Llum Far, configurada a la funció faro() en escena.cpp.
// Posició de la font de llum (x,y,z)=(50,50,-50).
	llumGL[7].posicio[0] = 50.0;		llumGL[7].posicio[1] = 50.0;		llumGL[7].posicio[2] = -50.0;	llumGL[7].posicio[3] = 1.0;

// Intensitats difusa i especular de la font de llum (r,g,b) = (0,1,1):
	llumGL[7].difusa[0] = 0.0f;			llumGL[7].difusa[1] = 1.0f;			llumGL[7].difusa[2] = 1.0f;		llumGL[7].difusa[3] = 1.0f;
	llumGL[7].especular[0] = 0.0f;		llumGL[7].especular[1] = 1.0f;		llumGL[7].especular[2] = 1.0f;	llumGL[7].especular[3] = 1.0f;

// Coeficients factor atenuació f_att=1/(ad2+bd+c). Llum sense atenuació per distància (a,b,c)=(0,0,1):
	llumGL[7].atenuacio.a = 0.0;		llumGL[7].atenuacio.b = 0.0;		llumGL[7].atenuacio.c = 1.0;

// Paràmetres font de llum restringida:
	llumGL[7].restringida = true;
	llumGL[7].spotdirection[0] = -1.0;	llumGL[7].spotdirection[1] = -1.0;	llumGL[7].spotdirection[2] = 1.0;
	llumGL[7].spotcoscutoff = 0.5;		llumGL[7].spotexponent = 5.0;

// Activació font de llum: APAGADA
	llumGL[7].encesa = false;
// ---------------- FI DEFINICIÓ LLUMS
	sw_llambient = false;
	col_llambient.r = 1.0;	col_llambient.g = 1.0;	col_llambient.b = 1.0; col_llambient.a = 1.0;

// Entorn V3D: Variables de control del menú SHADERS
	sw_shader = false;				shader_menu = CAP;				shader_program = 0;

// Entorn V3D: Variables de control dels botons de mouse
	m_PosEAvall = (0, 0);		m_PosDAvall = (0, 0);
	m_ButoEAvall = false;		m_ButoDAvall = false;
	m_EsfeEAvall.R = 0.0;		m_EsfeEAvall.alfa = 0.0;	m_EsfeEAvall.beta = 0.0;
	m_EsfeIncEAvall.R = 0.0;	m_EsfeIncEAvall.alfa = 0.0;	m_EsfeIncEAvall.beta = 0.0;

// Entorn V3D: Variables que controlen paràmetres visualització: Mides finestra Windows i PV (càmera)
	w = 0;				h = 0;								// Mides finestra
	w_old = 0;			h_old = 0;							// Copia mides finestre per a FullScreen

// Entorn V3D: Color de fons i de l'objecte
	fonsR = true;		fonsG = true;		fonsB = true;
	c_fons.r = 0.0;		c_fons.g = 0.0;		c_fons.b = 0.0;
	sw_color = false;
	col_obj.r = 1.0;	col_obj.g = 1.0;	col_obj.b = 1.0;		col_obj.a = 1.0;

// Entorn V3D: Objecte 3DS
	Ob3DS = NULL;

// Entorn V3D: Objecte OBJ
	ObOBJ = NULL;

// V3D: OBJECTE --> Corbes Bezier, BSpline, Catmull-Rom i Loxodroma
	npts_T = 0;
	for (i = 0; i < MAX_PATCH_CORBA; i = i++)
		{	PC_t[i].x = 0.0;
			PC_t[i].y = 0.0;
			PC_t[i].z = 0.0;
		}
	pas_Corba = PAS_BSPLINE;	pas_CS = PAS_BSPLINE;
	sw_Punts_Control = false;

// TRIEDRE DE FRENET / DARBOUX: VT: vector Tangent, VNP: Vector Normal Principal, VBN: vector BiNormal
	dibuixa_TriedreFrenet = false;		dibuixa_TriedreDarboux = false;
	VT = { 0.0, 0.0, 1.0 };		VNP = { 1.0, 0.0, 0.0 };	VBN = { 0.0, 1.0, 0.0 };

// Entorn V3D: Variables del Timer
	t = 0;			anima = false;

// Entorn V3D: FPS
	beginFrame = 0;	endFrame = 0;	deltaTime = 0;
	frames = 0;
	frameRate = 30;	averageFrameTimeMilliseconds = 33.333;
	fps = 0;

// Entorn V3D. QUATERNIONS:  Animació de cossos rígids per QUATERNIONS. Inicialització de variables
	rotaQ = false;		animaQ = false;		np_InQ = 0;
	for (i = 0; i < MAX_KEYFRAMES_Q; i++)
	{	keyf_Rota[i].x = 0;	keyf_Rota[i].y = 0;	keyf_Rota[i].z = 0;
		keyf_Tras[i].x = 0;	keyf_Tras[i].y = 0;	keyf_Tras[i].z = 0;
		keyf_Quat[i].w = 0.0f;	keyf_Quat[i].y = 0.0f;	keyf_Quat[i].y = 0.0f; keyf_Quat[i].z = 0.0f;
	}
	int_Tras.x = 0.0f;	int_Tras.y = 0;			int_Tras.z = 0.0f;
	qI.w = 1;			qI.x = 0;				qI.y = 0;				qI.z = 0;
	qF.w = 1;			qF.x = 0;				qF.y = 0;				qF.z = 0;
	qT.w = 1;			qT.x = 0;				qT.y = 0;				qT.z = 0;
	for (i = 0; i < 16; i++) QMatrix[i] = 0;	QMatrix[0] = QMatrix[5] = QMatrix[10] = QMatrix[15] = 1.0;
	//	keyf_Tras[0].x = 10.0f;		keyf_Tras[0].y = -20.0f;	keyf_Tras[0].z = 30.0f;
	//	keyf_Tras[1].x = 0.0f;		keyf_Tras[1].y = 0.0f;		keyf_Tras[1].z = 0.0f;
	//	keyf_Tras[2].x = -10.0f;	keyf_Tras[2].y = 20.0f;		keyf_Tras[2].z = 30.0f;

/* 
// Entorn V3D.ESCENA: Inicialització variables animació ESCENA (CATMULL-ROM, BSPLINE o LOXODROMA)
	t = 0;					delta = 0.0f;		animaE = false;
	for (i = 0; i < 3; i++)
		{	ctr[i].x = 0.0;		ctr[i].y = 0.0f;	ctr[i].z = 0.0f;	ctr[i].w = 0.0f;
		}
	patch = 0;
	VT.x = 0.0f;	VT.y = 0.0f;	VT.z = 0.0f;	VT.w = 0.0f;	// Vector Tangent del Triedre de Frenet
	VBN.x = 0.0f;	VBN.y = 0.0f;	VBN.z = 0.0f;	VBN.w = 0.0f;	// Vector Binormal del Triedre de Frenet
	VNP.x = 0.0f;	VNP.y = 0.0f;	VNP.z = 0.0f;	VNP.w = 0.0f;	// Vector Normal del Triedre de Frenet
	pos_traject.x = 0.0f;	pos_traject.y = 0.0f;	pos_traject.z = 0.0f;	pos_traject.w = 1.0f;	// Posició (x,y,z) de la trajectòria
	for (i = 0; i < 16; i++) CBMatrix[i] = 0;	CBMatrix[0] = CBMatrix[5] = CBMatrix[10] = CBMatrix[15] = 1.0; // Matriu Identitat
*/

// Entorn V3D: Altres variables
	mida = 1.0;			nom = "";		buffer = "";

// Entorn V3D: Inicialització de les llibreries DevIL per a la càrrega de textures i fitxers .3DS
	ilInit();					// Inicialitzar llibreria IL
	iluInit();					// Inicialitzar llibreria ILU
	ilutRenderer(ILUT_OPENGL);	// Inicialitzar llibreria ILUT per a OpenGL

// Entorn V3D: Definir desplegable per a Full Screen
	ContextMenu = new CMenu();
	if (!ContextMenu->LoadMenu(IDR_MENU_WINDOW))	AfxMessageBox(_T("Fail to create context menu"));
}

CEntornV3DView::~CEntornV3DView()
{
}

BOOL CEntornV3DView::PreCreateWindow(CREATESTRUCT& cs)
{
// TODO: modificar aquí la clase Window o los estilos cambiando
//  CREATESTRUCT cs

// An OpenGL window must be created with the following flags and must not
// include CS_PARENTDC for the class style.
	cs.style |= WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

	return CView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// Impresión de CEntornV3DView

void CEntornV3DView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CEntornV3DView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// Preparación predeterminada
	return DoPreparePrinting(pInfo);
}

void CEntornV3DView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar inicialización adicional antes de imprimir
}

void CEntornV3DView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: agregar limpieza después de imprimir
}



void CEntornV3DView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}

/////////////////////////////////////////////////////////////////////////////
// Diagnósticos de CEntornV3DView

#ifdef _DEBUG
void CEntornV3DView::AssertValid() const
{
	CView::AssertValid();
}

void CEntornV3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CEntornV3DDoc* CEntornV3DView::GetDocument() const // La versión de no depuración está alineada
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CEntornV3DDoc)));
	return (CEntornV3DDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// Controladores de mensajes de CEntornV3DView

int CEntornV3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

// TODO:  Agregue aquí su código de creación especializado
	//CDC* pDC = GetDC();
	//m_glRenderer.CreateGLContext(pDC);
	m_pDC = new CClientDC(this);

	PIXELFORMATDESCRIPTOR pfd;
	memset(&pfd, 0, sizeof(PIXELFORMATDESCRIPTOR));
	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DOUBLEBUFFER | PFD_SUPPORT_OPENGL | PFD_DRAW_TO_WINDOW;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cDepthBits = 32;
	pfd.iLayerType = PFD_MAIN_PLANE;

	// Entorn V3D: Format del pixel que millor s'ajusti al descrit en pfd
		//int nPixelFormat = ChoosePixelFormat(pDC->m_hDC, &pfd);
	int nPixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd);

	if (!nPixelFormat)
	{	::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// Activació format pixel per al contexte dispositiu
		//BOOL bResult = SetPixelFormat(pDC->m_hDC, nPixelFormat, &pfd);
	BOOL bResult = SetPixelFormat(m_pDC->GetSafeHdc(), nPixelFormat, &pfd);

	if (!bResult)
	{	::MessageBoxW(NULL, L"Error en SetPixelFormat", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// Entorn V3D: Creació contexte generació OpenGL
		// m_hRC=wglCreateContext(m_hDC);
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (!m_hRC)
	{
		::MessageBoxW(NULL, L"Error en GL Rendering Context", L"Error", MB_OK | MB_ICONERROR);
		PostQuitMessage(0);		// This sends a message telling the program to quit
		return false;
	}

	// OPENGL 2.0 - OPENGL 3.0 **********************************************************
		// --- OpenGL 3.x ---
	HGLRC tempContext = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), tempContext);

	//Get access to modern OpenGL functionality from this old style context.
	glewExperimental = GL_TRUE;
	if (GLEW_OK != glewInit())
	{
		AfxMessageBox(_T("GLEW could not be initialized!"));
		return FALSE;
	}

	//InitAPI();

	//	if (glewIsExtensionSupported("GLEW_ARB_vertex_shader"))
	//	AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader supported, proceeding\n"));
	//	else AfxMessageBox(_T("INFO: GLEW_ARB_vertex_shader NOT supported, proceeding\n"));
	//
	//	if (glewIsExtensionSupported("GLEW_ARB_fragment_shader"))
	//	AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader supported, proceeding\n"));
	//	else AfxMessageBox(_T("INFO: GLEW_ARB_fragment_shader NOT supported, proceeding\n"));
	//
	if (glewIsSupported("GL_VERSION_2_0")) //(GLEW_VERSION_2_0)
	{
	} //AfxMessageBox(_T("INFO: OpenGL 2.0 supported!. Proceed\n"));
	else
	{
		AfxMessageBox(_T("INFO: OpenGL 2.0 not supported!. Exit\n"));
		//return EXIT_FAILURE;
	}

	int major, minor;
	GetGLVersion(&major, &minor);

	if (major < 3 || (major == 3 && minor < 2))
		AfxMessageBox(_T("OpenGL 3.2 is not supported!. Exit\n"));
	//	else 
	//		AfxMessageBox(_T("OpenGL 3.2 is supported!. Proceed"));

	int attribs[] =
	{	WGL_CONTEXT_MAJOR_VERSION_ARB, major,
		WGL_CONTEXT_MINOR_VERSION_ARB, minor,
		WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,// | WGL_CONTEXT_DEBUG_BIT_ARB,
		WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB, //0x9126 , 0x00000001,
		0
	};

	PFNWGLCREATECONTEXTATTRIBSARBPROC wglCreateContextAttribsARB = NULL;
	wglCreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)wglGetProcAddress("wglCreateContextAttribsARB");
	if (wglCreateContextAttribsARB != NULL)
	{
		m_hrc = wglCreateContextAttribsARB(m_pDC->GetSafeHdc(), 0, attribs);
	}

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hrc);

// Entorn V3D: Inicialització de funcions de shaders per a OpenGL 2.0
	InitAPI();

	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(tempContext);

	if (!m_hrc)
	{	AfxMessageBox(_T("OpenGL 3.x RC was not created!"));
		return false;
	}

// Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

	// Mostrar maximizado
	ShowWindow(SW_MAXIMIZE);

	//return 0;
	return true;
}


void CEntornV3DView::InitAPI()
{
/*	Vendor, Renderer, Version, Shading Laguage Version i Extensions suportades per la placa gràfica gravades en fitxer extensions.txt
		CString nomf;
		nomf = "extensions.txt";
		char *nomfitxer = CString2Char(nomf);
		char* str = (char*)glGetString(GL_VENDOR);
		FILE* f = fopen(nomfitxer, "w");
		if(f)	{	fprintf(f,"VENDOR: %s\n",str);
					str = (char*)glGetString(GL_RENDERER);
					fprintf(f, "RENDERER: %s\n", str);
					str = (char*)glGetString(GL_VERSION);
					fprintf(f, "VERSION: %s\n", str);
					str = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
					fprintf(f, "SHADING_LANGUAGE_VERSION: %s\n", str);
					str = (char*)glGetString(GL_EXTENSIONS);
					fprintf(f, "EXTENSIONS: %s\n", str);
					fclose(f);
				}
*/

// Program
	glCreateProgram = (PFNGLCREATEPROGRAMPROC)wglGetProcAddress("glCreateProgram");
	glDeleteProgram = (PFNGLDELETEPROGRAMPROC)wglGetProcAddress("glDeleteProgram");
	glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
	glAttachShader = (PFNGLATTACHSHADERPROC)wglGetProcAddress("glAttachShader");
	glDetachShader = (PFNGLDETACHSHADERPROC)wglGetProcAddress("glDetachShader");
	glLinkProgram = (PFNGLLINKPROGRAMPROC)wglGetProcAddress("glLinkProgram");
	glGetProgramiv = (PFNGLGETPROGRAMIVPROC)wglGetProcAddress("glGetProgramiv");
	glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)wglGetProcAddress("glGetShaderInfoLog");
	glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)wglGetProcAddress("glGetUniformLocation");
	glUniform1i = (PFNGLUNIFORM1IPROC)wglGetProcAddress("glUniform1i");
	glUniform1iv = (PFNGLUNIFORM1IVPROC)wglGetProcAddress("glUniform1iv");
	glUniform2iv = (PFNGLUNIFORM2IVPROC)wglGetProcAddress("glUniform2iv");
	glUniform3iv = (PFNGLUNIFORM3IVPROC)wglGetProcAddress("glUniform3iv");
	glUniform4iv = (PFNGLUNIFORM4IVPROC)wglGetProcAddress("glUniform4iv");
	glUniform1f = (PFNGLUNIFORM1FPROC)wglGetProcAddress("glUniform1f");
	glUniform1fv = (PFNGLUNIFORM1FVPROC)wglGetProcAddress("glUniform1fv");
	glUniform2fv = (PFNGLUNIFORM2FVPROC)wglGetProcAddress("glUniform2fv");
	glUniform3fv = (PFNGLUNIFORM3FVPROC)wglGetProcAddress("glUniform3fv");
	glUniform4fv = (PFNGLUNIFORM4FVPROC)wglGetProcAddress("glUniform4fv");
	glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)wglGetProcAddress("glUniformMatrix4fv");
	glGetAttribLocation = (PFNGLGETATTRIBLOCATIONPROC)wglGetProcAddress("glGetAttribLocation");
	glVertexAttrib1f = (PFNGLVERTEXATTRIB1FPROC)wglGetProcAddress("glVertexAttrib1f");
	glVertexAttrib1fv = (PFNGLVERTEXATTRIB1FVPROC)wglGetProcAddress("glVertexAttrib1fv");
	glVertexAttrib2fv = (PFNGLVERTEXATTRIB2FVPROC)wglGetProcAddress("glVertexAttrib2fv");
	glVertexAttrib3fv = (PFNGLVERTEXATTRIB3FVPROC)wglGetProcAddress("glVertexAttrib3fv");
	glVertexAttrib4fv = (PFNGLVERTEXATTRIB4FVPROC)wglGetProcAddress("glVertexAttrib4fv");
	glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glEnableVertexAttribArray");
	glDisableVertexAttribArray = (PFNGLDISABLEVERTEXATTRIBARRAYPROC)wglGetProcAddress("glDisableVertexAttribArray");
	glBindAttribLocation = (PFNGLBINDATTRIBLOCATIONPROC)wglGetProcAddress("glBindAttribLocation");
	glGetActiveUniform = (PFNGLGETACTIVEUNIFORMPROC)wglGetProcAddress("glGetActiveUniform");

// Shader
	glCreateShader = (PFNGLCREATESHADERPROC)wglGetProcAddress("glCreateShader");
	glDeleteShader = (PFNGLDELETESHADERPROC)wglGetProcAddress("glDeleteShader");
	glShaderSource = (PFNGLSHADERSOURCEPROC)wglGetProcAddress("glShaderSource");
	glCompileShader = (PFNGLCOMPILESHADERPROC)wglGetProcAddress("glCompileShader");
	glGetShaderiv = (PFNGLGETSHADERIVPROC)wglGetProcAddress("glGetShaderiv");

// VBO
	glGenBuffers = (PFNGLGENBUFFERSPROC)wglGetProcAddress("glGenBuffers");
	glBindBuffer = (PFNGLBINDBUFFERPROC)wglGetProcAddress("glBindBuffer");
	glBufferData = (PFNGLBUFFERDATAPROC)wglGetProcAddress("glBufferData");
	glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)wglGetProcAddress("glVertexAttribPointer");
}

void CEntornV3DView::GetGLVersion(int* major, int* minor)
{
	// for all versions
	char* ver = (char*)glGetString(GL_VERSION); // ver = "3.2.0"

	*major = ver[0] - '0';
	if (*major >= 3)
	{
		// for GL 3.x
		glGetIntegerv(GL_MAJOR_VERSION, major);		// major = 3
		glGetIntegerv(GL_MINOR_VERSION, minor);		// minor = 2
	}
	else
	{
		*minor = ver[2] - '0';
	}

// GLSL
	ver = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);	// 1.50 NVIDIA via Cg compiler
}


void CEntornV3DView::OnDestroy()
{
	CView::OnDestroy();

// TODO: Agregue aquí su código de controlador de mensajes
	CDC* pDC = GetDC();
	ReleaseDC(pDC);

}

void CEntornV3DView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

// TODO: Agregue aquí su código de controlador de mensajes
// A resize event occured; cx and cy are the window's new width and height.
// Find the OpenGL change size function given in the Lab 1 notes and call it here

// Entorn V3D: MODIFICACIÓ ->Establim les mides de la finestra actual
	w = cx;
	h = cy;
}

/////////////////////////////////////////////////////////////////////////////
// Dibujo de CEntornV3DView

void CEntornV3DView::OnDraw(CDC* /*pDC*/)
{
	CEntornV3DDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

// TODO: agregar aquí el código de dibujo para datos nativos
}


void CEntornV3DView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
// TODO: Agregue aquí su código de controlador de mensajes
// No llama a CView::OnPaint() para mensajes de pintura
	GLdouble vpv[3] = { 0.0, 0.0, 1.0 };

// EntornVGI: FPS
	beginFrame = clock();	// Iniciar Rellotge per a FPS.

// Entorn V3D: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);


// Color de l'objecte condicionat al color de fons en Ortogràfica
	if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
	{	col_obj.r = 1.0;  col_obj.g = 1.0;  col_obj.b = 1.0; col_obj.a = 1.0;
	}
	else {	col_obj.r = 0.0;  col_obj.g = 0.0;  col_obj.b = 0.0; col_obj.a = 1.0;
	}


// Cridem a les funcions de l'escena i la projecció segons s'hagi 
// seleccionat una projecció o un altra
	switch (projeccio)
	{
	case AXONOM:
		// PROJECCIÓ AXONOMÈTRICA
		// Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Aquí farem les crides per a definir Viewport, Projecció i Càmara: INICI -------------------------
		Projeccio_Orto(0, 0, w, h, OPV.R);
		Vista_Esferica(OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida,
			front_faces, oculta, test_vis, back_line,
			ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		// Aquí farem les crides per a definir Viewport, Projecció i Càmara:: FI -------------------------
				// Dibuixar Model (escena)
		glPushMatrix();
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		// glScalef();			// Escalat d'objectes, per adequar-los a les vistes ortogràfiques
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
		glPopMatrix();

		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	case ORTO:
		// PROJECCIÓ ORTOGRÀFICA
		// Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Fons condicionat al color de fons
		if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))
			FonsB();
		else
			FonsN();

		// Aquí farem les quatre crides a ProjeccioOrto i Ortografica per obtenir 
		// les quatre vistes ortogràfiques
		// ---------- Entorn V3D: DESCOMENTAR QUAN S'IMPLEMENTI PROJECCIO ORTOGRÀFICA

		// PLANTA (Inferior Esquerra) 
				// Definició de Viewport, Projecció i Càmara
		Projeccio_Orto(0, 0, w/2 -2, h/2 -2, OPV.R);
		Vista_Ortografica(0, OPV.R, c_fons, col_obj, objecte, mida, front_faces, oculta,
			test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		// Dibuix de l'Objecte o l'Escena
		glPushMatrix();
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes
		// glScalef();			// Escalat d'objectes, per adequar-los a les vistes ortogràfiques (Pràctica 2)
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
		glPopMatrix();
		
		// ISOMÈTRICA (Inferior Dreta)
				// Definició de Viewport, Projecció i Càmara
				Projeccio_Orto(w/2 +2, 0, w/2 -2, h/2 -2, OPV.R);
				Vista_Ortografica(3, OPV.R, c_fons, col_obj, objecte, mida, front_faces, oculta,
					test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides,eixos, grid, hgrid);
				// Dibuix de l'Objecte o l'Escena
				glPushMatrix();
					configura_Escena();     // Aplicar Transformacions Geomètriques segons persiana Transformacio i configurar objectes
					// glScalef();			// Escalat d'objectes, per adequar-los a les vistes ortogràfiques (Pràctica 2)
					dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
				glPopMatrix();

		// ALÇAT (Superior Esquerra)
				// Definició de Viewport, Projecció i Càmara
				Projeccio_Orto(0, h/2 +2, w/2 -2, h/2, OPV.R);
				Vista_Ortografica(1, OPV.R, c_fons, col_obj, objecte, mida, front_faces, oculta,
					test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
				// Dibuix de l'Objecte o l'Escena
				glPushMatrix();
					configura_Escena();     // Aplicar Transformacions Geomètriques segons persiana Transformacio i configurar objectes
					//glScalef();			// Escalat d'objectes, per adequar-los a les vistes ortogràfiques (Pràctica 2)
					dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
				glPopMatrix();

		// PERFIL (Superior Dreta)
				// Definició de Viewport, Projecció i Càmara
				Projeccio_Orto(w/2 +2 , h/2 +2, w/2 -2, h/2, OPV.R);
				Vista_Ortografica(2, OPV.R, c_fons, col_obj, objecte, mida, front_faces, oculta,
					test_vis, back_line, ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
				// Dibuix de l'Objecte o l'Escena
				glPushMatrix();
					configura_Escena();     // Aplicar Transformacions Geom?triques segons persiana Transformacio i configurar objectes
					// glScalef();			// Escalat d'objectes, per adequar-los a les vistes ortogràfiques (Pràctica 2)
					dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
				glPopMatrix();
		
		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	case PERSPECT:
		// PROJECCIÓ PERSPECTIVA
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
		glDisable(GL_SCISSOR_TEST);		// Desactivació del retall de pantalla

		// Definició de Viewport, Projecció i Càmara
		Projeccio_Perspectiva(0, 0, w, h, OPV.R);

		if (camera == CAM_ESFERICA) {
			n[0] = 0;		n[1] = 0;		n[2] = 0;
			Vista_Esferica(OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		}
		else if (camera == CAM_NAVEGA) {
			Vista_Navega(opvN, n, vpv, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		}
		else if (camera == CAM_GEODE) {
			Vista_Geode(OPV_G, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		}
		else if (camera == CAM_LOXODROMA_TDARBOUX || camera == CAM_LOXODROMA_TFRENET) {
			// Càmera LOXODROMA: Definició de la càmera segons el Triedre definit
		}
		else {
			n[0] = 0;		n[1] = 0;		n[2] = 0;
			Vista_Esferica(OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida,
				front_faces, oculta, test_vis, back_line,
				ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);
		}

		// Dibuix de l'Objecte o l'Escena
		glPushMatrix();
		configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
		glPopMatrix();

		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	// Entorn V3D.ESCENA: PASSEJADA VIRTUAL Per a l'objecte ESCENA
	case PASSEIG_V:
		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST); // Set Perspective Calculations To Most Accurate
		// Activació del retall de pantalla
		glEnable(GL_SCISSOR_TEST);

		// Retall
		glScissor(0, 0, w, h);
		glViewport(0, 0, w, h);

		// Fons condicionat al color de fons
		if ((c_fons.r < 0.5) || (c_fons.g < 0.5) || (c_fons.b < 0.5))	FonsB();
		else FonsN();

		// Definició de Viewport, Projecció i Càmara i geometria VIEWPORT ESQUERRA
// Entorn V3D. CAL FER: Calcular els 4 paràmetres de Projecció_Perspectiva() per a definir viewport esquerra 
//						en funció del tamany de la finestra (w,h).
//		Projeccio_Perspectiva(<ELS 4 PARÀMETRES DEL VIEWPORT ESQUERRA>, OPV.R);
//		Vista_Esferica(OPV, Vis_Polar, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, mida,
//			front_faces, oculta, test_vis, back_line,
//			ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);

		// Dibuix de l'Objecte o l'Escena
//		glPushMatrix();
//		  configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
//		  dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
//		glPopMatrix();

		// Definició de Viewport, Projecció i Càmara i geometria VIEWPORT DRET
// Entorn V3D. CAL FER: Calcular els 4 paràmetres de Projecció_Perspectiva() per a definir viewport dret
//						en funció del tamany de la finestra (w,h).
//		Projeccio_Perspectiva(<ELS 4 PARÀMETRES DEL VIEWPORT DRET>, OPV.R);
// Entorn V3D. CAL FER: Càlcul dels tres primers paràmetres de la funció Vista_Navega:
//						<ORIGEN CAMERA>, <PUNT ON MIRA CAMERA>, <VECTOR V CAMERA>.
//		Vista_Navega(<ORIGEN CAMERA>, <PUNT ON MIRA>, <VECTOR V>, pan, tr_cpv, tr_cpvF, c_fons, col_obj, objecte, true,
//			front_faces, oculta, test_vis, back_line,
//			ilumina, llum_ambient, llumGL, ifixe, ilum2sides, eixos, grid, hgrid);

// Dibuix de l'Objecte o l'Escena
//		glPushMatrix();
//		  configura_Escena();     // Aplicar Transformacions Geometriques segons persiana Transformacio i configurar objectes.
		// Dibuix de l'escena amb el l'objecte semitransparent (darrer paràmetre a true).
//		  dibuixa_Escena();		// Dibuix geometria de l'escena amb comandes GL.
//		glPopMatrix();

// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;

	default:
		// Entorn V3D: Creació de la llista que dibuixarà els eixos Coordenades Món. Funció on està codi per dibuixar eixos	
		glNewList(EIXOS, GL_COMPILE);
		// Dibuix dels eixos sense il.luminació
		glDisable(GL_LIGHTING);
		// Dibuixar eixos sense textures
		glDisable(GL_TEXTURE_2D);
		// Desactivar boira
		glDisable(GL_FOG);
		deixos();						// Funció que dibuixa els Eixos Coordenades Món
		if (ilumina != FILFERROS) glEnable(GL_LIGHTING);
		if (textura) glEnable(GL_TEXTURE_2D);
		glEndList();

		// Crida a la funció Fons Blanc
		FonsB();

		// Intercanvia l'escena al front de la pantalla
		SwapBuffers(m_pDC->GetSafeHdc());
		break;
	}

// Entorn V3D: Activació el contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// EntornV3D: FPS
	endFrame = clock();
	deltaTime += endFrame - beginFrame;
	frames++;

	if ((deltaTime / (double)CLOCKS_PER_SEC) * 1000.0 > 1000.0)
	{ //every second
		frameRate = (double)frames * 0.5 + frameRate * 0.5; //more stable
		frames = 0;
		deltaTime -= CLOCKS_PER_SEC;
		averageFrameTimeMilliseconds = 1000.0 / (frameRate == 0 ? 0.001 : frameRate);
		fps = (frameRate == 0 ? 0.001 : frameRate);
	}

//  Actualitzar la barra d'estat de l'aplicació amb els valors R,A,B,PVx,PVy,PVz
	Barra_Estat();
}

// configura_Escena: Funcio que configura els parametres de Model i dibuixa les
//                   primitives OpenGL dins classe Model
void CEntornV3DView::configura_Escena() {

	// Aplicar Transformacions Geometriques segons persiana Transformacio i Quaternions
	instancia(transf, TG, TGF, rotaQ, animaQ, QMatrix, int_Tras);
}

// dibuixa_Escena: Funcio que crida al dibuix dels diferents elements de l'escana
void CEntornV3DView::dibuixa_Escena() {

	//	Dibuix geometria de l'escena amb comandes GL.
	dibuixa_EscenaGL(objecte, col_obj, true, sw_material, textura, texturesID,
		npts_T, PC_t, pas_CS, sw_Punts_Control, QMatrix,
		dibuixa_TriedreFrenet, dibuixa_TriedreDarboux);
}

// Barra_Estat: Actualitza la barra d'estat (Status Bar) de l'aplicació amb els
//      valors R,A,B,PVx,PVy,PVz en Visualització Interactiva.
void CEntornV3DView::Barra_Estat()
{
	CString sss;
	CEsfe3D OPVAux = {0.0,0.0,0.0};
	float PVx, PVy, PVz;
	//GLfloat color;

// Status Bar fitxer fractal
	if (nom != "") GetStatusBar().SetPaneText(0, nom);

// Càlcul dels valors per l'opció Vista->Navega
	if (projeccio != CAP && projeccio != ORTO) {
		if (camera == CAM_ESFERICA)
		{	// Càmera Esfèrica
			OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta;
		}
		else if (camera == CAM_NAVEGA)
		{	// Càmera Navega
			OPVAux.R = sqrt(opvN.x * opvN.x + opvN.y * opvN.y + opvN.z * opvN.z);
			OPVAux.alfa = (asin(opvN.z / OPVAux.R) * 180) / PI;
			OPVAux.beta = (atan(opvN.y / opvN.x)) * 180 / PI;
		}
		else {	//Càmera Geode
			OPVAux.R = OPV_G.R; OPVAux.alfa = OPV_G.alfa; OPVAux.beta = OPV_G.beta;
		}
	}
	else {
		OPVAux.R = OPV.R; OPVAux.alfa = OPV.alfa; OPVAux.beta = OPV.beta; //Raux=R; angv=anglev; angh=angleh; 
	}

// Status Bar R Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = " ORTO   ";
	else if (camera == CAM_NAVEGA) buffer = " NAV   ";
	else buffer.Format(_T("%.1f"), OPVAux.R);
	sss = _T("R=") + buffer;
	// Refrescar posició R Status Bar
	GetStatusBar().SetPaneText(1, sss);

// Status Bar angle alfa Origen Punt de Vista
	if (projeccio == CAP) buffer = "       ";
	else if (projeccio == ORTO) buffer = "ORTO   ";
	else if (camera == CAM_NAVEGA) buffer = " NAV   ";
	else if (camera == CAM_LOXODROMA_TDARBOUX || camera == CAM_LOXODROMA_TFRENET) buffer.Format(_T("%.3f"), a_CamLoxo);
	else buffer.Format(_T("%.1f"), OPVAux.alfa);
	sss = _T("a=") + buffer;
	// Refrescar posició angleh Status Bar
	GetStatusBar().SetPaneText(2, sss);

// Status Bar paràmetre t de la Corba Loxodroma
	if (camera == CAM_LOXODROMA_TDARBOUX || camera == CAM_LOXODROMA_TFRENET)
	{
		buffer.Format(_T("%.3f"), cam_t);
		sss = _T("t=") + buffer;
	}
	else {	// Status Bar angle beta Origen Punt de Vista
		if (projeccio == CAP) buffer = "       ";
		else if (projeccio == ORTO) buffer = "ORTO   ";
		else if (camera == CAM_NAVEGA) buffer = " NAV   ";
		else buffer.Format(_T("%.1f"), OPVAux.beta);
		sss = _T("b=") + buffer;
	}
// Refrescar posició anglev Status Bar
	GetStatusBar().SetPaneText(3, sss);

// Transformació PV de Coord. esfèriques (R,anglev,angleh) --> Coord. cartesianes (PVx,PVy,PVz)
	if (camera == CAM_NAVEGA) { PVx = opvN.x; PVy = opvN.y; PVz = opvN.z; }
	else {
		if (Vis_Polar == POLARZ) {
			PVx = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			if (camera == CAM_GEODE)
			{	// Vector camp on mira (cap a (R,alfa,beta)
				PVx = PVx + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				PVy = PVy + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				PVz = PVz + sin(OPVAux.alfa * PI / 180);
			}
		}
		else if (Vis_Polar == POLARY) {
			PVx = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			if (camera == CAM_GEODE)
			{	// Vector camp on mira (cap a (R,alfa,beta)
				PVx = PVx + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				PVy = PVy + sin(OPVAux.alfa * PI / 180);
				PVz = PVz + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			}
		}
		else {	
			PVx = OPVAux.R * sin(OPVAux.alfa * PI / 180);
			PVy = OPVAux.R * cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			PVz = OPVAux.R * sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			if (camera == CAM_GEODE)
			{	// Vector camp on mira (cap a (R,alfa,beta)
				PVx = PVx + sin(OPVAux.alfa * PI / 180);
				PVy = PVy + cos(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
				PVz = PVz + sin(OPVAux.beta * PI / 180) * cos(OPVAux.alfa * PI / 180);
			}
		}
	}

// Status Bar PVx
	if (projeccio == CAP) buffer = "       ";
	else if (pan) float2CString(tr_cpv.x);
	else buffer.Format(_T("%.4f"), PVx); // float2CString(PVx);
	sss = _T("PVx=") + buffer;
	// Refrescar posició PVx Status Bar
	GetStatusBar().SetPaneText(4, sss);

// Status Bar PVy
	if (projeccio == CAP) buffer = "       ";
	else if (pan) float2CString(tr_cpv.y);
	else buffer.Format(_T("%.4f"), PVy); //float2CString(PVy);
	sss = _T("PVy=") + buffer;
	// Refrescar posició PVy Status Bar
	GetStatusBar().SetPaneText(5, sss);

// Status Bar PVz
	if (projeccio == CAP) buffer = "       ";
	else if (pan) float2CString(tr_cpv.z);
	else buffer.Format(_T("%.4f"), PVz); //float2CString(PVz);
	sss = _T("PVz=") + buffer;
	// Refrescar posició PVz Status Bar
	GetStatusBar().SetPaneText(6, sss);

// Status Bar per indicar el modus de canvi de color (FONS o OBJECTE)
	sss = " ";
	if (sw_grid) sss = "GRID";
	else if (pan) sss = " PAN";
	else if (sw_color) sss = " OBJ";
	// EntornV3D. QUATERNIONS: Displaiar el missatge 'QTN' en Status Bar
	else if (transf) {
		if (rota) { if (rotaQ) sss = "QTN"; else sss = "ROT"; } //sss = "ROT";
		else if (trasl) sss = "TRA";
		else if (escal) sss = "ESC";
	}
	else sss = "FONS";

// Refrescar posició Transformacions en Status Bar
	GetStatusBar().SetPaneText(7, sss);

// Status Bar per indicar tipus de Transformació (TRAS, ROT, ESC)
	sss = " ";
	if (transf) {	// EntornV3D. QUATERNIONS: Displaiar el valor w del Quaternió en Status Bar
		if (rotaQ) {
			buffer.Format(_T("%.2f"), qT.w);
			sss = buffer + _T("  ");
		}
	}
	else {
		// Components d'intensitat de fons que varien per teclat
		if ((fonsR) && (fonsG) && (fonsB)) sss = " RGB";
		else if ((fonsR) && (fonsG)) sss = " RG ";
		else if ((fonsR) && (fonsB)) sss = " R   B";
		else if ((fonsG) && (fonsB)) sss = "   GB";
		else if (fonsR) sss = " R  ";
		else if (fonsG) sss = "   G ";
		else if (fonsB) sss = "      B";
	}
// Refrescar posició Transformacions en Status Bar
	GetStatusBar().SetPaneText(8, sss);

// Status Bar dels paràmetres de Transformació, Color i posicions de Robot i Cama
	sss = " ";
	if (transf)
	{	if (rota)
		{		if (rotaQ) // Entorn V3D. QUATERNIONS: Visualitzar el quaternió (x,y,z) o angles d'Euler equivalents
			{
				buffer.Format(_T("%.2f"), qT.x);
				sss = buffer + _T("  ");

				buffer.Format(_T("%.2f"), qT.y);
				sss = sss + buffer + _T("  ");

				buffer.Format(_T("%.2f"), qT.z);
				sss = sss + buffer;
			}
			else {	buffer.Format(_T("%.2f"), TG.VRota.x);
					sss = buffer + _T("  ");

					buffer.Format(_T("%.2f"), TG.VRota.y);
					sss = sss + buffer + _T("  ");

					buffer.Format(_T("%.2f"), TG.VRota.z);
					sss = sss + buffer;
				}
		}
		else if (trasl)
		{	buffer.Format(_T("%.2f"), TG.VTras.x);
			sss = _T("  ") + buffer + _T("   ");

			buffer.Format(_T("%.2f"), TG.VTras.y);
			sss = sss + buffer + _T("   ");

			buffer.Format(_T("%.2f"), TG.VTras.z);
			sss = sss + buffer;
		}
		else if (escal)
		{	buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.y);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.2f"), TG.VScal.x);
			sss = sss + buffer;
		}
	}
	else {	// Color fons
		if (!sw_color)
		{	buffer.Format(_T("%.3f"), c_fons.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), c_fons.b);
			sss = sss + buffer;
		}
		else
		{	// Color objecte
			buffer.Format(_T("%.3f"), col_obj.r);
			sss = _T(" ") + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.g);
			sss = sss + buffer + _T("  ");

			buffer.Format(_T("%.3f"), col_obj.b);
			sss = sss + buffer;
		}
	}

// Refrescar posició PVz Status Bar
	GetStatusBar().SetPaneText(9, sss);

	if (camera == CAM_LOXODROMA_TDARBOUX || camera == CAM_LOXODROMA_TFRENET ||
		objecte == C_BEZIER || objecte == C_BSPLINE || objecte == C_LOXODROMA)
	{
		buffer.Format(_T("%.3f"), pas_CS);
		sss = _T("Pas=") + buffer;
	}

// Entorn V3D. QUATERNIONS: Status Bar pels Keyframes de l'animació d'objectes rígids per QUATERNIONS
	else if (rotaQ) {
		if (np_InQ == 1) sss = " KEYFQ 1";
		else if (np_InQ == 2) sss = " KEYFQ 2";
		else if (np_InQ == 3) sss = " KEYFQ 3";
		else sss = "       ";
	}
	else { sss = "          "; }

// Refrescar posició Pas de Corbes o Keyframes de Quaternions a Status Bar
	GetStatusBar().SetPaneText(10, sss);

	//buffer.Format(_T("%.2f"), averageFrameTimeMilliseconds);	// Milisegons per a cada frama
	//sss = _T("t=") + buffer;
	buffer.Format(_T("%.2f"), fps);	// Nombre de frames per segon (FPS)
	sss = _T("fps=") + buffer;
// Refrescar temps en milisegons per a cada frame o Frames Per Second en Status Bar
	GetStatusBar().SetPaneText(11, sss);
}


void CEntornV3DView::float2CString(float varf)
{
	float vfloat;
	vfloat = varf;
	if (abs(varf) < 1.0) buffer.Format(_T("%.5f"), varf);
	else if (abs(varf) < 99.0) buffer.Format(_T("%.4f"), varf);
	else if (abs(varf) < 999.0) buffer.Format(_T("%.3f"), varf);
	else if (abs(varf) < 9999.0) buffer.Format(_T("%.2f"), varf);
	else if (abs(varf) < 99999.0) buffer.Format(_T("%.1f"), varf);
	else buffer.Format(_T("%.0f"), varf);
}


void CEntornV3DView::double2CString(double varf)
{
	double vdouble;
	vdouble = varf;
	if (abs(varf) < 1.0) buffer.Format(_T("%.5f"), varf);
	else if (abs(varf) < 99.0) buffer.Format(_T("%.4f"), varf);
	else if (abs(varf) < 999.0) buffer.Format(_T("%.3f"), varf);
	else if (abs(varf) < 9999.0) buffer.Format(_T("%.2f"), varf);
	else if (abs(varf) < 99999.0) buffer.Format(_T("%.1f"), varf);
	else buffer.Format(_T("%.0f"), varf);
}


/* ------------------------------------------------------------------------- */
/*                                  SHADERS                                  */
/* ------------------------------------------------------------------------- */

/*****************************************************************************
* initializeShaders()
*      Loads custom shader by specifying filename (expects frag/vert pair)
*****************************************************************************/
GLuint CEntornV3DView::initializeShaders(CString filename)
{
	GLuint v, f, p;
	char* vs = NULL, * fs = NULL;
	CString file_Vert, file_Frag;
	int info_log_length = 0, compile_result = 0;
	FILE* fd;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	// Llegir Vertex Shader <filename>.vert
	file_Vert = filename + _T(".vert");
	char* fitxer_Vert = CString2Char(file_Vert);
	vs = textFileRead(fitxer_Vert);

	// Llegir Fragment Shader <filename>.vert
	file_Frag = filename + _T(".frag");
	char* fitxer_Frag = CString2Char(file_Frag);
	fs = textFileRead(fitxer_Frag);

	// Si s'han trobats els fitxers, procedir.
	if ((vs != NULL) && (fs != NULL))
	{
		const char* vv = vs;
		const char* ff = fs;

		glShaderSource(v, 1, &vv, NULL);
		glShaderSource(f, 1, &ff, NULL);

		free(vs);
		free(fs);
		glCompileShader(v);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compile_result);
		// Llista error de compilació del Vertex Shader
		if (compile_result == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);

			AfxMessageBox(_T("Error compilació Vertex Shader"));

			// Volcar missatges error a fitxer Compile_Error.VERT
			if ((fd = fopen("GLSL_Error.VERT", "w")) == NULL)
			{
				AfxMessageBox(_T("GLSL_Error.VERT was not opened"));
				return false;
			}
			for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
			fclose(fd);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(v); // Don't leak the shader.
			return 0;
		}

		// Compilació Fragment Shader
		glCompileShader(f);
		glGetShaderiv(v, GL_COMPILE_STATUS, &compile_result);

		// Llista error de compilació del Vertex Shader
		if (compile_result == GL_FALSE) {
			GLint maxLength = 0;

			glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
			// The maxLength includes the NULL character
			std::vector<GLchar> errorLog(maxLength);
			glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);

			AfxMessageBox(_T("Error compilació Fragment Shader"));

			// Volcar missatges error a fitxer Compile_Error.FRAG
			if ((fd = fopen("GLSL_Error.FRAG", "w")) == NULL)
			{
				AfxMessageBox(_T("GLSL_Error.FRAG was not opened"));
				return false;
			}
			for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
			fclose(fd);

			// Provide the infolog in whatever manor you deem best.
			// Exit with failure.
			glDeleteShader(v); // Don't leak the shader.
			return 0;
		}

		// Creació del Program que inclou Vertex i Fragment Shader
		p = glCreateProgram();
		glAttachShader(p, v);	// Relaciona Vertex Shader al Program
		glAttachShader(p, f);	// relaciona Fragment Shader al Program

		// Linkedició del Program
		glLinkProgram(p);

		// Activació del Programa
		glUseProgram(p);

		// Retorna referència al Program creat
		return p;
	}
	else return 0;	// retornar 0 si no s'ha trobat el fitxer.
}


/*****************************************************************************
* loadFileShaders()
*      Loads custom shader by specifying filename (expects frag/vert pair)
*****************************************************************************/
GLuint CEntornV3DView::loadFileShaders(CString file_Vert, CString file_Frag)
{

	GLuint v, f, p;
	char* vs = NULL, * fs = NULL;
	//	CString file_Vert, file_Frag;
	int info_log_length = 0, compile_result = 0;
	FILE* fd;

	v = glCreateShader(GL_VERTEX_SHADER);
	f = glCreateShader(GL_FRAGMENT_SHADER);

	//	file_Vert = filename + _T(".vert");
	char* fitxer_Vert = CString2Char(file_Vert);

	vs = textFileRead(fitxer_Vert);
	//vs = textFileRead(filename + ".vert");

//	file_Frag = filename + _T(".frag");
	char* fitxer_Frag = CString2Char(file_Frag);

	fs = textFileRead(fitxer_Frag);
	//fs = textFileRead(filename + ".frag");

	const char* vv = vs;
	const char* ff = fs;

	glShaderSource(v, 1, &vv, NULL);
	glShaderSource(f, 1, &ff, NULL);

	free(vs);
	free(fs);
	glCompileShader(v);
	glGetShaderiv(v, GL_COMPILE_STATUS, &compile_result);
	// Llista error de compilació
	if (compile_result == GL_FALSE) {
		GLint maxLength = 0;

		glGetShaderiv(v, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(v, maxLength, &maxLength, &errorLog[0]);

		AfxMessageBox(_T("Error compilació Vertex Shader"));

		// Volcar missatges error a fitxer Compile_Error.VERT
		if ((fd = fopen("GLSL_Error.VERT", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.VERT was not opened"));
			return false;
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(v); // Don't leak the shader.
		return 0;
	}

	glCompileShader(f);
	glGetShaderiv(f, GL_COMPILE_STATUS, &compile_result);
	// Llista error de compilació
	if (compile_result == GL_FALSE) {
		GLint maxLength = 0;

		glGetShaderiv(f, GL_INFO_LOG_LENGTH, &maxLength);
		// The maxLength includes the NULL character
		std::vector<GLchar> errorLog(maxLength);
		glGetShaderInfoLog(f, maxLength, &maxLength, &errorLog[0]);

		AfxMessageBox(_T("Error compilació Fragment Shader"));

		// Volcar missatges error a fitxer Compile_Error.FRAG
		if ((fd = fopen("GLSL_Error.FRAG", "w")) == NULL)
		{
			AfxMessageBox(_T("GLSL_Error.FRAG was not opened"));
			return false;
		}
		for (int i = 0; i <= maxLength; i = i++) fprintf(fd, "%c", errorLog[i]);
		fclose(fd);

		// Provide the infolog in whatever manor you deem best.
		// Exit with failure.
		glDeleteShader(f); // Don't leak the shader.
		return 0;
	}

	p = glCreateProgram();
	glAttachShader(p, v);
	glAttachShader(p, f);

	glLinkProgram(p);
	glUseProgram(p);

	return p;
}


char* CEntornV3DView::textFileRead(char* fn) {
	FILE* fp;
	char* content = NULL;
	int count = 0;
	if (fn != NULL) {
		fp = fopen(fn, "rt");
		if (fp != NULL) {
			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);
			if (count > 0) {
				content = (char*)malloc(sizeof(char) * (count + 1));
				count = (int)fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
		else {
			AfxMessageBox(_T("Vertex or Fragment Program not found"));
			return NULL;
		}
	}
	return content;
}

/*****************************************************************************
* releaseAllShaders()
*      Unload shaders, return to OpenGL fixed function.
*****************************************************************************/
void CEntornV3DView::releaseAllShaders()
{
// Entorn V3D: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	glUseProgram(0);

// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL TECLAT                              */
/* ------------------------------------------------------------------------- */

// OnKeyDown: Funció de tractament de teclat (funció que es crida quan es prem una tecla)
//   PARÀMETRES:
//    - nChar: Codi del caracter seleccionat
//    - nRepCnt: Nombre de vegades que s'ha apretat la tecla (acceleració)
//    - nFlags: Flags d'interrupció activats.
void CEntornV3DView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	const float incr = 0.025f;
	float modul = 0;
	GLfloat vdir[3] = { 0, 0, 0 };

	if (nChar == 'F') this->OnVistaFullScreen();	// Activació-Desactivació Full Screen
	if (camera == CAM_LOXODROMA_TDARBOUX || camera == CAM_LOXODROMA_TFRENET) Teclat_Camera_Loxodroma(nChar, nRepCnt);
	else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
	else if (objecte == ESCENA_LOXODROMA || objecte == PAISATGE || objecte == C_LOXODROMA ||
		objecte == C_BEZIER || objecte == C_BSPLINE || objecte == C_LEMNISCATA) Teclat_PasCorbes(nChar, nRepCnt);
	else if (objecte == PAISATGE) Teclat_Paisatge(nChar, nRepCnt);
	else if ((sw_grid) && ((grid.x) || (grid.y) || (grid.z))) Teclat_Grid(nChar, nRepCnt);
	else if (((nChar == 'G') || (nChar == 'g')) && ((grid.x) || (grid.y) || (grid.z))) sw_grid = !sw_grid;
	else if (pan || transf) {
		if (transf) {
			if (rota) Teclat_TransRota(nChar, nRepCnt);
				else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
						else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
	}
	else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
	else Teclat_ColorObjecte(nChar, nRepCnt);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CEntornV3DView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado

	CView::OnKeyUp(nChar, nRepCnt, nFlags);
}


// Teclat_Camera_Loxodroma: Teclat per incrementar-Decrementar el pas de dibuix de les corbes (pas_CS).
void CEntornV3DView::Teclat_Camera_Loxodroma(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Tecla cursor esquerra: Retrocedir posició de la càmera segons la corba Loxodroma
	case VK_LEFT:

		break;

	// Tecla cursor esquerra: Avançar la posició de la càmera segons la corba Loxodroma
	case VK_RIGHT:

		break;

	// Tecla F3 (incrementar pas)
	case VK_F3:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5) pas_CS = 0.5;
		break;

	// Tecla F4 (decrementar pas)
	case VK_F4:
		pas_CS = pas_CS / 2.0;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		break;

	// Tecla PgUp ('9') (incrementar pas) - NUMERICAL PAD
	case VK_PRIOR:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5)  pas_CS = 0.5;
		break;

	// Tecla PgUp ('9') (incrementar pas)
	case '9':
		pas_CS = pas_CS * 2.0;
		if (pas_CS > 0.5)  pas_CS = 0.5;
		break;

	// Tecla PgDown ('3') (decrementar pas) - NUMERICAL PAD
	case VK_NEXT:
		pas_CS = pas_CS / 2.0;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		break;

	// Tecla PgDown ('3') (decrementar pas)
	case '3':
		pas_CS = pas_CS / 2.0;
		if (pas_CS < 0.0125) pas_CS = 0.00625;
		break;

	// Tecla F5: Activar animació Càmera loxodroma
	case VK_F5:

		break;

	// Tecla PAUSE: Pausar / reactivar animació Càmera loxodroma - NUMERICAL PAD
	case VK_PAUSE:

		break;

	// Tecla F7, Pausa: Pausar / reactivar animació Càmera loxodroma - NUMERICAL PAD
	case VK_F7:

		break;

	// Tecla END: Desactivar animació Càmera loxodroma - NUMERICAL PAD
	case VK_END:

		break;

	// Tecla F8, Fi: Desactivar animació Càmera loxodroma 
	case VK_F8:

		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_PasCorbes: Teclat per incrementar-Decrementar el pas de dibuix de les corbes (pas_CS).
void CEntornV3DView::Teclat_PasCorbes(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Tecla '+' (incrementar pas_CS)
	case 107:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > max_pasCS) pas_CS = max_pasCS; // max_pasCS: valor constant
		break;

	// Tecla '-' (decrementar pas)
	case 109:
		pas_CS = pas_CS / 2.0;
		if (pas_CS < min_pasCS) pas_CS = min_pasCS;	// min_pasCS: valor constant
		break;

	// Tecla F1 (incrementar pas_CS)
	case VK_F1:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > max_pasCS) pas_CS = max_pasCS; // max_pasCS: valor constant
		break;

	// Tecla F2 (decrementar pas)
	case VK_F2:
		pas_CS = pas_CS / 2.0;
		if (pas_CS < min_pasCS) pas_CS = min_pasCS;	// min_pasCS: valor constant
		break;

	// Tecla PgUp ('9') (incrementar pas) - NUMERICAl PAD
	case VK_PRIOR:
		pas_CS = pas_CS * 2.0;
		if (pas_CS > max_pasCS) pas_CS = max_pasCS;
		break;

	// Tecla PgUp ('9') (incrementar pas) - NUMERICAl PAD
	case '9':
		pas_CS = pas_CS * 2.0;
		if (pas_CS > max_pasCS) pas_CS = max_pasCS;
		break;

	// Tecla PgDown ('3') (decrementar pas) - NUMERICAL PAD
	case VK_NEXT:
		pas_CS = pas_CS / 2.0;
		if (pas_CS < min_pasCS) pas_CS = min_pasCS;
		break;

	// Tecla PgDown ('3') (decrementar pas) - NUMERICAL PAD
	case '3':
		pas_CS = pas_CS / 2.0;
		if (pas_CS < min_pasCS) pas_CS = min_pasCS;
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_Paisatge: Teclat pel control de tecles de l'objecte PAISATGE.
void CEntornV3DView::Teclat_Paisatge(UINT nChar, UINT nRepCnt)
{
	bool pos = false;
	int i = 0;
	float modul = 0.0;

	switch (nChar)
	{
		// Entorn V3D. ESCENA: Tecla F5 (Activar animació trajectòria BSpline o Catmull-ROM en PAISATGE)
	case VK_F5:

		break;

		// Entorn V3D. ESCENA: Tecla Pausa (Activar/parar animació trajectòria BSpline o Catmull-Rom en PAISATGE) - NUMERICAL PAD
	case VK_PAUSE:

		break;

		// Entorn V3D. ESCENA: Tecla F7, Pausa (Activar/parar animació trajectòria BSpline o Catmull-Rom en PAISATGE)
	case VK_F7:

		break;

		// Entorn V3D. ESCENA: Tecla End (Para animació trajectòria BSpline o Catmull-Rom en PAISATGE) - NUMERICAL PAD
	case VK_END:

		break;

		// Entorn V3D. ESCENA: Tecla F8, Final (Para animació trajectòria BSpline o Catmull-Rom en PAISATGE)
	case VK_F8:

		break;

		// Entorn V3D. ESCENA: Tecla F3 (Accelerar animació trajectòria BSpline o Catmull-Rom en PAISATGE)
	case VK_F3:

		break;

		// Entorn V3D. ESCENA: Tecla F4 (Desaccelerar animació trajectòria BSpline o Catmull-Rom en PAISATGE)
	case VK_F4:

		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_ColorObjecte: Teclat pels canvis de color de l'objecte per teclat.
void CEntornV3DView::Teclat_ColorObjecte(UINT nChar, UINT nRepCnt)
{
	const float incr = 0.025f;

	if (nChar == VK_DOWN)
	{
		if (fonsR) {	col_obj.r -= nRepCnt * incr;
						if (col_obj.r < 0.0) col_obj.r = 0.0;
					}
		if (fonsG) {	col_obj.g -= nRepCnt * incr;
						if (col_obj.g < 0.0) col_obj.g = 0.0;
					}
		if (fonsB) {	col_obj.b -= nRepCnt * incr;
						if (col_obj.b < 0.0) col_obj.b = 0.0;
					}
	}
	else if (nChar == VK_UP)
	{	if (fonsR) {	col_obj.r += nRepCnt * incr;
						if (col_obj.r > 1.0) col_obj.r = 1.0;
					}
		if (fonsG) {	col_obj.g += nRepCnt * incr;
						if (col_obj.g > 1.0) col_obj.g = 1.0;
					}
		if (fonsB) {	col_obj.b += nRepCnt * incr;
						if (col_obj.b > 1.0) col_obj.b = 1.0;
					}
	}
	else if (nChar == VK_SPACE)
	{	if ((fonsR) && (fonsG) && (fonsB)) {	fonsG = false;
												fonsB = false;
											}
		else if ((fonsR) && (fonsG)) {	fonsG = false;
										fonsB = true;
										}
		else if ((fonsR) && (fonsB)) {	fonsR = false;
										fonsG = true;
									}
		else if ((fonsG) && (fonsB)) fonsR = true;
		else if (fonsR) {	fonsR = false;
							fonsG = true;
						}
		else if (fonsG) {	fonsG = false;
							fonsB = true;
						}
		else if (fonsB) {	fonsR = true;
							fonsG = true;
							fonsB = false;
						}
	}
	else if ((nChar == 'o') || (nChar == 'O')) sw_color = !sw_color;
	else if ((nChar == 'b') || (nChar == 'B')) sw_color = !sw_color;
}


// Teclat_ColorFons: Teclat pels canvis del color de fons.
void CEntornV3DView::Teclat_ColorFons(UINT nChar, UINT nRepCnt)
{
	const float incr = 0.025f;

	if (nChar == VK_DOWN) {
		if (fonsR) {	c_fons.r -= nRepCnt * incr;
						if (c_fons.r < 0.0) c_fons.r = 0.0;
					}
		if (fonsG) {	c_fons.g -= nRepCnt * incr;
						if (c_fons.g < 0.0) c_fons.g = 0.0;
					}
		if (fonsB) {	c_fons.b -= nRepCnt * incr;
						if (c_fons.b < 0.0) c_fons.b = 0.0;
					}
		}
	else if (nChar == VK_UP) {
		if (fonsR) {	c_fons.r += nRepCnt * incr;
						if (c_fons.r > 1.0) c_fons.r = 1.0;
					}
		if (fonsG) {	c_fons.g += nRepCnt * incr;
						if (c_fons.g > 1.0) c_fons.g = 1.0;
					}
		if (fonsB) {	c_fons.b += nRepCnt * incr;
						if (c_fons.b > 1.0) c_fons.b = 1.0;
					}
		}
	else if (nChar == VK_SPACE) {
		if ((fonsR) && (fonsG) && (fonsB)) {	fonsG = false;
												fonsB = false;
											}
		else if ((fonsR) && (fonsG)) {	fonsG = false;
										fonsB = true;
									}
		else if ((fonsR) && (fonsB)) {	fonsR = false;
										fonsG = true;
									}
		else if ((fonsG) && (fonsB)) fonsR = true;
		else if (fonsR) {	fonsR = false;
							fonsG = true;
						}
		else if (fonsG) {	fonsG = false;
							fonsB = true;
						}
		else if (fonsB) {	fonsR = true;
							fonsG = true;
							fonsB = false;
						}
		}
	else if ((nChar == 'o') || (nChar == 'O')) sw_color = !sw_color;
	else if ((nChar == 'b') || (nChar == 'B')) sw_color = !sw_color;
}

// Teclat_Navega: Teclat pels moviments de navegació.
void CEntornV3DView::Teclat_Navega(UINT nChar, UINT nRepCnt)
{
	GLfloat vdir[3] = { 0, 0, 0 };
	float modul = 0;

// Entorn V3D: Controls de moviment de navegació
	vdir[0] = n[0] - opvN.x;
	vdir[1] = n[1] - opvN.y;
	vdir[2] = n[2] - opvN.z;
	modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
	vdir[0] = vdir[0] / modul;
	vdir[1] = vdir[1] / modul;
	vdir[2] = vdir[2] / modul;
	switch (nChar)
	{
	// Tecla cursor amunt segons configuració eixos en Polars
	case VK_UP:
		if (Vis_Polar == POLARZ) {  // (X,Y,Z)
			opvN.x += nRepCnt * fact_pan * vdir[0];
			opvN.y += nRepCnt * fact_pan * vdir[1];
			n[0] += nRepCnt * fact_pan * vdir[0];
			n[1] += nRepCnt * fact_pan * vdir[1];
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.x += nRepCnt * fact_pan * vdir[0];
			opvN.z += nRepCnt * fact_pan * vdir[2];
			n[0] += nRepCnt * fact_pan * vdir[0];
			n[2] += nRepCnt * fact_pan * vdir[2];
		}
		else if (Vis_Polar == POLARX) {	//(X,Y,Z) --> (Y,Z,X)
			opvN.y += nRepCnt * fact_pan * vdir[1];
			opvN.z += nRepCnt * fact_pan * vdir[2];
			n[1] += nRepCnt * fact_pan * vdir[1];
			n[2] += nRepCnt * fact_pan * vdir[2];
		}
		break;

	// Tecla cursor avall segons configuració eixos en Polars
	case VK_DOWN:
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			opvN.x -= nRepCnt * fact_pan * vdir[0];
			opvN.y -= nRepCnt * fact_pan * vdir[1];
			n[0] -= nRepCnt * fact_pan * vdir[0];
			n[1] -= nRepCnt * fact_pan * vdir[1];
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.x -= nRepCnt * fact_pan * vdir[0];
			opvN.z -= nRepCnt * fact_pan * vdir[2];
			n[0] -= nRepCnt * fact_pan * vdir[0];
			n[2] -= nRepCnt * fact_pan * vdir[2];
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			opvN.y -= nRepCnt * fact_pan * vdir[1];
			opvN.z -= nRepCnt * fact_pan * vdir[2];
			n[1] -= nRepCnt * fact_pan * vdir[1];
			n[2] -= nRepCnt * fact_pan * vdir[2];
		}
		break;

	// Tecla cursor esquerra segons configuració eixos en Polars
	case VK_LEFT:
		angleZ = +nRepCnt * fact_pan;
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
			n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
			n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
			n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}
		break;

	// Tecla cursor dret segons configuració eixos en Polars
	case VK_RIGHT:
		angleZ = 360 - nRepCnt * fact_pan;
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180) - n[1] * sin(angleZ * PI / 180);
			n[1] = n[0] * sin(angleZ * PI / 180) + n[1] * cos(angleZ * PI / 180);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[0] = vdir[0]; // n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180) - n[0] * sin(angleZ * PI / 180);
			n[0] = n[2] * sin(angleZ * PI / 180) + n[0] * cos(angleZ * PI / 180);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = vdir[1]; // n[1] - opvN.y;
			n[2] = vdir[2]; // n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180) - n[2] * sin(angleZ * PI / 180);
			n[2] = n[1] * sin(angleZ * PI / 180) + n[2] * cos(angleZ * PI / 180);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}
		break;

	// Tecla Inicio segons configuració eixos en Polars
	case VK_HOME:
		if (Vis_Polar == POLARZ) {
			opvN.z += nRepCnt * fact_pan;
			n[2] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y += nRepCnt * fact_pan;
			n[1] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x += nRepCnt * fact_pan;
			n[0] += nRepCnt * fact_pan;
		}
		break;

	// Tecla 7 NUMERIC KEYPAD: Inicio segons configuració eixos en Polars
	case VK_NUMPAD7:
		if (Vis_Polar == POLARZ) {
			opvN.z += nRepCnt * fact_pan;
			n[2] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y += nRepCnt * fact_pan;
			n[1] += nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x += nRepCnt * fact_pan;
			n[0] += nRepCnt * fact_pan;
		}
		break;

	// Tecla Fin segons configuració eixos en Polars
	case VK_END:
		if (Vis_Polar == POLARZ) {
			opvN.z -= nRepCnt * fact_pan;
			n[2] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y -= nRepCnt * fact_pan;
			n[1] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x -= nRepCnt * fact_pan;
			n[0] -= nRepCnt * fact_pan;
		}
		break;

	// Tecla 1 NUMERIC KEYPAD: Fin segons configuració eixos en Polars
	case VK_NUMPAD1:
		if (Vis_Polar == POLARZ) {
			opvN.z -= nRepCnt * fact_pan;
			n[2] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARY) {
			opvN.y -= nRepCnt * fact_pan;
			n[1] -= nRepCnt * fact_pan;
		}
		else if (Vis_Polar == POLARX) {
			opvN.x -= nRepCnt * fact_pan;
			n[0] -= nRepCnt * fact_pan;
		}
		break;

	// Tecla PgUp
	case VK_PRIOR:
		fact_pan /= 2.0;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla PgUp - NUMERIC KEYPAD
	case VK_NUMPAD9:
		fact_pan /= 2.0;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla PgDown
	case VK_NEXT:
		fact_pan *= 2.0;
		if (fact_pan > 2048.0) fact_pan = 2048.0;
		break;

	// Tecla PgDown - NUEMRIC KEYPAD
	case VK_NUMPAD3:
		fact_pan *= 2.0;
		if (fact_pan > 2048.0) fact_pan = 2048.0;
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (pan) Teclat_Pan(nChar, nRepCnt);
		else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_Pan: Teclat pels moviments de Pan.
void CEntornV3DView::Teclat_Pan(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Tecla cursor amunt
	case VK_UP:
		tr_cpv.y -= nRepCnt * fact_pan;
		if (tr_cpv.y < -100000.0) tr_cpv.y = 100000.0;
		break;

	// Tecla cursor avall
	case VK_DOWN:
		tr_cpv.y += nRepCnt * fact_pan;
		if (tr_cpv.y > 100000.0) tr_cpv.y = 100000.0;
		break;

	// Tecla cursor esquerra
	case VK_LEFT:
		tr_cpv.x += nRepCnt * fact_pan;
		if (tr_cpv.x > 100000.0) tr_cpv.x = 100000.0;
		break;

	// Tecla cursor dret
	case VK_RIGHT:
		tr_cpv.x -= nRepCnt * fact_pan;
		if (tr_cpv.x < -100000.0) tr_cpv.x = 100000.0;
		break;

	// Tecla PgUp - NUMERIC KEYPAD
	case VK_PRIOR:
		fact_pan /= 2.0;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla Avançar
	case '9':
		fact_pan /= 2.0;
		if (fact_pan < 0.125) fact_pan = 0.125;
		break;

	// Tecla PgDown - NUMERIC KEYPAD
	case VK_NEXT:
		fact_pan *= 2.0;
		if (fact_pan > 2048.0) fact_pan = 2048.0;
		break;

	// Tecla Retrocedir 
	case '3':
		fact_pan *= 2;
		if (fact_pan > 2048.0) fact_pan = 2048.0;
		break;

	// Tecla Insert: Fixar el desplaçament de pantalla (pan)
	case VK_INSERT:
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000.0) tr_cpvF.z = 100000.0;
		break;

	// Tecla 0 NUMERIC KEYPAD, Insert: Fixar el desplaçament de pantalla (pan)
	case VK_NUMPAD0:
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000.0) tr_cpvF.z = 100000.0;
		break;

	// Tecla 0, Insert: Fixar el desplaçament de pantalla (pan)
	case '0':
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000.0) tr_cpvF.z = 100000.0;
		break;

	// Tecla 5, Insert: Fixar el desplaçament de pantalla (pan)
	case '5':
		// Acumular desplaçaments de pan (tr_cpv) en variables fixes (tr_cpvF).
		tr_cpvF.x += tr_cpv.x;		tr_cpv.x = 0.0;
		if (tr_cpvF.x > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.y += tr_cpv.y;		tr_cpv.y = 0.0;
		if (tr_cpvF.y > 100000.0) tr_cpvF.y = 100000.0;
		tr_cpvF.z += tr_cpv.z;		tr_cpv.z = 0.0;
		if (tr_cpvF.z > 100000.0) tr_cpvF.z = 100000.0;
		break;

	// Tecla Delete: Inicialitzar el desplaçament de pantalla (pan)
	case VK_DELETE:
		// Inicialitzar els valors de pan tant de la variable tr_cpv com de la tr_cpvF.
		tr_cpv.x = 0.0;			tr_cpv.y = 0.0;			tr_cpv.z = 0.0;
		tr_cpvF.x = 0.0;		tr_cpvF.y = 0.0;		tr_cpvF.z = 0.0;
		break;

	default:
		if (transf)
		{	if (rota) Teclat_TransRota(nChar, nRepCnt);
			else if (trasl) Teclat_TransTraslada(nChar, nRepCnt);
			else if (escal) Teclat_TransEscala(nChar, nRepCnt);
		}
		if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
		else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_TransEscala: Teclat pels canvis del valor d'escala per X,Y,Z.
void CEntornV3DView::Teclat_TransEscala(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
// Modificar vector d'Escalatge per teclat (actiu amb Escalat únicament)
	// Tecla '+' NUMERIC KEYPAD  (augmentar tot l'escalat)
	case VK_ADD: // 107:
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla '+' (augmentar tot l'escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla '-' NUMERIC KEYPAD (disminuir tot l'escalat)
	case VK_SUBTRACT: // 109:
		TG.VScal.x = TG.VScal.x / 2.0;
		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		TG.VScal.y = TG.VScal.y / 2.0;
		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		TG.VScal.z = TG.VScal.z / 2.0;
		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		break;

	// Tecla cursor amunt ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: //104:
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		break;

	// Tecla cursor amunt ('8')
	case '8':
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		break;

	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: //98:
		TG.VScal.x = TG.VScal.x / 2.0;
		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		break;

	// Tecla cursor avall ('2')
	case '2':
		TG.VScal.x = TG.VScal.x / 2.0;
		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		break;

	// Tecla cursor esquerra ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: //100:
		TG.VScal.y = TG.VScal.y / 2.0;
		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		break;

	// Tecla cursor esquerra ('4')
	case '4':
		TG.VScal.y = TG.VScal.y / 2.0;
		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		break;

	// Tecla cursor dret ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: //102:
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		break;

	// Tecla HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: //103:
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla HOME ('7')
	case '7':
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: //97:
		TG.VScal.z = TG.VScal.z / 2.0;
		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		break;

	// Tecla END ('1')
	case '1':
		TG.VScal.z = TG.VScal.z / 2.0;
		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		break;

	// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0)		TGF.VScal.x = 8192.0;	if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0)		TGF.VScal.y = 8192.0;	if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0)		TGF.VScal.z = 8192.0;	if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;			if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;			if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		break;

	// Tecla '0' NUMERIC KEYPAD, INSERT
	case VK_NUMPAD0:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;		TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 	if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	// Tecla '0', INSERT
	case '0':
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 	if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y; TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		break;

	// Tecla '5', INSERT
	case '5':
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;		TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 	if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		break;

		// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
			else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
					else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
							else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}

// Teclat_TransRota: Teclat pels canvis del valor del vector de l'angle de rotació per X,Y,Z.
void CEntornV3DView::Teclat_TransRota(UINT nChar, UINT nRepCnt)
{
	int i = 0;
	double angle = 0.0;
	GL_Quat angleQ;

	// Inicialitzar quaternio angleQ.
	angleQ.w = 1.0;	 angleQ.x = 0.0;	angleQ.y = 0.0, angleQ.z = 0.0;

	switch (nChar)
	{
	// Tecla cursor amunt 104 ('8') - NUMERIC PAD
	case VK_NUMPAD8: //104:
		TG.VRota.x += nRepCnt * fact_Rota;
		if (TG.VRota.x >= 360.0f) TG.VRota.x -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació X com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. 
		if (rotaQ) {
			angle += nRepCnt * fact_Rota;
			while (angle >= 360.0f) angle -= 360.0f;
			EixAngleToQuat(1.0, 0.0, 0.0, angle, angleQ);
			QuatMul(angleQ, qT, qT);
			QuatToMatrix(qT, QMatrix);
		}
		break;

		// Tecla cursor amunt ('8')
	case '8':
		TG.VRota.x += nRepCnt * fact_Rota;
		if (TG.VRota.x >= 360.0f) TG.VRota.x -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació X com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. 
		if (rotaQ) {
			angle += nRepCnt * fact_Rota;
			while (angle >= 360.0f) angle -= 360.0f;
			EixAngleToQuat(1.0, 0.0, 0.0, angle, angleQ);
			QuatMul(angleQ, qT, qT);
			QuatToMatrix(qT, QMatrix);
		}
		break;

		// Tecla cursor avall ('2') - NUMERIC PAD
	case VK_NUMPAD2: //98:
		TG.VRota.x -= nRepCnt * fact_Rota;
		if (TG.VRota.x < 0.0f) TG.VRota.x += 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació X com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. 
		if (rotaQ) {
			angle -= nRepCnt * fact_Rota;
			while (angle < 0.0f) angle += 360.0f;
			EixAngleToQuat(1.0, 0.0, 0.0, angle, angleQ);
			QuatMul(angleQ, qT, qT);
			QuatToMatrix(qT, QMatrix);
		}
		break;

		// Tecla cursor avall ('2')
	case '2':
		TG.VRota.x -= nRepCnt * fact_Rota;
		if (TG.VRota.x < 0.0f) TG.VRota.x += 360;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació X com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. 
		if (rotaQ) {
			angle -= nRepCnt * fact_Rota;
			while (angle < 0.0f) angle += 360.0f;
			EixAngleToQuat(1.0, 0.0, 0.0, angle, angleQ);
			QuatMul(angleQ, qT, qT);
			QuatToMatrix(qT, QMatrix);
		}
		break;

		// Tecla cursor esquerra ('4') - NUMERIC PAD
	case VK_NUMPAD4: //100:
		TG.VRota.y -= nRepCnt * fact_Rota;
		if (TG.VRota.y < 0.0f) TG.VRota.y += 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Y com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT.- PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER.

					}
		break;

		// Tecla cursor esquerra ('4')
	case '4':
		TG.VRota.y -= nRepCnt * fact_Rota;
		if (TG.VRota.y < 0.0f) TG.VRota.y += 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Y com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT.- PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER.

					}
		break;

	// Tecla cursor dret ('6') - NUMERIC PAD
	case VK_NUMPAD6: //102:
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360.0f) TG.VRota.y -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Y com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VRota.y += nRepCnt * fact_Rota;
		if (TG.VRota.y >= 360.0f) TG.VRota.y -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Y com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

	// Tecla HOME ('7') - NUMERIC PAD
	case VK_NUMPAD7: //103:
		TG.VRota.z += nRepCnt * fact_Rota;
		if (TG.VRota.z >= 360.0f) TG.VRota.z -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Z com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO: PENDENT DE FER

					}
		break;

	// Tecla HOME ('7')
	case '7':
		TG.VRota.z += nRepCnt * fact_Rota;
		if (TG.VRota.z >= 360.0f) TG.VRota.z -= 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Z com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

	// Tecla END ('1') - NUMERIC PAD
	case VK_NUMPAD1: //97:
		TG.VRota.z -= nRepCnt * fact_Rota;
		if (TG.VRota.z < 0.0f) TG.VRota.z += 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Z com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

	// Tecla END ('1')
	case '1':
		TG.VRota.z -= nRepCnt * fact_Rota;
		if (TG.VRota.z < 0.0f) TG.VRota.z += 360.0f;
		// Entorn V3D. QUATERNIONS: Definir nou angle rotació Z com a quaternió (angleQ) i multiplicar-lo per qT --> qT=angleQ*qT. - PENDENT DE FER
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

	// Tecla PgUp ('9') - NUMERIC PAD
	case VK_NUMPAD9: //105:
		fact_Rota /= 2.0;
		if (fact_Rota < 1.0) fact_Rota = 1.0;
		break;

	// Tecla PgUp ('9')
	case '9':
		fact_Rota /= 2.0;
		if (fact_Rota < 1.0) fact_Rota = 1.0;
		break;

	// Tecla PgDown ('3') - NUMERIC PAD
	case VK_NUMPAD3: // 99:
		fact_Rota *= 2.0;
		if (fact_Rota > 90.0) fact_Rota = 90.0;
		break;

	// Tecla PgDown ('3')
	case '3':
		fact_Rota *= 2.0;
		if (fact_Rota > 90.0) fact_Rota = 90.0;
		break;

	// Entorn V3D. QUATERNIONS: Tecla F5 (Activar animació cossos rígids per QUATERNIONS) -> PENDENT DE FER
	case VK_F5:
		if (rotaQ) { // TO DO. PENDENT DE FER

					}
		break;

		// Entorn V3D. QUATERNIONS: Tecla Pausa (Activar/parar animació cossos rígids per QUATERNIONS) -> PENDENT DE FER
	case VK_PAUSE:
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

		// Entorn V3D. QUATERNIONS: Tecla End (Para animació i inicialitzar vector Keyframes - PENDENT DE FER
	case VK_END:
		if (rotaQ) {	// TO DO. PENDENT DE FER

					}
		break;

		// Entorn V3D. QUATERNIONS:  Tecla F3 (Accelerar animació cossos rígids per QUATERNIONS) - PENDENT DE FER (ADDICIONAL)
	case VK_F3:
		if (rotaQ) {	// TO DO. PENDENT DE FER (ADDICIONAL)

					}
		break;

		// Entorn V3D. QUATERNIONS: Tecla F4 (Desaccelerar animació cossos rígids per QUATERNIONS) - PENDENT DE FER (ADDICIONAL)
	case VK_F4:
		if (rotaQ) {	// TO Do. PENDENT DE FER (ADDICIONAL)

					}
		break;

	// Tecla Insert: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case VK_INSERT:
		// Entorn V3D. QUATERNIONS: Fixar els Quaternions en els keyframes (animació cossos rígids per QUATERNIONS)
		if (rotaQ) { // TO DO. PENDENT DE FER

				}
		else {
			TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
			if (TGF.VScal.x > 8192.0)	TGF.VScal.x = 8192.0;		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
			if (TGF.VScal.y > 8192.0)	TGF.VScal.y = 8192.0;		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
			if (TGF.VScal.z > 8192.0)	TGF.VScal.z = 8192.0;		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
			TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
			TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
			while (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		while (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
			while (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		while (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
			while (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		while (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
			TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
			TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
			if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
			if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
			if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
			TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		}
		break;

	// Tecla '5', INSERT: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case VK_NUMPAD0: //'5':
		// Entorn V3D. QUATERNIONS: Fixar els Quaternions en els keyframes (animació cossos rígids per QUATERNIONS)
		if (rotaQ) { // PENDENT DE FER
		}
		else {
			TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
			if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;			if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
			if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;			if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
			if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;			if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
			TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
			TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
			while (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		while (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
			while (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		while (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
			while (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		while (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
			TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
			TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
			if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
			if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
			if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
			TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		}
		break;

	// Tecla '0', INSERT: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case '0':
		// Entorn V3D. QUATERNIONS: Fixar els Quaternions en els keyframes (animació cossos rígids per QUATERNIONS)
		if (rotaQ) { // TO DO. PENDENT DE FER

					}
		else {
			TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
			if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;			if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
			if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;			if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
			if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;			if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
			TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
			TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
			while (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0;		while (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
			while (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		while (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
			while (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		while (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
			TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
			TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
			if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
			if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
			if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
			TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		}
		break;

		// Tecla '5', INSERT: Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
	case '5':
		// Entorn V3D. QUATERNIONS: Fixar els Quaternions en els keyframes (animació cossos rígids per QUATERNIONS)
		if (rotaQ) { // TO DO. PENDENT DE FER
		
					}
		else {
			TGF.VScal.x *= TG.VScal.x;	TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
			if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;			if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
			if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;			if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
			if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;			if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
			TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
			TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y; TGF.VRota.z += TG.VRota.z;
			while (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		while (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
			while (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		while (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
			while (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		while (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
			TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
			TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
			if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
			if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
			if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
			TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		}
		break;

// Modificar vector d'Escalatge per teclat (actiu amb Rotació)
	// Tecla '+' NUMERIC KEYPAD (augmentar escalat)
	case VK_ADD: // 107:
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla '+' (augmentar escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla '-' NUMERIC KEYPAD (disminuir escalat)
	case VK_SUBTRACT: // 109:
		TG.VScal.x = TG.VScal.x / 2.0;
		if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
		TG.VScal.y = TG.VScal.y / 2.0;
		if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
		TG.VScal.z = TG.VScal.z / 2.0;
		if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
		break;

		// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;	TGF.VScal.y = 1.0;;	TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;	TG.VScal.y = 1.0;	TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;	TGF.VRota.y = 0.0;	TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;	TG.VRota.y = 0.0;	TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;	TGF.VTras.y = 0.0;	TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0.0;
		break;

	// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
			else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
					else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
							else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_TransTraslada: Teclat pels canvis del valor de traslació per X,Y,Z.
void CEntornV3DView::Teclat_TransTraslada(UINT nChar, UINT nRepCnt)
{
	GLfloat vdir[3] = { 0, 0, 0 };
	float modul = 0;

	switch (nChar)
	{
	// Tecla cursor amunt ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: //104:
		TG.VTras.x -= nRepCnt * fact_Tras;
		if (TG.VTras.x < -100000.0) TG.VTras.x = 100000.0;
		break;

	// Tecla cursor amunt ('8')
	case '8':
		TG.VTras.x -= nRepCnt * fact_Tras;
		if (TG.VTras.x < -100000.0) TG.VTras.x = 100000.0;
		break;

	// Tecla cursor avall ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: //98:
		TG.VTras.x += nRepCnt * fact_Tras;
		if (TG.VTras.x > 10000.0) TG.VTras.x = 100000.0;
		break;

	// Tecla cursor avall ('2')
	case '2':
		TG.VTras.x += nRepCnt * fact_Tras;
		if (TG.VTras.x > 10000.0) TG.VTras.x = 100000.0;
		break;

	// Tecla cursor esquerra ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: //100:
		TG.VTras.y -= nRepCnt * fact_Tras;
		if (TG.VTras.y < -100000.0) TG.VTras.y = -100000.0;
		break;

	// Tecla cursor esquerra ('4')
	case '4':
		TG.VTras.y -= nRepCnt * fact_Tras;
		if (TG.VTras.y < -100000.0) TG.VTras.y = -100000.0;
		break;

		// Tecla cursor dret ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: //102:
		TG.VTras.y += nRepCnt * fact_Tras;
		if (TG.VTras.y > 100000.0) TG.VTras.y = 100000.0;
		break;

	// Tecla cursor dret ('6')
	case '6':
		TG.VTras.y += nRepCnt * fact_Tras;
		if (TG.VTras.y > 100000.0) TG.VTras.y = 100000.0;
		break;

	// Tecla HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: //103:
		TG.VTras.z += nRepCnt * fact_Tras;
		if (TG.VTras.z > 100000.0) TG.VTras.z = 100000.0;
		break;

	// Tecla HOME ('7')
	case '7':
		TG.VTras.z += nRepCnt * fact_Tras;
		if (TG.VTras.z > 100000.0) TG.VTras.z = 100000.0;
		break;

	// Tecla END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: // 97:
		TG.VTras.z -= nRepCnt * fact_Tras;
		if (TG.VTras.z < -100000.0) TG.VTras.z = -100000.0;
		break;

	// Tecla END ('1')
	case '1':
		TG.VTras.z -= nRepCnt * fact_Tras;
		if (TG.VTras.z < -100000.0) TG.VTras.z = -100000.0;
		break;

	// Tecla PgUp ('9') - NUMERIC KEYPAD
	case VK_NUMPAD9: //105:
		fact_Tras /= 2.0;
		if (fact_Tras < 1.0) fact_Tras = 1.0;
		break;

		// Tecla PgUp
	case VK_PRIOR: //105:
		fact_Tras /= 2;
		if (fact_Tras < 1.0) fact_Tras = 1.0;
		break;

		// Tecla PgUp ('9')
	case '9':
		fact_Tras /= 2.0;
		if (fact_Tras < 1.0) fact_Tras = 1.0;
		break;

	// Tecla PgDown ('3') - NUMERIC KEYPAD
	case VK_NUMPAD3:
		fact_Tras *= 2.0;
		if (fact_Tras > 100000.0) fact_Tras = 100000.0;
		break;

	// Tecla PgDown - NUMERIC KEYPAD
	case VK_NEXT:
		fact_Tras *= 2;
		if (fact_Tras > 100000.0) fact_Tras = 100000.0;
		break;

	// Tecla PgDown ('3')
	case '3':
		fact_Tras *= 2.0;
		if (fact_Tras > 100000.0) fact_Tras = 100000.0;
		break;

// Modificar vector d'Escalatge per teclat (actiu amb Traslació)
	// Tecla '+' NUMERIC KEYPAD (augmentar escalat)
	case VK_ADD:
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

		// Modificar vector d'Escalatge per teclat (actiu amb Traslació)
		// Tecla '+' (augmentar escalat)
	case '+':
		TG.VScal.x = TG.VScal.x * 2.0;
		if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
		TG.VScal.y = TG.VScal.y * 2.0;
		if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
		TG.VScal.z = TG.VScal.z * 2.0;
		if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		break;

	// Tecla '-' (disminuir escalat)
	case 109:
		TG.VScal.x = TG.VScal.x / 2.0;
		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		TG.VScal.y = TG.VScal.y / 2.0;
		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		TG.VScal.z = TG.VScal.z / 2.0;
		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		break;

	// Tecla INSERT
	case VK_INSERT:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;		TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;			if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;			if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;			if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;			if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;			if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		break;

	// Tecla 0, INSERT - NUMERIC KEYPAD
	case VK_NUMPAD0:
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;		TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;		if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;		if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;		if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 	if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;		if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;		if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;				TG.VTras.y = 0.0;			TG.VTras.z = 0.0;
		break;

	// Tecla '5', INSERT
	case '5':
		// Acumular transformacions Geomètriques (variable TG) i de pan en variables fixes (variable TGF)
		TGF.VScal.x *= TG.VScal.x;		TGF.VScal.y *= TG.VScal.y; TGF.VScal.z *= TG.VScal.z;
		if (TGF.VScal.x > 8192.0) TGF.VScal.x = 8192.0;			if (TG.VScal.x < 0.125) TG.VScal.x = 0.125;
		if (TGF.VScal.y > 8192.0) TGF.VScal.y = 8192.0;			if (TG.VScal.y < 0.125) TG.VScal.y = 0.125;
		if (TGF.VScal.z > 8192.0) TGF.VScal.z = 8192.0;			if (TG.VScal.z < 0.125) TG.VScal.z = 0.125;
		TG.VScal.x = 1.0;				TG.VScal.y = 1.0;			TG.VScal.z = 1.0;
		TGF.VRota.x += TG.VRota.x;		TGF.VRota.y += TG.VRota.y;	TGF.VRota.z += TG.VRota.z;
		if (TGF.VRota.x >= 360.0) TGF.VRota.x -= 360.0; 		if (TGF.VRota.x < 0.0) TGF.VRota.x += 360.0;
		if (TGF.VRota.y >= 360.0) TGF.VRota.y -= 360.0;			if (TGF.VRota.y < 0.0) TGF.VRota.y += 360.0;
		if (TGF.VRota.z >= 360.0) TGF.VRota.z -= 360.0;			if (TGF.VRota.z < 0.0) TGF.VRota.z += 360.0;
		TG.VRota.x = 0.0;				TG.VRota.y = 0.0;			TG.VRota.z = 0.0;
		TGF.VTras.x += TG.VTras.x;		TGF.VTras.y += TG.VTras.y;	TGF.VTras.z += TG.VTras.z;
		if (TGF.VTras.x < -100000.0) TGF.VTras.x = 100000.0;	if (TGF.VTras.x > 10000.0) TGF.VTras.x = 100000.0;
		if (TGF.VTras.y < -100000.0) TGF.VTras.y = 100000.0;	if (TGF.VTras.y > 10000.0) TGF.VTras.y = 100000.0;
		if (TGF.VTras.z < -100000.0) TGF.VTras.z = 100000.0;	if (TGF.VTras.z > 10000.0) TGF.VTras.z = 100000.0;
		TG.VTras.x = 0.0;			TG.VTras.y = 0.0;				TG.VTras.z = 0.0;
		break;

	// Tecla Delete: Esborrar les Transformacions Geomètriques Calculades
	case VK_DELETE:
		// Inicialitzar els valors de transformacions Geomètriques i de pan en variables fixes.
		TGF.VScal.x = 1.0;		TGF.VScal.y = 1.0;;		TGF.VScal.z = 1.0;
		TG.VScal.x = 1.0;		TG.VScal.y = 1.0;		TG.VScal.z = 1.0;
		TGF.VRota.x = 0.0;		TGF.VRota.y = 0.0;		TGF.VRota.z = 0.0;
		TG.VRota.x = 0.0;		TG.VRota.y = 0.0;		TG.VRota.z = 0.0;
		TGF.VTras.x = 0.0;		TGF.VTras.y = 0.0;		TGF.VTras.z = 0.0;
		TG.VTras.x = 0.0;		TG.VTras.y = 0.0;		TG.VTras.z = 0.0;
		break;

		// Tecla Espaiador
	case VK_SPACE:
		rota = !rota;
		trasl = !trasl;
		break;

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
			else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
					else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
							else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


// Teclat_Grid: Teclat pels desplaçaments dels gridXY, gridXZ i gridYZ.
void CEntornV3DView::Teclat_Grid(UINT nChar, UINT nRepCnt)
{
	switch (nChar)
	{
	// Key Up cursor ('8') - NUMERIC KEYPAD
	case VK_NUMPAD8: //104:
		hgrid.x -= nRepCnt * PAS_GRID;
		break;

	// Key Up cursor ('8')
	case '8':
		hgrid.x -= nRepCnt * PAS_GRID;
		break;

	// Key Down cursor ('2') - NUMERIC KEYPAD
	case VK_NUMPAD2: //98:
		hgrid.x += nRepCnt * PAS_GRID;
		break;

	// Key Down cursor ('2')
	case '2':
		hgrid.x += nRepCnt * PAS_GRID;
		break;

	// Key Left cursor ('4') - NUMERIC KEYPAD
	case VK_NUMPAD4: //100:
		hgrid.y -= nRepCnt * PAS_GRID;
		break;

	// Key Left cursor ('4')
	case '4':
		hgrid.y -= nRepCnt * PAS_GRID;
		break;

	// Key Right cursor ('6') - NUMERIC KEYPAD
	case VK_NUMPAD6: //102:
		hgrid.y += nRepCnt * PAS_GRID;
		break;

	// Key Right cursor ('6')
	case '6':
		hgrid.y += nRepCnt * PAS_GRID;
		break;

	// Key HOME ('7') - NUMERIC KEYPAD
	case VK_NUMPAD7: //103:
		hgrid.z += nRepCnt * PAS_GRID;
		break;

	// Key HOME ('7')
	case '7':
		hgrid.z += nRepCnt * PAS_GRID;
		break;

	// Key END ('1') - NUMERIC KEYPAD
	case VK_NUMPAD1: //97:
		hgrid.z -= nRepCnt * PAS_GRID;
		break;

	// Key END ('1')
	case '1':
		hgrid.z -= nRepCnt * PAS_GRID;
		break;

	// Key grid ('G')
	case 'G':
		sw_grid = !sw_grid;
		break;

/*
	// Key grid ('g')
	case 'g':
		sw_grid = !sw_grid;
		break;
*/

	default:
		if (pan) Teclat_Pan(nChar, nRepCnt);
			else if (camera == CAM_NAVEGA) Teclat_Navega(nChar, nRepCnt);
					else if (!sw_color) Teclat_ColorFons(nChar, nRepCnt);
							else Teclat_ColorObjecte(nChar, nRepCnt);
		break;
	}
}


/* ------------------------------------------------------------------------- */
/*                           CONTROL DEL RATOLI                              */
/* ------------------------------------------------------------------------- */

// OnLButtonDown: Funció que es crida quan s'apreta el botó esquerra del mouse.
void CEntornV3DView::OnLButtonDown(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado

// Entorn V3D: Detectem en quina posició s'ha apretat el botó esquerra del
//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoEAvall
	m_ButoEAvall = true;
	m_PosEAvall = point;
	m_EsfeEAvall = OPV;

	CView::OnLButtonDown(nFlags, point);
}


// OnLButtonUp: Funció que es crida quan deixem d'apretar el botó esquerra del mouse.
void CEntornV3DView::OnLButtonUp(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
// Entorn V3D: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
	m_ButoEAvall = false;

// OPCIÓ VISTA-->SATÈLIT: Càlcul increment desplaçament del Punt de Vista
	if ((satelit) && (projeccio != ORTO))
	{	//m_EsfeIncEAvall.R = m_EsfeEAvall.R - OPV.R;
		m_EsfeIncEAvall.alfa = 0.01f * (OPV.alfa - m_EsfeEAvall.alfa); //if (abs(m_EsfeIncEAvall.alfa)<0.01) { if ((m_EsfeIncEAvall.alfa)>0.0) m_EsfeIncEAvall.alfa = 0.01 else m_EsfeIncEAvall.alfa=0.01}
		m_EsfeIncEAvall.beta = 0.01f * (OPV.beta - m_EsfeEAvall.beta);
		if (abs(m_EsfeIncEAvall.beta) < 0.01) {
			if ((m_EsfeIncEAvall.beta) > 0.0) m_EsfeIncEAvall.beta = 0.01;
			else m_EsfeIncEAvall.beta = 0.01;
		}
		if ((m_EsfeIncEAvall.R == 0.0) && (m_EsfeIncEAvall.alfa == 0.0) && (m_EsfeIncEAvall.beta == 0.0)) KillTimer(WM_TIMER);
		else SetTimer(WM_TIMER, 10, NULL);
	}

	CView::OnLButtonUp(nFlags, point);
}


// OnLButtonDown: Funció que es crida quan s'apreta el botó dret del mouse.
void CEntornV3DView::OnRButtonDown(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
// Entorn V3D: Detectem en quina posició s'ha apretat el botó esquerra del
//				mouse i ho guardem a la variable m_PosEAvall i activem flag m_ButoDAvall
	m_ButoDAvall = true;
	m_PosDAvall = point;

	CView::OnRButtonDown(nFlags, point);
}


// OnLButtonUp: Funció que es crida quan deixem d'apretar el botó dret del mouse.
void CEntornV3DView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
// Entorn V3D: Desactivem flag m_ButoEAvall quan deixem d'apretar botó esquerra del mouse.
	m_ButoDAvall = false;

//	ClientToScreen(&point);
//	OnContextMenu(this, point);

// Si fullscreen afegir menú per a restaurar Pantalla Normal en posició del cursor.
	if (fullscreen) {
		ClientToScreen(&point);
		if (ContextMenu->GetSubMenu(0)) {
			CMenu* pSubMenu = ContextMenu->GetSubMenu(0);

			if (fullscreen)	pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_CHECKED);
			else pSubMenu->CheckMenuItem(ID_VISTA_FULLSCREEN, MF_UNCHECKED);
			pSubMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x, point.y, this);
		}
	}
}


// OnMouseMove: Funció que es crida quan es mou el mouse. La utilitzem per la 
//				  Visualització Interactiva amb les tecles del mouse apretades per 
//				  modificar els paràmetres de P.V. (R,angleh,anglev) segons els 
//				  moviments del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el botó es apretat o no.
//					- point: Estructura (x,y) que dóna la posició del mouse 
//							 (coord. pantalla) quan el botó s'ha apretat.
void CEntornV3DView::OnMouseMove(UINT nFlags, CPoint point)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0.0, 0.0, 0.0 };

	if (m_ButoEAvall && mobil && projeccio != CAP)
	{	// Entorn V3D: Determinació dels angles (en graus) segons l'increment
		//				horitzontal i vertical de la posició del mouse per càmeres Esfèrica i Geode.
		CSize gir = m_PosEAvall - point;
		m_PosEAvall = point;
		if (camera == CAM_ESFERICA)
		{	// Càmera Esfèrica
			OPV.beta = OPV.beta - gir.cx / 2.0;
			OPV.alfa = OPV.alfa + gir.cy / 2.0;

			// Entorn V3D: Control per evitar el creixement desmesurat dels angles.
			while (OPV.alfa >= 360.0)	OPV.alfa = OPV.alfa - 360.0;
			while (OPV.alfa < 0.0)		OPV.alfa = OPV.alfa + 360.0;
			while (OPV.beta >= 360.0)	OPV.beta = OPV.beta - 360.0;
			while (OPV.beta < 0.0)		OPV.beta = OPV.beta + 360.0;
			}
		else { // Càmera Geode
			OPV_G.beta = OPV_G.beta + gir.cx / 2.0;
			OPV_G.alfa = OPV_G.alfa + gir.cy / 2.0;
			// Entorn V3D: Control per evitar el creixement desmesurat dels angles
			while (OPV_G.alfa >= 360.0)	OPV_G.alfa = OPV_G.alfa - 360.0;
			while (OPV_G.alfa < 0.0f)		OPV_G.alfa = OPV_G.alfa + 360.0;
			while (OPV_G.beta >= 360.0)		OPV_G.beta = OPV_G.beta - 360.0;
			while (OPV_G.beta < 0.0)		OPV_G.beta = OPV_G.beta + 360.0;
			}
		InvalidateRect(NULL, false);
	}
	else if (m_ButoEAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO)) // Opció Navegació
	{
		// Entorn V3D: Canviar orientació en opció de Navegació
		CSize girn = m_PosEAvall - point;
		angleZ = girn.cx / 2.0;
		// Entorn V3D: Control per evitar el creixement desmesurat dels angles.
		if (angleZ >= 360.0) angleZ = angleZ - 360.0;
		if (angleZ < 0.0)	angleZ = angleZ + 360.0;

		// Entorn V3D: Segons orientació dels eixos Polars (Vis_Polar)
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			n[0] = n[0] - opvN.x;
			n[1] = n[1] - opvN.y;
			n[0] = n[0] * cos(angleZ * PI / 180.0) - n[1] * sin(angleZ * PI / 180.0);
			n[1] = n[0] * sin(angleZ * PI / 180.0) + n[1] * cos(angleZ * PI / 180.0);
			n[0] = n[0] + opvN.x;
			n[1] = n[1] + opvN.y;
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			n[2] = n[2] - opvN.z;
			n[0] = n[0] - opvN.x;
			n[2] = n[2] * cos(angleZ * PI / 180.0) - n[0] * sin(angleZ * PI / 180.0);
			n[0] = n[2] * sin(angleZ * PI / 180.0) + n[0] * cos(angleZ * PI / 180.0);
			n[2] = n[2] + opvN.z;
			n[0] = n[0] + opvN.x;
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			n[1] = n[1] - opvN.y;
			n[2] = n[2] - opvN.z;
			n[1] = n[1] * cos(angleZ * PI / 180.0) - n[2] * sin(angleZ * PI / 180.0);
			n[2] = n[1] * sin(angleZ * PI / 180.0) + n[2] * cos(angleZ * PI / 180.0);
			n[1] = n[1] + opvN.y;
			n[2] = n[2] + opvN.z;
		}

		m_PosEAvall = point;
		InvalidateRect(NULL, false);
	}

// Entorn V3D: Transformació Geomètrica interactiva pels eixos X,Y boto esquerra del mouse.
	else {
		bool transE = transX || transY;
		if (m_ButoEAvall && transE && transf)
		{	// Calcular increment
			CSize girT = m_PosEAvall - point;
			if (transX)
			{	long int incrT = girT.cx;
				if (trasl)
				{	TG.VTras.x += incrT * fact_Tras;
					if (TG.VTras.x < -100000.0) TG.VTras.x = 100000.0;
					if (TG.VTras.x > 100000.0) TG.VTras.x = 100000.0;
				}
				else if (rota)
				{	TG.VRota.x += incrT * fact_Rota;
					while (TG.VRota.x >= 360.0) TG.VRota.x -= 360.0;
					while (TG.VRota.x < 0.0) TG.VRota.x += 360.0;
				}
				else if (escal)
				{	if (incrT < 0) incrT = -1 / incrT;
					TG.VScal.x = TG.VScal.x * incrT;
					if (TG.VScal.x < 0.25) TG.VScal.x = 0.25;
					if (TG.VScal.x > 8192.0) TG.VScal.x = 8192.0;
				}
			}
			if (transY)
			{	long int incrT = girT.cy;
				if (trasl)
				{	TG.VTras.y += incrT * fact_Tras;
					if (TG.VTras.y < -100000.0) TG.VTras.y = 100000.0;
					if (TG.VTras.y > 100000.0) TG.VTras.y = 100000.0;
				}
				else if (rota)
				{	TG.VRota.y += incrT * fact_Rota;
					while (TG.VRota.y >= 360.0) TG.VRota.y -= 360.0;
					while (TG.VRota.y < 0.0) TG.VRota.y += 360.0;
				}
				else if (escal)
				{	if (incrT <= 0.0) {
						if (incrT >= -2.0) incrT = -2.0;
						incrT = 1.0 / Log2(-incrT);
					}
					else incrT = Log2(incrT);
					TG.VScal.y = TG.VScal.y * incrT;
					if (TG.VScal.y < 0.25) TG.VScal.y = 0.25;
					if (TG.VScal.y > 8192.0) TG.VScal.y = 8192.0;
				}
			}
			m_PosEAvall = point;
			InvalidateRect(NULL, false);
		}
	}

	// Entorn V3D: Determinació del desplaçament del pan segons l'increment
	//				vertical de la posició del mouse (tecla dreta apretada).
	if (m_ButoDAvall && pan && (projeccio != CAP && projeccio != ORTO))
	{	CSize zoomincr = m_PosDAvall - point;
		long int incrx = zoomincr.cx;
		long int incry = zoomincr.cy;

		// Desplaçament pan vertical
		tr_cpv.y -= incry * fact_pan;
		if (tr_cpv.y > 100000.0) tr_cpv.y = 100000.0;
		else if (tr_cpv.y < -100000) tr_cpv.y = -100000.0;

		// Desplaçament pan horitzontal
		tr_cpv.x += incrx * fact_pan;
		if (tr_cpv.x > 100000.0) tr_cpv.x = 100000;
		else if (tr_cpv.x < -100000.0) tr_cpv.x = -100000;

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
	// Determinació del paràmetre R segons l'increment
	//   vertical de la posició del mouse (tecla dreta apretada)
		//else if (m_ButoDAvall && zzoom && (projeccio!=CAP && projeccio!=ORTO))
	else if (m_ButoDAvall && (zzoom || zzoomO) && (projeccio != CAP))
	{	CSize zoomincr = m_PosDAvall - point;
		long int incr = zoomincr.cy / 1.0;
		if (camera == CAM_ESFERICA) {	// Càmera Esfèrica
			OPV.R = OPV.R + incr;
			if (OPV.R < 0.25) OPV.R = 0.25;
		}
		else { // Càmera Geode
			OPV_G.R = OPV_G.R + incr;
			if (OPV_G.R < 0.0) OPV_G.R = 0.0;
		}
		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}
	else if (m_ButoDAvall && camera == CAM_NAVEGA && (projeccio != CAP && projeccio != ORTO))
	{	// Avançar en opció de Navegació
		if (m_PosDAvall != point)
		{	CSize zoomincr = m_PosDAvall - point;
			double incr = zoomincr.cy / 2;

			vdir[0] = n[0] - opvN.x;
			vdir[1] = n[1] - opvN.y;
			vdir[2] = n[2] - opvN.z;
			modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
			vdir[0] = vdir[0] / modul;
			vdir[1] = vdir[1] / modul;
			vdir[2] = vdir[2] / modul;

			// Entorn V3D: Segons orientació dels eixos Polars (Vis_Polar)
			if (Vis_Polar == POLARZ) { // (X,Y,Z)
				opvN.x += incr * vdir[0];
				opvN.y += incr * vdir[1];
				n[0] += incr * vdir[0];
				n[1] += incr * vdir[1];
			}
			else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
				opvN.z += incr * vdir[2];
				opvN.x += incr * vdir[0];
				n[2] += incr * vdir[2];
				n[0] += incr * vdir[0];
			}
			else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
				opvN.y += incr * vdir[1];
				opvN.z += incr * vdir[2];
				n[1] += incr * vdir[1];
				n[2] += incr * vdir[2];
			}

			m_PosDAvall = point;
			InvalidateRect(NULL, false);
		}
	}

// Entorn V3D: Transformació Geomètrica interactiva per l'eix Z amb boto dret del mouse.
	else if (m_ButoDAvall && transZ && transf)
	{	// Calcular increment
		CSize girT = m_PosDAvall - point;
		long int incrT = girT.cy;
		if (trasl)
		{	TG.VTras.z += incrT * fact_Tras;
			if (TG.VTras.z < -100000.0) TG.VTras.z = 100000.0;
			if (TG.VTras.z > 100000.0) TG.VTras.z = 100000.0;
		}
		else if (rota)
		{	incrT = girT.cx;
			TG.VRota.z += incrT * fact_Rota;
			while (TG.VRota.z >= 360.0) TG.VRota.z -= 360.0;
			while (TG.VRota.z < 0) TG.VRota.z += 360.0;
		}
		else if (escal)
		{	if (incrT <= 0.0) {
				if (incrT >= -2.0) incrT = -2.0;
				incrT = 1 / Log2(-incrT);
				}
			else incrT = Log2(incrT);
			TG.VScal.z = TG.VScal.z * incrT;
			if (TG.VScal.z < 0.25) TG.VScal.z = 0.25;
			if (TG.VScal.z > 8192.0) TG.VScal.z = 8192.0;
		}

		m_PosDAvall = point;
		InvalidateRect(NULL, false);
	}

	CView::OnMouseMove(nFlags, point);
}


// OnMouseWheel: Funció que es crida quan es mou el rodet del mouse. La utilitzem per la 
//				  Visualització Interactiva per modificar el paràmetre R de P.V. (R,angleh,anglev) 
//				  segons el moviment del rodet del mouse.
//      PARAMETRES: - nFlags: Flags que controlen si el botó es apretat o no.
//					- zDelta: Unitats de desplaçament del rodet del mouse.
//					- pt: Estructura (x,y) que dóna la posició del mouse 
//							 (coord. pantalla) quan el botó s'ha apretat.
BOOL CEntornV3DView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	double modul = 0;
	GLdouble vdir[3] = { 0.0, 0.0, 0.0 };

// Funció de zoom quan està activada la funció pan o les T. Geomètriques
	if ((zzoom || zzoomO) || (transX) || (transY) || (transZ))
	{	if (camera == CAM_ESFERICA) {	// Càmera Esfèrica
			OPV.R = OPV.R + zDelta / 4.0;
			if (OPV.R < 0.25) OPV.R = 0.25;
			InvalidateRect(NULL, false);
		}
		else if (camera == CAM_GEODE)
		{	// Càmera Geode
			OPV_G.R = OPV_G.R + zDelta / 4.0;
			if (OPV_G.R < 0.0) OPV_G.R = 0.0;
			InvalidateRect(NULL, false);
		}
	}
	else if (camera == CAM_NAVEGA)
	{	vdir[0] = n[0] - opvN.x;
		vdir[1] = n[1] - opvN.y;
		vdir[2] = n[2] - opvN.z;
		modul = sqrt(vdir[0] * vdir[0] + vdir[1] * vdir[1] + vdir[2] * vdir[2]);
		vdir[0] = vdir[0] / modul;
		vdir[1] = vdir[1] / modul;
		vdir[2] = vdir[2] / modul;

		// Entorn V3D: Segons orientació dels eixos Polars (Vis_Polar)
		if (Vis_Polar == POLARZ) { // (X,Y,Z)
			opvN.x += (zDelta / 4.0) * vdir[0];
			opvN.y += (zDelta / 4.0) * vdir[1];
			n[0] += (zDelta / 4.0) * vdir[0];
			n[1] += (zDelta / 4.0) * vdir[1];
		}
		else if (Vis_Polar == POLARY) { //(X,Y,Z) --> (Z,X,Y)
			opvN.z += (zDelta / 4.0) * vdir[2];
			opvN.x += (zDelta / 4.0) * vdir[0];
			n[2] += (zDelta / 4.0) * vdir[2];
			n[0] += (zDelta / 4.0) * vdir[0];
		}
		else if (Vis_Polar == POLARX) { //(X,Y,Z) --> (Y,Z,X)
			opvN.y += (zDelta / 4.0) * vdir[1];
			opvN.z += (zDelta / 4.0) * vdir[2];
			n[1] += (zDelta / 4.0) * vdir[1];
			n[2] += (zDelta / 4.0) * vdir[2];
		}
		InvalidateRect(NULL, false);
	}

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}


/* ------------------------------------------------------------------------- */
/*					     TIMER (ANIMACIÓ)									 */
/* ------------------------------------------------------------------------- */
void CEntornV3DView::OnTimer(UINT_PTR nIDEvent)
{
// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
// EntornV3D. QUATERNIONS: Animació Objecte Rígid per Quaternions
	if (satelit) {	// OPCIÓ SATÈLIT: Increment OPV segons moviments mouse.
		OPV.alfa = OPV.alfa + m_EsfeIncEAvall.alfa;
		while (OPV.alfa > 360) OPV.alfa = OPV.alfa - 360;	while (OPV.alfa < 0) OPV.alfa = OPV.alfa + 360;
		OPV.beta = OPV.beta + m_EsfeIncEAvall.beta;
		while (OPV.beta > 360) OPV.beta = OPV.beta - 360;	while (OPV.beta < 0) OPV.beta = OPV.beta + 360;

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
// EntornV3D. QUATERNIONS: Animació Objecte Rígid per Quaternions
	else if (animaQ) {	// CAL FER: Càlcul del nou quaternió per a l'orientació de l'objecte.

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
// EntornV3D. LOXODROMA: Animació de les càmeres associades als Triedres de Darboux i Frenet
	else if ((camera == CAM_LOXODROMA_TDARBOUX) && animaCamLxd)
	{//PASSOS:
		//1) Actualitzar paràmetre t de la corba (cam_t) i verificar límits per a que animació sigui sense fi.
		//2) Calcular posició de la càmera (opvN) segons corba Loxodroma amb paràmetre t (cam_t)
		//3) Calcular vectors n (nCam) i v (vCam) de la càmera segons el Triedre de Darboux.

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
	else if ((camera == CAM_LOXODROMA_TFRENET) && animaCamLxd)
	{//PASSOS:
		//1) Actualitzar paràmetre t de la corba (cam_t) i verificar límits per a que animació sigui sense fi.
		//2) Calcular posició de la càmera (opvN) segons corba Loxodroma amb paràmetre t (cam_t)
		//3) Calcular vectors n (nCam) i v (vCam) de la càmera segons el Triedre de Frenet.

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}
	// EntornV3D. PAISATGE: Animació de l'objecte segons la corba BSpline, Catmull-Rom, Bezier, etc.) 
	else if (objecte == PAISATGE) // && animaE)
	{//PASSOS:
		//1) Actualitzar paràmetre t de la corba (BSpline, Catmull-Rom, Bezier, etc.) i verificar límits del recorregut dels punts de control.
		//2) Calcular punt de la corba (BSpline, Catmull-Rom, Bezier, etc.) amb paràmetre t i punts de control
		//3) Calcular Triedre de Frenet (VT, VBN, VNP) en el punt de la corba.
		//4) Calcular les transformacions per a colocar objecte Tie a la posició i orientació de la corba, 
		//		per a passar-les com a paràmetre a la funció dibuixa_escenaGL() en escena.cpp

		// Crida a OnPaint() per redibuixar l'escena
		InvalidateRect(NULL, false);
	}

	CView::OnTimer(nIDEvent);
}

/* ------------------------------------------------------------------------- */
/*   RECURSOS DE MENU (persianes) DE L'APLICACIO:                            */
/*					1. ARXIUS												 */
/*					4. CÀMERA: Esfèrica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/*					5. VISTA: Pantalla Completa, Pan i Eixos	             */
/*					6. PROJECCIÓ                                             */
/*					7. OBJECTE					                             */
/*					8. TRANSFORMA											 */
/*					9. OCULTACIONS											 */
/*				   10. IL.LUMINACIÓ											 */
/*				   11. LLUMS												 */
/*				   12. SHADERS												 */
/* ------------------------------------------------------------------------- */

/* ------------------------------------------------------------------------- */
/*					1. ARXIUS 												 */
/* ------------------------------------------------------------------------- */

// Obrir fitxer Fractal
void CEntornV3DView::OnArxiuObrirFractal()
{
// TODO: Agregue aquí su código de controlador de comandos
// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.MNT)
	CFileDialog openMunt(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("MNT Files(*.mnt)|*.mnt|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openMunt.DoModal() != IDOK)
		return;                 // stay with old data file
	else {
		nom = openMunt.GetPathName();

		// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

		// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
		//		compatible amb  les funcions de càrrega de fitxers fractals
		char* nomfitx = CString2Char(nom);

		// Entorn V3D: Variable de tipus char *nomfitx conté el nom del fitxer seleccionat

	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

// OnArchivoObrirFitxerObj: Obrir fitxer en format gràfic OBJ
void CEntornV3DView::OnArxiuObrirFitxerOBJ()
{
// TODO: Agregue aquí su código de controlador de comandos
	//if (ObOBJ != NULL) delete ObOBJ;

// Entorn V3D: Obrir diàleg de lectura de fitxer
	CFileDialog openOBJ(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ Files(*.obj)|*.obj|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openOBJ.DoModal() != IDOK)	return;  // stay with old data file
	else {
		nom = openOBJ.GetPathName();

		objecte = OBJOBJ;	textura = true;

		// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

		// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
		//		compatible amb  les funcions de càrrega de fitxers fractals
		char* nomfitx = CString2Char(nom);

		// i carreguem
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL

		if (ObOBJ == NULL) ObOBJ = new COBJModel;
		ObOBJ->LoadModel(nomfitx, OBJECTEOBJ, false);	// Carregar objecte OBJ SENSE textura
		ObOBJ->LoadModel(nomfitx, OBJECTEOBJT, true);	// Carregar objecte OBJ AMB textura

		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnArxiuObrirFitxer3DS()
{
// TODO: Agregue aquí su código de controlador de comandos
	//if(Ob3DS!=NULL) delete Ob3DS;

// Entorn V3D: Obrir diàleg de lectura de fitxer
	CFileDialog open3DS(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("3DS Files(*.3ds)|*.3ds|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (open3DS.DoModal() != IDOK)	return;  // stay with old data file
	else {
		nom = open3DS.GetPathName();

		objecte = OBJ3DS;	textura = true;

		// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

		// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
		//		compatible amb  les funcions de càrrega de fitxers fractals
		char* nomfitx = CString2Char(nom);

		// i carreguem
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC); // Activem contexte OpenGL

		if (Ob3DS == NULL) Ob3DS = new Obj_3DS;
		Ob3DS->EliminarMemoria();
		Ob3DS->Inicialitzar();
		Ob3DS->Carregar3DS(nomfitx);

		// Precompilació de dos objectes nous: OBJECTE3DS: Objecte 3DS sense textures i OBJECTE3DST amb textures,
		Ob3DS->Dibuixa3DS(false, OBJECTE3DS, false);
		Ob3DS->Dibuixa3DS(false, OBJECTE3DST, true);

		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

		//  Entorn V3D: Modificar R per centrar Vista amb mida de l'objecte
		mida = sqrtf(3.0) * 10;
		OPV.R = 0.5 * mida / sin(30 * PI / 180) + p_near;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


/* ------------------------------------------------------------------------- */
/*					4. CÀMERA: Esfèrica (Mobil, Zoom, ZoomO, Satelit), Navega*/
/* ------------------------------------------------------------------------- */
// CÀMERA: Mode Esfèrica (Càmera esfèrica en polars-opció booleana)
void CEntornV3DView::OnCameraEsferica()
{
// TODO: Agregue aquí su código de controlador de comandos
	camera = CAM_ESFERICA;

// Inicialitzar paràmetres Càmera Esfèrica
	OPV.R = 15.0;		OPV.alfa = 0.0;		OPV.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;	zzoom = true;	zzoomO = false;		satelit = false;
	Vis_Polar = POLARZ;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraEsferica(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_ESFERICA) 	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

//CÀMERA--> ESFERICA: Mobil. Punt de Vista Interactiu (opció booleana)
void CEntornV3DView::OnCameraMobil()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP) && (camera == CAM_ESFERICA)) mobil = !mobil;
// Desactivació de Transformacions Geomètriques via mouse 
//		si Visualització Interactiva activada.	
	if (mobil) {
		transX = false;	transY = false; transZ = false;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraMobil(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA) && (mobil)) 	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> ESFERICA: Zoom. Zoom Interactiu (opció booleana)
void CEntornV3DView::OnCameraZoom()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_LOXODROMA_TDARBOUX)) zzoom = !zzoom;
// Desactivació de Transformacions Geomètriques via mouse 
//		si Zoom activat.
	if (zzoom) {	zzoomO = false;
					transX = false;	transY = false;	transZ = false;
				}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraZoom(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA) && (zzoom)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> ESFERICA: Satèlit. Vista interactiva i animada en que increment de movimen és activat per mouse (opció booleana)
void CEntornV3DView::OnCameraSatelit()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio == AXONOM || projeccio == PERSPECT) && (camera == CAM_ESFERICA)) satelit = !satelit;
	if (satelit) mobil = true;
	bool testA = animaCamLxd;							// Testejar si hi ha alguna animació activa apart de Satèlit.
	if ((!satelit) && (!testA)) KillTimer(WM_TIMER);	// Si es desactiva Satèlit i no hi ha cap animació activa es desactiva el Timer.

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraSatelit(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA) && (satelit)) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> ESFERICA|GEODE: Polars Eix X cap amunt per a Visualització Interactiva
void CEntornV3DView::OnCameraPolarsEixX()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) Vis_Polar = POLARX;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraPolarsEixX(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && (Vis_Polar == POLARX))  pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// CÀMERA--> ESFERICA|GEODE: Polars Eix Y cap amunt per a Visualització Interactiva
void CEntornV3DView::OnCameraPolarsEixY()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) Vis_Polar = POLARY;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraPolarsEixY(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && (Vis_Polar == POLARY))  pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> ESFERICA|GEODE: Polars Eix Z cap amunt per a Visualització Interactiva
void CEntornV3DView::OnCameraPolarsEixZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != CAP) && (camera == CAM_ESFERICA || camera == CAM_GEODE)) Vis_Polar = POLARZ;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraPolarsEixZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if ((camera == CAM_ESFERICA || camera == CAM_GEODE) && (Vis_Polar == POLARZ))  pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> GEODE:  Mode de navegació centrat a l'origent mirant un punt en coord. esfèriques (R,alfa,beta) (opció booleana)
void CEntornV3DView::OnCameraGeode()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != ORTO || projeccio != CAP) camera = CAM_GEODE;

// Inicialitzar paràmetres Càmera Geode
	OPV_G.R = 0.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;		zzoom = true;		zzoomO = false;		satelit = false;	pan = false;
	Vis_Polar = POLARZ;
	llumGL[5].encesa = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL
	glFrontFace(GL_CW);
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCameraGeode(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_GEODE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> GEODE. Tornar a lloc d'origen
void CEntornV3DView::OnCameraOrigenGeode()
{
// TODO: Agregue aquí su código de controlador de comandos
// Inicialitzar paràmetres Càmera Geode
	OPV_G.R = 0.0;	OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;	zzoom = true;		zzoomO = false;		 satelit = false;	pan = false;
	Vis_Polar = POLARZ;
	llumGL[5].encesa = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// CÀMERA--> NAVEGA. Navegació sobre un pla amb botons de teclat o de mouse (nav) (opció booleana)
void CEntornV3DView::OnCameraNavega()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio == PERSPECT || projeccio == AXONOM) camera = CAM_NAVEGA;

// Desactivació de zoom, mobil, Transformacions Geomètriques via mouse i pan 
//		si navega activat
	mobil = false;	zzoom = false;
	transX = false;	transY = false;	transZ = false;
	//pan = false;
	tr_cpv.x = 0.0;		tr_cpv.y = 0.0;		tr_cpv.z = 0.0;	// Inicialitzar a 0 desplaçament de pantalla
	tr_cpvF.x = 0.0;	tr_cpvF.y = 0.0;	tr_cpvF.x = 0.0; // Inicialitzar a 0 desplaçament de pantalla

// Incialitzar variables Navega
	n[0] = 0.0;		n[1] = 0.0;		n[2] = 0.0;
	opvN.x = 10.0;	opvN.y = 0.0;		opvN.z = 0.0;
	angleZ = 0.0;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornV3DView::OnUpdateCameraNavega(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (camera == CAM_NAVEGA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// CÀMERA--> NAVEGA. Tornar a lloc d'origen
void CEntornV3DView::OnCameraOrigenNavega()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (projeccio != ORTO || projeccio != CAP) camera = CAM_GEODE;

	// Inicialitzar paràmetres Càmera Geode
	OPV_G.R = 0.0;		OPV_G.alfa = 0.0;	OPV_G.beta = 0.0;				// Origen PV en esfèriques
	mobil = true;		zzoom = true;		zzoomO = false;		satelit = false;	pan = false;
	Vis_Polar = POLARZ;
	llumGL[5].encesa = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL
	glFrontFace(GL_CW);
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL); // Desactivem contexte OpenGL

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


/* -------------------------------------------------------------------------------- */
/*					5. VISTA: Pantalla Completa, Pan i Eixos	                    */
/* -------------------------------------------------------------------------------- */
/*
// VISTA: FullScreen (Pantalla Completa-opció booleana)
void CEntornV3DView::OnVistaFullscreen()
{
	// TODO: Agregue aquí su código de controlador de comandos

	if (!fullscreen)
	{	// I note that I go to full-screen mode
		fullscreen = true;
		// Remembers the address of the window in which the view was placed (probably a frame)
		saveParent = this->GetParent();
		// Assigns a view to a new parent - desktop
		this->SetParent(GetDesktopWindow());
		CRect rect; // It's about the dimensions of the desktop-desktop
		GetDesktopWindow()->GetWindowRect(&rect);
		// I set the window on the desktop
		MoveWindow(rect);
	}
	else {	// Switching off the full-screen mode
		fullscreen = false;
		// Assigns an old parent view
		this->SetParent(saveParent);
		CRect rect; // It's about the dimensions of the desktop-desktop
		// Get client screen dimensions
		saveParent->GetClientRect(&rect);
		// Changes the position and dimensions of the specified window.
		MoveWindow(rect, FALSE);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}
*/

// VISTA: FullScreen (Pantalla Completa-opció booleana)
void CEntornV3DView::OnVistaFullScreen()
{
// TODO: Agregue aquí su código de controlador de comandos

	if (!fullscreen)
	{	// I note that I go to full-screen mode
		fullscreen = true;
		// Remembers the address of the window in which the view was placed (probably a frame)
		saveParent = this->GetParent();
		// Assigns a view to a new parent - desktop
		this->SetParent(GetDesktopWindow());
		CRect rect; // It's about the dimensions of the desktop-desktop
		GetDesktopWindow()->GetWindowRect(&rect);
		// I set the window on the desktop
		MoveWindow(rect);
	}
	else {	// Switching off the full-screen mode
		fullscreen = false;
		// Assigns an old parent view
		this->SetParent(saveParent);
		CRect rect; // It's about the dimensions of the desktop-desktop
		// Get client screen dimensions
		saveParent->GetClientRect(&rect);
		// Changes the position and dimensions of the specified window.
		MoveWindow(rect, FALSE);
		}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaFullScreen(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (fullscreen) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA -> PAN?: Mode de Desplaçament horitzontal i vertical per pantalla del Punt de Vista (pan) (opció booleana)
void CEntornV3DView::OnVistaPan()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((projeccio != ORTO) || (projeccio != CAP)) pan = !pan;
// Desactivació de Transformacions Geomètriques via mouse i navega si pan activat
	if (pan) {
		mobil = true;		zzoom = true;
		transX = false;	transY = false;	transZ = false;
		//navega = false;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaPan(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (pan) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA -> PAN?: Tornar a lloc d'origen
void CEntornV3DView::OnVistaOrigenPan()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (pan) {	fact_pan = 1;
				tr_cpv.x = 0;	tr_cpv.y = 0;	tr_cpv.z = 0;
			}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// VISTA -> EIXOS?: Visualitzar eixos coordenades món (opció booleana)
void CEntornV3DView::OnVistaEixos()
{
// TODO: Agregue aquí su código de controlador de comandos
	eixos = !eixos;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaEixos(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (eixos) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// VISTA -> GRID XY?: Grid d'una unitat sobre pla XY (opció booleana)
void CEntornV3DView::OnVistaGridXY()
{
// TODO: Agregue aquí su código de controlador de comandos
	grid.x = !grid.x;	hgrid.x = 0.0;
	if (grid.x) grid.w = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaGridXY(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (grid.x) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA -> GRID XZ?: Grid d'una unitat sobre pla XZ (opció booleana)
void CEntornV3DView::OnVistaGridXZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	grid.y = !grid.y;	hgrid.y = 0.0;
	if (grid.y) grid.w = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaGridXZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (grid.y) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// VISTA -> GRID YZ?: Grid d'una unitat sobre pla YZ (opció booleana)
void CEntornV3DView::OnVistaGridYZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	grid.z = !grid.z;	hgrid.z = 0.0;
	if (grid.z) grid.w = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaGridYZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (grid.z) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// VISTA -> GRID XYZ?: Grid d'una unitat sobre pla XYZ (opció booleana)
void CEntornV3DView::OnVistaGridXYZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	grid.w = !grid.w;	hgrid.w = 0.0;
	if (grid.w)	{	grid.x = false;	grid.y = false;		grid.z = false;
				}
// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateVistaGridXYZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (grid.w) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					6. PROJECCIÓ                                             */
/* ------------------------------------------------------------------------- */
// PROJECCIÓ: Perspectiva
void CEntornV3DView::OnProjeccioPerspectiva()
{
// TODO: Agregue aquí su código de controlador de comandos
	projeccio = PERSPECT;
	mobil = true;			zzoom = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateProjeccioPerspectiva(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == PERSPECT) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}
void CEntornV3DView::OnProjeccioOrtografica()
{
	// TODO: Agregue aquí su código de controlador de comandos
	projeccio = ORTO;
	eixos = true;
	mobil = false;			zzoom = false;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateProjeccioOrtografica(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == ORTO) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

/* ------------------------------------------------------------------------- */
/*					7. OBJECTE					                             */
/* ------------------------------------------------------------------------- */

// OBJECTE: Cap objecte
void CEntornV3DView::OnObjecteCap()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = CAP;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCap(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CAP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE: Cub
void CEntornV3DView::OnObjecteCub()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = CUB;

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)


// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCub(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CUB) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE: Cub RGB
void CEntornV3DView::OnObjecteCubRGB()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = CUB_RGB;

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCubRGB(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == CUB_RGB) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Esfera
void CEntornV3DView::OnObjecteEsfera()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = ESFERA;

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteEsfera(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == ESFERA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Tetera
void CEntornV3DView::OnObjecteTetera()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = TETERA;

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteTetera(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == TETERA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Tie
void CEntornV3DView::OnObjecteTie()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = TIE;

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteTie(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == TIE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Truck
void CEntornV3DView::OnObjecteTruck()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = TRUCK;		textura = true;

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL
// Inicialitza vector imatges textura pel Truck.
	texturesID[1] = loadIMA_ILUT("./textures/vent.bmp");

	texturesID[2] = loadIMA_ILUT("./textures/plat.bmp");

	texturesID[3] = loadIMA_ILUT("./textures/reixeta.bmp");

	texturesID[4] = loadIMA_ILUT("./textures/fars.bmp");

	texturesID[5] = loadIMA_ILUT("./textures/txapa.bmp");

	texturesID[6] = loadIMA_ILUT("./textures/metall.bmp");
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL

//  Modificar R per centrar la Vista a la mida de l'objecte (Perspectiva)
//	Canviar l'escala per a centrar la vista (Ortogràfica)

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteTruck(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == TRUCK) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ----------------- OBJECTES CORBES BEZIER, LEMNISCATA i B-SPLINE
// OBJECTE Corba Bezier
void CEntornV3DView::OnObjecteCorbaBezier()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_BEZIER;

// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	else nom = openSpline.GetPathName();

// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

// 	---- Entorn V3D: Modificar R per centrar Vista amb mida de l'objecte

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCorbaBezier(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_BEZIER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba Lemniscata 3D
void CEntornV3DView::OnObjecteCorbaLemniscata()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = C_LEMNISCATA;

// 	---- Entorn V3D: Modificar R per centrar Vista amb mida de l'objecte

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCorbaLemniscata(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_LEMNISCATA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE Corba B-Spline
void CEntornV3DView::OnObjecteCorbaBSpline()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_BSPLINE;

// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openSpline(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openSpline.DoModal() != IDOK)
		return;                 // stay with old data file
	// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat
else nom = openSpline.GetPathName();

	// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

// 	---- Entorn V3D: Modificar R per centrar Vista amb mida de l'objecte

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteCorbaBSpline(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_BSPLINE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

void CEntornV3DView::OnObjecteCorbaCatmullRom()
{
// TODO: Agregue aquí su código de controlador de comandos
	nom = "";
	objecte = C_CATMULLROM;

// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.crv)
	CFileDialog openCatmullR(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("SPL Files(*.crv)|*.crv|All Files (*.*)|*.*||"));;

	if (openCatmullR.DoModal() != IDOK)
		return;                 // stay with old data file
// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat
	else nom = openCatmullR.GetPathName();

// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

	npts_T = llegir_ptsC(nomfitx);

// 	---- Entorn V3D: Modificar R per centrar Vista amb mida de l'objecte

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// OBJECTE Corba Catmull-Rom
void CEntornV3DView::OnUpdateObjecteCorbaCatmullRom(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == C_CATMULLROM) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// OBJECTE --> CORBES: Activar o desactivar visualització Punts de control de les corbes Bezier i BSpline
void CEntornV3DView::OnCorbesPuntsControl()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_Punts_Control = !sw_Punts_Control;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCorbesPuntsControl(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_Punts_Control) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE --> CORBES: Activar o desactivar visualització Triedre de Frenet Punts de control de les corbes 
//					Lemniscata, Loxodroma, Bezier i BSpline
void CEntornV3DView::OnCorbesTriedreFrenet()
{
// TODO: Agregue aquí su código de controlador de comandos
	dibuixa_TriedreFrenet = !dibuixa_TriedreFrenet;
	if (dibuixa_TriedreFrenet) dibuixa_TriedreDarboux = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateCorbesTriedreFrenet(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (dibuixa_TriedreFrenet) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

//----------------------- OBJECTES ADDICIONALS

// OBJECTE Escena Loxodroma on es dibuixa un objecte OBJ al centre, una esfera i la corba Loxodroma
void CEntornV3DView::OnObjecteEscenaLoxodroma()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = ESCENA_LOXODROMA;

	ilumina = SUAU;		oculta = true;		textura = true;

// Entorn V3D: Obrir diàleg de lectura de fitxer
	CFileDialog openOBJ(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("OBJ Files(*.obj)|*.obj|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openOBJ.DoModal() != IDOK)	return;  // stay with old data file
	else nom = openOBJ.GetPathName();
// Entorn V3D: Variable de tipus CString 'nom' conté el nom del fitxer seleccionat

// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
	char* nomfitx = CString2Char(nom);

// i carreguem
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Activem contexte OpenGL

	if (ObOBJ == NULL) ObOBJ = new COBJModel;
	ObOBJ->LoadModel(nomfitx, OBJECTEOBJ, false);	// Carregar objecte OBJ SENSE textura
	ObOBJ->LoadModel(nomfitx, OBJECTEOBJT, true);	// Carregar objecte OBJ AMB textura

	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);	// Desactivem contexte OpenGL

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteEscenaLoxodroma(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == ESCENA_LOXODROMA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OBJECTE -> PAISATGE: Escena amb un pla i la nau Tie per a animació per Corbes (Bezier, BSpline, Catmull-Rom, Loxodroma)
void CEntornV3DView::OnObjectePaisatge()
{
// TODO: Agregue aquí su código de controlador de comandos
	objecte = PAISATGE;

	ilumina = SUAU;		oculta = true;		textura = true;

// Entorn V3D: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Càrrega de la textura del núvol.
	texturesID[7] = loadIMA_SOIL("./textures/clouds2.jpg.");

//	Pas de textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_program, "texture0"), GLint(0));

// Desactivació contexte OpenGL: Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// V3D.CAL FER: Definir el pas del desplaçament de la trajectòria, depenent del tipus de corba que s'implementi
	// pas_CS = PAS_BSPLINE;	pas_CS = PAS_CATMULL_ROM;

// V3D.CAL FER: Calcular la primera posició del BSpline (punts, vectors, Triedre) i calcular les matrius 
// de transformació per a colocar l'objecte Tie en la primera posició del BSpline. Cal passar les variables 
// (o matrius) d'aquestes transformacions per paràmetres a la funció dibuixa_EscenaGL().


// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjectePaisatge(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == PAISATGE) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

void CEntornV3DView::OnObjecteRespall()
{
	// TODO: Agregue aquí su código de controlador de comandos
	objecte = RESPALL;
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateObjecteRespall(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (objecte == RESPALL) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					8. TRANSFORMA											 */
/* ------------------------------------------------------------------------- */

// TRANSFORMA -> TRASLACIÓ: (opció booleana) --> Activar o desactivar la traslació de l'objecte visualitzat
void CEntornV3DView::OnTransformaTraslacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	trasl = !trasl;
	rota = false;
	if (trasl) escal = true;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTransformaTraslacio(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (trasl) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFORMA -> TRASLACIÓ. Tornar a lloc d'origen
void CEntornV3DView::OnTransformaOrigenTraslacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (trasl)
	{	fact_Tras = 1;
		TG.VTras.x = 0.0;	TG.VTras.y = 0.0;	TG.VTras.z = 0;
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA -> ROTACIÓ (opció booleana) --> Activar o desactivar la rotació de l'objecte visualitzat
void CEntornV3DView::OnTransformaRotacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	rota = !rota;
	trasl = false;
	if (rota) escal = true;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTransformaRotacio(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (rota) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFORMA -> ROTACIÓ. Tornar a l'orientació d'origen
void CEntornV3DView::OnTransformaOrigenRotacio()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (rota)
	{	fact_Rota = 90;
		TG.VRota.x = 0;		TG.VRota.y = 0;		TG.VRota.z = 0;
		qT.w = 1.0;			qT.x = 1.0;			qT.y = 0.0;		qT.z = 0.0;	// Inicialitzar Quaternió i matriu.
		QuatToMatrix(qT, QMatrix);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFORMA -> ESCALAT. (opció booleana) --> Activar o desactivar l'escala de l'objecte visualitzat
void CEntornV3DView::OnTransformaEscalat()
{
// TODO: Agregue aquí su código de controlador de comandos
	if ((!rota) && (!trasl)) escal = !escal;
	transf = trasl || rota || escal;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTransformaEscalat(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (escal) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFORMA -> ESCALAT. Tornar a l'escala d'origen
void CEntornV3DView::OnTransformaOrigenEscalat()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (escal) { TG.VScal.x = 1;	TG.VScal.y = 1;	TG.VScal.z = 1; }

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnTransformaMobilEixX()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transX = !transX;
		if (transX) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transY) && (!transZ)) {
			mobil = true;
			zzoom = true;
		}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


// TRANSFOMA: Mòbil Eix X? (opció booleana).
void CEntornV3DView::OnUpdateTransformaMobilEixX(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transX) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFOMA: Mòbil Eix Y? (opció booleana).
void CEntornV3DView::OnTransformaMobilEixY()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transY = !transY;
		if (transY) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transX) && (!transZ)) {
			mobil = true;
			zzoom = true;
		}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTransformaMobilEixY(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transY) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// TRANSFOMA: Mòbil Eix Z? (opció booleana).
void CEntornV3DView::OnTransformaMobilEixZ()
{
// TODO: Agregue aquí su código de controlador de comandos
	if (transf)
	{	transZ = !transZ;
		if (transZ) {
			mobil = false;	zzoom = false;
			pan = false;	//navega = false;
		}
		else if ((!transX) && (!transY)) {
			mobil = true;
			zzoom = true;
		}
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTransformaMobilEixZ(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (transZ) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					9. OCULTACIONS											 */
/* ------------------------------------------------------------------------- */

// OCULTACIONS -> FRONT FACES (opció booleana) --> Activar o desactivar visualització de les cares frontals de l'objecte visualitzat
void CEntornV3DView::OnOcultacionsFrontFaces()
{
// TODO: Agregue aquí su código de controlador de comandos
	front_faces = !front_faces;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateOcultacionsFrontFaces(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (front_faces) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OCULTACIONS -> TEST VISIBILITAT (opció booleana) --> Implementació algorisme back-face culling, especialment en filferros.
void CEntornV3DView::OnOcultacionsTestVis()
{
// TODO: Agregue aquí su código de controlador de comandos
	test_vis = !test_vis;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateOcultacionsTestVis(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (test_vis) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// OCULTACIONS -> Z-BUFFER (opció booleana) --> Implementació algorisme Z-Buffer, especialment en iluminació Plana o Gouraud.
void CEntornV3DView::OnOcultacionsZBuffer()
{
// TODO: Agregue aquí su código de controlador de comandos
	oculta = !oculta;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateOcultacionsZBuffer(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (oculta) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					10. IL.LUMINACIÓ										 */
/* ------------------------------------------------------------------------- */

// IL.LUMINACIÓ Font de llum fixe? (opció booleana).
void CEntornV3DView::OnIluminacioLlumFixe()
{
// TODO: Agregue aquí su código de controlador de comandos
	ifixe = !ifixe;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioLlumFixe(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ifixe) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// IL.LUMINACIÓ: Mantenir iluminades les Cares Front i Back
void CEntornV3DView::OnIluminacioBothSides()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilum2sides = !ilum2sides;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioBothSides(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilum2sides) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ PUNTS (opció booleana) --> Activar o desactivar iluminació dels punts  de l'escena.
void CEntornV3DView::OnIluminacioPunts()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = PUNTS;
	test_vis = false;		oculta = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioPunts(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == PUNTS) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ FILFERROS (opció booleana) --> Activar o desactivar iluminació de filferros (arestes) de l'escena.
void CEntornV3DView::OnIluminacioFilferros()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = FILFERROS;
	test_vis = false;		oculta = false;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioFilferros(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == FILFERROS) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ PLANA (opció booleana) --> Activar o desactivar visualització d'iluminació plana de l'escena.
void CEntornV3DView::OnIluminacioPlana()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = PLANA;
	test_vis = false;		oculta = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioPlana(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == PLANA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ SUAU (opció booleana) --> Activar o desactivar visualització d'iluminació suau de l'escena.
void CEntornV3DView::OnIluminacioSuau()
{
// TODO: Agregue aquí su código de controlador de comandos
	ilumina = SUAU;
	test_vis = false;		oculta = true;

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioSuau(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (ilumina == SUAU) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL EMISSIÓ: Activació i desactivació de la reflectivitat pròpia del material.
void CEntornV3DView::OnMaterialEmissio()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[0] = !sw_material[0];

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[0]"), sw_material[0]);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateMaterialEmissio(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[0])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL AMBIENT: Activació i desactivació de la reflectivitat ambient del material.
void CEntornV3DView::OnMaterialAmbient()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[1] = !sw_material[1];

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[1]"), sw_material[1]);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateMaterialAmbient(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[1])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL DIFUSA: Activació i desactivació de la reflectivitat difusa del materials.
void CEntornV3DView::OnMaterialDifusa()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[2] = !sw_material[2];

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[2]"), sw_material[2]);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateMaterialDifusa(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[2])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ->REFLECTIVITAT MATERIAL ESPECULAR: Activació i desactivació de la reflectivitat especular del material.
void CEntornV3DView::OnMaterialEspecular()
{
// TODO: Agregue aquí su código de controlador de comandos
	sw_material[3] = !sw_material[3];

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[3]"), sw_material[3]);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateMaterialEspecular(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (sw_material[3])	pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ: Textures?: Activació (TRUE) o desactivació (FALSE) de textures.
void CEntornV3DView::OnIluminacioTextures()
{
// TODO: Agregue aquí su código de controlador de comandos
	textura = !textura;

// Entorn V3D: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

//	Pas de textura al shader
	if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_program, "texture"), textura);

// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateIluminacioTextures(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (textura) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// ILUMINACIÓ -> FITXER IMATGE: Llegir una imatge JPG, BMP, com a textura.
void CEntornV3DView::OnTexturaFitxerImatge()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString nomf;
	t_textura = FITXERIMA;		textura = true;	// Activar la textura.

// Obrir diàleg de lectura de fitxer
	CFileDialog openTextur(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("JPG Files(*.jpg)|*.jpg|Error Files (*.err)|*err|All Files (*.*)|*.*||"));;

	if (openTextur.DoModal() != IDOK)	return;                 // stay with old data file
	else nomf = openTextur.GetPathName();

// Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers textura
	char* nomfitx = CString2Char(nomf);

// Entorn V3D: Activació el contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	texturesID[0] = loadIMA_SOIL(nomfitx);

//	Pas de textura al shader
if (shader_menu != CAP_SHADER) glUniform1i(glGetUniformLocation(shader_program, "texture0"), GLint(0));

// Desactivació contexte OpenGL: Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateTexturaFitxerImatge(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (t_textura == FITXERIMA) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					11. LLUMS												 */
/* ------------------------------------------------------------------------- */

// LLUMS: Activació / Desactivació llum ambient 
void CEntornV3DView::OnLlumsLlumAmbient()
{
// TODO: Agregue aquí su código de controlador de comandos
	llum_ambient = !llum_ambient;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[1]"), (llum_ambient && sw_material[1]));
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlumAmbient(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llum_ambient) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 0 (GL_LIGHT0)
void CEntornV3DView::OnLlumsLlum0()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[0].encesa = !llumGL[0].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[0]"), llumGL[0].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum0(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[0].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 1 (GL_LIGHT1)
void CEntornV3DView::OnLlumsLlum1()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[1].encesa = !llumGL[1].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[1]"), llumGL[1].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum1(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[1].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 2 (GL_LIGHT2)
void CEntornV3DView::OnLlumsLlum2()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[2].encesa = !llumGL[2].encesa;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[2]"), llumGL[2].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum2(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[2].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 3 (GL_LIGHT3)
void CEntornV3DView::OnLlumsLlum3()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[3].encesa = !llumGL[3].encesa;
	//	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[3]"), llumGL[3].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum3(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[3].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// LLUMS-->ON/OFF: Activació /Desactivació llum 4 (GL_LIGHT4)
void CEntornV3DView::OnLlumsLlum4()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[4].encesa = !llumGL[4].encesa;
	//	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[4]"), llumGL[4].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum4(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[4].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

// LLUMS-->ON/OFF: Activació /Desactivació llum 5 (GL_LIGHT5)
void CEntornV3DView::OnLlumsLlum5()
{
// TODO: Agregue aquí su código de controlador de comandos
	llumGL[5].encesa = !llumGL[5].encesa;
	//	sw_il = true;

	if (shader_menu != CAP_SHADER) {
		// Entorn V3D: Activació del contexte OpenGL
		wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);
		// Pas màscara llums
		glUniform1i(glGetUniformLocation(shader_program, "sw_lights[5]"), llumGL[5].encesa);
		// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
		wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);
	}

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateLlumsLlum5(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (llumGL[5].encesa) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*					12. SHADERS												 */
/* ------------------------------------------------------------------------- */

// SHADERS: Desactivació de tots els shaders
void CEntornV3DView::OnShadersSense()
{
// TODO: Agregue aquí su código de controlador de comandos
	shader_menu = CAP_SHADER;
	releaseAllShaders();

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateShadersSense(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader_menu == CAP) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: Iluminació Gouraud amb Shaders
void CEntornV3DView::OnShaderGouraud()
{
// TODO: Agregue aquí su código de controlador de comandos
	bool apagat = false;
	if (sw_shader) releaseAllShaders();
	else sw_shader = true;
	shader_menu = GOURAUD_SHADER;

// Entorn V3D: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Carregar shaders gouraud_shdrML.vert i gouraud_shdrML.frag en Program shader_program
	shader_program = initializeShaders(_T("./shaders/gouraud_shdrML"));

	glUniform1i(glGetUniformLocation(shader_program, "texture"), textura);
	glUniform1i(glGetUniformLocation(shader_program, "texture0"), GLint(0));
// Pas màscara reflectivitat
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[0]"), sw_material[0]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[1]"), sw_material[1]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[2]"), sw_material[2]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[3]"), sw_material[3]);
//Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[0]"), llumGL[0].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[1]"), llumGL[1].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[2]"), llumGL[2].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[3]"), llumGL[3].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[4]"), llumGL[4].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[5]"), llumGL[5].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[6]"), llumGL[6].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[7]"), llumGL[7].encesa);

// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateShaderGouraud(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader_menu == GOURAUD_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


// SHADERS: Iluminació Phong amb Shaders
void CEntornV3DView::OnShaderPhong()
{
// TODO: Agregue aquí su código de controlador de comandos
	bool apagat = false;
	if (sw_shader) releaseAllShaders();		// Eliminar Programs previs
	else sw_shader = true;
	shader_menu = PHONG_SHADER;

// Entorn V3D: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

// Carregar shaders phong_shdrML.vert i phong_shdrML.frag en Program shader_program
	shader_program = initializeShaders(_T("./shaders/phong_shdrML"));

// Linkar uniform variables amb variables C++
	glUniform1i(glGetUniformLocation(shader_program, "texture"), textura);
	glUniform1i(glGetUniformLocation(shader_program, "texture0"), GLint(0));
// Pas màscara reflectivitat
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[0]"), sw_material[0]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[1]"), sw_material[1]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[2]"), sw_material[2]);
	glUniform1i(glGetUniformLocation(shader_program, "sw_intensity[3]"), sw_material[3]);
//Pas màscara llums
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[0]"), llumGL[0].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[1]"), llumGL[1].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[2]"), llumGL[2].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[3]"), llumGL[3].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[4]"), llumGL[4].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[5]"), llumGL[5].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[6]"), llumGL[6].encesa);
	glUniform1i(glGetUniformLocation(shader_program, "sw_lights[7]"), llumGL[7].encesa);

// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateShaderPhong(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader_menu == PHONG_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}

void CEntornV3DView::OnShaderLoadFiles()
{
// TODO: Agregue aquí su código de controlador de comandos
	CString nomVert, nomFrag;

	if (sw_shader) releaseAllShaders();
	else sw_shader = true;
	shader_menu = FILE_SHADER;

// Càrrega fitxer VERT
// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openVert(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("VERT Files(*.vert)|*.vert|All Files (*.*)|*.*||"));;

	if (openVert.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomVert = openVert.GetPathName();
// Entorn V3D: Variable de tipus CString 'nomVert' conté el nom del fitxer seleccionat

// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
//	char *nomfitxV = CString2Char(nomVert);

// Càrrega fitxer FRAG
// Entorn V3D: Obrir diàleg de lectura de fitxer (fitxers (*.VERT)
	CFileDialog openFrag(TRUE, NULL, NULL,
		OFN_FILEMUSTEXIST | OFN_HIDEREADONLY,
		_T("FRAG Files(*.frag)|*.frag|All Files (*.*)|*.*||"));;

	if (openFrag.DoModal() != IDOK)
		return;                 // stay with old data file
	else nomFrag = openFrag.GetPathName();
// Entorn V3D: Variable de tipus CString 'nomFrag' conté el nom del fitxer seleccionat

// Entorn V3D: Conversió de la variable CString nom a la variable char *nomfitx, 
//		compatible amb  les funcions de càrrega de fitxers fractals
//	char *nomfitxF = CString2Char(nomFrag);

// Entorn V3D: Activació del contexte OpenGL
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC);

	shader_program = loadFileShaders(nomVert, nomFrag);

//	shader_program = initializeShaders(_T("./shaders/gouraud_shdrML"));

	glUniform1i(glGetUniformLocation(shader_program, "texture"), textura);
	glUniform1i(glGetUniformLocation(shader_program, "texture0"), GLint(0));

// Entorn V3D: Desactivació del contexte OpenGL. Permet la coexistencia d'altres contextes de generació
	wglMakeCurrent(m_pDC->GetSafeHdc(), NULL);

// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}

void CEntornV3DView::OnUpdateShaderLoadFiles(CCmdUI* pCmdUI)
{
// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (shader_menu == FILE_SHADER) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}


/* ------------------------------------------------------------------------- */
/*   RECURSOS DE BOTONS (ToolBar) DE L'APLICACIO                             */
/* ------------------------------------------------------------------------- */
// Incloure funcions de tractament de botons


// ---------------- Entorn V3D: Funcions locals a EntornV3DView.cpp

// Log2: Càlcul del log base 2 de num
int CEntornV3DView::Log2(int num)
{
	int tlog;

	if (num >= 16384) tlog = 14;
	else if (num >= 8192) tlog = 13;
	else if (num >= 4096) tlog = 12;
	else if (num >= 2048) tlog = 11;
	else if (num >= 1024) tlog = 10;
	else if (num >= 512) tlog = 9;
	else if (num >= 256) tlog = 8;
	else if (num >= 128) tlog = 7;
	else if (num >= 64) tlog = 6;
	else if (num >= 32) tlog = 5;
	else if (num >= 16) tlog = 4;
	else if (num >= 8) tlog = 3;
	else if (num >= 4) tlog = 2;
	else if (num >= 2) tlog = 1;
	else tlog = 0;

	return tlog;
}


// CString2char: Funció de conversió de variable CString a char * per a noms de fitxers 
char* CEntornV3DView::CString2Char(CString entrada)
{
	//char * par_sortida=" ";
	// Variable de tipus CString 'entrada' conté un string tipus CString
	//-------------------------------------------------------------------------------------
	// Conversió de la variable CString entrada a la variable char *sortida, 
	//	compatible amb la funció Carregar3DS, de càrrega de fitxers 3DS
	//	char * nomfitx = (char *)(LPCTSTR)nom;

	// Conversió variable w_char --> char *
	//	char *nomf="  ";
	//	wcstombs(nomf,strFileName.GetBuffer(3),90);
	//	char *nomf = reinterpret_cast<char *> (nom.GetBuffer(3));

	size_t origsize = wcslen(entrada.GetBuffer(3)) + 1;
	size_t convertedChars = 0;

	// Use a multibyte string to append the type of string
	// to the new string before displaying the result.
	char strConcat[] = " (char *)";
	size_t strConcatsize = (strlen(strConcat) + 1) * 2;

	// Allocate two bytes in the multibyte output string for every wide
	// character in the input string (including a wide character
	// null). Because a multibyte character can be one or two bytes,
	// you should allot two bytes for each character. Having extra
	// space for the new string is not an error, but having
	// insufficient space is a potential security problem.
	const size_t newsize = origsize * 2;
	// The new string will contain a converted copy of the original
	// string plus the type of string appended to it.
	//	char *nomfitx = new char[newsize + strConcatsize];
	char* par_sortida = new char[newsize + strConcatsize];

	// Put a copy of the converted string into nstring
	wcstombs_s(&convertedChars, par_sortida, newsize, entrada.GetBuffer(3), _TRUNCATE);
	// append the type of string to the new string.
	//----------------------------------------------------------------------------------

	// Variable de tipus char *nomfitx conté el nom del fitxer seleccionat
	return par_sortida;
}


// Refl_MaterialOff: Desactivar Reflexió de Material
void CEntornV3DView::Refl_MaterialOff()
{
	sw_material_old[0] = sw_material[0];	sw_material[0] = false;
	sw_material_old[1] = sw_material[1];	sw_material[1] = false;
	sw_material_old[2] = sw_material[2];	sw_material[2] = false;
	sw_material_old[3] = sw_material[3];	sw_material[3] = false;
}

// Refl_MaterialOn: Activar Reflexió de Material
void CEntornV3DView::Refl_MaterialOn()
{
	sw_material[0] = sw_material_old[0];
	sw_material[1] = sw_material_old[1];
	sw_material[2] = sw_material_old[2];
	sw_material[3] = sw_material_old[3];
}


// -------------------- FUNCIONS CORBES SPLINE i BEZIER

// llegir_ptsC: Llegir punts de control de corba (spline o Bezier) d'un fitxer .crv. 
//				Retorna el nombre de punts llegits en el fitxer.
//int llegir_pts(CString nomf)
int CEntornV3DView::llegir_ptsC(char* nomf)
{
	int i, j;
	FILE* fd;

	// Inicialitzar vector punts de control de la corba spline
	for (i = 0; i < MAX_PATCH_CORBA; i = i++) {
		PC_t[i].x = 0.0;
		PC_t[i].y = 0.0;
		PC_t[i].z = 0.0;
	}

	//	ifstream f("altinicials.dat",ios::in);
	//    f>>i; f>>j;
	if ((fd = fopen(nomf, "rt")) == NULL) {
		LPCWSTR texte1 = reinterpret_cast<LPCWSTR>("ERROR:");
		LPCWSTR texte2 = reinterpret_cast<LPCWSTR>("File .crv was not opened");
		//MessageBox(NULL, texte1, texte2, MB_OK);
		MessageBox(texte1, texte2, MB_OK);
		//AfxMessageBox("file was not opened");
		return false;
	}
	fscanf(fd, "%d \n", &i);
	if (i == 0) return false;
	else {
		for (j = 0; j < i; j = j++) {
			fscanf(fd, "%lf %lf %lf \n", &PC_t[j].x, &PC_t[j].y, &PC_t[j].z);
		}
	}
	fclose(fd);

	return i;
}

void CEntornV3DView::OnProjeccioAxonometrica()
{
	// TODO: Agregue aquí su código de controlador de comandos
	projeccio = AXONOM;
	eixos = true;
	mobil = true;			zzoom = true;

	// Crida a OnPaint() per redibuixar l'escena
	InvalidateRect(NULL, false);
}


void CEntornV3DView::OnUpdateProjeccioAxonometrica(CCmdUI* pCmdUI)
{
	// TODO: Agregue aquí su código de controlador de IU para actualización de comandos
	if (projeccio == ORTO) pCmdUI->SetCheck(1);
	else pCmdUI->SetCheck(0);
}
