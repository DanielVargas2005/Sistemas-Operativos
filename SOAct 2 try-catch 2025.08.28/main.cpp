/*
Aprendizaje 2 programa 1 Simular el Procesamiento por Lotes
Hugo Gabriel Garcia Saldivar
Oswaldo Daniel Maciel Vargas
*/

#include <iostream>
#include <string>
#include <limits>
#include <cmath>

using namespace std;

#define MAX_NAME_SIZE 50

enum{C_0,C_1,C_2,C_3};

int globalCounter;

//Lotes / Filas
//int [4][new];

class Calculator{
    private:
        char operation;
        float operandA;
        float operandB;
    public:
        void setOperation (char operation){
            this->operation=operation;
        }

        void setOperandA (float operandA){
            this->operandA=operandA;
        }

        void setOperandB (float operandB){
            this->operandB=operandB;
        }

        char getOperation (void){
            return operation;
        }

        float getOperandA (void){
            return operandA;
        }

        float getOperandB (void){
            return operandB;
        }

        float calculate (void){
            switch (operation){
                case '+':
                    return operandA + operandB;
                case '-':
                    return operandA - operandB;
                case '*':
                    return operandA * operandB;
                case '/':
                    return operandA / operandB;
                case '^':
                    return pow(operandA,operandB);
                default:
                    cout << "Error: Operacion no valida" << endl;
                    return 0;
            }
        }
    };

class Process{
private:
    string programmerName;
    unsigned int maxTime;
    unsigned int id;
    Calculator calculator;

public:
    void setProgrammerName (string programmerName){
        this->programmerName=programmerName;
    }
    void setMaxTime (unsigned int maxTime){
        this->maxTime=maxTime;
    }

    void setId (unsigned int id){
        this->id=id;
    }

    string getProgrammerName (void){
        return programmerName;
    }

    unsigned int getMaxTime (void){
        return maxTime;
    }

    unsigned int getId (void){
        return id;
    }

    Calculator& getCalculator (void){
        return calculator;
    }
};

bool uniqueIdVerifier();


int main() {
    Process process1;
    string programmerName;
    char operation;
    float operandA, operandB;
    unsigned int maxTime, id;
    int numberProcesses;

    bool validProcess;
    bool validName;
    bool validOperation;
    bool validOperandA;
    bool validOperandB;
    bool validMaxTime;
    bool validId;

    globalCounter = C_0;
    validProcess = false;
    validName = false;
    validOperation = false;
    validOperandA = false;
    validOperandB = false;
    validMaxTime = false;
    validId = false;
    numberProcesses = C_0;


    cout << "--------Simulacion de Procesamiento por Lotes--------" << endl;

    //Validacion cantidad de procesos
    do {
        try {
            cout << "Cuantos procesos desea ingresar? ";
            string input;
            getline(cin, input);

            if (input.empty()) {
                throw runtime_error("La entrada no puede estar vacia");
            }
            int temp = stoi(input);

            if (temp <= C_0) {
                throw runtime_error("El numero debe ser mayor a 0");
            }

            numberProcesses = static_cast<unsigned int>(temp);
            validProcess = true;
        } catch (const exception& e) {
            cout << "Error: Ingresa un numero valido" << endl;
            cin.clear();
        }
    } while (!validProcess);
    cout << endl << endl;

    // Validaci�n para el nombre del programador
    do {
        try {
            cout << "Nombre del programador: ";
            getline(cin, programmerName);

            if (programmerName.empty()) {
                throw runtime_error("La entrada no puede estar vacia");
            }
            if (programmerName.length() > MAX_NAME_SIZE) {
                throw runtime_error("El nombre no puede exceder 50 caracteres");
            }
            validName = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
        }
    } while (!validName);
    cout << endl << endl;

    // Validaci�n para la operaci�n
    do {
        try {
            cout << "Ingresa la operacion a realizar (+, -, *, /, ^): ";
            string input;
            cin >> input;

            if (input.length() != C_1) {
                throw runtime_error("Debe ingresar un solo caracter");
            }

            operation = input[C_0];
            if (operation != '+' && operation != '-' && operation != '*' &&
                operation != '/' && operation != '^') {
                throw runtime_error("Operacion no valida. Use +, -, *, /, ^");
            }
            validOperation = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validOperation);
    cout << endl << endl;

    // Validaci�n para el primer operando
    do {
        try {
            if (operation == '^') {
                cout << "Ingresa la base: ";
            } else {
                cout << "Ingresa el primer operando: ";
            }

            string input;
            cin >> input;

            size_t pos;
            operandA = stof(input, &pos);

            if (pos != input.length()) {
                throw runtime_error("Entrada no valida. Debe ser un numero");
            }
            validOperandA = true;
        } catch (const exception& e) {
            cout << "Error: Ingrese un numero" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validOperandA);
    cout << endl << endl;

    // Validaci�n para el segundo operando
    do {
        try {
            if (operation == '^') {
                cout << "Ingresa el exponente: ";
            } else {
                cout << "Ingresa el segundo operando: ";
            }

            string input;
            cin >> input;

            size_t pos;
            operandB = stof(input, &pos);

            if (pos != input.length()) {
                throw runtime_error("Entrada no valida. Debe ser un numero");
            }

            // Validación especial para división por cero
            if (operation == '/' && operandB == C_0) {
                throw runtime_error("division_cero"); // Usamos un código especial
            }

            validOperandB = true;
        } catch (const runtime_error& e) {
            // Manejar específicamente el error de división por cero
            if (string(e.what()) == "division_cero") {
                cout << "Error: No se puede dividir entre cero. Ingrese un numero diferente de cero" << endl;
            } else {
                cout << "Error: Entrada no valida. Debe ser un numero" << endl;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        } catch (const exception& e) {
            // Manejar otros errores de conversión
            cout << "Error: Entrada no valida. Debe ser un numero" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validOperandB);
    cout << endl << endl;

    // Validaci�n para el tiempo m�ximo
    do {
        try {
            cout << "Ingresa el tiempo maximo estimado (mayor a 0): ";
            string input;
            cin >> input;

            size_t pos;
            int temp = stoi(input, &pos);

            if (pos != input.length()) {
                throw runtime_error("Entrada no valida. Debe ser un numero entero");
            }
            if (temp <= C_0) {
                throw runtime_error("El tiempo debe ser mayor a 0");
            }

            maxTime = static_cast<unsigned int>(temp);
            validMaxTime = true;
        } catch (const exception& e) {
            cout << "Error: Ingrese un numero mayor a 0 como tiempo maximo estimado" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validMaxTime);
    cout << endl << endl;

    // Validaci�n para el ID
    do {
        try {
            cout << "Ingresa el ID del programa (mayor a 0): ";
            string input;
            cin >> input;

            size_t pos;
            int temp = stoi(input, &pos);

            if (pos != input.length()) {
                throw runtime_error("Entrada no valida. Debe ser un numero entero");
            }
            if (temp <= C_0) {
                throw runtime_error("El ID debe ser mayor a 0");
            }

            id = static_cast<unsigned int>(temp);
            validId = true;
        } catch (const exception& e) {
            cout << "Error: Ingrese un numero mayor a 0 como ID" << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validId);
    cout << endl << endl;

    // Mostrar resultados
    cout << "\nDatos ingresados correctamente:" << endl;
    cout << "Cantidad de procesos: " << numberProcesses << endl;
    cout << "Nombre: " << programmerName << endl;
    cout << "Operacion: " << operation << endl;

    if (operation == '^') {
        cout << "Base: " << operandA << endl;
        cout << "Exponente: " << operandB << endl;
    } else {
        cout << "Operando A: " << operandA << endl;
        cout << "Operando B: " << operandB << endl;
    }

    cout << "Tiempo maximo: " << maxTime << endl;
    cout << "ID: " << id << endl;

    return 0;
}
