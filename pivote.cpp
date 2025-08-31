/*
 Aprendizaje 2 programa 1 - Simular el Procesamiento por Lotes
 Hugo Gabriel Garcia Saldivar
 Oswaldo Daniel Maciel Vargas
*/

#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <vector>
using namespace std;

#define PROCESS_NUMBER 4
#define MAX_NAME_SIZE 50

int globalCounter = 0;

// Clase Calculator
class Calculator {
private:
    char operation;
    float operandA;
    float operandB;

public:
    void setOperation(char op) {
        operation = op;
    }

    void setOperandA(float a) {
        operandA = a;
    }

    void setOperandB(float b) {
        operandB = b;
    }

    char getOperation() const {
        return operation;
    }

    float getOperandA() const {
        return operandA;
    }

    float getOperandB() const {
        return operandB;
    }

    float calculate() const {
        switch (operation) {
            case '+': return operandA + operandB;
            case '-': return operandA - operandB;
            case '*': return operandA * operandB;
            case '/': return operandB != 0 ? operandA / operandB : 0;
            case '^': return pow(operandA, operandB);
            default:
                cout << "Error: Operacion no valida" << endl;
                return 0;
        }
    }
};

// Clase Process
class Process {
private:
    string programmerName;
    unsigned int maxTime;
    unsigned int id;
    Calculator calculator;

public:
    Process() {
        programmerName = "Nulo";
        maxTime = 0;
        id = 0;
        calculator.setOperation('+');
        calculator.setOperandA(0);
        calculator.setOperandB(0);
    }

    Process(string name, char op, float a, float b, unsigned int t, unsigned int i) {
        programmerName = name;
        maxTime = t;
        id = i;
        calculator.setOperation(op);
        calculator.setOperandA(a);
        calculator.setOperandB(b);
    }

    void setProgrammerName(string name) {
        programmerName = name;
    }

    void setMaxTime(unsigned int t) {
        maxTime = t;
    }

    void setId(unsigned int i) {
        id = i;
    }

    string getProgrammerName() const {
        return programmerName;
    }

    unsigned int getMaxTime() const {
        return maxTime;
    }

    unsigned int getId() const {
        return id;
    }

    const Calculator& getCalculator() const {
        return calculator;
    }

    int lotsCreator(int numberProcesses) const {
        int lots = numberProcesses / PROCESS_NUMBER;
        if (numberProcesses % PROCESS_NUMBER != 0) lots++;
        return lots;
    }
};

// Prototipos
void fillProcessMatrix(vector<vector<Process>>& processMatrix, int numberProcesses);
void printProcessMatrix(const vector<vector<Process>>& processMatrix, int numberProcesses);

int main() {
    Process process1;
    int numberProcesses = 0;
    int lotsNumber = 0;
    bool validProcess = false;

    cout << "--------Simulacion de Procesamiento por Lotes--------" << endl;

    // Validar cantidad de procesos
    do {
        try {
            cout << "Cuantos procesos desea ingresar? ";
            string input;
            getline(cin, input);

            if (input.empty())
                throw runtime_error("La entrada no puede estar vacia");

            int temp = stoi(input);
            if (temp <= 0)
                throw runtime_error("El numero debe ser mayor a 0");

            numberProcesses = temp;
            validProcess = true;
        } catch (...) {
            cout << "Error: Ingresa un numero valido" << endl;
        }
    } while (!validProcess);

    lotsNumber = process1.lotsCreator(numberProcesses);

    vector<vector<Process>> processMatrix(lotsNumber, vector<Process>(PROCESS_NUMBER));

    fillProcessMatrix(processMatrix, numberProcesses);
    printProcessMatrix(processMatrix, numberProcesses);

    return 0;
}

// Función para llenar la matriz de procesos
void fillProcessMatrix(vector<vector<Process>>& processMatrix, int numberProcesses) {
    string programmerName;
    char operation;
    float operandA, operandB;
    unsigned int maxTime, id;
    int count = 0;

    for (int i = 0; i < processMatrix.size(); i++) {
        cout << "\n--- Lote " << i + 1 << " ---\n";

        for (int j = 0; j < PROCESS_NUMBER; j++) {
            if (count >= numberProcesses) break; // Evita pedir más procesos de los necesarios

            cout << "\nIngreso de proceso " << count + 1 << ":\n";

            // Nombre
            while (true) {
                cout << "Nombre del programador: ";
                getline(cin, programmerName);
                if (!programmerName.empty() && programmerName.length() <= MAX_NAME_SIZE) break;
                cout << "Error: nombre invalido (max 50 caracteres, no vacio)\n";
            }

            // Tiempo máximo
            while (true) {
                cout << "Tiempo maximo (>0): ";
                string input;
                getline(cin, input);
                try {
                    maxTime = stoi(input);
                    if (maxTime > 0) break;
                } catch(...) {}
                cout << "Error: ingrese un numero valido\n";
            }

            // ID
            while (true) {
                cout << "ID (>0): ";
                string input;
                getline(cin, input);
                try {
                    id = stoi(input);
                    if (id > 0) break;
                } catch(...) {}
                cout << "Error: ingrese un numero valido\n";
            }

            // Operación
            while (true) {
                cout << "Operacion (+, -, *, /, ^): ";
                string input;
                getline(cin, input);
                if (input.length() == 1 && (input[0] == '+' || input[0] == '-' || input[0] == '*' || input[0] == '/' || input[0] == '^')) {
                    operation = input[0];
                    break;
                }
                cout << "Error: operacion invalida\n";
            }

            // Operando A
            while (true) {
                cout << (operation == '^' ? "Base: " : "Operando A: ");
                string input;
                getline(cin, input);
                try {
                    operandA = stof(input);
                    break;
                } catch(...) {}
                cout << "Error: ingrese un numero valido\n";
            }

            // Operando B
            while (true) {
                cout << (operation == '^' ? "Exponente: " : "Operando B: ");
                string input;
                getline(cin, input);
                try {
                    operandB = stof(input);
                    if (operation == '/' && operandB == 0) {
                        cout << "Error: division por cero\n";
                        continue;
                    }
                    break;
                } catch(...) {
                    cout << "Error: ingrese un numero valido\n";
                }
            }

            // Asignar objeto Process a la matriz
            processMatrix[i][j] = Process(programmerName, operation, operandA, operandB, maxTime, id);

            cout << "Proceso ingresado correctamente.\n";
            count++;
        }
    }
}

// Función para imprimir la matriz de procesos
void printProcessMatrix(const vector<vector<Process>>& processMatrix, int numberProcesses) {
    int count = 0;

    for (int i = 0; i < processMatrix.size(); i++) {
        cout << "\n--- Lote " << i + 1 << " ---\n";

        for (int j = 0; j < PROCESS_NUMBER; j++) {
            if (count >= numberProcesses) break;

            const Process& p = processMatrix[i][j];
            const Calculator& calc = p.getCalculator();

            cout << "\n--- Proceso numero " << count + 1 << " ---\n";
            cout << "Nombre: " << p.getProgrammerName() << endl;
            cout << "Operacion: " << calc.getOperation() << endl;

            if (calc.getOperation() == '^') {
                cout << "Base: " << calc.getOperandA() << endl;
                cout << "Exponente: " << calc.getOperandB() << endl;
            } else {
                cout << "Operando A: " << calc.getOperandA() << endl;
                cout << "Operando B: " << calc.getOperandB() << endl;
            }

            cout << "Tiempo maximo: " << p.getMaxTime() << endl;
            cout << "ID: " << p.getId() << endl;

            count++;
        }
    }
}
