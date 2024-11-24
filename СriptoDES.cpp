#include <windows.h>
#include "Key.h"
#include "DES.h"

string message, cod;
unsigned __int64* Cod;
bool Check(string s);
void Decoder();
void Coder();
int Menu();
void Transmitted(unsigned __int64* Cod, int Size);

int main(){
	setlocale(LC_ALL, "RU");
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);
	while (true) {
		if (Menu() == -100) break;
		Coder();
		Decoder();
		delete Cod;
	}
	system("pause");
	return 1;
};

//Выбор действия
int Menu() {
	bool Exit = true;
	while (Exit){
		int Key = 0;
		cout << endl << "Задать ключ автоматически : 1" << endl << "Задать ключ с клавиатуры : 2" << endl;
		cout << "Ввести текст от руки: 3" << endl << "Завершить работу : 4" << endl << endl << "Введите номер пункта : ";
		cin >> Key;
		cin.ignore(1);
		string str = "";
		switch (Key){
		case 1:
			cout << "Создан ключ" << endl << endl;
			MakePrimaryKeyRandom();
			Exit = true;
			break;
		case 2:
			cout << "Введите 8 символов ключа" << endl;
			str.clear();
			getline(cin, str);
			if (!Check(str)) {
				Exit = true;
			}
			else {
				cout << "Создан ключ" << endl << endl;
				MakePrimaryKeyHand(str);
				Exit = true;
			}
			break;
		case 3:
			message.clear();
			getline(cin, message);
			Exit = false;
			break;
		case 4:
			return -100;
		default:
			cout << "Неверный номер меню!" << endl;
			Exit = true;
			break;
		}
	}
	return 1;
}

bool Check(string s) {
	size_t l = s.length();
	if (l > 8) {
		cout << "Введённая строка содержит более восьми символов" << endl;
		return false;
	}
	return true;
}

void Coder() {
	MakeExtendKey();
	MakeBloksCD();
	InitMassivKeys();
	unsigned __int64 LR = 0;
	size_t size_m = message.length();
	for (int c = 0; c < 8 - (size_m % 8); c++)
		message.push_back('#');
	size_m = message.length();
	unsigned __int64 Text = 0;
	unsigned __int64 BlokCod = 0;
	const int QuantityBloks = size_m / 8;
	Cod = new unsigned __int64[QuantityBloks];
	unsigned char ch = 0;
	unsigned int b = 0;
	for (unsigned __int64 blk = 0; blk < QuantityBloks; blk++) {//цикл по блокам 8 байт сообщения
		for (int byt = 0; byt < 8; byt++) {
			ch = message.at(blk * 8 + byt);
			for (int bit = 0; bit < 8; bit++) {
				b = (ch >> (7 - bit)) & 0b1;
				Text = Text + ((ch >> (7 - bit)) & 0b1);
				if (byt != 7 || bit != 7)
					Text = Text << 1;
			}
		}//PrintDigit(">>>>Coder  Вывод блока текста<<<<", Text, 64);
		Text = BeginChange(Text);	//начальная перестановка
		unsigned __int32 L = Text >> 32;		//цикл Фейстеля
		unsigned __int32 L1 = 0;	            //деление на левую и правую часть
		unsigned __int32 R = (Text << 32) >> 32;	//		PrintDigit(">>>>Coder  Вывод L<<<<", L, 32);
		for (int f = 0; f < 16; f++) {		        //функция Фейстеля
			L1 = R;
			unsigned __int64 R_ex = Extend_R(R);	    //расширение R до 48 бит
			unsigned __int64 XOR = R_ex ^ GetKey(f);	//сложение по модулю 2
			R = L ^ EndChange_Func(S_B8(XOR));          //PrintDigit(">>>>Coder  Вывод L = R<<<<", L, 28);
			L = L1;                          //	PrintDigit(">>>>Coder  Вывод R=L^EndChange_Func(S_B8(XOR))  <<<<", R, 32);
		}
		LR = 0;
		LR = ((LR + L) << 32) + R;
		Cod[blk] = EndChange(LR);
		PrintDigit(">>>>Coder  Вывод Cod[blk]  <<<<", Cod[blk], 64);
	}
	Transmitted(Cod, QuantityBloks);
}

void Transmitted(unsigned __int64* Cod, int Size) {
	char s = 0;
	cod.clear();
	cout << ">>>>Transmitted<<<<" << endl;
	for (int c = 0; c < Size; c++)
		for (int b = 7; b >= 0; b--) {
			s = (Cod[c] >> (b * 8)) & 0b11111111;
			cod.push_back(s);
		}
	cout << cod;
	cout << endl << ">>>>Transmitted<<<<" << endl;
}

void Decoder() {
	unsigned __int64 LR = 0;
	size_t size_m = cod.length();
	unsigned __int64 Text = 0;
	unsigned __int64 BlokCod = 0;
	const int QuantityBloks = size_m / 8;
	for (unsigned __int64 blk = 0; blk < QuantityBloks; blk++) {//цикл по блокам 8 байт сообщения
		Text = Cod[blk];
		PrintDigit(">>>>Decoder  Вывод Cod[blk]  <<<<", Cod[blk], 64);//PrintDigit(">>>>Decoder  Вывод блока шифра<<<<", Text, 64);
		Text = BeginChange(Text);//начальная перестановка
		//цикл Фейстеля
		unsigned __int32 L = Text >> 32;	//деление на левую и правую часть
		unsigned __int32 L1 = 0;
		unsigned __int32 R = Text << 32 >> 32;//		PrintDigit(">>>>Decoder  Вывод L<<<<", L, 32);
		unsigned __int32 R1 = 0;//		PrintDigit(">>>>Decoder  Вывод R<<<<", R, 32);
		for (int f = 15; f >= 0; f--) {//функция Фейстеля
			R1 = L;
			unsigned __int64 L_ex = Extend_R(L);//расширение R до 48 бит
			unsigned __int64 XOR = L_ex ^ GetKey(f);	//сложение по модулю 2
			L = R ^ EndChange_Func(S_B8(XOR));//PrintDigit(">>>>Decoder  Вывод L_ex ^ ShiftBloksCD_Right(f, &C, &D)<<<<", XOR, 48);
			R = R1;    //PrintDigit(">>>>Decoder  Вывод R=L^EndChange_Func(S_B8(XOR))  <<<<", R, 32);
		}
		LR = 0;
		LR = ((LR + L) << 32) + R;
		Cod[blk] = EndChange(LR);
	}
	Transmitted(Cod, QuantityBloks);
}