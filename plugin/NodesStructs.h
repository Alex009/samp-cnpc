
#pragma pack(1)
struct NodeHeader // 0x14
{
	unsigned long	NumNodes;
	unsigned long	NumVehicleNodes;
	unsigned long	NumPedNodes;
	unsigned long	NumNaviNodes;
	unsigned long	NumLinks;
};

#pragma pack(1)
struct NodePath // 0x28
{
	unsigned long	MemAddress;
	unsigned long	Unused;
	signed short	PosX;
	signed short	PosY;
	signed short	PosZ;
	signed short	Unknown;
	unsigned short	LinkId;
	unsigned short	AreaId;
	unsigned short	NodeId;
	unsigned char	PathWidth;
	unsigned char	NodeType;
	unsigned long	Flags;
};

#pragma pack(1)
struct NodeNavi // 0x14
{
	signed short	PosX;
	signed short	PosY;
	unsigned short	AreaId;
	unsigned short	NodeId;
	unsigned char	DirectionX;
	unsigned char	DirectionY;
	unsigned long	Flags;
};

#pragma pack(1)
struct NodeLink // 0x4
{
	unsigned short	AreaId;
	unsigned short	NodeId;
};