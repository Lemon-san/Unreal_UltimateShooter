#pragma once
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

#define DRAW_ARROW(LineStart, LineEnd, Color) if (GetWorld()) UKismetSystemLibrary::DrawDebugArrow(GetWorld(),LineStart,LineEnd,5.f,FColor::Color,5.f);
#define DRAW_SPHERE(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, true);
#define DRAW_SPHERE_COLOR(Location,Color) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 8.f, 12, FColor::Color, false, 5.f);
#define DRAW_SPHERE_SINGLEFRAME(Location) if (GetWorld()) DrawDebugSphere(GetWorld(), Location, 25.f, 12, FColor::Red, false,-1.f);
#define DRAW_LINE(StartLocation,EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,true,-1.f,0,1.f);
#define DRAW_LINE_SINGLEFRAME(StartLocation,EndLocation) if (GetWorld()) DrawDebugLine(GetWorld(),StartLocation,EndLocation,FColor::Red,false,-1.f,0,1.f);
#define DRAW_POINT(PointLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),PointLocation,15.f,FColor::Red, true);
#define DRAW_POINT_SINGLEFRAME(PointLocation) if(GetWorld()) DrawDebugPoint(GetWorld(),PointLocation,15.f,FColor::Red, false, -1.f);
#define DRAW_VECTOR(StartLocation,EndLocation) if (GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, true, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(),EndLocation,15.f, FColor::Red,true); \
	}
#define DRAW_VECTOR_SINGLEFRAME(StartLocation,EndLocation) if (GetWorld()) \
	{ \
		DrawDebugLine(GetWorld(), StartLocation, EndLocation, FColor::Red, false, -1.f, 0, 1.f); \
		DrawDebugPoint(GetWorld(),EndLocation,15.f, FColor::Red,false,-1.f); \
	}
