#include <iostream>
#include <string>
#include <bitset>
#include <map>
#include <vector>
#include <fstream>

#include <boost/multiprecision/cpp_int.hpp>
#include <chrono>
class BigNumber {
private:
	std::vector<int> N;
	void Output(std::vector<int> Npriv) { // использовался только при поиске ошибок в коде
		for (int i = Npriv.size() - 1; i >= 0; i--) {
			std::cout << Npriv[i];
		}
	}
	std::vector<int> clear0(std::vector<int> No0) { // очистка значений от незначащих нулей, так как докидывал нули для получения одинаковой размерности
		int i = No0.size() - 1;
		while ((No0[i] == 0) && (i > 0)) {
			No0.pop_back();
			i--;
		}
		return No0;
	}
	std::string toString(std::vector<int> Nnew) { // в возврощаемом значении нужно было вернуть тип данных BigNumber, а он строится из строкового значения. Поэтому такой костыль
		Nnew = clear0(Nnew);
		std::string Str;
		for (int i = Nnew.size() - 1; i >= 0; i--) {
			Str.push_back((char)(Nnew[i] + 48));
		}
		return Str;
	}
	int equality(std::vector<int> N1, std::vector<int> N2) { // 3 - одинаковы, 2 - N2 больше, 1 - N1 больше
		N1 = clear0(N1); N2 = clear0(N2);
		if (N1.size() > N2.size()) { return 1; } // Проверка по размерности
		else {
			if (N2.size() > N1.size()) { return 2; } // Проверка по размерности
			else {
				for (int i = N1.size() - 1; i >= 0; i--) { // Проверка от старшего разряда к младшему
					if (N1[i] > N2[i]) { return 1; }
					if (N2[i] > N1[i]) { return 2; }
				}
			}
		}
		return 3;
	}
	std::vector<int> sum(std::vector<int> N1, std::vector<int> N2) {
		if (N2.size() > N1.size()) { // Доводил числа до одинаковой размерности
			while (N2.size() != N1.size()) { N1.push_back(0); }
		}
		else {
			while (N2.size() != N1.size()) { N2.push_back(0); }
		}
		N1.push_back(0); N2.push_back(0); // Сумма последних разрядов может быть больше 10
		for (int i = 0; i < N2.size() - 1; i++) {
			N1[i] += N2[i]; // результат сразу в существующую переменную, так как этот разряд дальше не понадобится, а тратить память на результирующее значение не хочется
			N1[i + 1] += N1[i] / 10; // если больше 9, то повышаю последующий разряд
			N1[i] %= 10; // идиницы от суммы цифр записываются в виде результата
		}
		N1 = clear0(N1); // чищу итоговое значение от лишних единиц
		return N1;
	}
	std::vector<int> multiplication(std::vector<int> N1, std::vector<int> N2) {
		std::vector<int> NResult(N1.size() + N2.size(), 0); // результат сразу с максимально возможным размером
		for (int i = 0; i < N1.size(); i++) { // итератор по нижнему значению (меньшему числу)
			for (int j = 0; j < N2.size(); j++) { // итератоп по верхнему значению (большему числу)
				NResult[i + j] += N1[i] * N2[j]; 
				NResult[i + j + 1] += NResult[i + j] / 10;
				NResult[i + j] %= 10;
			}
		}
		NResult = clear0(NResult); // очистка от лишних нулей
		return NResult;
	}
	std::vector<int> diff(std::vector<int> NGreater, std::vector<int> NLess) {
		while (NGreater.size() != NLess.size()) { NLess.push_back(0); }
		std::vector<int> res(NGreater.size(), 0);
		for (int i = 0; i < NGreater.size(); i++) {
			if (i < (NGreater.size() - 1)) {
				NGreater[i + 1]--; // берём единицу из разряда выше сразу
				res[i] += 10 + NGreater[i];
			}
			else { res[i] += NGreater[i]; }
			res[i] -= NLess[i];
			if (res[i] / 10 > 0) { // если единица из разряда выше была не нужна
				NGreater[i + 1]++; // возврощаем единицу
				res[i] %= 10; // записываем корректный результат
			}
		}
		return res;
	}
	std::vector<int> difference(std::vector<int> N1, std::vector<int> N2) { // нет возможности работы с отрицательными числами, можно доработать но лень
		if (equality(N1, N2) == 1) { return diff(N1, N2); }
		else { return diff(N2, N1); }
	}
	std::vector<int> mod(std::vector<int> N1, std::vector<int> Nmod) {
		Nmod = clear0(Nmod);
		if (Nmod[Nmod.size() - 1] == 0) { // если число начинается с нуля - значит идет деление на ноль
			std::cout << "Division by zero" << std::endl;
			return std::vector<int> {1, 0};
		}
		std::vector<int> InterDivisible(1, 0); // промежуточное делимое
		if (equality(N1, Nmod) == 2) { return N1; } // если модуль числа больше - значит сразу возвращаю это число
		int i = N1.size() - 1;
		while (i >= 0) {
			while (equality(InterDivisible, Nmod) == 2) {
				InterDivisible.insert(InterDivisible.begin(), N1[i]);
				//InterDivisible.push_back(N1[i]);
				//if (i < 0) { break; }
				i--;
				if (i < 0) { break; }
			}
			while (equality(InterDivisible, Nmod) != 2) { // добавив отслеживание количества итераций можно получить частное
				InterDivisible = clear0(InterDivisible);
				InterDivisible = difference(InterDivisible, Nmod);
			}
		}
		return InterDivisible;
	}
public:
	BigNumber(std::string S_N) {
		std::vector<int> Nnew;
		for (int i = 0; i < S_N.length(); i++) {
			Nnew.push_back(int(S_N[i]) - 48); // из char в int используя порядковый номер в кодировке ascii
		}
		reverse(Nnew.begin(), Nnew.end()); // запись чисел в обратном порядки для привычного написания циклов for
		N = Nnew;
	};
	BigNumber operator + (BigNumber& NR) { // правый аргумент в скобках, перегрузка оператора сложения
		return BigNumber{ toString(sum(N, NR.N)) };
	}
	BigNumber operator * (BigNumber& NR) { // правый аргумент в скобках, перегрузка оператора умножения
		return BigNumber{ toString(multiplication(N, NR.N)) };
	}
	BigNumber operator % (BigNumber& NR) { // модуль числа в скобках, перегрузка оператора взятия модуля
		return BigNumber{ toString(mod(N, NR.N)) };
	}
	std::string ToString() { // использовал при тестировании 
		return toString(N);
	}
	BigNumber operator - (BigNumber& NR) { // вычитаемое числа в скобках, перегрузка оператора взятия вычитания, использовал при тестировании для последующего использования в взятии по модулю
		return BigNumber{ toString(difference(N, NR.N)) };
	}
	void Output() {
		for (int i = N.size() - 1; i >= 0; i--) {
			std::cout << N[i];
		}
	}
};
const int Size = 8;
void help(){
    std::cout << "" << std::endl; // лень добавлять
}
void output(int N) {
    std::cout << (std::bitset<Size>(N)) << "<-- binary representation" << std::endl;
    std::cout << N << "<-- decimal representation" << std::endl;
    printf("%02X ", N);
    std::cout << "<-- hexadecimal representation" << std::endl;
}
void BinaryRepresentationOfData(std::vector<std::string> Command) {
    if (Command.size() < 2) { std::cout << "too few arguments" << std::endl; exit(1); } // проверка количества аргументов
    std::string Path = Command[1];
    std::ifstream TextFile;
    TextFile.open(Path);
    if (!TextFile.is_open()) {
        std::cout << "file opening error" << std::endl;
    }
    else {
        char ch;
        int N;
        while (TextFile.get(ch)) {
            N = static_cast<int>(ch); // ch в int
            if (N < 0) { N += 192; }
            if (Command[0] == "hex8") { printf("%02X ", N); }
            else if (Command[0] == "dec8") { printf("%03d ", N); }
            else if (Command[0] == "hex16") { printf("%04X ", N); }
            else if (Command[0] == "dec16") { printf("%05d ", N); }
            else if (Command[0] == "hex32") { printf("%08X ", N); }
            else { std::cout << "unrecognized command" << std::endl; }
        }
    }
    TextFile.close();
	std::cout << std::endl;
}
void mix(unsigned long long Number1, unsigned long long Number2) {
	unsigned long long result = 0;
	unsigned long long matrix[8][2];
	int position;
	int N;
	for (struct { int i; int j; } n = { 0, 7 }; n.i < 8; n.i++) {
		matrix[n.i][0] = n.j;
		matrix[n.i][1] = n.i;
		n.j--;
	} // matrix [[0, 7], [1, 6], [2, 5], [3, 4], [4, 3], [5, 2], [6, 1], [7, 0]]
	for (int i = 0; i < 8; i++) {
		position = 0;
		N = Number1;
		while (N % 10 != i) {
			N = static_cast<int>(N / 10);
			position++;
		}
		if ((Number2 & (unsigned long long)std::pow(2, position)) != 0) {
			result += (unsigned long long)std::pow(2, matrix[i][0]);
		}
	}
	output(result);
}
void BitOperations(std::vector<std::string> Command) {
    if (Command.size() < 3) { std::cout << "too few arguments" << std::endl; exit(1); } // проверка количества аргументов
	unsigned long long Number1 = std::stoi(Command[1]);
	unsigned long long Number2 = std::stoi(Command[2]);
	std::cout << (std::bitset<Size>(Number1)) << "<-- binary representation Number1" << std::endl;
	std::cout << (std::bitset<Size>(Number2)) << "<-- binary representation Number2" << std::endl;
	if (Command[0] == "xor") { output(Number1 ^ Number2); }
	else if (Command[0] == "and") { output(Number1 & Number2); }
	else if (Command[0] == "or")  { output(Number1 | Number2); }
	else if (Command[0] == "set1") { output(Number2 | (unsigned long long)(std::pow(2, Number1))); }
	else if (Command[0] == "set0") { output(Number2 & ~(unsigned long long)std::pow(2, Number1)); }
	else if (Command[0] == "shl") { output(Number2 << Number1); }
	else if (Command[0] == "shr") { output(Number2 >> Number1); }
	else if (Command[0] == "rol") {
		while (Number1 > 0) {
			if ((Number2 & (unsigned long long)(std::pow(2, sizeof(Number2) - 1))) == 0) {
				Number2 = Number2 << 1;
				Number1 -= 1;
			}
			else
			{
				Number2 = Number2 << 1;
				Number2 = Number2 | 1;
				Number1 -= 1;
			}
		}
		output(Number2);
	}
	else if (Command[0] == "ror") {
		while (Number1 > 0) {
			if ((Number2 & 1) == 1) { // проверка первого бита на 1 (проверка на четность)
				Number2 = Number2 >> 1;
				Number2 = Number2 | (unsigned long long)(std::pow(2, sizeof(Number2) - 1));
				Number1 -= 1;
			}
			else {
				Number2 = Number2 >> 1;
				Number1 -= 1;
			}
		}
		output(Number2);
	}
	else if (Command[0] == "mix") {
		std::cout << Command[1] << "<-- bit order" << std::endl;
		mix(Number1, Number2);
	}
}
void ArithmeticOfLargeNumbers(std::vector<std::string> Command) {
	if (Command.size() < 3) { std::cout << "too few arguments" << std::endl; exit(1); } // проверка количества аргументов
	BigNumber N1{ Command[1] }, N2{ Command[2] };
	boost::multiprecision::cpp_int N1Boost(Command[1]), N2Boost(Command[2]);
	std::chrono::nanoseconds elapsed_ns;
	std::chrono::time_point<std::chrono::steady_clock> end_time, start_time = std::chrono::steady_clock::now();
	if (Command[0] == "+") {
		std::cout << Command[1] << " + " << Command[2] << " = ";
		(N1 + N2).Output(); std::cout << "<-- My class" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed_ns.count() << " ms\n";

		start_time = std::chrono::steady_clock::now();
		std::cout << Command[1] << " + " << Command[2] << " = ";
		std::cout << (N1Boost + N2Boost) << "<-- Boost" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed_ns.count() << " ms\n";
	}
	else if (Command[0] == "*") {
		std::cout << Command[1] << " * " << Command[2] << " = ";
		(N1 * N2).Output(); std::cout << "<-- My class" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed_ns.count() << " ms\n";

		start_time = std::chrono::steady_clock::now();
		std::cout << (N1Boost * N2Boost) << "<-- Boost" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed_ns.count() << " ms\n";
	}
	else if (Command[0] == "%") {
		std::cout << Command[1] << " % " << Command[2] << " = ";
		(N1 % N2).Output(); std::cout << "<-- My class" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);

		std::cout << elapsed_ns.count() << " ms\n";

		start_time = std::chrono::steady_clock::now();
		std::cout << (N1Boost % N2Boost) << "<-- Boost" << std::endl;
		end_time = std::chrono::steady_clock::now();
		elapsed_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time);
		std::cout << elapsed_ns.count() << " ms\n";
	}
}
void CheckingNumberForSimplicity(std::vector<std::string> Command) {
	if (Command.size() < 2) { std::cout << "too few arguments" << std::endl; exit(1); } // проверка количества аргументов
	unsigned int N = std::stoi(Command[1]);
	bool Simple = true;
	for (int i = 2; i < sqrt(N); i++) {
		if (N % i == 0)	{
			Simple = false;
			break;
		}
	}
	if (Simple) { std::cout << "Its a prime number" << std::endl; }
	else { std::cout << "Its a composite number" << std::endl; }
	N++;
	std::vector<bool> VBool(N, true);
	for (int i = 2; i < N; i++) {
		if (VBool[i] == true) {
			for (int j = 2 * i; j < N; j += i) {
				VBool[j] = false;
			}
		}
	}
	std::cout << "A series of prime numbers" << std::endl;
	for (int i = 2; i < N; i++) {
		if (VBool[i]) {
			std::cout << i << " ";
		}
	}
	std::cout << std::endl;
}
int gcd_ext(int a, int b, int& x, int& y) {
	if (b == 0) {
		x = 1;
		y = 0;
		return a;
	}
	int d = gcd_ext(b, a % b, x, y);
	x -= (a / b) * y;
	std::swap(x, y);
	return d;
}
int modInverse(int b, int m) { // b - число, m - модуль числа
	int x, y;
	int g = gcd_ext(b, m, x, y);
	if (g != 1) { return -1; }
	return (x % m + m) % m;
}
void ModularArithmetic(std::vector<std::string> Command) {
	if (Command.size() < 3) {std::cout << "too few arguments" << std::endl; exit(1);} // проверка количества аргументов
	std::vector<int> Numbers;
	for (int i = 1; i < Command.size(); i++) {
		Numbers.push_back(std::stoi(Command[i]));
	}
	if (Numbers.size() == 3) {
		std::cout << Numbers[0] << " " << Command[0] << " " << Numbers[1] << " % " << Numbers[2] << " = ";
		if (Command[0] == "+'") { std::cout << ((Numbers[0] + Numbers[1]) % Numbers[2]) << std::endl; } 
		else if (Command[0] == "-'") { std::cout << ((Numbers[0] - Numbers[1]) % Numbers[2]) << std::endl; } 
		else if (Command[0] == "*'") { std::cout << ((Numbers[0] * Numbers[1]) % Numbers[2]) << std::endl; }
		else if (Command[0] == "^'") { std::cout << ((Numbers[0] ^ Numbers[1]) % Numbers[2]) << std::endl; }
		else if (Command[0] == "/'") { std::cout << ((Numbers[0] / Numbers[1]) % Numbers[2]) << std::endl; }
	}
	else if (Numbers.size() == 2){
		std::cout << Numbers[0] << " " << Command[0] << " % " << Numbers[1] << " = ";
		std::cout << modInverse(Numbers[0], Numbers[1]) << std::endl;
	}
}
void ModularArithmetic2(std::vector<std::string> Command) {
	if (Command.size() < 3) {std::cout << "too few arguments" << std::endl; exit(1); } // проверка количества аргументов
	std::vector<int> Numbers;
	for (int i = 1; i < Command.size(); i++) {
		Numbers.push_back(std::stoi(Command[i]));
	}
	if (Numbers.size() == 3) {
		std::cout << Numbers[0] << " " << Command[0] << " " << Numbers[1] << " % " << Numbers[2] << " = ";
		if (Command[0] == "+''") { std::cout << ( ((Numbers[0] % Numbers[2]) + (Numbers[1] % Numbers[2])) % Numbers[2] ) << std::endl; } // (a+b) mod p = ((a mod p) + (b mod p)) mod p
		else if (Command[0] == "-''") { std::cout << ( (Numbers[2] + (Numbers[0] % Numbers[2]) - (Numbers[1] % Numbers[2])) % Numbers[2] ) << std::endl; } // (a-b) mod p = (p + (a mod p) - (b mod p)) mod p
		else if (Command[0] == "*''") { std::cout << ( ((Numbers[0] % Numbers[2]) * (Numbers[1] % Numbers[2])) % Numbers[2] ) << std::endl; } // (a*b) mod p = (p + (a mod p) - (b mod p)) mod p
		// степень числа - это сложение примененное n раз
		// деление выполняется в столбик, но я уже делал это с большими числами, снова прописывать мне лень
	}
	else if (Numbers.size() == 2){
		std::cout << Numbers[0] << " " << Command[0] << " % " << Numbers[1] << " = ";
		std::cout << modInverse(Numbers[0], Numbers[1]) << std::endl;
	}
}
struct CommandMap {
    void(*FuncPtr)(std::vector<std::string>); // указатель на функцию для соответствующей команды
    std::vector<std::string> Command; // команды
};
// подразумевается сначала операция, а после операнды
int main2(int argc, std::vector<std::string> argv) // можно заменить на int main(int argc, char* argv[])
{
    if (argc == 1) { help(); return 0; } // не переданы аргументы
    std::vector<CommandMap> commandMap = { {BinaryRepresentationOfData, {"hex8", "dec8", "hex16", "dec16", "hex32"}},
    {BitOperations, {"xor", "and", "or", "set1", "set0", "shl", "shr", "rol", "ror", "mix"}},
	{CheckingNumberForSimplicity, {"simple"}},
	{ArithmeticOfLargeNumbers, {"+", "*", "%"}},
	{ModularArithmetic, {"+'", "-'", "*'", "^'", "^(-1)'", "/'"}},
	{ModularArithmetic2, {"+''", "-''", "*''", "^(-1)''"}}
    };
    std::vector<std::string> FuncArg; // вектор содержащий аргументы функций (комманды и операнды)
    for (int i = 1; i < argc; i++) {
        FuncArg.push_back(argv[i]); // добавление аргументов в векрот
    }
    // поиск по команде соответствующей функции
    for (int i = 0; i < commandMap.size(); i++) // итерации по списку парам из команд и функций
    {
        for (int j = 0; j < commandMap[i].Command.size(); j++) { // итерации по командам
            if (argv[1] == commandMap[i].Command[j]) { // вызов функции при нахождении команды
                commandMap[i].FuncPtr(FuncArg); // указатель на функцию + аргументы к функции
            }
        }
    }
    return 0;
}
int main() {
	std::vector<std::string> CommansForMain(5, "");
	CommansForMain[0] = "Path";
	goto jump;
#pragma region BinaryRepresentationOfData
	CommansForMain[2] = "C:\\Users\\Dmitry\\Documents\\Test.txt"; // путь к файлу
	for (auto i : { "hex8", "dec8", "hex16", "dec16", "hex32" }) { // набор команд
		CommansForMain[1] = i; // запись команды
		main2(3, CommansForMain); // вызов основной программы
	}
#pragma endregion BinaryRepresentationOfData
#pragma region BitOperations
	for (auto i : { "xor", "and", "or", "set1", "set0", "shl", "shr", "rol", "ror"}) {
		for (int j = 0; j < 3; j++) {
			std::cout << "\t---------------" << std::endl;
			CommansForMain[1] = i; // запись команды
			CommansForMain[2] = std::to_string(rand() % 255); // запись первого случайного числа 
			CommansForMain[3] = std::to_string(rand() % 255); // запись второго случайного числа
			main2(4, CommansForMain);
		}
	}
	CommansForMain[1] = "mix"; // запись команды
	for (auto i : { "76543210", "70615243", "01234567" }) {
		for (int j = 0; j < 3; j++) {
			std::cout << "\t---------------" << std::endl;
			CommansForMain[2] = i; // запись порядка чисел
			CommansForMain[3] = std::to_string(rand() % 255); //  запись числа для изменения после его последовательности
			main2(4, CommansForMain);
		}
	}
#pragma endregion BitOperations
#pragma region CheckingNumberForSimplicity
	CommansForMain[1] = "simple"; // запись команды
	for (int i = 0; i < 3; i++) {
		CommansForMain[2] = std::to_string(rand() % 2500); // запись числа для проверки
		std::cout << CommansForMain[2] << std::endl;
		main2(3, CommansForMain);
	}
#pragma endregion CheckingNumberForSimplicity
#pragma region ArithmeticOfLargeNumbers
	for (auto i : { "+", "*", "%" }) {
		for (int j = 0; j < 3; j++) {
			std::cout << "\t---------------" << std::endl;
			CommansForMain[1] = i; // запись команды
			CommansForMain[2] = std::to_string(rand()) + std::to_string(rand()) + std::to_string(rand()); // получение и запись большого числа
			CommansForMain[3] = std::to_string(rand()) + std::to_string(rand()) + std::to_string(rand()); // получение и запись большого числа
			main2(4, CommansForMain);
		}
	}
#pragma endregion ArithmeticOfLargeNumbers
	jump:
#pragma region ModularArithmetic
	for (auto i : { "+'", "-'", "*'", "^'", "/'" }) {
		for (int j = 0; j < 3; j++) {
			std::cout << "\t---------------" << std::endl;
			CommansForMain[1] = i; // запись команды
			CommansForMain[2] = std::to_string(rand() % 25000); // запись первого операнда
			CommansForMain[3] = std::to_string(rand() % 25000); // запись второго операнда
			CommansForMain[4] = std::to_string(rand() % 250); // запись модуля числа
			main2(5, CommansForMain);
		}
	}
	CommansForMain[1] = "^(-1)'";
	for (int j = 0; j < 3; j++) {
		std::cout << "\t---------------" << std::endl;
		CommansForMain[2] = std::to_string(rand() % 25000);
		CommansForMain[3] = std::to_string(rand() % 25000);
		main2(4, CommansForMain);
	}
#pragma endregion ModularArithmetic
#pragma region ModularArithmetic2
	for (auto i : { "+''", "-''", "*''"}) {
		for (int j = 0; j < 3; j++) {
			std::cout << "\t---------------" << std::endl;
			CommansForMain[1] = i; // запись команды
			CommansForMain[2] = std::to_string(rand() % 25000); // запись первого операнда
			CommansForMain[3] = std::to_string(rand() % 25000); // запись второго операнды
			CommansForMain[4] = std::to_string(rand() % 250); // запись модуля
			main2(5, CommansForMain);
		}
	}
	CommansForMain[1] = "^(-1)''";
	for (int j = 0; j < 3; j++) {
		std::cout << "\t---------------" << std::endl;
		CommansForMain[2] = std::to_string(rand() % 25000);
		CommansForMain[3] = std::to_string(rand() % 25000);
		main2(4, CommansForMain);
	}
#pragma endregion ModularArithmetic2
}