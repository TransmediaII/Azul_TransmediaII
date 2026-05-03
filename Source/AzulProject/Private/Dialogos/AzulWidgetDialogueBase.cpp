/*
 * UAzulWidgetDialogueBase - Implementación
 * ------------------------------------------------------
 * Este archivo implementa el comportamiento del widget de diálogo.
 *
 * Aquí se gestiona:
 * - El botón de continuar.
 * - Los botones de elección.
 * - La actualización del texto visible en pantalla.
 * - El click general sobre el widget para avanzar diálogo
 *   cuando la fila actual no es una decisión.
 *
 * Este archivo actúa como puente entre la UI del widget
 * y el sistema central de diálogo.
 */

#include "Dialogos/AzulWidgetDialogueBase.h"
#include "Dialogos/AzulDialogue.h"
#include "AzulSubsystem/AzulGameSubsystem.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Engine/GameInstance.h"
#include "Input/Reply.h"
#include "InputCoreTypes.h"
#include "Libraries/AzulLibrary.h"

void UAzulWidgetDialogueBase::NativeConstruct()
{
    Super::NativeConstruct();

    if (ButtonContinue)
    {
        ButtonContinue->OnClicked.RemoveDynamic(this, &UAzulWidgetDialogueBase::PressContinue);
        ButtonContinue->OnClicked.AddUniqueDynamic(this, &UAzulWidgetDialogueBase::PressContinue);
    }

    if (ChoiceButton1)
    {
        ChoiceButton1->OnClicked.RemoveDynamic(this, &UAzulWidgetDialogueBase::HandleChoice1);
        ChoiceButton1->OnClicked.AddUniqueDynamic(this, &UAzulWidgetDialogueBase::HandleChoice1);
    }

    if (ChoiceButton2)
    {
        ChoiceButton2->OnClicked.RemoveDynamic(this, &UAzulWidgetDialogueBase::HandleChoice2);
        ChoiceButton2->OnClicked.AddUniqueDynamic(this, &UAzulWidgetDialogueBase::HandleChoice2);
    }

    if (ChoiceButton3)
    {
        ChoiceButton3->OnClicked.RemoveDynamic(this, &UAzulWidgetDialogueBase::HandleChoice3);
        ChoiceButton3->OnClicked.AddUniqueDynamic(this, &UAzulWidgetDialogueBase::HandleChoice3);
    }

    if (ChoiceButton4)
    {
        ChoiceButton4->OnClicked.RemoveDynamic(this, &UAzulWidgetDialogueBase::HandleChoice4);
        ChoiceButton4->OnClicked.AddUniqueDynamic(this, &UAzulWidgetDialogueBase::HandleChoice4);
    }
}

void UAzulWidgetDialogueBase::PressContinue()
{
    if (UGameInstance* GI = GetGameInstance())
    {
        if (UAzulGameSubsystem* GameSubsystem = GI->GetSubsystem<UAzulGameSubsystem>())
        {
            GameSubsystem->RequestAdvanceDialogue();
        }
    }
}

void UAzulWidgetDialogueBase::PressChoice(int32 ChoiceIndex)
{
    if (!Dialogue)
    {
        UE_LOG(LogTemp, Error, TEXT("DialogueWidget: Dialogue es NULL"));
        return;
    }

    Dialogue->OnChoiceClicked(ChoiceIndex);

    if (UGameInstance* GI = GetGameInstance())
    {
        if (UAzulGameSubsystem* GameSubsystem = GI->GetSubsystem<UAzulGameSubsystem>())
        {
            GameSubsystem->RefreshDialogueWidget();
        }
    }
}

void UAzulWidgetDialogueBase::SetDialogueText(const FString& NewText)
{
    if (!DialogueTextBlock)
    {
        return;
    }

    DialogueTextBlock->SetText(FText::FromString(NewText));
}

FString UAzulWidgetDialogueBase::GetDialogueTextString() const
{
    if (!DialogueTextBlock)
    {
        return FString();
    }

    return DialogueTextBlock->GetText().ToString();
}

FReply UAzulWidgetDialogueBase::NativeOnMouseButtonDown(
    const FGeometry& InGeometry,
    const FPointerEvent& InMouseEvent
)
{
    if (!InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton))
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    if (!Dialogue || !Dialogue->CurrentRow)
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    if (Dialogue->CurrentRow->IsDecision)
    {
        return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
    }

    PressContinue();
    return FReply::Handled();
}

void UAzulWidgetDialogueBase::RefreshDecisionUI()
{
    if (!GetWorld())
    {
        return;
    }

    GetWorld()->GetTimerManager().ClearTimer(DecisionsDelayTimer);

    if (!Dialogue || !Dialogue->CurrentRow)
    {
        if (TextName)
        {
            TextName->SetText(FText::GetEmpty());
        }

        if (ButtonContinue)
        {
            ButtonContinue->SetVisibility(ESlateVisibility::Collapsed);
        }

        if (ChoiceButton1) ChoiceButton1->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton2) ChoiceButton2->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton3) ChoiceButton3->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton4) ChoiceButton4->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    SetSpeakerName(Dialogue->CurrentRow->Name);

    const bool bIsDecision = Dialogue->CurrentRow->IsDecision;
    const int32 NumChoices = Dialogue->CurrentRow->ChoicesText.Num();

    if (ButtonContinue)
    {
        ButtonContinue->SetVisibility(ESlateVisibility::Visible);
        ButtonContinue->SetIsEnabled(!bIsDecision);
    }

    auto SetupChoiceLabel = [](UTextBlock* Label, const FString& Text)
        {
            if (Label)
            {
                Label->SetText(FText::FromString(Text));
            }
        };

    SetupChoiceLabel(ChoiceText1, NumChoices > 0 ? Dialogue->CurrentRow->ChoicesText[0] : FString());
    SetupChoiceLabel(ChoiceText2, NumChoices > 1 ? Dialogue->CurrentRow->ChoicesText[1] : FString());
    SetupChoiceLabel(ChoiceText3, NumChoices > 2 ? Dialogue->CurrentRow->ChoicesText[2] : FString());
    SetupChoiceLabel(ChoiceText4, NumChoices > 3 ? Dialogue->CurrentRow->ChoicesText[3] : FString());

    if (!bIsDecision)
    {
        if (ChoiceButton1) ChoiceButton1->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton2) ChoiceButton2->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton3) ChoiceButton3->SetVisibility(ESlateVisibility::Collapsed);
        if (ChoiceButton4) ChoiceButton4->SetVisibility(ESlateVisibility::Collapsed);
        return;
    }

    if (ChoiceButton1) ChoiceButton1->SetVisibility(ESlateVisibility::Collapsed);
    if (ChoiceButton2) ChoiceButton2->SetVisibility(ESlateVisibility::Collapsed);
    if (ChoiceButton3) ChoiceButton3->SetVisibility(ESlateVisibility::Collapsed);
    if (ChoiceButton4) ChoiceButton4->SetVisibility(ESlateVisibility::Collapsed);

    GetWorld()->GetTimerManager().SetTimer(
        DecisionsDelayTimer,
        this,
        &UAzulWidgetDialogueBase::ShowDecisionButtons,
        2.0f,
        false
    );
}

void UAzulWidgetDialogueBase::ShowDecisionButtons()
{
    if (!Dialogue || !Dialogue->CurrentRow || !Dialogue->CurrentRow->IsDecision)
    {
        return;
    }

    const int32 NumChoices = Dialogue->CurrentRow->ChoicesText.Num();

    if (ChoiceButton1) ChoiceButton1->SetVisibility(NumChoices > 0 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    if (ChoiceButton2) ChoiceButton2->SetVisibility(NumChoices > 1 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    if (ChoiceButton3) ChoiceButton3->SetVisibility(NumChoices > 2 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
    if (ChoiceButton4) ChoiceButton4->SetVisibility(NumChoices > 3 ? ESlateVisibility::Visible : ESlateVisibility::Collapsed);
}

void UAzulWidgetDialogueBase::SetSpeakerName(const FString& NewName)
{
    if (!TextName)
    {
        return;
    }

    if (BorderName && TextName)
    {
        BorderName->SetVisibility(
            TextName->GetText().IsEmpty()
            ? ESlateVisibility::Hidden
            : ESlateVisibility::Visible
        );
    }

    FString FinalText = UAzulLibrary::ReplaceSonName(this, NewName);
    TextName->SetText(FText::FromString(FinalText));
}

void UAzulWidgetDialogueBase::HandleChoice1()
{
    PressChoice(0);
}

void UAzulWidgetDialogueBase::HandleChoice2()
{
    PressChoice(1);
}

void UAzulWidgetDialogueBase::HandleChoice3()
{
    PressChoice(2);
}

void UAzulWidgetDialogueBase::HandleChoice4()
{
    PressChoice(3);
}

