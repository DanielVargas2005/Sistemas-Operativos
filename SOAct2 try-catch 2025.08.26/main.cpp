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

#define C_0 0

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

int globalCounter;

int main() {
    Process process1;
    string programmerName;
    char operation;
    float operandA, operandB;
    unsigned int maxTime, id;

    bool validName;
    bool validOperation;
    bool validOperandA;
    bool validOperandB;
    bool validMaxTime;
    bool validId;

    globalCounter = C_0;
    validName = false;
    validOperation = false;
    validOperandA = false;
    validOperandB = false;
    validMaxTime = false;
    validId = false;


    cout << "--------Simulacion de Procesamiento por Lotes--------" << endl;
    // Validación para el nombre del programador
    do {
        try {
            cout << "Nombre del programador: ";
            getline(cin, programmerName);

            if (programmerName.empty()) {
                throw runtime_error("El nombre no puede estar vacio");
            }
            if (programmerName.length() > 100) {
                throw runtime_error("El nombre no puede exceder 100 caracteres");
            }
            validName = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
        }
    } while (!validName);
    cout << endl << endl;

    // Validación para la operación
    do {
        try {
            cout << "Ingresa la operacion a realizar (+, -, *, /, ^): ";
            string input;
            cin >> input;

            if (input.length() != 1) {
                throw runtime_error("Debe ingresar un solo caracter");
            }

            operation = input[0];
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

    // Validación para el primer operando
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
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validOperandA);
    cout << endl << endl;

    // Validación para el segundo operando
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
            if ((operation == '/') && (operandB == 0)) {
                throw runtime_error
                    ("Error: No se puede dividir entre cero. Ingrese un numero diferente de cero");
            }

            validOperandB = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validOperandB);
    cout << endl << endl;

    // Validación para el tiempo máximo
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
            if (temp <= 0) {
                throw runtime_error("El tiempo debe ser mayor a 0");
            }

            maxTime = static_cast<unsigned int>(temp);
            validMaxTime = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validMaxTime);
    cout << endl << endl;

    // Validación para el ID
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
            if (temp <= 0) {
                throw runtime_error("El ID debe ser mayor a 0");
            }

            id = static_cast<unsigned int>(temp);
            validId = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    } while (!validId);
    cout << endl << endl;

    // Mostrar resultados
    cout << "\nDatos ingresados correctamente:" << endl;
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

    /*
    cout << "Nombre del programador: ";
    getline(cin,programmerName);

    cout << "Ingresa la operacion a realizar: ";
    cin >> operation;

    cout << "Ingresa el primer operando: ";
    cin >> operandA;

    cout << "Ingresa el segundo operando: ";
    cin >> operandB;

    cout << "Ingresa el tiempo maximo estimado: ";
    cin >> maxTime;

    cout << "Ingresa el ID del programa: ";
    cin >> id;
    */
    return 0;
}

