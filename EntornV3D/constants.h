//******** PRACTICA VISUALITZACIÓ 3D (Facultat de Ciències / Escola d'Enginyeria - UAB)
//******** Entorn bàsic VS2026 MULTIFINESTRA amb OpenGL, interfície MFC i Status Bar
//******** Ferran Poveda, Marc Vivet, Carme Julià, Débora Gil, Enric Martí (Setembre 2026)
// constants.h : Definició de constants compartides a
//				 CENtornV3DView.cpp, visualitzacio.cpp i escena.cpp

#ifndef CONST_H
#define CONST_H

//--------------- V3D: Tipus de Càmera
#define CAP ' '
#define CAM_ESFERICA 'E'
#define CAM_GEODE 'G'
#define CAM_LOXODROMA_TFRENET 'F'
#define CAM_LOXODROMA_TDARBOUX 'D'
#define CAM_NAVEGA 'N'

//--------------- V3D: Tipus de Projecció
#define AXONOM 'A'
#define ORTO 'O'
#define PERSPECT 'P'
#define IMA_PICK 3
#define PASSEIG_V 4


//--------------- V3D: Tipus de Polars (per la Visualització Interactiva)
#define POLARZ 'Z'
#define POLARY 'Y'
#define POLARX 'X'

//-------------- V3D: Tipus d'Objectes
#define CAMIO 'm'
#define CILINDRE 'y'
#define CUB 'c'
#define CUB_RGB 'C'
#define C_BEZIER '2'
#define C_LEMNISCATA 'K'
#define C_LOXODROMA 'X'
#define C_BSPLINE 'q'
#define C_CATMULLROM 'R'
#define ESFERA 'e'
#define ESCENA_LOXODROMA 'M'
#define PAISATGE 'F'
#define HIDROAVIO 'H'
#define TETERA 't'
#define TIE 'I'
#define RESPALL 'r'
#define TRUCK 'T'
#define VAIXELL 'v'
#define OBJ3DS '3'		// Objecte format 3DS
#define OBJOBJ '4'		// Objecte format OBJ

//-------------- V3D: Tipus d'Iluminacio
#define PUNTS 'P'
#define FILFERROS 'w'
#define PLANA 'p'
#define SUAU 's'

//-------------- V3D: Objectes precompilats OpenGL amb glNewList
#define EIXOS 1			// Eixos
#define FRACTAL 2		// Fractal
#define OBJECTE3DS 3	// Objecte 3DS sense textures
#define OBJECTE3DST 4	// Objecte 3DS amb textures
#define OBJECTEOBJ 5	// Objecte OBJ sense textures
#define OBJECTEOBJT 6   // Objecte OBJ amb textures

// -------------- V3D: Definició dels valors del pla near i far del Volum de Visualització en Perspectiva
const double p_near=0.01;
const double p_far=50000.0;

//-------------- V3D: Tipus d'Objectes Picking
#define PICKCAP 0
#define PICKFAR 1
#define PICKVAIXELL 2 
#define PICKHIDRO 3 

// -------------- V3D: CONSTANTS TEXTURES
// Nombre màxim de textures
#define NUM_MAX_TEXTURES 10

// Tipus de textures
#define CAP ' '
#define FUSTA 'F'
#define MARBRE 'M'
#define METALL 'E'
#define FITXER 'f'
#define FITXERBMP 'f'
#define FITXERIMA 'I'


// --------------  V3D: NOMBRE DE LLUMS: Nombre de Llums de l'aplicació, les d'OpenGL
const int NUM_MAX_LLUMS = 8;

// Permet prendre les funcions _s enlloc de les deprecated (strcpy -> strcpy_s). Ja definit
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1

// Elimina missatges warning que venen de crides deprecated (strcpy -> strcpy_s).
//#define _CRT_SECURE_NO_WARNINGS -> Posat a "Project"->"Properties"->"Configuration properties"->"C/C++"->"Preprocessor"->"Preprocessor definitions".


// -------------- V3D: SHADERS --> Tipus de Shaders
#define CAP_SHADER ' '
#define GOURAUD_SHADER 'G'
#define PHONG_SHADER 'P'
#define GOURAUD_TOON 'g'
#define PHONG_TOON 'p'
#define FILE_SHADER 'f'

//--------------- V3D: Valor constant de pi
const double PI=3.14159;
const double TWOPI = 2 * PI;
const double PID2 = PI / 2;
//const double pi=3.14159265358979323846264338327950288419716939937510f;

// --------------- GMS. GRID
#define GRID_SIZE 50	// Maximum size of the grid in OpenGL World Coordenates unities.
#define PAS_GRID 1		// Step to shift the grid planes.

// --------------- V3D: TRUCK: Resolució de les rodes (Nombre de divisions del cilindre i discos)
#define RESOLUCIO_RODA 16

// --------------- V3D: Nombre de frames de l'animació Escena BSpline
#define NFRAMES_E 120

// --------------- V3D. MIN i MÀXIM INCREMENT EN EL PAS DE DIBUIX DE LES CORBES
const GLfloat min_pasCS = 0.0125f;			// = 0.00625;	// Mínim increment
const GLfloat max_pasCS = 1.0f;				// = 0.5;		// Màxim increment
const GLfloat mida_Triedre_Corbes = 5.0;	// Longitut Triedre de Frenet/Darboux per a corbes
const GLfloat mida_Triedre_Escena = 100.0; // Longitut Triedre de Frenet/Darboux per a corbes

// --------------- V3D. CAMERA / ESCENA LOXODROMA
const GLfloat a_CamLoxo = 0.125; // Paràmetre a per a la corba Loxodroma que defineix la Càmera i l'Escena Loxodroma 
const GLfloat tmin_CamLoxo = -15; // Mínim del paràmetre t per a la corba Loxodroma que defineix la Càmera i l'Escena Loxodroma 
const GLfloat tmax_CamLoxo = 15; // Mínim del paràmetre t per a la corba Loxodroma que defineix la Càmera i l'Escena Loxodroma 

/* --------------- V3D: Estructura coordenada 2D (Ja definida en Visual Studio 2022)
struct CPoint
{   GLint x;
    GLint y;
};
*/

// --------------- V3D: Estructura coordenada 3D
struct CPunt3D
{   GLdouble x;
    GLdouble y;
	GLdouble z;
	GLdouble w;
};

// --------------- GMS: 3Màscara booleana sobre coordenades 3D
struct CMask3D
{	bool x;
	bool y;
	bool z;
	bool w;
};

// --------------- V3D: Estructura de color R,G,B,A
struct CColor
{   GLdouble r;
    GLdouble g;
	GLdouble b;
	GLdouble a;
};

// --------------- V3D: Estructura coordenada Esfèrica 3D
struct CEsfe3D
{   GLdouble R;
    GLdouble alfa;
	GLdouble beta;
};

// --------------- V3D: INSTANCIA (TG d'instanciació d'un objecte)
struct INSTANCIA
{	CPunt3D VTras;	// Vector de Traslació
	CPunt3D VScal;	// Vector d'Escalatge
	CPunt3D VRota;	// Vector de Rotació
};

// --------------- V3D: Coeficients equació d'atenuació de la llum fatt=1/(ad2+bd+c)
struct CAtenua
{   GLdouble a;
    GLdouble b;
	GLdouble c;
};

// --------------- V3D: Estructura de coeficients de reflectivitat de materials
struct MATERIAL
{	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat emission[4];
	GLfloat shininess;
};

// --------------- V3D: Estructura font de llum
struct LLUM
{	bool encesa;			// Booleana que controla si la llum és encesa [TRUE] o no [FALSE]
	GLfloat posicio[4];		// Posició de la font de llum en coordenades (x,y,z,w).
	GLfloat difusa[4];			// Intensitat difusa de la font de llum (r,g,b,a)
	GLfloat especular[4];		// Intensitat especular de la font de llum (r,g,b,a)
	CAtenua atenuacio;		// Coeficients de l'equació d'atenuació de la llum fatt=1/(ad2+bd+c)
	bool restringida;		// Booleana que indica si la font de llum és restringida [TRUE] i per tant són vàlids els coeficients posteriors o no [FALSE].
	GLfloat spotdirection[3];	// Vector de direció de la font de llum restringida (x,y,z).
	GLfloat spotcoscutoff;	// Coseno de l'angle d'obertura de la font de llum restringida.
	GLfloat spotexponent;	// Exponent que indica l'atenuació de la font del centre de l'eix a l'exterior, segons model de Warn.
};

#endif