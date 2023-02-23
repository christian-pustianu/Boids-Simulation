// Code adapted from coursework and exercises of the 2022-2023 Semester 1
// module "COMP3811 Computer Graphics" at the University of Leeds.

#include "mat44.hpp"

Mat44f invert( Mat44f const& Matrix ) noexcept
{
	// Implementation from:
	// http://www.euclideanspace.com/maths/algebra/matrix/functions/inverse/fourD/index.htm
	Mat44f result;
	result(0,0) = Matrix(1,2)*Matrix(2,3)*Matrix(3,1) - Matrix(1,3)*Matrix(2,2)*Matrix(3,1) 
		+ Matrix(1,3)*Matrix(2,1)*Matrix(3,2) - Matrix(1,1)*Matrix(2,3)*Matrix(3,2) 
		- Matrix(1,2)*Matrix(2,1)*Matrix(3,3) + Matrix(1,1)*Matrix(2,2)*Matrix(3,3);
	result(0,1) = Matrix(0,3)*Matrix(2,2)*Matrix(3,1) - Matrix(0,2)*Matrix(2,3)*Matrix(3,1) 
		- Matrix(0,3)*Matrix(2,1)*Matrix(3,2) + Matrix(0,1)*Matrix(2,3)*Matrix(3,2) 
		+ Matrix(0,2)*Matrix(2,1)*Matrix(3,3) - Matrix(0,1)*Matrix(2,2)*Matrix(3,3);
	result(0,2) = Matrix(0,2)*Matrix(1,3)*Matrix(3,1) - Matrix(0,3)*Matrix(1,2)*Matrix(3,1) 
		+ Matrix(0,3)*Matrix(1,1)*Matrix(3,2) - Matrix(0,1)*Matrix(1,3)*Matrix(3,2) 
		- Matrix(0,2)*Matrix(1,1)*Matrix(3,3) + Matrix(0,1)*Matrix(1,2)*Matrix(3,3);
	result(0,3) = Matrix(0,3)*Matrix(1,2)*Matrix(2,1) - Matrix(0,2)*Matrix(1,3)*Matrix(2,1) 
		- Matrix(0,3)*Matrix(1,1)*Matrix(2,2) + Matrix(0,1)*Matrix(1,3)*Matrix(2,2) 
		+ Matrix(0,2)*Matrix(1,1)*Matrix(2,3) - Matrix(0,1)*Matrix(1,2)*Matrix(2,3);
	result(1,0) = Matrix(1,3)*Matrix(2,2)*Matrix(3,0) - Matrix(1,2)*Matrix(2,3)*Matrix(3,0) 
		- Matrix(1,3)*Matrix(2,0)*Matrix(3,2) + Matrix(1,0)*Matrix(2,3)*Matrix(3,2) 
		+ Matrix(1,2)*Matrix(2,0)*Matrix(3,3) - Matrix(1,0)*Matrix(2,2)*Matrix(3,3);
	result(1,1) = Matrix(0,2)*Matrix(2,3)*Matrix(3,0) - Matrix(0,3)*Matrix(2,2)*Matrix(3,0) 
		+ Matrix(0,3)*Matrix(2,0)*Matrix(3,2) - Matrix(0,0)*Matrix(2,3)*Matrix(3,2) 
		- Matrix(0,2)*Matrix(2,0)*Matrix(3,3) + Matrix(0,0)*Matrix(2,2)*Matrix(3,3);
	result(1,2) = Matrix(0,3)*Matrix(1,2)*Matrix(3,0) - Matrix(0,2)*Matrix(1,3)*Matrix(3,0) 
		- Matrix(0,3)*Matrix(1,0)*Matrix(3,2) + Matrix(0,0)*Matrix(1,3)*Matrix(3,2) 
		+ Matrix(0,2)*Matrix(1,0)*Matrix(3,3) - Matrix(0,0)*Matrix(1,2)*Matrix(3,3);
	result(1,3) = Matrix(0,2)*Matrix(1,3)*Matrix(2,0) - Matrix(0,3)*Matrix(1,2)*Matrix(2,0) 
		+ Matrix(0,3)*Matrix(1,0)*Matrix(2,2) - Matrix(0,0)*Matrix(1,3)*Matrix(2,2) 
		- Matrix(0,2)*Matrix(1,0)*Matrix(2,3) + Matrix(0,0)*Matrix(1,2)*Matrix(2,3);
	result(2,0) = Matrix(1,1)*Matrix(2,3)*Matrix(3,0) - Matrix(1,3)*Matrix(2,1)*Matrix(3,0) 
		+ Matrix(1,3)*Matrix(2,0)*Matrix(3,1) - Matrix(1,0)*Matrix(2,3)*Matrix(3,1) 
		- Matrix(1,1)*Matrix(2,0)*Matrix(3,3) + Matrix(1,0)*Matrix(2,1)*Matrix(3,3);
	result(2,1) = Matrix(0,3)*Matrix(2,1)*Matrix(3,0) - Matrix(0,1)*Matrix(2,3)*Matrix(3,0) 
		- Matrix(0,3)*Matrix(2,0)*Matrix(3,1) + Matrix(0,0)*Matrix(2,3)*Matrix(3,1) 
		+ Matrix(0,1)*Matrix(2,0)*Matrix(3,3) - Matrix(0,0)*Matrix(2,1)*Matrix(3,3);
	result(2,2) = Matrix(0,1)*Matrix(1,3)*Matrix(3,0) - Matrix(0,3)*Matrix(1,1)*Matrix(3,0) 
		+ Matrix(0,3)*Matrix(1,0)*Matrix(3,1) - Matrix(0,0)*Matrix(1,3)*Matrix(3,1) 
		- Matrix(0,1)*Matrix(1,0)*Matrix(3,3) + Matrix(0,0)*Matrix(1,1)*Matrix(3,3);
	result(2,3) = Matrix(0,3)*Matrix(1,1)*Matrix(2,0) - Matrix(0,1)*Matrix(1,3)*Matrix(2,0) 
		- Matrix(0,3)*Matrix(1,0)*Matrix(2,1) + Matrix(0,0)*Matrix(1,3)*Matrix(2,1) 
		+ Matrix(0,1)*Matrix(1,0)*Matrix(2,3) - Matrix(0,0)*Matrix(1,1)*Matrix(2,3);
	result(3,0) = Matrix(1,2)*Matrix(2,1)*Matrix(3,0) - Matrix(1,1)*Matrix(2,2)*Matrix(3,0) 
		- Matrix(1,2)*Matrix(2,0)*Matrix(3,1) + Matrix(1,0)*Matrix(2,2)*Matrix(3,1) 
		+ Matrix(1,1)*Matrix(2,0)*Matrix(3,2) - Matrix(1,0)*Matrix(2,1)*Matrix(3,2);
	result(3,1) = Matrix(0,1)*Matrix(2,2)*Matrix(3,0) - Matrix(0,2)*Matrix(2,1)*Matrix(3,0) 
		+ Matrix(0,2)*Matrix(2,0)*Matrix(3,1) - Matrix(0,0)*Matrix(2,2)*Matrix(3,1) 
		- Matrix(0,1)*Matrix(2,0)*Matrix(3,2) + Matrix(0,0)*Matrix(2,1)*Matrix(3,2);
	result(3,2) = Matrix(0,2)*Matrix(1,1)*Matrix(3,0) - Matrix(0,1)*Matrix(1,2)*Matrix(3,0) 
		- Matrix(0,2)*Matrix(1,0)*Matrix(3,1) + Matrix(0,0)*Matrix(1,2)*Matrix(3,1) 
		+ Matrix(0,1)*Matrix(1,0)*Matrix(3,2) - Matrix(0,0)*Matrix(1,1)*Matrix(3,2);
	result(3,3) = Matrix(0,1)*Matrix(1,2)*Matrix(2,0) - Matrix(0,2)*Matrix(1,1)*Matrix(2,0) 
		+ Matrix(0,2)*Matrix(1,0)*Matrix(2,1) - Matrix(0,0)*Matrix(1,2)*Matrix(2,1) 
		- Matrix(0,1)*Matrix(1,0)*Matrix(2,2) + Matrix(0,0)*Matrix(1,1)*Matrix(2,2);

	float const d = Matrix(0,0) * result(0,0) + Matrix(0,1) * result(1,0) 
		+ Matrix(0,2) * result(2,0) + Matrix(0,3) * result(3,0);

	for( auto& v : result.v )
		v /= d;

	return result;
}

