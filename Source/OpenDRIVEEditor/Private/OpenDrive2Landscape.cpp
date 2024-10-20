// Fill out your copyright notice in the Description page of Project Settings.


#include "OpenDrive2Landscape.h"
#include "LevelEditor.h"
#include "Editor.h"
#include "Engine/Selection.h"
#include "LandscapeProxy.h"
#include "LandscapeInfo.h"
#include "LandscapeSplinesComponent.h"
#include "LandscapeSplineSegment.h"
#include "LandscapeSplineControlPoint.h"
#include "LandscapeSplineActor.h"
#include "ILandscapeSplineInterface.h"
#include "RoadManager.hpp"
#include "CoordTranslate.h"


UOpenDrive2Landscape::UOpenDrive2Landscape() {
	Spline = CreateDefaultSubobject<USplineComponent>(TEXT("Spline"));
}

void UOpenDrive2Landscape::SculptLandscape(
	float RoadZOffset,
	float Falloff,
	ULandscapeLayerInfoObject *PaintLayer,
	FName LayerName
) {
	// Landscapes to sculpt
	USelection *Selection = GEditor->GetSelectedActors();
	TArray<ALandscapeProxy *> Landscapes;
	for (FSelectionIterator Iter(*Selection); Iter; ++Iter) {
		ALandscapeProxy *Landscape = Cast<ALandscapeProxy>(*Iter);
		if (!Landscape) continue;
		Landscapes.Add(Landscape);
	}

	roadmanager::OpenDrive *Odr = roadmanager::Position::GetOpenDrive();
	roadmanager::Road *road = 0;
	roadmanager::Position p;
	size_t nrOfRoads = Odr->GetNumOfRoads();

	// Building splines
	for (int i = 0; i < (int)nrOfRoads; i++) {
		Spline->ClearSplinePoints();
		bool shouldStop = false;
		road = Odr->GetRoadByIdx(i);
		if (!road) continue;
		double roadLen = road->GetLength();
		FVector sp;
		double WidthStart = road->GetWidth(0., 0) * 50;
		double WidthEnd = road->GetWidth(roadLen, 0) * 50;
		double s = 0.;
		while (!shouldStop) {
			// Adding spline point every 5m
			if (s > roadLen) {
				s = roadLen;
				shouldStop = true;
			}
			// Since UE4's splines want the same width on both sides, I have to place to point to the
			// geometric center of the road, which might totally differ from lane 0 (think one-way road)
			double t = (road->GetWidth(s, 1) - road->GetWidth(s, -1)) / 2.;
			// I can't just do p = roadmanager::Position(...), because Position's default snapping is
			// driving lane, which breaks things. So I reinit the point and set it to snap to any.
			p.Init();
			p.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
			p.SetLanePos(road->GetId(), 0, s, t);
			p.SetHeadingRelativeRoadDirection(0.);
			sp = CoordTranslate::OdrToUe::ToLocation(p);
			// Slight Z down offset to avoid Z-fighting
			sp.Z += RoadZOffset;
			Spline->AddSplinePoint(sp, ESplineCoordinateSpace::World);

			s += 5;
		}

		// Set all points to linear (default causes issues)
		for (int j = 0; j < Spline->GetNumberOfSplinePoints(); j++) {
			Spline->SetSplinePointType(j, ESplinePointType::Type::Linear);
		}

		for (auto &ls : Landscapes) {
			// The following doesn't work (EditorApplySpline isn't accessible via C++)
			// See https://answers.unrealengine.com/questions/228146/editorapplyspline-is-not-accessible-via-c.html
			//ls->EditorApplySpline(Spline);
			ApplySpline(ls, Spline, WidthStart, WidthEnd, Falloff, PaintLayer, LayerName);
		}
	}
}

void UOpenDrive2Landscape::CreateRoadSplines(
	float RoadZOffset,
	float Falloff,
	ULandscapeLayerInfoObject* PaintLayer,
	FName LayerName
) {
	USelection* Selection = GEditor->GetSelectedActors();
	for (FSelectionIterator Iter(*Selection); Iter; ++Iter) {
		ALandscapeProxy* Landscape = Cast<ALandscapeProxy>(*Iter);
		if (!Landscape) continue;
		ULandscapeInfo* Info = Landscape->GetLandscapeInfo();
		ULandscapeSplinesComponent* LandscapeSpline = nullptr;
		ILandscapeSplineInterface* SplineOwner = nullptr;

		SplineOwner = Landscape;
		LandscapeSpline = SplineOwner->GetSplinesComponent();
		if (!LandscapeSpline) {
			SplineOwner->CreateSplineComponent();
			LandscapeSpline = SplineOwner->GetSplinesComponent();
		}
		if (!LandscapeSpline->IsRegistered()) {
			LandscapeSpline->RegisterComponent();
		} else {
			LandscapeSpline->MarkRenderStateDirty();
		}

		roadmanager::OpenDrive* Odr = roadmanager::Position::GetOpenDrive();
		roadmanager::Road* road = 0;
		roadmanager::Position p;
		size_t nrOfRoads = Odr->GetNumOfRoads();

		// Building splines
		for (int i = 0; i < (int)nrOfRoads; i++) {
			bool shouldStop = false;
			road = Odr->GetRoadByIdx(i);
			if (!road) continue;
			double roadLen = road->GetLength();
			FVector sp;
			double WidthStart = road->GetWidth(0., 0) * 50;
			double WidthEnd = road->GetWidth(roadLen, 0) * 50;
			double s = 0.;
			ULandscapeSplineControlPoint* PrevPoint = nullptr;
			while (!shouldStop) {
				// Adding spline point every 5m
				if (s > roadLen) {
					s = roadLen;
					shouldStop = true;
				}
				// Since UE4's splines want the same width on both sides, I have to place to point to the
				// geometric center of the road, which might totally differ from lane 0 (think one-way road)
				double t = (road->GetWidth(s, 1) - road->GetWidth(s, -1)) / 2.;
				// I can't just do p = roadmanager::Position(...), because Position's default snapping is
				// driving lane, which breaks things. So I reinit the point and set it to snap to any.
				p.Init();
				p.SetSnapLaneTypes(roadmanager::Lane::LaneType::LANE_TYPE_ANY);
				p.SetLanePos(road->GetId(), 0, s, t);
				p.SetHeadingRelativeRoadDirection(0.);
				sp = CoordTranslate::OdrToUe::ToLocation(p);
				// Slight Z down offset to avoid Z-fighting
				sp.Z += RoadZOffset;
				float Width = road->GetWidth(s, 0) * 50;

				FRotator r = CoordTranslate::OdrToUe::ToRotation(p);

				LandscapeSpline->Modify();
				ULandscapeSplineControlPoint* NewControlPoint = NewObject<ULandscapeSplineControlPoint>(LandscapeSpline, NAME_None, RF_Transactional);
				LandscapeSpline->GetControlPoints().Add(NewControlPoint);
				NewControlPoint->Location = LandscapeSpline->GetComponentTransform().InverseTransformPositionNoScale(sp);
				NewControlPoint->Rotation = FRotator(LandscapeSpline->GetComponentTransform().InverseTransformRotation(r.Quaternion()));
				NewControlPoint->Width = Width;

				if (PrevPoint != nullptr) {
					LandscapeSpline->Modify();
					PrevPoint->Modify();
					NewControlPoint->Modify();
					ULandscapeSplineSegment* NewSegment = NewObject<ULandscapeSplineSegment>(LandscapeSpline, NAME_None, RF_Transactional);
					LandscapeSpline->GetSegments().Add(NewSegment);

					NewSegment->Connections[0].ControlPoint = PrevPoint;
					NewSegment->Connections[1].ControlPoint = NewControlPoint;

					NewSegment->Connections[0].SocketName = NewControlPoint->GetBestConnectionTo(PrevPoint->Location);
					NewSegment->Connections[1].SocketName = PrevPoint->GetBestConnectionTo(NewControlPoint->Location);

					FVector StartLocation; FRotator StartRotation;
					PrevPoint->GetConnectionLocationAndRotation(NewSegment->Connections[0].SocketName, StartLocation, StartRotation);
					FVector EndLocation; FRotator EndRotation;
					NewControlPoint->GetConnectionLocationAndRotation(NewSegment->Connections[1].SocketName, EndLocation, EndRotation);

					// Set up tangent lengths
					NewSegment->Connections[0].TangentLen = (EndLocation - StartLocation).Size();
					NewSegment->Connections[1].TangentLen = NewSegment->Connections[0].TangentLen;

					NewSegment->AutoFlipTangents();

					PrevPoint->ConnectedSegments.Add(FLandscapeSplineConnection(NewSegment, 0));
					NewControlPoint->ConnectedSegments.Add(FLandscapeSplineConnection(NewSegment, 1));
					//NewControlPoint->UpdateSplinePoints();
					//PrevPoint->UpdateSplinePoints();
					NewSegment->UpdateSplinePoints();
				}

				PrevPoint = NewControlPoint;

				s += 5;
			}
		}
	}
}
