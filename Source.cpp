#include <iostream>
#include <fstream>
using namespace std;

void readFile(double**& basis, ifstream& file, int& n) {

	file >> n; // Чтение кол-ва

	// Инициализация Basis массива
	basis = new double* [n];
	for (int i = 0; i < n; i++)
		basis[i] = new double[2];

	// Чтение X
	for (int i = 0; i < n; i++)
		file >> basis[i][0];

	// Чтение Y
	for (int i = 0; i < n; i++)
		file >> basis[i][1];

}

void fromBasisToMatr(double** basis, double**& matr, int n, int exclude = -1) {

	if (exclude == -1) { // Ничего не исключать
		// Инициализация Matr
		matr = new double* [2 * n - 1];
		for (int i = 0; i < (2 * n - 1); i++)
			matr[i] = new double[5];

		// Заполнение Matr
		int counter = 0;
		for (int i = 0; i < (2 * n - 1); i += 2) {
			matr[i][0] = basis[counter][0];
			matr[i][1] = basis[counter][1];
			counter++;
		}
	}
	else { // Исключить строку exclude
		// Инициализация Matr
		matr = new double* [2 * (n - 1) - 1];
		for (int i = 0; i < (2 * (n - 1) - 1); i++)
			matr[i] = new double[5];

		// Заполнение Matr
		int counter = 0;
		for (int i = 0; i < n; i++) {
			if (i != exclude) {
				matr[counter][0] = basis[i][0];
				matr[counter][1] = basis[i][1];
				counter += 2;
			}
		}
	}
}

bool pp(double**& matr, int n) { //Вычисление РР и проверка рав-ва PP3
	for (int i = 1; i < (2*n - 2); i += 2) { // PP1
		matr[i][2] = ((matr[i - 1][1] - matr[i + 1][1]) / (matr[i - 1][0] - matr[i + 1][0]));
	}
	for (int i = 2; i < (2 * n - 3); i += 2) { // PP2
		matr[i][3] = ((matr[i - 1][2] - matr[i + 1][2]) / (matr[i - 2][0] - matr[i + 2][0]));
	}
	for (int i = 3; i < (2 * n - 4); i += 2) { // PP3
		matr[i][4] = ((matr[i - 1][3] - matr[i + 1][3]) / (matr[i - 3][0] - matr[i + 3][0]));
	}
	for (int i = 3; i < (2 * n - 6); i += 2) {
		if (matr[i][4] != matr[i + 2][4])
			return 0;
	}
	return 1;
}

int searchErrorString(double** basis, double**& newMatr, int n) {
	for (int i = 0; i < n; i++) {
		fromBasisToMatr(basis, newMatr, n, i);
		if (pp(newMatr, n - 1))
			return i;
	}
}

int main() {
	setlocale(LC_ALL, "RUS");

	int n;

	double** basis;
	double** matr;
	
	// Чтение файла и заполнение Basis массива
	ofstream outf("output.txt");
	ifstream file("input.txt");
	readFile(basis, file, n);

	// IER=2
	if (n < 6) {
		outf << "IER=2";
		return 0;
	}

	// IER=3
	for (int i = 0; i < (n - 1); i++) {
		if (basis[i][0] > basis[i + 1][0]) {
			outf << "IER=3";
			return 0;
		}
	}

	fromBasisToMatr(basis, matr, n);

	if (pp(matr, n)) {
		cout << "Нет ошибок!";
		outf << "IER=1";
	}
		
	else {
		cout << "Есть ошибка!" << endl;
		outf << "IER=0" << endl;
		double** newMatr;
		int errorString = searchErrorString(basis, newMatr, n);
		int x = basis[errorString][0];
		cout << "Ошибка в X = " << x << endl;

		// Исправление 
		double N3 = newMatr[0][1] + newMatr[1][2] * (x - newMatr[0][0]) + newMatr[2][3] * (x - newMatr[0][0]) * (x - newMatr[2][0]) +
			newMatr[3][4] * (x - newMatr[0][0]) * (x - newMatr[2][0]) * (x - newMatr[4][0]);
		basis[errorString][1] = N3; // Записываем в исходный массив правильное значение
		cout << "При X = " << x << " должно быть значение: " << N3;

		// Вывод в файл
		for (int i = 0; i < n; i++) {
			outf << basis[i][1] << ' ';
		}
		outf << endl;
		outf << "Коэффициенты полинома:" << endl;
		outf << "C0:\t" << newMatr[0][1] << endl;
		outf << "C1:\t" << newMatr[1][2] << endl;
		outf << "C2:\t" << newMatr[2][3] << endl;
		outf << "C3:\t" << newMatr[3][4] << endl;

	}
	return 0;
}