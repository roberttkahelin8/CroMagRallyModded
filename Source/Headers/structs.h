//
// structs.h
//

#pragma once


#include "globals.h"
#include "ogl_support.h"
#include "metaobjects.h"

#define	MAX_ANIMS			25
#define	MAX_KEYFRAMES		15
#define	MAX_JOINTS			20
#define	MAX_CHILDREN		(MAX_JOINTS-1)
#define	MAX_LIMBS	MAX_JOINTS

#define MAX_FLAGS_IN_OBJNODE			4		// # flags in ObjNode


#define	MAX_DECOMPOSED_POINTS	1000
#define	MAX_DECOMPOSED_NORMALS	800
#define	MAX_POINTS_ON_BONE		100		// DONT CHANGE!! MUST MATCH VALUE IN BIO-OREO PRO SINCE IS WIRED INTO ARRAY IN SKELETON FILE!!
#define	MAX_POINT_REFS			10		// max times a point can be re-used in multiple places
#define	MAX_DECOMPOSED_TRIMESHES 10

#define	MAX_MORPH_TRIMESHES		10
#define	MAX_MORPH_POINTS		1000

#define	MAX_SPECIAL_DATA_BYTES	(sizeof(uint64_t) * 8)



			/*********************/
			/* SPLINE STRUCTURES */
			/*********************/

typedef	struct
{
	float	x,z;
}SplinePointType;

typedef struct
{
	float			placement;			// where on spline to start item (0=front, 1.0 = end)
	uint16_t		type;
	Byte			parm[4];
	uint16_t		flags;
}SplineItemType;


typedef struct
{
	int16_t			numNubs;			// # nubs in spline
	int16_t 		_pad1;
	int32_t			_junkptr1;
	int32_t			numPoints;			// # points in spline
	int32_t			_junkptr2;
	int16_t			numItems;			// # items on the spline
	int16_t 		_pad2;
	int32_t			_junkptr3;
	Rect			bBox;				// bounding box of spline area
}File_SplineDefType;


typedef struct
{
//	short			numNubs;			// # nubs in spline -- unused in-game
//	SplinePointType	**nubList;			// handle to nub list -- unused in-game
	long			numPoints;			// # points in spline
	SplinePointType	**pointList;		// handle to calculated spline points
	short			numItems;			// # items on the spline
	SplineItemType	**itemList;			// handle to spline items

//	Rect			bBox;				// bounding box of spline area -- unused in-game
}SplineDefType;




		/* COLLISION BOX */

typedef struct
{
	long	left,right,front,back,top,bottom;
	long	oldLeft,oldRight,oldFront,oldBack,oldTop,oldBottom;
}CollisionBoxType;


		/****************************/
		/* POLYGONAL COLLISION INFO */
		/****************************/

typedef struct
{
	OGLPoint3D			verts[3];			// coords of each vertex
	OGLPlaneEquation	planeEQ;			// plane equation of triangle
}CollisionTriangleType;

typedef struct
{
	OGLBoundingBox			bBox;				// bounding box of these triangles
	short					numTriangles;		// # triangles in list
	CollisionTriangleType	*triangles;			// ptr to list of collision triangles
}TriangleCollisionList;


//*************************** SKELETON *****************************************/

		/* BONE SPECIFICATIONS */
		//
		//
		// NOTE: Similar to joint definition but lacks animation, rot/scale info.
		//

typedef struct
{
	long 				parentBone;			 			// index to previous bone
	void				*ignored1;
	OGLMatrix4x4		ignored2;
	void				*ignored3;
	unsigned char		ignored4[32];
	OGLPoint3D			coord;							// absolute coord (not relative to parent!)
	uint16_t				numPointsAttachedToBone;		// # vertices/points that this bone has
	uint16_t				*pointList;						// indecies into gDecomposedPointList
	uint16_t				numNormalsAttachedToBone;		// # vertex normals this bone has
	uint16_t				*normalList;					// indecies into gDecomposedNormalsList
}BoneDefinitionType;


			/* DECOMPOSED POINT INFO */

typedef struct
{
	OGLPoint3D	realPoint;							// point coords as imported in 3DMF model
	OGLPoint3D	boneRelPoint;						// point relative to bone coords (offset from bone)

	Byte		numRefs;							// # of places this point is used in the geometry data
	Byte		whichTriMesh[MAX_POINT_REFS];		// index to trimeshes
	short		whichPoint[MAX_POINT_REFS];			// index into pointlist of triMesh above
	short		whichNormal[MAX_POINT_REFS];		// index into gDecomposedNormalsList
}DecomposedPointType;



		/* CURRENT JOINT STATE */

typedef struct
{
	int32_t		tick;					// time at which this state exists
	int32_t		accelerationMode;		// mode of in/out acceleration
	OGLPoint3D	coord;					// current 3D coords of joint (relative to link)
	OGLVector3D	rotation;				// current rotation values of joint (relative to link)
	OGLVector3D	scale;					// current scale values of joint mesh
}JointKeyframeType;


		/* JOINT DEFINITIONS */

typedef struct
{
	int8_t				numKeyFrames[MAX_ANIMS];				// # keyframes
	JointKeyframeType 	**keyFrames;							// 2D array of keyframe data keyFrames[anim#][keyframe#]
}JointKeyFrameHeader;

			/* ANIM EVENT TYPE */

typedef struct
{
	int16_t	time;
	Byte	type;
	Byte	value;
}AnimEventType;


			/* SKELETON INFO */

typedef struct
{
	Byte				NumBones;						// # joints in this skeleton object
	JointKeyFrameHeader	JointKeyframes[MAX_JOINTS];		// array of joint definitions

	Byte				numChildren[MAX_JOINTS];		// # children each joint has
	Byte				childIndecies[MAX_JOINTS][MAX_CHILDREN];	// index to each child

	Byte				NumAnims;						// # animations in this skeleton object
	Byte				*NumAnimEvents;					// ptr to array containing the # of animevents for each anim
	AnimEventType		**AnimEventsList;				// 2 dimensional array which holds a anim event list for each anim AnimEventsList[anim#][event#]

	BoneDefinitionType	*Bones;							// data which describes bone heirarachy

	long				numDecomposedTriMeshes;			// # trimeshes in skeleton
	MOVertexArrayData	*decomposedTriMeshes;			// array of triMeshData

	long				numDecomposedPoints;			// # shared points in skeleton
	DecomposedPointType	*decomposedPointList;			// array of shared points

	short				numDecomposedNormals ;			// # shared normal vectors
	OGLVector3D			*decomposedNormalsList;			// array of shared normals


}SkeletonDefType;


		/* THE STRUCTURE ATTACHED TO AN OBJNODE */
		//
		// This contains all of the local skeleton data for a particular ObjNode
		//

typedef struct
{
	Boolean			JointsAreGlobal;				// true when joints are already in world-space coords
	Byte			AnimNum;						// animation #

	Boolean			IsMorphing;						// flag set when morphing from an anim to another
	float			MorphSpeed;						// speed of morphing (1.0 = normal)
	float			MorphPercent;					// percentage of morph from kf1 to kf2 (0.0 - 1.0)

	JointKeyframeType	JointCurrentPosition[MAX_JOINTS];	// for each joint, holds current interpolated keyframe values
	JointKeyframeType	MorphStart[MAX_JOINTS];		// morph start & end keyframes for each joint
	JointKeyframeType	MorphEnd[MAX_JOINTS];

	float			CurrentAnimTime;				// current time index for animation
	float			LoopBackTime;					// time to loop or zigzag back to (default = 0 unless set by a setmarker)
	float			MaxAnimTime;					// duration of current anim
	float			AnimSpeed;						// time factor for speed of executing current anim (1.0 = normal time)
	Byte			AnimEventIndex;					// current index into anim event list
	Byte			AnimDirection;					// if going forward in timeline or backward
	Byte			EndMode;						// what to do when reach end of animation
	Boolean			AnimHasStopped;					// flag gets set when anim has reached end of sequence (looping anims don't set this!)

	OGLMatrix4x4	jointTransformMatrix[MAX_JOINTS];	// holds matrix xform for each joint

	SkeletonDefType	*skeletonDefinition;						// point to skeleton's common/shared data

	MOMaterialObject	*overrideTexture;			// an illegal ref to a texture object

}SkeletonObjDataType;


			/* TERRAIN ITEM ENTRY TYPE */

typedef struct
{
	uint32_t						x;
	uint32_t						y;
	uint16_t						type;
	Byte							parm[4];
	uint16_t						flags;
}TerrainItemEntryType;




			/****************************/
			/*  OBJECT RECORD STRUCTURE */
			/****************************/

struct ObjNode
{
	struct ObjNode	*PrevNode;			// address of previous node in linked list
	struct ObjNode	*NextNode;			// address of next node in linked list
	struct ObjNode	*ChainNode;
	struct ObjNode	*ChainHead;			// a chain's head (link back to 1st obj in chain)

	struct	ObjNode	*ShadowNode;		// ptr to node's shadow (if any)
	struct ObjNode	*TwitchNode;		// ptr to node's twitch driver (if any)

	uint16_t		Slot;				// sort value
	Byte			Genre;				// obj genre
	Byte			Type;				// obj type
	Byte			Group;				// obj group
	Byte			Kind;				// kind
	Byte			Projection;
	void			(*MoveCall)(struct ObjNode *);			// pointer to object's move routine
	void			(*SplineMoveCall)(struct ObjNode *);	// pointer to object's spline move routine
	void			(*CustomDrawFunction)(struct ObjNode *);// pointer to object's custom draw function
	void			(*Destructor)(struct ObjNode *);		// pointer to object's pre-delete function
	uint32_t		StatusBits;			// various status bits

	OGLPoint3D		Coord;				// coord of object
	OGLPoint3D		OldCoord;			// coord @ previous frame
	OGLPoint3D		InitCoord;			// coord where was created
	OGLVector3D		Delta;				// delta velocity of object
	OGLVector3D		DeltaRot;
	OGLVector3D		Rot;				// rotation of object
	OGLVector2D		AccelVector;		// current acceleration vector
	OGLVector3D		RealMotion;			// actual motion vector from old to new coord

	float			Speed3D;			// length of Delta vector x,y,z (not scaled to fps)
	float			Speed2D;			// length of Delta vector x,z (not scaled to fps)
	float			OldSpeed2D;

	OGLVector3D		Scale;				// scale of object

	uint32_t		CType;				// collision type bits
	uint32_t		CBits;				// collision attribute bits
	Byte			NumCollisionBoxes;
	CollisionBoxType	*CollisionBoxes;// Ptr to array of collision rectangles
	int				LeftOff,RightOff,FrontOff,BackOff,TopOff,BottomOff;		// box offsets (only used by simple objects with 1 collision box)

	float			BoundingSphereRadius;

	signed char		Mode;				// mode
	short			PlayerNum;			// player #

	union
	{
		struct
		{
			Ptr		SpecialPtr[6];
			long	Special[6];
			float	SpecialF[6];
			int8_t	Flag[6];
		};

		char		SpecialPadding[MAX_SPECIAL_DATA_BYTES];
	};

	float			Health;				// health 0..1

	OGLMatrix4x4		BaseTransformMatrix;	// matrix which contains all of the transforms for the object as a whole
	MOMatrixObject		*BaseTransformObject;	// extra LEGAL object ref to BaseTransformMatrix (other legal ref is kept in BaseGroup)
	MOGroupObject		*BaseGroup;				// group containing all geometry,etc. for this object (for drawing)

	SkeletonObjDataType	*Skeleton;				// pointer to skeleton record data

	TerrainItemEntryType *TerrainItemPtr;		// if item was from terrain, then this pts to entry in array
	SplineItemType 		*SplineItemPtr;			// if item was from spline, then this pts to entry in array
	uint8_t				SplineNum;				// which spline this spline item is on
	float				SplinePlacement;		// 0.0->.9999 for placement on spline
	short				SplineObjectIndex;		// index into gSplineObjectList of this ObjNode

	short				EffectChannel;			// effect sound channel index (-1 = none)
	short				ParticleGroup;
	uint32_t			ParticleMagicNum;

	OGLColorRGBA		ColorFilter;
};
typedef struct ObjNode ObjNode;


		/* NEW OBJECT DEFINITION TYPE */

typedef struct
{
	Byte		genre;
	Byte		group;
	Byte		type;
	Byte		animNum;
	Byte		projection;
	Byte		player;
	uint16_t	slot;
	OGLPoint3D	coord;
	uint32_t	flags;
	void		(*moveCall)(ObjNode *);
	void		(*drawCall)(ObjNode *);
	float		rot;
	float		scale;
    // objects can have offsets
    float offsetX;
    float offsetY;
    float offsetZ;
}NewObjectDefinitionType;
