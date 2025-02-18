// Fill out your copyright notice in the Description page of Project Settings.


#include "Clyde.h"
#include "TestGridGameMode.h"

AGridBaseNode* AClyde::GetPlayerRelativeTarget()
{
	return Super::GetPlayerRelativeTarget();
}

AClyde::AClyde()
{
	CurrentGridCoords = FVector2D(18, 15);
	ScatterTarget = nullptr;
	bIsWaiting = true;
	ghostExitPoints = 90; // 60 + 30 from inky's
}

void AClyde::BeginPlay()
{
	Super::BeginPlay();
	FVector2D ClydeScatter = FVector2D(1,0);
	ScatterTarget = *GridGenTMap.Find(ClydeScatter);
	//SetNextNodeByDir(FVector(0, 0, 0), true);



}

void AClyde::SetScatterTarget()
{

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue, TEXT("Scatter target Function"));
	AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), ScatterTarget->GetGridPosition(), -(this->GetLastValidDirection()));

	const FVector Dimensions(60, 60, 20);
	//DrawDebugBox(GetWorld(), PossibleNode->GetTileCoordinates(), Dimensions, FColor::Red);

	if (PossibleNode)
	{
		this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
	}
}

void AClyde::SetChaseTarget()
{
	const AGridBaseNode* Target;
	FVector2D CurrentPCoords = GetPlayerRelativeTarget()->GetGridPosition();
	FVector2D CurrentCCoords = GameMode->ClydePtr->GetCurrentGridCoords();


	double distance = FVector::DistXY(FVector(CurrentPCoords.X,CurrentPCoords.Y,6), FVector(CurrentCCoords.X, CurrentCCoords.Y, 6));
	
	if(distance > 8)
		Target = *GridGenTMap.Find(CurrentPCoords);
	else
		Target = *GridGenTMap.Find(FVector2D(1, 0));


	if (Target != nullptr) {

		AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));

		const FVector Dimensions(60, 60, 20);
		DrawDebugBox(GetWorld(), PossibleNode->GetTileCoordinates(), Dimensions, FColor::Red);

		if (PossibleNode)
		{
			this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
		}
	}


}

void AClyde::SetEatenTarget() {

	const AGridBaseNode* Target = *GridGenTMap.Find(FVector2D(21, 13));


	//once outside, the ghost goes to the center of the ghost house
	if (CurrentGridCoords == FVector2D(21, 15))
	{
		SetTargetNode(*GridGenTMap.Find(FVector2D(18, 15)));

	}
	//once inside, the ghost is respawned and instantly kicked out of the ghost house
	else if (CurrentGridCoords == FVector2D(18, 15))
	{
		ResetOriginalColor();
		SetTargetNode(*GridGenTMap.Find(FVector2D(21, 14)));
		bIsEaten = false;
	}
	//if the ghost is eaten, it will go to the ghost house
	else {
		AGridBaseNode* PossibleNode = TheGridGen->GetClosestNodeFromMyCoordsToTargetCoords(this->GetLastNodeCoords(), Target->GetGridPosition(), -(this->GetLastValidDirection()));
		const FVector Dimensions(60, 60, 20);
		DrawDebugBox(GetWorld(), PossibleNode->GetTileCoordinates(), Dimensions, FColor::Red);
		if (PossibleNode)
		{
			this->SetNextNodeByDir(TheGridGen->GetThreeDOfTwoDVector(PossibleNode->GetGridPosition() - this->GetLastNodeCoords()), true);
		}
	}

}

void AClyde::resetGhost()
{
	const FVector2D ClydeSpawn = FVector2D(18, 15);
	CurrentGridCoords = ClydeSpawn;
	LastNode = *(GridGenTMap.Find(ClydeSpawn));
	NextNode = *(GridGenTMap.Find(ClydeSpawn));
	SetTargetNode(*GridGenTMap.Find(ClydeSpawn));
	SetActorLocation(FVector(1850.f, 1550.f, 5.f));
	bIsEaten = false;
	bIsWaiting = true;
	bIsTimerStarted = false;
}

void AClyde::ghostWait()
{
	if (!GameMode->InkyPtr->getIsWaiting())
		//once Inky has left, Clyde starts checking for the score 60+30(Inky's points)
		//using this if to fire the timer only once, knowing that ghostWait is called every tick
		if (!bIsTimerStarted) {
			bIsTimerStarted = true;
			GetWorld()->GetTimerManager().SetTimer(GameMode->HouseTimer, this, &AClyde::leaveHouse, 4.f, false);
		}
		else if (PointsGameInstance->partialScore > 90) 
			leaveHouse();
		
}

