#include <stdint.h>
#include <stdio.h>
#include <algorithm>

// credits: wav
float normalize( float angle )
{
	auto revolutions = angle / 360.f;
	if ( angle > 180.f || angle < -180.f )
	{
		revolutions = std::round( std::abs( revolutions ) );
		if ( angle < 0.f )
			angle = ( angle + 360.f * revolutions );
		else
			angle = ( angle - 360.f * revolutions );
		return angle;
	}
	return angle;
}

float compute_body_pitch( float viewangle )
{
	// purpose: 
	// creating properly handled angle
	// as the server, and hit registration
	// would handle it
	// this could also be used to show the real angle in thirdperson
	// example: LocalPlayer->v_angle( ) = ComputeBodyPitch( cmd->viewangles.x

	// reference:
	// CBasePlayerAnimState::ComputePoseParam_BodyPitch
	// https://github.com/ValveSoftware/source-sdk-2013/blob/master/sp/src/game/shared/base_playeranimstate.cpp#L719-L724

	if ( viewangle > 180.0f )
		viewangle -= 360.0f;

	return std::clamp( viewangle, -90.0f, 90.0f );
}

bool is_fake_pitch( const float& viewangle, const float& goal_fake )
{
	// purpose:
	// determine if there's a desync between networked player angle 
	// and body pitch, indicating a fake angle 

	// usage: see FindSmallestFake

	const float eye_angles = std::abs( normalize( viewangle ) );

	if ( eye_angles == goal_fake && compute_body_pitch( viewangle ) != eye_angles )
		return true;

	return false;
}

float find_smallest_fake_pitch( const float& goal )
{
	// purpose:
	// find viewangle that would produce a fake angle

	// usage:
	// cmd->viewangles.x = FindSmallestFake( 0.0f ) 
	// cmd->viewangles.x = FindSmallestFake( 180.0f ) 

	for ( float angle = 270.0f; angle < 1000.0f; angle += 1.0f ) // >_>
	{
		if ( is_fake_pitch( angle, std::abs( goal ) ) )
		{
			if ( goal < 0.0f )
				return -angle;
			else
				return angle;
		}
	}

	return std::numeric_limits< float >::max( );
}

int main( )
{
	printf( "fake: %.1f\n", find_smallest_fake_pitch( 0.0f ) );
	printf( "fake: %.1f\n", find_smallest_fake_pitch( 90.0f ) );
	printf( "fake: %.1f\n", find_smallest_fake_pitch( -90.0f ) );

	system( "pause" );
} 