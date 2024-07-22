//
// terrain.h
//

#pragma once

#include "main.h"


#define	MAP_ITEM_MYSTARTCOORD		0											// map item # for my start coords



		/* SUPER TILE MODES */

enum
{
	SUPERTILE_MODE_FREE,
	SUPERTILE_MODE_USED
};

#define	SUPERTILE_TEXMAP_SIZE		128												// the width & height of a supertile's texture

#define	OREOMAP_TILE_SIZE			16 												// pixel w/h of texture tile
#define TILE_DATA_SIZE16			(OREOMAP_TILE_SIZE*OREOMAP_TILE_SIZE*2)

#define	TERRAIN_POLYGON_SIZE		1600.0f 											// size in world units of terrain polygon
// was 800.0f


#define	TERRAIN_POLYGON_SIZE_Frac	((float)1.0f/(float)TERRAIN_POLYGON_SIZE)

#define	SUPERTILE_SIZE				8  												// size of a super-tile / terrain object zone




#define	NUM_TRIS_IN_SUPERTILE		(SUPERTILE_SIZE * SUPERTILE_SIZE * 2)			// 2 triangles per tile
#define	NUM_VERTICES_IN_SUPERTILE	((SUPERTILE_SIZE+1)*(SUPERTILE_SIZE+1))			// # vertices in a supertile

#define	TEMP_TEXTURE_BUFF_SIZE		(OREOMAP_TILE_SIZE * SUPERTILE_SIZE)

#define	MAP2UNIT_VALUE				((float)TERRAIN_POLYGON_SIZE/OREOMAP_TILE_SIZE)	//value to xlate Oreo map pixel coords to 3-space unit coords

#define	TERRAIN_SUPERTILE_UNIT_SIZE	(SUPERTILE_SIZE*TERRAIN_POLYGON_SIZE)			// world unit size of a supertile
#define	TERRAIN_SUPERTILE_UNIT_SIZE_Frac (1.0f / TERRAIN_SUPERTILE_UNIT_SIZE)

#define	SUPERTILE_ACTIVE_RANGE		6
#define	SUPERTILE_ITEMRING_MARGIN	0												// # supertile margin for adding new items

#define	SUPERTILE_DIST_WIDE			(SUPERTILE_ACTIVE_RANGE*2)
#define	SUPERTILE_DIST_DEEP			(SUPERTILE_ACTIVE_RANGE*2)

								// # visible supertiles * N players * 2 buffers
								// We need the x2 buffer because we dont free unused supertiles
								// until after we've allocated new supertiles, so we'll always
								// need more supertiles than are actually ever used - worst case
								// scenario is twice as many.

#define	MAX_SUPERTILES			((SUPERTILE_DIST_WIDE * SUPERTILE_DIST_DEEP) * MAX_SPLITSCREENS * 2)



#define	MAX_TERRAIN_TILES		0x10000

#define	MAX_TERRAIN_WIDTH		400
#define	MAX_TERRAIN_DEPTH		400

#define	MAX_SUPERTILES_WIDE		(MAX_TERRAIN_WIDTH/SUPERTILE_SIZE)
#define	MAX_SUPERTILES_DEEP		(MAX_TERRAIN_DEPTH/SUPERTILE_SIZE)


#define	MAX_SUPERTILE_TEXTURES	(MAX_SUPERTILES_WIDE*MAX_SUPERTILES_DEEP)


//=====================================================================


struct SuperTileMemoryType
{
	Byte				hiccupTimer;							// # frames to skip for use
	Byte				mode;									// free, used, etc.
	float				x,z,y;									// world coords
	long				left,back;								// integer coords of back/left corner
	MOMaterialObject	*texture;								// refs to materials
	MOVertexArrayData	*meshData;								// mesh's data for the supertile
	OGLBoundingBox		bBox;									// bounding box
};
typedef struct SuperTileMemoryType SuperTileMemoryType;


typedef struct
{
	uint16_t		flags;
	Byte			parm[2];
}TileAttribType;



typedef struct
{
	Boolean		    isEmpty;									// true if supertile is empty
	uint16_t	    superTileID;								// ID # of supertile to use here
}SuperTileGridType;


typedef struct
{
	uint16_t		numItems;
	uint16_t		itemIndex;
}SuperTileItemIndexType;


		/* TILE ATTRIBUTE BITS */

enum
{
	TILE_ATTRIB_ICE				=	1,
	TILE_ATTRIB_SNOW			=	1<<1,
	TILE_ATTRIB_KICKUP_DUST		=	1<<2,
	TILE_ATTRIB_KICKUP_MUD		=	1<<3,
	TILE_ATTRIB_KICKUP_SNOW		=	1<<4,
	TILE_ATTRIB_KICKUP_GRASS	=	1<<5,
	TILE_ATTRIB_NOSKIDMARKS		=	1<<6,
	TILE_ATTRIB_ROCK			=	1<<7
};



		/* TERRAIN ITEM FLAGS */

enum
{
	ITEM_FLAGS_INUSE	=	(1),
	ITEM_FLAGS_USER1	=	(1<<1),
	ITEM_FLAGS_USER2	=	(1<<1)
};


enum
{
	SPLIT_BACKWARD = 0,
	SPLIT_FORWARD,
	SPLIT_ARBITRARY
};


typedef	struct
{
	uint16_t	supertileIndex;
	uint8_t		statusFlags;
	uint8_t		playerHereFlags;
}SuperTileStatus;

enum									// statusFlags
{
	SUPERTILE_IS_DEFINED			=	1,
	SUPERTILE_IS_USED_THIS_FRAME	=	(1<<1)
};


//=====================================================================



void CreateSuperTileMemoryList(void);
void DisposeSuperTileMemoryList(void);
extern 	void DisposeTerrain(void);
void DrawTerrain(void);
extern	void GetSuperTileInfo(long x, long z, long *superCol, long *superRow, long *tileCol, long *tileRow);
extern	void InitTerrainManager(void);
float	GetTerrainY(float x, float z);
float	GetMinTerrainY(float x, float z, short group, short type, float scale);
void InitCurrentScrollSettings(void);

extern 	void BuildTerrainItemList(void);
void AddTerrainItemsOnSuperTile(long row, long col, short playerNum);
extern 	Boolean TrackTerrainItem(ObjNode *theNode);
Boolean SeeIfCoordsOutOfRange(float x, float z, short playerToSkip);
extern 	void FindPlayerStartCoordItems(void);
void InitSuperTileGrid(void);
void RotateOnTerrain(ObjNode *theNode, float yOffset, OGLVector3D *surfaceNormal);
void DoPlayerTerrainUpdate(void);
void CalcTileNormals(long row, long col, OGLVector3D *n1, OGLVector3D *n2);
void CalculateSplitModeMatrix(void);

uint16_t	GetTileAttribsAtRowCol(float x, float z);
