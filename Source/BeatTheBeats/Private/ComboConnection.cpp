// Fill out your copyright notice in the Description page of Project Settings.


#include "ComboConnection.h"

FComboConnection::FComboConnection()
{
}

FComboConnection::FComboConnection(Attacks connectingAttack, int fromComboStep, int toCombo) : AttackWhichConnects(connectingAttack), FromComboStep(fromComboStep), ToCombo(toCombo)
{
}

FComboConnection::~FComboConnection()
{
}
