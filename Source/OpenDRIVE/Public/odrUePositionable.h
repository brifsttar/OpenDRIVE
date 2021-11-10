// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "OdrPositionable.h"

class AActor;

/**
 * 
 */
class OPENDRIVE_API odrUePositionable : virtual public odrPositionable
{
private:
	AActor *_Actor;

protected:
	virtual std::array<double, 6> Position() const override;

public:
	odrUePositionable(AActor *Actor);
	~odrUePositionable();
};
