#include "Widgets/AzulWidgetHUDPlayer.h"
#include "AzulSubsystem/AzulGameSubsystem.h"
#include "Engine/GameInstance.h"
#include "Libraries/AzulLibrary.h"

void UAzulWidgetHUDPlayer::SetUIState(EInteractUIState NewState)
{
    if (!InteractImage || !CrosshairTexture)
        return;

    InteractImage->SetBrushFromTexture(CrosshairTexture, true);

    switch (NewState)
    {
    case EInteractUIState::Default:
        InteractImage->SetColorAndOpacity(DefaultColor);
        break;

    case EInteractUIState::Active:
        InteractImage->SetColorAndOpacity(ActiveColor);
        break;
    }
}

void UAzulWidgetHUDPlayer::SetStoryText(const FString& NewText, float Delay)
{
    if (!GetWorld())
    {
        return;
    }

    const FString FinalText = UAzulLibrary::ReplaceSonName(this, NewText);

    GetWorld()->GetTimerManager().ClearTimer(StoryTextTimer);

    if (FinalText.IsEmpty())
    {
        if (StoryText)
        {
            StoryText->SetText(FText::GetEmpty());
        }

        if (TextBorder)
        {
            TextBorder->SetVisibility(ESlateVisibility::Hidden);
        }

        return;
    }

    if (StoryText)
    {
        StoryText->SetText(FText::FromString(FinalText));
    }

    if (TextBorder)
    {
        TextBorder->SetVisibility(ESlateVisibility::Visible);
    }

    if (Delay > 0.0f)
    {
        GetWorld()->GetTimerManager().SetTimer(
            StoryTextTimer,
            this,
            &UAzulWidgetHUDPlayer::ClearStoryText,
            Delay,
            false
        );
    }
}

void UAzulWidgetHUDPlayer::ClearStoryText()
{
    if (StoryText)
    {
        StoryText->SetText(FText::GetEmpty());
    }

    if (TextBorder)
    {
        TextBorder->SetVisibility(ESlateVisibility::Hidden);
    }
}
