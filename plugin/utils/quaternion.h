/*
 *	Copyright (C) 2011 Alex009
 *	License read in license.txt
 */

class Quaternion
{
public:
	// functions
	Quaternion();
	Quaternion(float x,float y,float z,float w);
	~Quaternion();
	void Normalize();
	void FromAxisAngle(float axis_x,float axis_y,float axis_z,float angle);
	void ToAxisAngle(float * axis_x,float * axis_y,float * axis_z,float * angle);
	void FromEuler(float * euler);
	void ToEuler(float * euler);
	void Multiply(Quaternion a);

	// vars
	float	X;
	float	Y;
	float	Z;
	float	W;
};