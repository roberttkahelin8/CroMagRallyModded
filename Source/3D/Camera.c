/****************************/
/*   	CAMERA.C    	    */
/* (c)2000 Pangea Software  */
/* By Brian Greenstone      */
/****************************/


/****************************/
/*    EXTERNALS             */
/****************************/

#include "game.h"
#include <SDL_scancode.h>


/****************************/
/*    PROTOTYPES            */
/****************************/

static void MoveCamera(int playerNum, Boolean	priming);
static float RotateCameraRingTowardTarget(float rot, OGLVector2D *carToNewCamVec, OGLVector2D *carToOldCamVec, Boolean putThereNow);


/****************************/
/*    CONSTANTS             */
/****************************/


#define	CAM_YOFF			0.0f

#define	NUM_FLARE_TYPES		4
#define	NUM_FLARES			6

#define	CAMERA_LOOKAT_YOFF		300.0f


/*********************/
/*    VARIABLES      */
/*********************/

Boolean				gDrawLensFlare = false;

float				gCameraStartupTimer;

#if 0				// lens flare unused in CMR

static OGLPoint3D	gSunCoord;

static const float	gFlareOffsetTable[]=
{
	1.0,
	.6,
	.3,
	1.0/8.0,
	-.25,
	-.5
};


static const float	gFlareScaleTable[]=
{
	.3,
	.1,
	.1,
	.2,
	.1,
	.1
};

static const Byte	gFlareImageTable[]=
{
	0,
	1,
	2,
	3,
	2,
	1
};


/*********************** DRAW LENS FLARE ***************************/

void DrawLensFlare(void)
{
short			i;
float			x,y,dot;
OGLPoint3D		sunScreenCoord,from;
float			cx,cy;
float			dx,dy,length;
OGLVector3D		axis,lookAtVector,sunVector;
static OGLColorRGBA	transColor = {1,1,1,1};
int				px,py,pw,ph;

	if (!gDrawLensFlare)
		return;

			/************/
			/* SET TAGS */
			/************/

	OGL_PushState();

	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glColor4f(1,1,1,1);										// full white & alpha to start with


			/* CALC SUN COORD */

	from = setupInfo->cameraPlacement[gCurrentSplitScreenPane].cameraLocation;
	gSunCoord.x = from.x - (gWorldSunDirection.x * setupInfo->yon);
	gSunCoord.y = from.y - (gWorldSunDirection.y * setupInfo->yon);
	gSunCoord.z = from.z - (gWorldSunDirection.z * setupInfo->yon);



	/* CALC DOT PRODUCT BETWEEN VIEW AND LIGHT VECTORS TO SEE IF OUT OF RANGE */

	FastNormalizeVector(from.x - gSunCoord.x,
						from.y - gSunCoord.y,
						from.z - gSunCoord.z,
						&sunVector);

	FastNormalizeVector(setupInfo->cameraPlacement[gCurrentSplitScreenPane].pointOfInterest.x - from.x,
						setupInfo->cameraPlacement[gCurrentSplitScreenPane].pointOfInterest.y - from.y,
						setupInfo->cameraPlacement[gCurrentSplitScreenPane].pointOfInterest.z - from.z,
						&lookAtVector);

	dot = OGLVector3D_Dot(&lookAtVector, &sunVector);
	if (dot >= 0.0f)
		goto bye;

#if 1
	dot = acos(dot) * -2.0f;				// get angle & modify it
	transColor.a = cos(dot);				// get cos of modified angle
#else
	transColor.a = -dot;
#endif

			/* CALC SCREEN COORDINATE OF LIGHT */

	OGLPoint3D_Transform(&gSunCoord, &gWorldToWindowMatrix[gCurrentSplitScreenPane], &sunScreenCoord);


			/* CALC CENTER OF VIEWPORT */

	OGL_GetCurrentViewport(setupInfo, &px, &py, &pw, &ph, gCurrentSplitScreenPane);
	cx = pw/2 + px;
	cy = ph/2 + py;


			/* CALC VECTOR FROM CENTER TO LIGHT */

	dx = sunScreenCoord.x - cx;
	dy = sunScreenCoord.y - cy;
	length = sqrt(dx*dx + dy*dy);
	FastNormalizeVector(dx, dy, 0, &axis);


			/***************/
			/* DRAW FLARES */
			/***************/

			/* INIT MATRICES */

	OGL_SetProjection(kProjectionType2DNDC);


	for (i = 0; i < NUM_FLARES; i++)
	{
		float	sx,sy,o,fx,fy;

		gGlobalTransparency = transColor.a;

		MO_DrawMaterial(gSpriteGroupList[SPRITE_GROUP_LENSFLARES][gFlareImageTable[i]].materialObject);		// activate material



		if (i == 1)												// always draw sun, but fade flares based on dot
		{
			if (transColor.a <= 0.0f)							// see if faded all out
				break;
			glColor4fv((float *)&transColor);
		}

		o = gFlareOffsetTable[i];
		sx = gFlareScaleTable[i];
		sy = sx * gCurrentAspectRatio;

		x = cx + axis.x * length * o;
		y = cy + axis.y * length * o;

		fx = x / (pw/2) - 1.0f;
		fy = (ph-y) / (ph/2) - 1.0f;

		glBegin(GL_QUADS);
		glTexCoord2f(0,0);	glVertex2f(fx - sx, fy - sy);
		glTexCoord2f(1,0);	glVertex2f(fx + sx, fy - sy);
		glTexCoord2f(1,1);	glVertex2f(fx + sx, fy + sy);
		glTexCoord2f(0,1);	glVertex2f(fx - sx, fy + sy);
		glEnd();
	}


			/* RESTORE MODES */

bye:
	glColor4f(1,1,1,1);										// reset this
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	OGL_PopState();

}
#endif


//===============================================================================================================================================================

#pragma mark -

/*************** INIT CAMERAS ***********************/
//
// This MUST be called after the players have been created so that we know
// where to put the camera.
//

void InitCameras(void)
{
int		p;

	gCameraStartupTimer = 1.0;

	for (p = 0; p < gNumTotalPlayers; p++)              // Both real & CPU players have cameras
	{
		gPlayerInfo[p].cameraRingRot = PI;
		gPlayerInfo[p].cameraUserRot = 0;
        if(!gPlayerInfo[p].isComputer){
            gPlayerInfo[p].cameraMode = 0;
        }
	}

		/* CALL SOME STUFF TO GET CAMERAS INTO POSITION & UPDATE DRAW CONTEXT INFO */

	gFramesPerSecondFrac = 1.0/100.0;
    
    for (int t = 0; t < gNumLocalPlayers; t++){
        gPlayerInfo[p].cameraMode = 1;
        UpdateCameras(false, true);
        gPlayerInfo[p].cameraMode = CAMERA_MODE_NORMAL1;
        UpdateCameras(false, true);
        gPlayerInfo[p].cameraMode = CAMERA_MODE_NORMAL2;
        UpdateCameras(false, true);
    }
    
    
	UpdateCameras(true, false);							// prime them
}


/*************** UPDATE CAMERAS ***************/

void UpdateCameras(Boolean priming, Boolean forceRefreshMode)
{
int		i;

		/* CHECK FOR CAMERA KEY CONTROLS */

	if (gNetGameInProgress)
	{
		for (i = 0; i < gNumRealPlayers; i++)
		{
			gPlayerInfo[i].quickRearView = GetControlState(i, kControlBit_RearView);

			if (GetControlStateNew(i, kControlBit_CameraMode))
			{
				gPlayerInfo[i].cameraMode++;
				if (gPlayerInfo[i].cameraMode >= NUM_CAMERA_MODES)
					gPlayerInfo[i].cameraMode = 0;

				if (gPlayerInfo[i].cameraMode == CAMERA_MODE_FIRSTPERSON)		// hide car if in 1st person mode
				{
					SetCarStatusBits(i,STATUS_BIT_NOSHOWTHISPLAYER);
					if (i == gMyNetworkPlayerNum)
						gCycloramaObj->Scale.x = gCycloramaObj->Scale.y = gCycloramaObj->Scale.z = gGameView->yon * .9f;	// hack to fix z-clip problem in this cam mode
				}
				else
				{
					ClearCarStatusBits(i,STATUS_BIT_NOSHOWTHISPLAYER);
					if (i == gMyNetworkPlayerNum)
						gCycloramaObj->Scale.x = gCycloramaObj->Scale.y = gCycloramaObj->Scale.z = gGameView->yon * .99f;
				}
				UpdateObjectTransforms(gCycloramaObj);
			}
		}
	}
	else
	{
		for (i = 0; i < gNumLocalPlayers; i++)
		{
			gPlayerInfo[i].quickRearView = GetNeedState(kControlBit_RearView, i);

			if (GetNewNeedState(kNeed_CameraMode, i))
			{
				gPlayerInfo[i].cameraMode++;
				forceRefreshMode = true;
			}

			if (forceRefreshMode)
			{
				if (gPlayerInfo[i].cameraMode >= NUM_CAMERA_MODES)
					gPlayerInfo[i].cameraMode = 0;

				if (gPlayerInfo[i].cameraMode == CAMERA_MODE_FIRSTPERSON)		// hide car if in 1st person mode
				{
					SetCarStatusBits(i,STATUS_BIT_NOSHOWTHISPLAYER);
					gCycloramaObj->Scale.x = gCycloramaObj->Scale.y = gCycloramaObj->Scale.z = gGameView->yon * .9f;	// hack to fix z-clip problem in this cam mode
				}
				else
				{
					ClearCarStatusBits(i,STATUS_BIT_NOSHOWTHISPLAYER);
					gCycloramaObj->Scale.x = gCycloramaObj->Scale.y = gCycloramaObj->Scale.z = gGameView->yon * .99f;
				}
				UpdateObjectTransforms(gCycloramaObj);
			}
		}
	}


		/* MOVE CAMERA(S) */

	for (gCurrentPlayerNum = 0; gCurrentPlayerNum < gNumTotalPlayers; gCurrentPlayerNum++)
		MoveCamera(gCurrentPlayerNum, priming);

	if (gCameraStartupTimer > 0.0f)
	{
		gCameraStartupTimer -= gFramesPerSecondFrac * .2f;
		if (gCameraStartupTimer < 0.0f)
			gCameraStartupTimer = 0.0f;

	}
}


/**************** SET DEFAULT CAMERA MODE FOR ALL PLAYERS ********************/
//
// This moves the camera away a smidge for players in tall panes
//

void SetDefaultCameraModeForAllPlayers(void)
{
	bool is2PTall = (gNumLocalPlayers == 2 && gGamePrefs.splitScreenMode2P == SPLITSCREEN_MODE_2P_TALL);
	bool is3PTall = (gNumLocalPlayers == 3 && gGamePrefs.splitScreenMode2P == SPLITSCREEN_MODE_3P_TALL);

	for (int i = 0; i < gNumTotalPlayers; i++)
	{
		if (is2PTall || (is3PTall && i == 2))	// P3 gets tall pane
		{
			gPlayerInfo[i].cameraMode = CAMERA_MODE_NORMAL2;
		}
		else
		{
			gPlayerInfo[i].cameraMode = CAMERA_MODE_NORMAL1;
		}
	}
}


/**************** MOVE CAMERA ********************/

static void MoveCamera(int playerNum, Boolean	priming)
{
OGLVector2D	oppositeMotionVec,oldRingPosVec;
OGLPoint3D	from,to;
float		dist;
float		rotY;
float		myX,myY,myZ;
ObjNode		*playerObj;
PlayerInfoType	*playerInfo;
Byte		cameraMode;
float		cameraRadius;
static const OGLVector3D forwardVec = { 0,0,-1 };
OGLVector3D up = { 0,1,0 };		// NOT const, it's transformed in 1st-person view

	playerObj = gPlayerInfo[playerNum].objNode;							// get player objNode
	if (!playerObj)
		return;

	playerInfo = &gPlayerInfo[playerNum];								// point to player info

	float rearViewMultiplier = playerInfo->quickRearView ? -1.0f : 1.0f;

	cameraMode = playerInfo->cameraMode;								// get camera mode
	switch(cameraMode)
	{
		case	CAMERA_MODE_NORMAL1:
		default:
				cameraRadius = 1000.0;
				break;

		case	CAMERA_MODE_NORMAL2:
				cameraRadius = 1500.0;
				break;

		case	CAMERA_MODE_NORMAL3:
				cameraRadius = 2000.0;
				break;

		case	CAMERA_MODE_FIRSTPERSON:
		{
				OGLVector3D_Transform(&up, &playerObj->BaseTransformMatrix, &up);			// calc up vector

				from.x = playerInfo->coord.x + up.x * 250.0f;			// calc from
				from.y = playerInfo->coord.y + up.y * 250.0f;
				from.z = playerInfo->coord.z + up.z * 250.0f;

				OGLVector3D v;
				OGLVector3D_Transform(&forwardVec, &playerObj->BaseTransformMatrix, &v);	// calc to
				to.x = from.x + v.x * rearViewMultiplier;
				to.y = from.y + v.y * rearViewMultiplier;
				to.z = from.z + v.z * rearViewMultiplier;
				goto update;
		}
        case CAMERA_MODE_FREECAM:
            if(_DEBUG){
                printf("Freecam disabled.");
            }
            gPlayerInfo[playerNum].cameraMode = CAMERA_MODE_NORMAL1;
            break;
	}
    
    // potential in the future freecam stuff
    if(gPlayerInfo[playerNum].cameraMode == CAMERA_MODE_FREECAM){
        return;
    }


			/* GET THIS PLAYER'S CAMERA INFO */

	myX = playerInfo->coord.x;
	myY = playerInfo->coord.y;
	myZ = playerInfo->coord.z;


			/**********************/
			/* CALC LOOK AT POINT */
			/**********************/

	to.x = myX;								// accelerate "to" toward target "to"
	to.y = myY + CAMERA_LOOKAT_YOFF;
	to.z = myZ;


			/*******************/
			/* CALC FROM POINT */
			/*******************/

		/* CALC VECTOR TO WHERE WE WANT CAMERA TO BE */

	if ((fabs(playerObj->Delta.x) < 200.0f) && (fabs(playerObj->Delta.z) < 200.0f))			// if basically not moving, then consider still
		goto still;

	FastNormalizeVector2D(-playerObj->Delta.x, -playerObj->Delta.z, &oppositeMotionVec, true);		// calc vector opposite car motion
	if ((oppositeMotionVec.x == 0.0f) && (oppositeMotionVec.y == 0.0f))							// see if no motion
	{
still:
		rotY = (playerObj->Rot.y+PI) + playerInfo->cameraUserRot;
		rotY += gCameraStartupTimer * PI;														// factor in the startup sequence
		oppositeMotionVec.x = -sin(rotY);														// if not moving, then use car's aim
		oppositeMotionVec.y = -cos(rotY);
	}
	else																						// otherwise, rotate the vector based on the user rotation value
	{
		OGLMatrix3x3	m;
		OGLMatrix3x3_SetRotate(&m, playerInfo->cameraUserRot + gCameraStartupTimer * PI);
		OGLVector2D_Transform(&oppositeMotionVec, &m, &oppositeMotionVec);
	}


		/* CALC VECTOR TO WHERE CAMERA IS */

	rotY = playerInfo->cameraRingRot;															// calc vector to old cam position on the ring.
	oldRingPosVec.x = -sin(rotY);
	oldRingPosVec.y = -cos(rotY);


			/* ROTATE CAM POSITION AROUND RING */

	rotY = playerInfo->cameraRingRot = RotateCameraRingTowardTarget(rotY, &oppositeMotionVec, &oldRingPosVec,priming);	// rotate toward new rotation


	from.x = myX - rearViewMultiplier * sin(rotY) * (cameraRadius + (gCameraStartupTimer * 3000.0f));												// calc new from coord
	from.z = myZ - rearViewMultiplier * cos(rotY) * (cameraRadius + (gCameraStartupTimer * 3000.0f));



		/***************/
		/* CALC FROM Y */
		/***************/

	from.y = to.y + CAM_YOFF + (gCameraStartupTimer * 2500.0f);

	if (gCameraStartupTimer == 0.0f)
	{
		dist = GetTerrainY(from.x, from.z);		// make sure not under ground
 		if (from.y < (dist + 200.0f))
			from.y = dist + 200.0f;
	}


update:
				/**********************/
				/* UPDATE CAMERA INFO */
				/**********************/

	if (playerInfo->onThisMachine)										// see if this camera is on this machine
	{
		if (gNetGameInProgress)											// if network, then update context pane 0
			OGL_UpdateCameraFromToUp(&from,&to,&up, 0);
		else
			OGL_UpdateCameraFromToUp(&from,&to,&up, playerInfo->splitPaneNum);	// otherwise, update context pane based on player/splitscreen #
	}


				/* UPDATE PLAYER'S CAMERA INFO */

	playerInfo->camera.cameraLocation = from;
	playerInfo->camera.pointOfInterest = to;

}


/************ ROTATE CAMERA RING TOWARD TARGET ****************/
//
// INPUT:	putThereNow = true if need to camera to be @ desired point *now*.  Used to init camera postion at start.
//

static float RotateCameraRingTowardTarget(float rot, OGLVector2D *carToNewCamVec, OGLVector2D *carToOldCamVec, Boolean putThereNow)
{
float					deltaRot;
float					cross,dot;


			/* DETERMINE THE AMOUNT WE NEED TO ROTATE */

	cross = OGLVector2D_Cross(carToNewCamVec, carToOldCamVec);  	// the sign of the cross product will tell us which way to turn
   	dot = OGLVector2D_Dot(carToNewCamVec, carToOldCamVec);			// also get dot product
 	deltaRot = acos(dot);  			                               	// convert dot to angle

 	if (cross > 0.0f)												// use cross product to set direction of rotation
		deltaRot = -deltaRot;

	if (!putThereNow)
		deltaRot *= fabs(cos(deltaRot * .5f)) * gFramesPerSecondFrac * 4.0f;	// some tweaky math to give us a non-linear delta (turn faster if have farther to go)

	rot -= deltaRot;

	return(rot);
}













