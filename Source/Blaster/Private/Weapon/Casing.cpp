// Fill out your copyright notice in the Description page of Project Settings.


#include "Weapon/Casing.h"

ACasing::ACasing()
{
	PrimaryActorTick.bCanEverTick = true;
	CasingMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CasingMesh"));
	SetRootComponent(CasingMesh);
}

void ACasing::BeginPlay()
{
	Super::BeginPlay();
}

