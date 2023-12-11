// Fill out your copyright notice in the Description page of Project Settings.


#include "Menu/MainMenuSecondaryButtonWidget.h"

#include "MainMenuWidget.h"

void UMainMenuSecondaryButtonWidget::StartNewGame()
{
	UMainMenuWidget::Get()->StartNewGame();
}

void UMainMenuSecondaryButtonWidget::ResumeLatestGame()
{
	UMainMenuWidget::Get()->ResumeLatestGame();
}

void UMainMenuSecondaryButtonWidget::OpenLoadGame()
{
	UMainMenuWidget::Get()->OpenLoadGame();
}

void UMainMenuSecondaryButtonWidget::OpenOptions()
{
	UMainMenuWidget::Get()->OpenOptions();
}

void UMainMenuSecondaryButtonWidget::QuitGame()
{
	UMainMenuWidget::Get()->QuitGame();
}