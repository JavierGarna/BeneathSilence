// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInteractor.h"
#include "PlayerCharacter.h"

void UPlayerInteractor::SpecifyAgentObservation_Implementation(FLearningAgentsObservationSchemaElement& OutObservationSchemaElement, ULearningAgentsObservationSchema* InObservationSchema)
{
	// Use the LearningAgents API to specify a struct observation for PlayerData
	TMap<FName, FLearningAgentsObservationSchemaElement> Elements;

	// Sub-elements of the PlayerData struct observation
	Elements.Add(TEXT("PreviousForwardInputValue"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("PreviousForwardInputValueObservation")));
	Elements.Add(TEXT("PreviousStrafeInputValue"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("PreviousStrafeInputValueObservation")));
	Elements.Add(TEXT("AgentRelativeYawSin"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("AgentRelativeYawSinObservation")));
	Elements.Add(TEXT("AgentRelativeYawCos"), ULearningAgentsObservations::SpecifyFloatObservation(InObservationSchema, 1.0f, TEXT("AgentRelativeYawCosObservation")));
	Elements.Add(TEXT("ObstacleDistances"), ULearningAgentsObservations::SpecifyContinuousObservation(InObservationSchema, 3, 1.0f,TEXT("ObstacleDistancesObservation")));

	OutObservationSchemaElement = ULearningAgentsObservations::SpecifyStructObservation(InObservationSchema, Elements, TEXT("Observations"));
}

void UPlayerInteractor::GatherAgentObservation_Implementation(FLearningAgentsObservationObjectElement& OutObservationObjectElement, ULearningAgentsObservationObject* InObservationObject, const int32 AgentId)
{
	APlayerCharacter* PlayerAI = Cast<APlayerCharacter>(GetAgent(AgentId));
	if (!PlayerAI) return;

	APlayerController* PC = Cast<APlayerController>(PlayerAI->GetController());
	if (!PC || !PC->PlayerCameraManager) return;

	const float ReferenceYaw = PC->PlayerCameraManager->GetCameraRotation().Yaw;
	const float PlayerYaw = PlayerAI->GetActorRotation().Yaw;
	const float AgentRelativeYaw = FMath::FindDeltaAngleDegrees(ReferenceYaw, PlayerYaw);
	const float RelativeYawRadians = FMath::DegreesToRadians(AgentRelativeYaw);
	const float RelativeYawSin = FMath::Sin(RelativeYawRadians);
	const float RelativeYawCos = FMath::Cos(RelativeYawRadians);

	// Log previous and current forward and strafe input values
	UE_LOG(LogTemp, Log, TEXT("PreviousForwardInputValue: %f, PreviousStrafeInputValue: %f"), PlayerAI->GetPreviousForwardInputValue(), PlayerAI->GetPreviousStrafeInputValue());
	UE_LOG(LogTemp, Log, TEXT("CurrentForwardInputValue: %f, CurrentStrafeInputValue: %f"), PlayerAI->GetForwardInputValue(), PlayerAI->GetStrafeInputValue());

	const float FrontDistance = GetObstacleDistance(PlayerAI, 0.0f, 500.0f);
	const float LeftDistance = GetObstacleDistance(PlayerAI, -45.0f, 500.0f);
	const float RightDistance = GetObstacleDistance(PlayerAI, 45.0f, 500.0f);

	// Use the LearningAgents API to gather a struct observation for PlayerData
	TMap<FName, FLearningAgentsObservationObjectElement> Elements;

	// Sub-elements of the PlayerData struct observation
	Elements.Add(TEXT("PreviousForwardInputValue"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, PlayerAI->GetPreviousForwardInputValue(), TEXT("PreviousForwardInputValueObservation")));
	Elements.Add(TEXT("PreviousStrafeInputValue"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, PlayerAI->GetPreviousStrafeInputValue(), TEXT("PreviousStrafeInputValueObservation")));
	Elements.Add(TEXT("AgentRelativeYawSin"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, RelativeYawSin, TEXT("AgentRelativeYawSinObservation")));
	Elements.Add(TEXT("AgentRelativeYawCos"), ULearningAgentsObservations::MakeFloatObservation(InObservationObject, RelativeYawCos, TEXT("AgentRelativeYawCosObservation")));
	Elements.Add(TEXT("ObstacleDistances"), ULearningAgentsObservations::MakeContinuousObservation(InObservationObject, { FrontDistance, LeftDistance, RightDistance }, TEXT("ObstacleDistancesObservation")));

	OutObservationObjectElement = ULearningAgentsObservations::MakeStructObservation(InObservationObject, Elements, TEXT("Observations"));
}

void UPlayerInteractor::SpecifyAgentAction_Implementation(FLearningAgentsActionSchemaElement& OutActionSchemaElement, ULearningAgentsActionSchema* InActionSchema)
{
	// Use the LearningAgents API to specify a struct action for PlayerData
	TMap<FName, FLearningAgentsActionSchemaElement> Elements;

	// Sub-elements of the PlayerData struct action
	Elements.Add(TEXT("ForwardInputValue"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 1.0f, TEXT("ForwardInputValueAction")));
	Elements.Add(TEXT("StrafeInputValue"), ULearningAgentsActions::SpecifyFloatAction(InActionSchema, 1.0f, TEXT("StrafeInputValueAction")));

	OutActionSchemaElement = ULearningAgentsActions::SpecifyStructAction(InActionSchema, Elements, TEXT("Actions"));
}

void UPlayerInteractor::PerformAgentAction_Implementation(const ULearningAgentsActionObject* InActionObject, const FLearningAgentsActionObjectElement& InActionObjectElement, const int32 AgentId)
{
	// Use the LearningAgents API to perform a struct action for PlayerData
	APlayerCharacter* PlayerAI = Cast<APlayerCharacter>(GetAgent(AgentId));

	if (!PlayerAI) return;

	TMap<FName, FLearningAgentsActionObjectElement> Elements;

	const bool bGotStruct =
		ULearningAgentsActions::GetStructAction(
			Elements,
			InActionObject,
			InActionObjectElement,
			TEXT("Actions")
		);

	if (!bGotStruct) return;

	PlayerAI->SetPreviousForwardInputValue(ForwardInputValue);
	PlayerAI->SetPreviousStrafeInputValue(StrafeInputValue);

	ULearningAgentsActions::GetFloatAction(ForwardInputValue, InActionObject, Elements[TEXT("ForwardInputValue")], TEXT("ForwardInputValueAction"));

	ULearningAgentsActions::GetFloatAction(StrafeInputValue, InActionObject, Elements[TEXT("StrafeInputValue")], TEXT("StrafeInputValueAction"));
	const float ReferenceYaw = PlayerAI->GetActorRotation().Yaw;

	APlayerController* PC = Cast<APlayerController>(PlayerAI->GetController());
	const FRotator CameraRotation = PC->PlayerCameraManager->GetCameraRotation();

	ForwardInputValue = QuantizeInput(ForwardInputValue);
	StrafeInputValue = QuantizeInput(StrafeInputValue);

	// Log forward and strafe values
	UE_LOG(LogTemp, Log, TEXT("ForwardInputValue: %f, StrafeInputValue: %f"), ForwardInputValue, StrafeInputValue);

	PlayerAI->AgentForwardInput = ForwardInputValue;
	PlayerAI->AgentStrafeInput = StrafeInputValue;
}

float UPlayerInteractor::QuantizeInput(float value)
{
	if (value > 0.1f)
	{
		return 1.0f;
	}
	else if (value < -0.1f)
	{
		return -1.0f;
	}

	return 0.0f;
}

float UPlayerInteractor::GetObstacleDistance(AActor* Actor, float AngleDegrees, float TraceLength) const
{
	if (!Actor || !GetWorld())
	{
		return 1.0f;
	}

	FVector Start = Actor->GetActorLocation();
	Start.Z -= 50.0f;
	const FVector Direction = Actor->GetActorForwardVector().RotateAngleAxis(AngleDegrees, FVector::UpVector);
	const FVector End = Start + (Direction * TraceLength);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Actor);

	FHitResult HitResult;

	const bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		Start,
		End,
		ECC_Visibility,
		QueryParams
	);

	if (!bHit)
	{
		return 1.0f;
	}

	return FMath::Clamp(
		HitResult.Distance / TraceLength,
		0.0f,
		1.0f
	);
}