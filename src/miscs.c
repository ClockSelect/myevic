#include "myevic.h"
#include "dataflash.h"
#include "display.h"
#include "atomizer.h"

//=========================================================================
//----- (00004FB0) --------------------------------------------------------
__myevic__ void ModeChange()
{
	dfResistance = AtoRez;

	switch ( dfMode )
	{
		case 0:
			dfTempAlgo = 1;
			if ( dfRezNI )
			{
				if ( ( dfRezLockedNI ) || !( gFlags.check_rez_ni ))
				{
					dfResistance = dfRezNI;
				}
			}
			break;
		case 1:
			dfTempAlgo = 2;
			if ( dfRezTI )
			{
				if (( dfRezLockedTI ) || !( gFlags.check_rez_ti ))
				{
					dfResistance = dfRezTI;
				}
			}
			break;
		case 2:
			dfTempAlgo = 3;
			if ( dfRezSS )
			{
				if (( dfRezLockedSS ) || !( gFlags.check_rez_ss ))
				{
					dfResistance = dfRezSS;
				}
			}
			break;
		case 3:
			dfTempAlgo = 4;
			if ( dfRezTCR )
			{
				if (( dfRezLockedTCR ) || !( gFlags.check_rez_tcr ))
				{
					dfResistance = dfRezTCR;
				}
			}
			break;
		default:
			break;
	}

	UpdateDFTimer = 50;
}


//=========================================================================
//----- (00005AF0) --------------------------------------------------------
__myevic__ void NextMode()
{
	do
	{
		if ( dfMode < 3 )
		{
			dfMode = 3;
		}
		else if ( dfMode < 6 )
		{
			++dfMode;
		}
		else
		{
			dfMode = dfLastTCMode;

			if ( dfModesSel & ( 1 << dfMode ) )
			{
				if (( dfModesSel & 0x07 ) != 0x07 )
				{
					do
					{
						if ( ++dfMode > 2 ) dfMode = 0;
					}
					while ( dfModesSel & ( 1 << dfMode ) );
				}
			}
		}
	}
	while ( dfModesSel & ( 1 << dfMode ) );

	ModeChange();
}


//=========================================================================
// Bhaskara I's approximation of sine
// returns sin(x) in 16.16 fixed point format
__myevic__ int32_t BhaskaraI( int32_t x )
{
	int32_t a = x * ( 180 - x );
	int32_t b = 40500 - a;
	return ( ( a << 18 ) / b );
}


//=========================================================================
// sine approx given for x in degrees
// result in 16.16 fixed point format
__myevic__ int32_t sine( int32_t x )
{
	if ( x < 0 )
	{
		x = 180 - x;
	}

	x %= 360;

	if ( x < 180 )
	{
		return BhaskaraI( x );
	}
	else
	{
		return ( - BhaskaraI( x - 180 ) );
	}
}


//=========================================================================
// cosine approx given for x in degrees
// result in 16.16 fixed point format
__myevic__ int32_t cosine( int32_t x )
{
	return sine( x + 90 );
}


//=========================================================================

typedef struct {
	int16_t x;
	int16_t y;
	int16_t z;
} angles_t;

typedef struct {
	int8_t x;
	int8_t y;
	int8_t z;
} pt3d_t;

typedef struct {
	int8_t pt1;
	int8_t pt2;
} line3d_t;

typedef struct {
	const uint8_t	scale;
	const uint8_t	npoints;
	const uint8_t	nlines;
	const pt3d_t	*points;
	const line3d_t	*lines;
} obj3d_t;

const pt3d_t cube_points[] = 
	{
		{  1,  1,  1 },	// 0
		{ -1,  1,  1 }, // 1
		{  1, -1,  1 }, // 2
		{ -1, -1,  1 }, // 3
		{  1,  1, -1 }, // 4
		{ -1,  1, -1 }, // 5
		{  1, -1, -1 }, // 6
		{ -1, -1, -1 }  // 7
	};

const line3d_t cube_lines[] = 
	{
		{ 0, 1 },
		{ 0, 2 },
		{ 0, 4 },
		{ 1, 3 },
		{ 1, 5 },
		{ 2, 3 },
		{ 2, 6 },
		{ 3, 7 },
		{ 4, 5 },
		{ 4, 6 },
		{ 5, 7 },
		{ 6, 7 },
	};
	
const obj3d_t cube =
{
	16,
	8,
	12,
	cube_points,
	cube_lines
};

typedef int32_t matrix3d_t[3][3];


static angles_t angles = { 0, 0, 0 };
static angles_t speeds = { 3, 5, 1 };
static matrix3d_t rot_matrix;
static pt3d_t points[8];

uint8_t Anim3d = 0;


__myevic__ void compute_matrix( matrix3d_t mat, const angles_t *a )
{
	int32_t A = cosine( a->x );
	int32_t B = sine( a->x );
	int32_t C = cosine( a->y );
	int32_t D = sine( a->y );
	int32_t E = cosine( a->z );
	int32_t F = sine( a->z );

	int32_t AD = ( A >> 8 ) * ( D >> 8 );
	int32_t BD = ( B >> 8 ) * ( D >> 8 );

	mat[0][0] = (  C >> 8 ) * ( E >> 8 );
	mat[0][1] = ( -C >> 8 ) * ( F >> 8 );
	mat[0][2] =  D;
	mat[1][0] = (  BD >> 8 ) * ( E >> 8 ) + ( A >> 8 ) * ( F >> 8 );
	mat[1][1] = ( -BD >> 8 ) * ( F >> 8 ) + ( A >> 8 ) * ( E >> 8 );
	mat[1][2] = (  -B >> 8 ) * ( C >> 8 );
	mat[2][0] = ( -AD >> 8 ) * ( E >> 8 ) + ( B >> 8 ) * ( F >> 8 );
	mat[2][1] = (  AD >> 8 ) * ( F >> 8 ) + ( B >> 8 ) * ( E >> 8 );
	mat[2][2] = (   A >> 8 ) * ( C >> 8 );
}


__myevic__ void rotate_object( pt3d_t *dst, const obj3d_t *src, const matrix3d_t mat )
{
	int32_t s, x, y, z;

	s = src->scale;

	for ( int i = 0 ; i < src->npoints ; ++i )
	{
		x = s * src->points[i].x;
		y = s * src->points[i].y;
		z = s * src->points[i].z;

		dst[i].x = ( mat[0][0] * x + mat[0][1] * y + mat[0][2] * z ) >> 16;
		dst[i].y = ( mat[1][0] * x + mat[1][1] * y + mat[1][2] * z ) >> 16;
		dst[i].z = ( mat[2][0] * x + mat[2][1] * y + mat[2][2] * z ) >> 16;
	}
}


__myevic__ void next_angle()
{
	angles.x += speeds.x; angles.x %= 360;
	angles.y += speeds.y; angles.y %= 360;
	angles.z += speeds.z; angles.z %= 360;
}


__myevic__ void anim3d( int refresh )
{
	static uint8_t tscaler = 0;

	if ( !refresh )
	{
		if ( ++tscaler < 4 ) return;
		tscaler = 0;

		next_angle();

		compute_matrix( rot_matrix, &angles );
		rotate_object( points, &cube, rot_matrix );

		int f = 256;
		for ( int i = 0 ; i < cube.npoints ; ++i )
		{
			points[i].x = f * points[i].x / ( f + points[i].z );
			points[i].y = f * points[i].y / ( f + points[i].z );
		}
	}

	DrawFillRect( 0, 44, 63, 106, 0 );

	for ( int i = 0 ; i < cube.nlines ; ++i )
	{
		DrawLine(
			points[cube.lines[i].pt1].x + 32,
			points[cube.lines[i].pt1].y + 75,
			points[cube.lines[i].pt2].x + 32,
			points[cube.lines[i].pt2].y + 75,
			1,
			1
		);
	}

	if ( !refresh )
	{
		DisplayRefresh();
	}
}


//=========================================================================
// Pseudo-random number generator

uint32_t RNGSeed;

//----- (000001CC) --------------------------------------------------------
__myevic__ uint32_t Random()
{
  RNGSeed = 0x41c64e6d * RNGSeed + 12345;
  return RNGSeed >> 1;
}

//----- (000001E0) --------------------------------------------------------
__myevic__ void SetRandSeed( uint32_t s )
{
  RNGSeed = s;
}
