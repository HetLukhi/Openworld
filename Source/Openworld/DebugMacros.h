#pragma once

#include "DrawDebugHelpers.h"

#define DRAW_SPHERE(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.0f, 12, FColor::Red, true);
#define DRAW_SPHERE_COLOR(Location, Color) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 8.0f, 12, Color, false, 5.0f);
#define DRAW_SPHERE_SINGAL_FRAME(Location) if(GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.0f, 12, FColor::Red, false, -1.0f);

#define DRAW_LINE(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.0f, 0, 1.0f);
#define DRAW_LINE_SINGAL_FRAME(StartLocation, EndLocation) if(GetWorld()) DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 1.0f);

#define DRAW_POINT(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.0f, FColor::Red, true);
#define DRAW_POINT_SINGAL_FRAME(Location) if(GetWorld()) DrawDebugPoint(GetWorld(), Location, 15.0f, FColor::Red, false, -1.0f);

#define DRAW_VECTOR(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.0f, 0, 1.0f); \
		DrawDebugPoint(GetWorld(), EndLocation, 15.0f, FColor::Red, true); \
	}
#define DRAW_VECTOR_SINGAL_FRAME(StartLocation, EndLocation) if(GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.0f, 0, 1.0f); \
		DrawDebugPoint(GetWorld(), EndLocation, 15.0f, FColor::Red, false, -1.0f); \
	}
