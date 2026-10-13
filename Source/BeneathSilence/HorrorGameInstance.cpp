// Fill out your copyright notice in the Description page of Project Settings.


#include "HorrorGameInstance.h"

UHorrorGameInstance::UHorrorGameInstance() 
{
	for (int32 i = 0; i < MAXSCANS; i++)
	{
		Scans[i] = -1;
	}
}

int UHorrorGameInstance::ClaimScan()
{
	for (int i = 0; i < MAXSCANS; i++)
	{
		if (Scans[i] == -1)
		{
			Scans[i] = i; // Mark as claimed
			return i; // Return the ID of the claimed scan
		}
	}

	return -1; // No available scans
}

void UHorrorGameInstance::RemoveScan(int ScanID)
{
	for (int i = 0; i < MAXSCANS; i++)
	{
		if (Scans[i] == ScanID)
		{
			Scans[i] = -1; // Mark as available
			return;
		}
	}
}
