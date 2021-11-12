// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "OpenDriveComponent.h"
#include "OpenDriveVehicle.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class OPENDRIVE_API UOpenDriveVehicle : public UOpenDriveComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UOpenDriveVehicle();

	virtual void PostLoad() override;

private:
	class AWheeledVehicle *_Car;
	mutable double _PrevSpeed = 0.;
	mutable double _PrevTime = 0.;
	mutable double _Acc = 0.;
	FBoxSphereBounds GetBounds() const;

protected:
	float _SpeedOverride = 0.f;
	double LengthFront() const;
	double LengthBack() const;

public:
	double VdSpeed() const;
	double VdAcceleration() const;
	double VdSteerAngle() const;
	double VdSteerAngleMax() const;
	double VdWheelbase() const;
	double VdTime() const;
	FVector VdPosition() const;
	// Overrides speed value for cluster slaves, since they don't have PhysX vehicle model
	void SetSpeedOverride(float Speed) { _SpeedOverride = Speed; }

	/**
	* Returns the longitudinal distance (cm, also known as "pk" or "S" distance) to another vehicle
	* @param Other Other vehicle
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float RoadDistanceTo(UPARAM(ref) UOpenDriveVehicle *&Other) const { return RoadDistanceTo(&*Other); }
	float RoadDistanceTo(const UOpenDriveVehicle *Other) const;

	/**
	* Returns the longitudinal distance (cm, also known as "pk" or "S" distance) to another static object
	* @param Other Other object
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float RoadDistanceToOdr(UPARAM(ref) UOpenDriveComponent*& Other) const { return RoadDistanceToOdr(&*Other); }
	float RoadDistanceToOdr(const UOpenDriveComponent* Other) const;

	/**
	* Returns the time headway (s) to another vehicle
	* @param Other Other vehicle
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators", meta = (KeyWords = "TIV"))
	float THW(UPARAM(ref) UOpenDriveVehicle *& Other) const { return THW(&*Other); }
	float THW(const UOpenDriveVehicle * Other) const;

	/**
	* Returns the time headway (s) to another static object
	* @param Other Other object
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float TimeTo(UPARAM(ref) UOpenDriveComponent *&Other) const { return TimeTo(&*Other); }
	float TimeTo(const UOpenDriveComponent* Other) const;

	/**
	* Returns the time headway (s) to another object, computed using current car acceleration
	* @param Other Other object
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float TimeToWithAcc(UPARAM(ref) UOpenDriveComponent *&Other) const { return TimeToWithAcc(&*Other); }
	float TimeToWithAcc(const UOpenDriveComponent* Other) const;

	/**
	* Returns the time to collision (s) to another vehicle
	* @param Other Other vehicle
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float TTC(UPARAM(ref) UOpenDriveVehicle *&Other) const { return TTC(&*Other); }
	float TTC(const UOpenDriveVehicle* Other) const;

	/**
	* Returns the enhanced time to collision (s) to another vehicle
	* @param Other Other vehicle
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float ETTC(UPARAM(ref) UOpenDriveVehicle *&Other) const { return ETTC(&*Other); }
	float ETTC(const UOpenDriveVehicle* Other) const;


	double ETTC(double DeltaDist, double DeltaSpeed, double DeltaAcc) const;

	/**
	* Returns the current speed (cm/s)
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float Speed() const { return VdSpeed() * MetersToUu(); }

	/**
	* Returns the current acceleration (cm/s²)
	*/
	UFUNCTION(BlueprintCallable, Category = "OpenDRIVE Vehicle|Indicators")
	float Acceleration() const { return VdAcceleration() * MetersToUu(); }

		
};
