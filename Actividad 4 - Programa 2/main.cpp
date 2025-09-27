/*
 Aprendizaje 4 programa 2 - Simular el Procesamiento por Lotes con Multiprogramación
 Hugo Gabriel Garcia Saldivar
 Oswaldo Daniel Maciel Vargas
*/

#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <stdexcept>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <conio.h>
#include <windows.h>


using namespace std;

#define PROCESS_NUMBER 4
#define SIMULATION_SPEED_MS 200

int globalCounter = 0;

class Calculator {
private:
    char operation;
    float operandA;
    float operandB;
public:
    void setOperation(char op) { operation = op; }
    void setOperandA(float a) { operandA = a; }
    void setOperandB(float b) { operandB = b; }
    char getOperation() const { return operation; }
    float getOperandA() const { return operandA; }
    float getOperandB() const { return operandB; }
    string getOperationString() const {
        stringstream ss;
        ss << fixed << setprecision(0) << operandA << " " << operation << " " << operandB;
        return ss.str();
    }
    float calculate() const {
        switch (operation) {
            case '+': return operandA + operandB;
            case '-': return operandA - operandB;
            case '*': return operandA * operandB;
            case '/': return operandA / operandB;
            case '%': return fmod(operandA, operandB);
            case '^': return pow(operandA, operandB);
            default: return 0;
        }
    }
};

class Process {
private:
    unsigned int maxTime;
    unsigned int id;
    unsigned int timeElapsed;
    Calculator calculator;
public:
    Process() : maxTime(0), id(0), timeElapsed(0) {}
    Process(char op, float a, float b, unsigned int t, unsigned int i) :
        maxTime(t), id(i), timeElapsed(0) {
        calculator.setOperation(op);
        calculator.setOperandA(a);
        calculator.setOperandB(b);
    }
    void setTimeElapsed(unsigned int t) { timeElapsed = t; }
    unsigned int getMaxTime() const { return maxTime; }
    unsigned int getId() const { return id; }
    unsigned int getTimeElapsed() const { return timeElapsed; }
    const Calculator& getCalculator() const { return calculator; }
    int lotsCreator(int numberProcesses) const {
        int lots = numberProcesses / PROCESS_NUMBER;
        if (numberProcesses % PROCESS_NUMBER != 0) lots++;
        return lots;
    }
};

struct FinishedProcess {
    int id;
    int batchNumber;
    string operationString;
    string resultString;
};

void generateProcessData(vector<vector<Process>>& processMatrix, int numberProcesses);
bool isIdUnique(const vector<vector<Process>>& processMatrix, unsigned int id, int currentCount);
void simulateProcessing(vector<vector<Process>>& processMatrix, int numberProcesses);
void displayStatus(const vector<Process>& loteEspera,
                   const Process* procesoEjecutando,
                   const vector<FinishedProcess>& terminados,
                   int lotesPendientes, bool isPaused);
void pause_ms(int milliseconds);

int main() {
    srand(time(NULL));
    Process p;
    int numProcesses = 0;
    bool valid = false;
    cout << "--------Simulacion de Procesamiento por Lotes con Multiprogramacion--------" << endl;
    do {
        cout << "Cuantos procesos desea generar? ";
        string input;
        getline(cin, input);
        try {
            if (input.empty()) throw runtime_error("La entrada no puede estar vacia.");
            numProcesses = stoi(input);
            if (numProcesses <= 0) throw out_of_range("El numero debe ser mayor a 0.");
            valid = true;
        } catch (const exception& e) {
            cout << "Error: " << e.what() << endl;
        }
    } while (!valid);

    int lotsNumber = p.lotsCreator(numProcesses);
    vector<vector<Process>> processMatrix(lotsNumber, vector<Process>(PROCESS_NUMBER));
    generateProcessData(processMatrix, numProcesses);
    cout << "\nPresione Enter para comenzar la simulacion...";
    cin.ignore();
    simulateProcessing(processMatrix, numProcesses);
    return 0;
}

void simulateProcessing(vector<vector<Process>>& processMatrix, int numberProcesses) {
    if (processMatrix.empty()) return;

    vector<FinishedProcess> terminados;
    globalCounter = 0;
    bool isPaused = false;

    for (size_t loteIndex = 0; loteIndex < processMatrix.size(); ++loteIndex) {
        vector<Process> loteActual;
        for (const auto& proceso : processMatrix[loteIndex]) {
            if (proceso.getId() != 0) {
                loteActual.push_back(proceso);
            }
        }

        while (!loteActual.empty()) {
            Process procesoActual = loteActual.front();
            loteActual.erase(loteActual.begin());

            unsigned int tiempoEjecutadoPrevio = procesoActual.getTimeElapsed();
            unsigned int tiempoRestante = procesoActual.getMaxTime() - tiempoEjecutadoPrevio;
            bool processAborted = false;
            bool processInterrupted = false;

            for (unsigned int t = 0; t < tiempoRestante; ++t) {
                if (_kbhit()) {
                    char key = tolower(_getch());

                    if (isPaused) {
                        if (key == 'c') {
                            isPaused = false;
                        }
                    } else {
                        switch (key) {
                            case 'e':
                                loteActual.push_back(procesoActual);
                                processInterrupted = true;
                                break;
                            case 'w':
                                terminados.push_back({(int)procesoActual.getId(), (int)loteIndex + 1, "X", "Error"});
                                processAborted = true;
                                break;
                            case 'p':
                                isPaused = true;
                                break;
                        }
                    }
                }

                if (processAborted || processInterrupted) {
                    break;
                }

                if (isPaused) {
                    t--;
                    displayStatus(loteActual, &procesoActual, terminados, processMatrix.size() - loteIndex - 1, isPaused);
                    pause_ms(SIMULATION_SPEED_MS);
                    continue;
                }

                procesoActual.setTimeElapsed(tiempoEjecutadoPrevio + t + 1);
                displayStatus(loteActual, &procesoActual, terminados, processMatrix.size() - loteIndex - 1, isPaused);
                pause_ms(SIMULATION_SPEED_MS);
                globalCounter++;
            }

            if (!processAborted && !processInterrupted) {
                float resultadoNum = procesoActual.getCalculator().calculate();
                stringstream ss;
                ss << fixed << setprecision(2) << resultadoNum;
                terminados.push_back({
                    (int)procesoActual.getId(),
                    (int)loteIndex + 1,
                    procesoActual.getCalculator().getOperationString(),
                    ss.str()
                });
            }

            if (processInterrupted) {
                 displayStatus(loteActual, nullptr, terminados, processMatrix.size() - loteIndex - 1, isPaused);
                 pause_ms(SIMULATION_SPEED_MS);
            } else {
                 displayStatus(loteActual, nullptr, terminados, processMatrix.size() - loteIndex - 1, isPaused);
                 pause_ms(SIMULATION_SPEED_MS * 3);
            }
        }
    }

    cout << "\n\n--- TODOS LOS LOTES HAN SIDO PROCESADOS ---\n";
    cout << "Contador Global Final: " << globalCounter << " unidades de tiempo.\n";
    cout << "Presione Enter para terminar...";
    while (_kbhit()) _getch();
    cin.ignore();
}

void displayStatus(const vector<Process>& loteEspera,
                   const Process* procesoEjecutando,
                   const vector<FinishedProcess>& terminados,
                   int lotesPendientes, bool isPaused) {
    system("cls");

    cout << "Lotes Pendientes: " << lotesPendientes << (isPaused ? " (PAUSADO)" : "") << "\n";
    cout << string(110, '-') << "\n";

    cout << left << setw(30) << "LOTE TRABAJANDO" << "| "
         << setw(28) << "PROCESO EN EJECUCION" << "| "
         << setw(30) << "TERMINADOS" << "| "
         << "LOTE" << "\n";
    cout << left << setw(30) << "ID     TME    TT" << "| "
         << setw(28) << "" << "| "
         << setw(30) << "ID     Operacion      Res" << "| "
         << "TERM." << "\n";
    cout << string(110, '-') << "\n";

    size_t executingLines = 5;
    size_t maxRows = max({loteEspera.size(), terminados.size(), (procesoEjecutando ? executingLines : 0)});

    for (size_t i = 0; i < maxRows; ++i) {
        if (i < loteEspera.size()) {
            cout << left << setw(7) << loteEspera[i].getId()
                 << setw(7) << loteEspera[i].getMaxTime()
                 << setw(16) << loteEspera[i].getTimeElapsed();
        } else {
            cout << left << setw(30) << "";
        }
        cout << "| ";

        if (procesoEjecutando != nullptr) {
            string line = "";
            if (i == 0) line = "ID: " + to_string(procesoEjecutando->getId());
            else if (i == 1) line = "Ope: " + procesoEjecutando->getCalculator().getOperationString();
            else if (i == 2) line = "TME: " + to_string(procesoEjecutando->getMaxTime());
            else if (i == 3) line = "TT: " + to_string(procesoEjecutando->getTimeElapsed());
            else if (i == 4) line = "TR: " + to_string(procesoEjecutando->getMaxTime() - procesoEjecutando->getTimeElapsed());
            cout << left << setw(28) << line;
        } else {
            cout << left << setw(28) << "";
        }
        cout << "| ";

        if (i < terminados.size()) {
            stringstream ss;
            ss << left << setw(7) << terminados[i].id
               << setw(17) << terminados[i].operationString
               << terminados[i].resultString;
            cout << left << setw(30) << ss.str();
            cout << "| ";
            cout << terminados[i].batchNumber;
        } else {
            cout << left << setw(30) << "" << "|";
        }
        cout << "\n";
    }
    cout << string(110, '-') << "\n";
    cout << "Contador Global: " << globalCounter << "\n";
}

bool isIdUnique(const vector<vector<Process>>& processMatrix, unsigned int id, int currentCount) {
    if (id == 0) return false;
    int checkedCount = 0;
    for (const auto& lote : processMatrix) {
        for (const auto& proceso : lote) {
            if (checkedCount >= currentCount) return true;
            if (proceso.getId() == id) return false;
            checkedCount++;
        }
    }
    return true;
}

void generateProcessData(vector<vector<Process>>& processMatrix, int numberProcesses) {
    char operations[] = {'+', '-', '*', '/', '%', '^'};
    int count = 0;
    cout << "\nGenerando " << numberProcesses << " procesos aleatoriamente..." << endl;
    for (size_t i = 0; i < processMatrix.size(); ++i) {
        for (size_t j = 0; j < PROCESS_NUMBER; ++j) {
            if (count >= numberProcesses) break;
            unsigned int maxTime = 6 + rand() % (20 - 6 + 1);
            unsigned int id;
            do {
                id = 1 + rand() % 200;
            } while (!isIdUnique(processMatrix, id, count));
            char operation = operations[rand() % 6];
            float operandA = rand() % 100 + 1;
            float operandB;
            if (operation == '/' || operation == '%') {
                do { operandB = rand() % 100 + 1; } while (operandB == 0);
            } else {
                operandB = rand() % 100 + 1;
            }
            processMatrix[i][j] = Process(operation, operandA, operandB, maxTime, id);
            count++;
        }
    }
    cout << "Generacion completada." << endl;
}

void pause_ms(int milliseconds) {
    Sleep(milliseconds);
}
