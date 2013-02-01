#pragma pack(1)

typedef signed short INT16;

struct NodeHeader // 0x14
{
	DWORD	NumNodes;
	DWORD	NumVehicleNodes;
	DWORD	NumPedNodes;
	DWORD	NumNaviNodes;
	DWORD	NumLinks;
};

#pragma pack(1)
struct NodePath // 0x28
{
	DWORD	MemAddress;
	DWORD	Unused;
	INT16	PosX;
	INT16	PosY;
	INT16	PosZ;
	INT16	Unknown;
	WORD	LinkId;
	WORD	AreaId;
	WORD	NodeId;
	BYTE	PathWidth;
	BYTE	NodeType;
	DWORD	Flags;
};

#pragma pack(1)
struct NodeNavi // 0x14
{
	INT16	PosX;
	INT16	PosY;
	WORD	AreaId;
	WORD	NodeId;
	BYTE	DirectionX;
	BYTE	DirectionY;
	DWORD	Flags;
};

#pragma pack(1)
struct NodeLink // 0x4
{
	WORD	AreaId;
	WORD	NodeId;
};