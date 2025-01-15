// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AuraAIController.h"

#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

AAuraAIController::AAuraAIController()
{
	BTComp = CreateDefaultSubobject<UBehaviorTreeComponent>("BehaviorTreeComp");

	BBComp = CreateDefaultSubobject<UBlackboardComponent>("BlackboardComp");
}
