#include <iostream>
#include <string>
#include <random>

using namespace std;

void MakePrimaryKeyHand(string Handkey);
void MakePrimaryKeyRandom();
void MakeExtendKey();
void MakeBloksCD();
void InitMassivKeys();
unsigned __int64 GetKey(int circle);
unsigned __int64 ShiftBloksCD_Left(int Circle, unsigned __int32* Ci, unsigned __int32* Di);
void PrintDigit(string NameFun, unsigned __int64 val, int size);