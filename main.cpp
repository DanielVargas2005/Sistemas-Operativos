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
#include <queue>
#include <iomanip>
#include <sstream>

using namespace std;

enum{C_0,C_1,C_2,C_3};

#define PROCESS_NUMBER 4
#define MAX_NAME_SIZE 50
#define WAITING_TIME 100000000
#define DECIMALS_DISPLAYED 2

int globalCounter = C_0;

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
            case '/': return operandB != C_0 ? operandA / operandB : C_0;
            case '^': return pow(operandA, operandB);
            default:
                cout << "Error: Operacion no valida" << endl;
                return 0;
        }
    }
};

class Process {
private:
    string programmerName;
    unsigned int maxTime;
    unsigned int id;
    Calculator calculator;

public:
    Process() {
        programmerName = "Nulo";
        maxTime = C_0;
        id = C_0;
        calculator.setOperation('+');
        calculator.setOperandA(C_0);
        calculator.setOperandB(C_0);
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
        if (numberProcesses % PROCESS_NUMBER != C_0) lots++;
        return lots;
    }
};

void fillProcessMatrix(vector<vector<Process>>& processMatrix, int numberProcesses);
void simulateProcessing(const vector<vector<Process>>& processMatrix, int numberProcesses);
void displayStatus(const vector<Process>& loteActual,
                  const Process* procesoEjecutando,
                  int tiempoTranscurrido,
                  int tiempoRestante,
                  const vector<pair<Process, float>>& terminados,
                  int lotesPendientes,
                  int contadorGlobal);

int main() {
    Process process1;
    int numberProcesses = C_0;
    int lotsNumber = C_0;
    bool validProcess = false;

    cout << "--------Simulacion de Procesamiento por Lotes--------" << endl;
    do {
        try {
            cout << "Cuantos procesos desea ingresar? ";
            string input;
            getline(cin, input);

            if (input.empty())
                throw runtime_error("La entrada no puede estar vacia");

            int temp = stoi(input);
            if (temp <= C_0)
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

    simulateProcessing(processMatrix, numberProcesses);
    return 0;
}

bool isIdUnique(const vector<vector<Process>>& processMatrix, unsigned int id, int currentCount) {
    for (int i = C_0; i < processMatrix.size(); i++) {
        for (int j = C_0; j < PROCESS_NUMBER; j++) {
            if (i * PROCESS_NUMBER + j >= currentCount) {
                break;
            }
            if (processMatrix[i][j].getId() == id) {
                return false;
            }
        }
    }
    return true;
}

void fillProcessMatrix(vector<vector<Process>>& processMatrix, int numberProcesses) {
    string programmerName;
    char operation;
    float operandA, operandB;
    unsigned int maxTime, id;
    int count = C_0;

    for (int i = C_0; i < processMatrix.size(); i++) {
        cout << "\n--- Lote " << i + C_1 << " ---\n";

        for (int j = C_0; j < PROCESS_NUMBER; j++) {
            if (count >= numberProcesses) break;

            cout << "\nIngreso de proceso " << count + 1 << ":\n";

            while (true) {
                cout << "Nombre del programador: ";
                getline(cin, programmerName);
                if (!programmerName.empty() && programmerName.length() <= MAX_NAME_SIZE) break;
                cout << "Error: nombre invalido (max 50 caracteres, no vacio)\n";
            }

            while (true) {
                cout << "Tiempo maximo (>0): ";
                string input;
                getline(cin, input);
                try {
                    if (input.empty())
                        throw runtime_error("La entrada no puede estar vacia");

                    int temp = stoi(input);
                    if (temp <= C_0)
                        throw runtime_error("El tiempo debe ser mayor a 0");

                    maxTime = temp;
                    break;
                } catch (const invalid_argument&) {
                    cout << "Error: Ingresa un numero valido\n";
                } catch (const out_of_range&) {
                    cout << "Error: Numero fuera de rango\n";
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                } catch (...) {
                    cout << "Error: Ingresa un numero valido\n";
                }
            }

            while (true) {
                cout << "ID: ";
                string input;
                getline(cin, input);
                try {
                    if (input.empty())
                        throw runtime_error("La entrada no puede estar vacia");

                    int temp = stoi(input);
                    if (temp <= C_0)
                        throw runtime_error("El ID debe ser mayor a 0");

                    if (!isIdUnique(processMatrix, temp, count)) {
                        throw runtime_error("El ID ya existe. Debe ser único.");
                    }

                    id = temp;
                    break;
                } catch (const invalid_argument&) {
                    cout << "Error: Ingresa un numero valido\n";
                } catch (const out_of_range&) {
                    cout << "Error: Numero fuera de rango\n";
                } catch (const runtime_error& e) {
                    cout << "Error: " << e.what() << endl;
                } catch (...) {
                    cout << "Error: Ingresa un numero valido\n";
                }
            }

            while (true) {
                cout << "Operacion (+, -, *, /, ^): ";
                string input;
                getline(cin, input);
                if (input.length() == C_1 && (input[C_0] == '+' || input[C_0] == '-' || input[C_0]
                    == '*' || input[C_0] == '/' || input[C_0] == '^')) {
                    operation = input[C_0];
                    break;
                }
                cout << "Error: operacion invalida\n";
            }

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

            while (true) {
                cout << (operation == '^' ? "Exponente: " : "Operando B: ");
                string input;
                getline(cin, input);
                try {
                    operandB = stof(input);
                    if (operation == '/' && operandB == C_0){
                        cout << "Error: division por cero\n";
                        continue;
                    }
                    break;
                } catch(...) {
                    cout << "Error: ingrese un numero valido\n";
                }
            }

            processMatrix[i][j] = Process(programmerName, operation, operandA, operandB, maxTime, id);
            cout << "Proceso ingresado correctamente.\n";
            count++;
        }
    }
}

void displayStatus(const vector<Process>& loteActual,
                  const Process* procesoEjecutando,
                  int tiempoTranscurrido,
                  int tiempoRestante,
                  const vector<pair<Process, float>>& terminados,
                  int lotesPendientes,
                  int contadorGlobal) {

    system("cls");
    cout << "No. Lotes pendientes: " << lotesPendientes << "\n\n";


    cout << left;
    cout << setw(20) << "LOTE TRABAJANDO" << " | "
         << setw(25) << "PROCESO EN EJECUCION" << " | "
         << setw(20) << "TERMINADOS" << endl;
    cout << setw(20) << "---------------" << " | "
         << setw(25) << "---------------------" << " | "
         << setw(20) << "----------" << endl;

    cout << setw(20) << "Nombre  TME" << " | ";
    if (procesoEjecutando != nullptr) {
        cout << setw(25) << "ID: " + to_string(procesoEjecutando->getId()) << " | ";
    } else {
        cout << setw(25) << "" << " | ";
    }
    cout << setw(25) << "ID  Operacion   Resultado" << endl;

    vector<Process> loteFiltrado;
    for (const auto& proceso : loteActual) {
        if (proceso.getId() != C_0 && proceso.getMaxTime() != C_0 && proceso.getProgrammerName() != "Nulo") {
            loteFiltrado.push_back(proceso);
        }
    }

    int maxAltura = max(static_cast<int>(loteFiltrado.size()),
                       max(1, static_cast<int>(terminados.size())));
    if (procesoEjecutando != nullptr) {
        maxAltura = max(maxAltura, 5);
    }

    for (int i = 0; i < maxAltura; i++) {
        if (i < loteFiltrado.size()) {
            if (procesoEjecutando == nullptr || loteFiltrado[i].getId() != procesoEjecutando->getId()) {
                string infoProceso = loteFiltrado[i].getProgrammerName() + "  " +
                                   to_string(loteFiltrado[i].getMaxTime());
                cout << setw(20) << infoProceso << " | ";
            } else {
                cout << setw(20) << "" << " | ";
            }
        } else {
            cout << setw(20) << "" << " | ";
        }

        if (procesoEjecutando != nullptr) {
            const Calculator& calc = procesoEjecutando->getCalculator();
            string opStr;
            switch (calc.getOperation()) {
                case '+': opStr = "+"; break;
                case '-': opStr = "-"; break;
                case '*': opStr = "*"; break;
                case '/': opStr = "/"; break;
                case '^': opStr = "^"; break;
            }

            string operacion = to_string(static_cast<int>(calc.getOperandA())) + opStr +
                             to_string(static_cast<int>(calc.getOperandB()));

            if (i == 0) {
                cout << setw(25) << ("Ope: " + operacion) << " | ";
            } else if (i == 1) {
                cout << setw(25) << ("Nombre: " + procesoEjecutando->getProgrammerName()) << " | ";
            } else if (i == 2) {
                cout << setw(25) << ("TME: " + to_string(procesoEjecutando->getMaxTime())) << " | ";
            } else if (i == 3) {
                cout << setw(25) << ("TT: " + to_string(tiempoTranscurrido)) << " | ";
            } else if (i == 4) {
                cout << setw(25) << ("TR: " + to_string(tiempoRestante)) << " | ";
            } else {
                cout << setw(25) << "" << " | ";
            }
        } else {
            cout << setw(25) << "" << " | ";
        }

        if (i < terminados.size()) {
            const Process& p = terminados[i].first;
            const Calculator& calc = p.getCalculator();
            string opStr;
            switch (calc.getOperation()) {
                case '+': opStr = "+"; break;
                case '-': opStr = "-"; break;
                case '*': opStr = "*"; break;
                case '/': opStr = "/"; break;
                case '^': opStr = "^"; break;
            }

            string operacion = to_string(static_cast<int>(calc.getOperandA())) + opStr +
                             to_string(static_cast<int>(calc.getOperandB()));

            stringstream stream;
            stream << fixed << setprecision(DECIMALS_DISPLAYED) << terminados[i].second;
            string terminadoInfo = to_string(p.getId()) + "   " + operacion + "         " + stream.str();
            cout << setw(20) << terminadoInfo;
        } else {
            cout << setw(20) << "";
        }
        cout << endl;
    }

    cout << "\nContador: " << contadorGlobal << endl;
}

void simulateProcessing(const vector<vector<Process>>& processMatrix, int numberProcesses) {
    if (processMatrix.empty()) return;

    vector<pair<Process, float>> terminados;
    globalCounter = C_0;

    for (int loteIndex = C_0; loteIndex < processMatrix.size(); loteIndex++) {
        vector<Process> loteActual;
        for (const auto& proceso : processMatrix[loteIndex]) {
            if (proceso.getId() != C_0 && proceso.getMaxTime() != C_0 && proceso.getProgrammerName() != "Nulo") {
                loteActual.push_back(proceso);
            }
        }

        int lotesPendientes = processMatrix.size() - loteIndex - C_1;

        int i = C_0;
        while (i < loteActual.size()) {
            Process procesoActual = loteActual[i];
            int tiempoTranscurrido = C_0;
            int tiempoRestante = procesoActual.getMaxTime();

            vector<Process> loteParaMostrar = loteActual;

            while (tiempoTranscurrido < procesoActual.getMaxTime()) {
                displayStatus(loteParaMostrar, &procesoActual, tiempoTranscurrido,
                             tiempoRestante, terminados, lotesPendientes, globalCounter);

                for (int delay = C_0; delay < WAITING_TIME; delay++) {}
                tiempoTranscurrido++;
                tiempoRestante--;
                globalCounter++;
            }

            float resultado = procesoActual.getCalculator().calculate();
            terminados.push_back({procesoActual, resultado});

            loteActual.erase(loteActual.begin() + i);
            displayStatus(loteActual, nullptr, C_0, C_0, terminados, lotesPendientes, globalCounter);

            for (int delay = C_0; delay < WAITING_TIME; delay++) {}
        }
    }

    cout << endl << "Todos los lotes han sido completados!\n";
    cout << "Total de procesos terminados: " << terminados.size() << endl;
    cout << "Tiempo global total: " << globalCounter << " unidades de tiempo\n";
    cout << "Presione Enter para terminar...";
    cin.ignore();
}
