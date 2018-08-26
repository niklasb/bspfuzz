struct CPhysCollide;
typedef float vec_t;

struct Vector
{
        // Members
        vec_t x, y, z;
};

struct Vector2D
{
        // Members
        vec_t x, y, z;
};

struct vcollide_t
{
  unsigned short solidCount : 15;
  unsigned short isPacked : 1;
  unsigned short descSize;
  // VPhysicsSolids
  CPhysCollide  **solids;
  char      *pKeyValues;
};

struct cmodel_t
{
  Vector    mins, maxs;
  Vector    origin;    // for sounds or lights
  int      headnode;

  vcollide_t  vcollisionData;
};

struct cleaf_t
{
        int                         contents;
        short                   cluster;
        short                   area : 9;
        short                   flags : 7;
        unsigned short  firstleafbrush;
        unsigned short  numleafbrushes;
        unsigned short  dispListStart;
        unsigned short  dispCount;
};

struct carea_t
{
        int             numareaportals;
        int             firstareaportal;
        int             floodnum;                                                       // if two areas have equal floodnums, they are connected
        int             floodvalid;
};

struct dareaportal_t
{
  unsigned short  m_PortalKey;    // Entities have a key called portalnumber (and in vbsp a variable
                  // called areaportalnum) which is used
                  // to bind them to the area portals by comparing with this value.

  unsigned short  otherarea;    // The area this portal looks into.

  unsigned short  m_FirstClipPortalVert;  // Portal geometry.
  unsigned short  m_nClipPortalVerts;

  int        planenum;
};

#define MAXLIGHTMAPS 4
typedef unsigned char byte;

struct dface_t
{
  unsigned short  planenum;
  byte    side;  // faces opposite to the node's plane direction
  byte    onNode; // 1 of on node, 0 if in leaf

  int      firstedge;    // we must support > 64k edges
  short    numedges;
  short    texinfo;
  // This is a union under the assumption that a fog volume boundary (ie. water surface)
  // isn't a displacement map.
  // FIXME: These should be made a union with a flags or type field for which one it is
  // if we can add more to this.
//  union
//  {
      short       dispinfo;
    // This is only for surfaces that are the boundaries of fog volumes
    // (ie. water surfaces)
    // All of the rest of the surfaces can look at their leaf to find out
    // what fog volume they are in.
    short    surfaceFogVolumeID;
//  };

  // lighting info
  byte    styles[MAXLIGHTMAPS];
  int      lightofs;    // start of [numstyles*surfsize] samples
    float       area;

  // TODO: make these unsigned chars?
  int      m_LightmapTextureMinsInLuxels[2];
  int      m_LightmapTextureSizeInLuxels[2];

  int origFace;        // reference the original face this face was derived from
};

struct CDispVert
{
  Vector    m_vVector;    // Vector field defining displacement volume.
  float    m_flDist;    // Displacement distances.
  float    m_flAlpha;    // "per vertex" alpha values.
};

#define MAX_PATH 256

struct CDispTri {
  unsigned short m_uiTags;    // Displacement triangle tags.
};

struct CMapLoadHelper {
        int                                     m_nLumpSize;
        int                                     m_nLumpOffset;
        int                                     m_nLumpVersion;
        byte                            *m_pRawData;
        byte                            *m_pData;
        byte                            *m_pUncompressedData;

        // Handling for lump files
        int                                     m_nLumpID;
        char                            m_szLumpFilename[MAX_PATH];
};

// sizeof(lump_t) == 16
struct lump_t
{
  int  fileofs;  // offset into file (bytes)
  int  filelen;  // length of lump (bytes)
  int  version;  // lump format version
  char  fourCC[4];  // lump ident code
};

#define  HEADER_LUMPS    64

struct dheader_t
{
  int  ident;                // BSP file identifier
  int  version;              // BSP file version
  lump_t  lumps[HEADER_LUMPS];  // lump directory array
  int  mapRevision;          // the map's revision (iteration, version) number
};

struct CDispSubNeighbor
{
        unsigned char           m_NeighborOrientation;
        unsigned char           m_Span;
        unsigned char           m_NeighborSpan;
};

class CDispNeighbor
{
        CDispSubNeighbor        m_SubNeighbors[2];
};

#define MAX_DISP_CORNER_NEIGHBORS  4

class CDispCornerNeighbors
{
        unsigned short  m_Neighbors[MAX_DISP_CORNER_NEIGHBORS]; // indices of neighbors.
        unsigned char   m_nNeighbors;
};

#define MAX_DISPVERTS 289

class ddispinfo_t
{
        Vector          startPosition;                                          // start position used for orientation -- (added BSPVERSION 6)
        int                     m_iDispVertStart;                                       // Index into LUMP_DISP_VERTS.
        int                     m_iDispTriStart;                                        // Index into LUMP_DISP_TRIS.

    int         power;                              // power - indicates size of map (2^power + 1)
    int         minTess;                            // minimum tesselation allowed
    float       smoothingAngle;                     // lighting smoothing angle
    int         contents;                           // surface contents

        unsigned short  m_iMapFace;                                             // Which map face this displacement comes from.

        int                     m_iLightmapAlphaStart;                          // Index into ddisplightmapalpha.
                                                                                                        // The count is m_pParent->lightmapTextureSizeInLuxels[0]*m_pParent->lightmapTextureSizeInLuxels[1].

        int                     m_iLightmapSamplePositionStart;         // Index into LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS.

        CDispNeighbor                   m_EdgeNeighbors[4];             // Indexed by NEIGHBOREDGE_ defines.
        CDispCornerNeighbors    m_CornerNeighbors[4];   // Indexed by CORNER_ defines.

        unsigned long   m_AllowedVerts[16];      // This is built based on the layout and sizes of our neighbors
                                                                                                                // and tells us which vertices are allowed to be active.
};

struct dedge_t
{
  unsigned short  v[2];    // vertex numbers
};

struct dnode_t
{
        int                     planenum;
        int                     children[2];    // negative numbers are -(leafs+1), not nodes
        short           mins[3];                // for frustom culling
        short           maxs[3];
        unsigned short  firstface;
        unsigned short  numfaces;       // counting both sides
        short                   area;           // If all leaves below this node are in the same area, then
                                                                // this is the area index. If not, this is -1.
};


struct dvertex_t
{
  Vector  point;
};

#define QUAD_POINT_COUNT 4
#define NUM_BUMP_VECTS 3

struct CCoreDispSurface {
  int      m_Index;                                // parent face (CMapFace, dface_t, msurface_t) index "handle"

  int      m_PointCount;                              // number of points in the face (should be 4!)
  Vector    m_Points[QUAD_POINT_COUNT];                        // points
  Vector    m_Normals[QUAD_POINT_COUNT];                      // normals at points
  Vector2D  m_TexCoords[QUAD_POINT_COUNT];                      // texture coordinates at points
  Vector2D  m_LuxelCoords[NUM_BUMP_VECTS+1][QUAD_POINT_COUNT];            // lightmap coordinates at points
  float    m_Alphas[QUAD_POINT_COUNT];                        // alpha at points

  // Luxels sizes
  int          m_nLuxelU;
  int          m_nLuxelV;

  // Straight from the BSP file.
  CDispNeighbor      m_EdgeNeighbors[4];
  CDispCornerNeighbors  m_CornerNeighbors[4];

    int      m_Flags;                                // surface flags - inherited from the "parent" face
  int      m_Contents;                                // contents flags - inherited from the "parent" face

  Vector    sAxis;                                  // used to generate start disp orientation (old method)
  Vector    tAxis;                                  // used to generate start disp orientation (old method)
  int      m_PointStartIndex;                            // index to the starting point -- for saving starting point
  Vector    m_PointStart;                              // starting point used to determine the orientation of the displacement map on the surface
};

/*
struct CDispCollTree {
  Vector              m_mins;
  int                m_iCounter;
  Vector              m_maxs;
  int                m_nContents;

//#ifdef ENGINE_DLL
  //memhandle_t            m_hCache;
//#endif

  char pad[40-32];

  // offset 40
  int                m_nPower;
  int                m_nFlags;

  Vector              m_vecSurfPoints[4];            // Base surface points.
  Vector              m_vecStabDir;              // Direction to stab for this displacement surface (is the base face normal)
  short              m_nSurfaceProps[2];            // Surface properties (save off from texdata for impact responses)

  CDispVector<Vector>        m_aVerts;                // Displacement verts.
  CDispVector<CDispCollTri>    m_aTris;                // Displacement triangles.
  CDispVector<CDispCollNode>    m_nodes;          // Nodes.
  CDispVector<CDispCollLeaf>    m_leaves;                // Leaves.
  // Cache
  CUtlVector<CDispCollTriCache>  m_aTrisCache;
  CUtlVector<Vector> m_aEdgePlanes;

  CDispCollHelper          m_Helper;

  unsigned int          m_nSize;
};
*/

#define MAX_NEIGHBOR_VERT_COUNT  8
#define MAX_SURF_AT_NODE_COUNT  8

struct cplane_t
{
    Vector    normal;
    float    dist;
    byte    type;            // for fast side tests
    byte    signbits;        // signx + (signy<<1) + (signz<<1)
    byte    pad[2];
};

struct CCoreDispNode {
    Vector        m_BBox[2];
    float        m_ErrorTerm;
    int            m_VertIndex;
    int            m_NeighborVertIndices[MAX_NEIGHBOR_VERT_COUNT];
    Vector        m_SurfBBoxes[MAX_SURF_AT_NODE_COUNT][2];
    cplane_t    m_SurfPlanes[MAX_SURF_AT_NODE_COUNT];
    Vector        m_RayBBoxes[4][2];
};

struct CCoreDispInfo {
    int unknown1;
    CCoreDispNode *m_Nodes; // LOD quad-tree nodes
    float m_Elevation; // distance along the subdivision normal (should

    // defines the size of the displacement surface
    int m_Power; // "size" of the displacement map

    // base surface data
    CCoreDispSurface    m_Surf;            // surface containing displacement data
                                        // be changed to match the paint normal next pass)
    // Vertex data..
    void        *m_pVerts;

    // Triangle data..
    void        *m_pTris;

    // render specific data
    int                    m_RenderIndexCount;        // number of indices used in rendering
    unsigned short        *m_RenderIndices;        // rendering index list (list of triangles)
    int                    m_RenderCounter;        // counter to verify surfaces are renderered/collided with only once per frame

    // utility data
    bool                m_bTouched;                // touched flag
    void        *m_pNext;                // used for chaining

    // The list that this disp is in (used for CDispUtils::IHelper implementation).
    void        **m_ppListBase;
    int                    m_ListSize;

    int    m_AllowedVerts_bitvec[10];        // Built in VBSP. Defines which verts are allowed to exist based on what the neighbors are.

    int                    m_nListIndex;
};

struct VectorAligned {
  vec_t x, y, z, w;
};

struct mnode_t
{
	// common with leaf
	int			contents;		// <0 to differentiate from leafs
	// -1 means check the node for visibility
	// -2 means don't check the node for visibility

	int			visframe;		// node needs to be traversed if current

	mnode_t		*parent;
	short		area;			// If all leaves below this node are in the same area, then
	// this is the area index. If not, this is -1.
	short		flags;

	VectorAligned		m_vecCenter;
	VectorAligned		m_vecHalfDiagonal;

// node specific
	cplane_t	*plane;
	mnode_t		*children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
};


struct worldbrushdata_t
{
  int x;
        int                     numsubmodels;

        int                     numplanes;
        cplane_t        *planes;

        int                     numleafs;               // number of visible leafs, not counting 0
        void         *leafs;

        int                     numleafwaterdata;
        void *leafwaterdata;

        int                     numvertexes;
        void       *vertexes;

        int                     numoccluders;
        void *occluders;

        int                     numoccluderpolys;
        void *occluderpolys;

        int                     numoccludervertindices;
        int                     *occludervertindices;

        int                             numvertnormalindices;   // These index vertnormals.
        unsigned short  *vertnormalindices;

        int                     numvertnormals;
        Vector          *vertnormals;

        int                     numnodes;
        mnode_t         *nodes;
        unsigned short *m_LeafMinDistToWater;

        int                     numtexinfo;
        void      *texinfo;

        int                     numtexdata;
        void      *texdata;

        int         numDispInfos;
        int  hDispInfos;     // Use DispInfo_Index to get IDispInfos..

        /*
        int         numOrigSurfaces;
        msurface_t  *pOrigSurfaces;
        */

        int                     numsurfaces;
        void     *surfaces1;
        void     *surfaces2;
        void *surfacelighting;
        void *surfacenormals;

        bool            unloadedlightmaps;

        int                     numvertindices;
        unsigned short *vertindices;

        int nummarksurfaces;
        void *marksurfaces;

        void           *lightdata;

        int                     numworldlights;
        void *worldlights;

        void *shadowzbuffers;

        // non-polygon primitives (strips and lists)
        int                     numprimitives;
        void *primitives;

        int                     numprimverts;
        void *primverts;

        int                     numprimindices;
        unsigned short *primindices;

        int                             m_nAreas;
        void                 *m_pAreas;

        int                             m_nAreaPortals;
        dareaportal_t   *m_pAreaPortals;

        int                             m_nClipPortalVerts;
        Vector                  *m_pClipPortalVerts;

        void  *m_pCubemapSamples;
        int                                m_nCubemapSamples;

        int                             m_nDispInfoReferences;
        unsigned short  *m_pDispInfoReferences;

        void             *m_pLeafAmbient;
        void  *m_pAmbientSamples;
};

struct cnode_t
{
        cplane_t        *plane;
        int                     children[2];
};

enum lumptype {
    LUMP_ENTITIES      = 0,
    LUMP_PLANES       = 1,
    LUMP_TEXDATA       = 2,
    LUMP_VERTEXES      = 3,
    LUMP_VISIBILITY      = 4,
    LUMP_NODES       = 5,
    LUMP_TEXINFO       = 6,
    LUMP_FACES       = 7,
    LUMP_LIGHTING      = 8,
    LUMP_OCCLUSION      = 9,
    LUMP_LEAFS       = 10,
    LUMP_FACEIDS       = 11,
    LUMP_EDGES       = 12,
    LUMP_SURFEDGES      = 13,
    LUMP_MODELS       = 14,
    LUMP_WORLDLIGHTS      = 15,
    LUMP_LEAFFACES      = 16,
    LUMP_LEAFBRUSHES      = 17,
    LUMP_BRUSHES       = 18,
    LUMP_BRUSHSIDES      = 19,
    LUMP_AREAS       = 20,
    LUMP_AREAPORTALS      = 21,
    LUMP_PROPCOLLISION_NEW       = 22,
    LUMP_PROPHULLS_NEW       = 23,
    LUMP_PROPHULLVERTS_NEW       = 24,
    LUMP_PROPTRIS_NEW       = 25,
    LUMP_DISPINFO      = 26,
    LUMP_ORIGINALFACES     = 27,
    LUMP_PHYSDISP      = 28,
    LUMP_PHYSCOLLIDE      = 29,
    LUMP_VERTNORMALS      = 30,
    LUMP_VERTNORMALINDICES    = 31,
    LUMP_DISP_LIGHTMAP_ALPHAS   = 32,
    LUMP_DISP_VERTS      = 33,
    LUMP_DISP_LIGHTMAP_SAMPLE_POSITIONS = 34,
    LUMP_GAME_LUMP      = 35,
    LUMP_LEAFWATERDATA     = 36,
    LUMP_PRIMITIVES      = 37,
    LUMP_PRIMVERTS      = 38,
    LUMP_PRIMINDICES      = 39,
    LUMP_PAKFILE       = 40,
    LUMP_CLIPPORTALVERTS     = 41,
    LUMP_CUBEMAPS      = 42,
    LUMP_TEXDATA_STRING_DATA    = 43,
    LUMP_TEXDATA_STRING_TABLE   = 44,
    LUMP_OVERLAYS      = 45,
    LUMP_LEAFMINDISTTOWATER    = 46,
    LUMP_FACE_MACRO_TEXTURE_INFO   = 47,
    LUMP_DISP_TRIS      = 48,
    LUMP_PROP_BLOB_NEW    = 49,
    LUMP_WATEROVERLAYS     = 50,
    LUMP_LEAF_AMBIENT_INDEX_HDR   = 51,
    LUMP_LEAF_AMBIENT_INDEX    = 52,
    LUMP_LIGHTING_HDR     = 53,
    LUMP_WORLDLIGHTS_HDR     = 54,
    LUMP_LEAF_AMBIENT_LIGHTING_HDR  = 55,
    LUMP_LEAF_AMBIENT_LIGHTING   = 56,
    LUMP_XZIPPAKFILE      = 57,
    LUMP_FACES_HDR      = 58,
    LUMP_MAP_FLAGS      = 59,
    LUMP_OVERLAY_FADES     = 60,
    LUMP_OVERLAY_SYSTEM_LEVELS_NEW     = 61,
    LUMP_PHYSLEVEL_NEW     = 62,
    LUMP_DISP_MULTIBLEND_NEW     = 63,
};

struct phyheader_t
{
	int		size;           // Size of this header section (generally 16)
	int		id;             // Often zero, unknown purpose.
	int		solidCount;     // Number of solids in file
	long	        checkSum;	// checksum of source .mdl file (4-bytes)
};

struct compactsurfaceheader_t
{
	int	size;			// Size of the content after this byte
	int	vphysicsID;		// Generally the ASCII for "VPHY" in newer files
	short	version;
	short	modelType;
	int	surfaceSize;
	Vector	dragAxisAreas;
	int	axisMapSize;
};

// old style phy format
struct legacysurfaceheader_t
{
	int	size;
	float	mass_center[3];
	float	rotation_inertia[3];
	float	upper_limit_radius;
	int	max_deviation : 8;
	int	byte_size : 24;
	int	offset_ledgetree_root;
	int	dummy[3]; 		// dummy[2] is "IVPS" or 0
};
