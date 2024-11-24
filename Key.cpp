#include "Key.h"

unsigned __int64 primarykey;
unsigned __int64 extendkey;
unsigned __int32 C0, D0;
unsigned __int64 Keys[16];
void MakePrimaryKeyHand(string Handkey)
{
	unsigned char ch = 0;
	unsigned int b = 0;
	for (int byt = 0; byt < 8; byt++) {
		ch = Handkey.at(byt);
		for (int bit = 0; bit < 8; bit++) {
			b = (ch >> (7 - bit)) & 0b1;
			primarykey = primarykey + ((ch >> (7 - bit)) & 0b1);
			if (byt != 7 || bit != 7)
				primarykey = primarykey << 1;
		}
	}
	PrintDigit(">>>>MakePrimaryKeyHand<<<<", primarykey, 64);
}
//изготовление первичного ключа случайным образом
void MakePrimaryKeyRandom() {
	random_device rd;
	mt19937 gen(rd());
	int a = 0;
	primarykey = 0;
	for (int i = 0; i < 64; i++) {
		uniform_int_distribution<int> dist(0, 1);
		a = dist(gen);	
		primarykey = (primarykey + a);
		if (i < 63)
			primarykey = primarykey << 1;
	}
}
//изготовление расширенного ключа
void MakeExtendKey() {
	unsigned int count = 0, a = 0, j = 0;
	unsigned __int64 buf = 0;
	extendkey = 0;
	for (int i = 0; i < 64; i++) {
		a = (primarykey >> (63 - i) & 0b1);
		if (a)
			count++;
		j++;
		if (j == 8) {
			extendkey = (extendkey + ((count + 1) % 2));
			count = 0;
			if (i < 63)
				extendkey = extendkey << 1;
			j = 0;
		}
		else {
			extendkey = (extendkey + a);
			if (i < 63)
				extendkey = extendkey << 1;
		}
	}
}
//перестановка расширенного ключа и определение блоков С и D
void MakeBloksCD() {
	const int table5[56] = { 57, 49, 41, 33, 25, 17, 9, 1, 58, 50, 42, 34, 26, 18,
							10, 2, 59, 51, 43, 35, 27, 19, 11, 3, 60, 52, 44, 36,
							63, 55, 47, 39, 31, 23, 15, 7, 62, 54, 46, 38, 30, 22,
							14, 6, 61, 53, 45, 37, 29, 21, 13, 5, 28, 20, 12, 4 };

	for (int i = 0; i < 28; i++)
	{
		C0 = C0 + ((extendkey >> (64 - table5[i])) & 0b1);
		if (i < 27)
			C0 = C0 << 1;
		D0 = D0 + ((extendkey >> (64 - table5[i + 28])) & 0b1);
		if (i < 27)
			D0 = D0 << 1;
	}
}

unsigned __int64 ShiftBloksCD_Left(int Circle, unsigned __int32* Ci, unsigned __int32* Di) {
	const int table6[16] = { 1,1,2,2,2,2,2,2,1,2,2,2,2,2,2,1 };
	const int maski[16] = { 0b1,0b1,0b11,0b11,0b11,0b11,0b11,0b11,0b1,0b11,0b11,0b11,0b11,0b11,0b11,0b1 };
	unsigned __int32 buf = 0;
	buf = (*Ci >> (28 - table6[Circle])) & maski[Circle];
	*Ci = (*Ci << table6[Circle]) + buf;
	*Ci = (*Ci << (3 + table6[Circle])) >> (3 + table6[Circle]);//PrintDigit(">>>>Ci<<<<", *Ci, 28);
	buf = 0;
	buf = (*Di >> (28 - table6[Circle])) & maski[Circle];
	*Di = (*Di << table6[Circle]) + buf;
	*Di = (*Di << (3 + table6[Circle])) >> (3 + table6[Circle]);	//PrintDigit(">>>>Di<<<<", *Di, 28);
	unsigned __int64 CD = 0;
	CD = ((CD + (*Ci)) << 28) + (*Di);
	//	PrintDigit(">>>>CD<<<<", CD, 56);
	const int table7[48] = { 14, 17, 11, 24, 1, 5, 3, 28, 15, 6, 21, 10, 23, 19, 12, 4,
								26, 8, 16, 7, 27, 20, 13, 2, 41, 52, 31, 37, 47, 55, 30, 40,
								51, 45, 33, 48, 44, 49, 39, 56, 34, 53, 46, 42, 50, 36, 29, 32 };
	unsigned __int64 Key = 0;
	for (int k = 0; k < 48; k++) {
		Key = Key + ((CD >> (48 - table7[k])) & 0b1);
		if (k < 47)		Key = Key << 1;
	}//PrintDigit(">>>>ShiftBloksCD_Left  Сдвинутый ключ<<<<", Key, 48);
	return Key;
}

void InitMassivKeys() {
	unsigned __int32* C = &C0, * D = &D0;
	for (int k = 0; k < 16; k++) {
		Keys[k] = ShiftBloksCD_Left(k, C, D);
	}
}

unsigned __int64 GetKey(int circle) {
	return Keys[circle];
}

void PrintDigit(string NameFun, unsigned __int64 val, int size) {
	cout << "---------------------------------------------------------------" << endl;
	cout << NameFun << endl;
	for (int i = 1; i <= size; i++)
		cout << ((val >> (size - i)) & 0b1);
	cout << endl;
}