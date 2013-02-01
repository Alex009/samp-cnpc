/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

class C3DVector
{
public:
	// functions
	C3DVector();
	C3DVector(float x,float y,float z);
	~C3DVector();
	void Normalize();
	void FromEuler(float * euler);
	void ToEuler(float * euler);
	void MoveAroundAxis(float * axis,float angle);

	// vars
	float	X;
	float	Y;
	float	Z;
};