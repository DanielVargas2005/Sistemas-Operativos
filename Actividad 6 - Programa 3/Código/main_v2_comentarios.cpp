/*
 Aprendizaje 6 programa 3 - Algoritmo de planificación FCFS (First Come First Server)
 Versión corregida y actualizada con modelo de 5 estados.
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

#define MAX_PROCESSES_IN_MEMORY 4
#define BLOCKED_TIME 8
#define SIMULATION_SPEED_MS 750

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
};

struct FinishedProcess {
    int id;
    string operationString;
    string resultString;
};

// --- PROTOTIPOS DE FUNCIONES ---
void generateProcessData(vector<Process>& newProcesses, int numberProcesses);
bool isIdUnique(const vector<Process>& processes, unsigned int id);
void simulateProcessing(vector<Process>& newProcesses, int numberProcesses);
void displayStatus(const vector<Process>& readyProcesses,
                   const vector<pair<Process, int>>& blockedProcesses,
                   const Process* executingProcess,
                   const vector<FinishedProcess>& finishedProcesses,
                   int newProcessCount, bool isPaused);
void pause_ms(int milliseconds);

// --- FUNCIÓN PRINCIPAL ---
int main() {
    srand(time(NULL));
    int numProcesses = 0;
    bool valid = false;
    cout << "--------Algoritmo de Planificacion FCFS--------" << endl;
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

    vector<Process> newProcesses(numProcesses);
    generateProcessData(newProcesses, numProcesses);

    cout << "\nPresione Enter para comenzar la simulacion...";
    cin.ignore();
    simulateProcessing(newProcesses, numProcesses);
    return 0;
}

// --- SIMULACIÓN PRINCIPAL CON MODELO DE 5 ESTADOS ---
void simulateProcessing(vector<Process>& newProcesses, int numberProcesses) {
    vector<Process> readyProcesses;
    vector<pair<Process, int>> blockedProcesses;
    vector<FinishedProcess> finishedProcesses;
    Process* executingProcess = nullptr;

    globalCounter = 0;
    bool isPaused = false;

    while (finishedProcesses.size() < numberProcesses) {
        // 1. ADMISIÓN: Mover de Nuevos a Listos si hay espacio en memoria
        while ((readyProcesses.size() + blockedProcesses.size() + (executingProcess ? 1 : 0)) < MAX_PROCESSES_IN_MEMORY && !newProcesses.empty()) {
            readyProcesses.push_back(newProcesses.front());
            newProcesses.erase(newProcesses.begin());
        }

        // 2. GESTIÓN DE BLOQUEADOS: Mover de Bloqueados a Listos si su tiempo ha terminado
        for (auto it = blockedProcesses.begin(); it != blockedProcesses.end();) {
            if (globalCounter - it->second >= BLOCKED_TIME) {
                readyProcesses.push_back(it->first);
                it = blockedProcesses.erase(it);
            } else {
                ++it;
            }
        }

        // 3. DESPACHO: Mover de Listos a Ejecución si el procesador está libre
        if (executingProcess == nullptr && !readyProcesses.empty()) {
            executingProcess = new Process(readyProcesses.front());
            readyProcesses.erase(readyProcesses.begin());
        }

        // 4. MANEJO DE TECLADO Y PAUSA
        if (_kbhit()) {
            char key = tolower(_getch());
            if (isPaused) {
                if (key == 'c') isPaused = false;
            } else {
                switch (key) {
                    case 'p': isPaused = true; break;
                    case 'e': // Interrupción
                        if (executingProcess != nullptr) {
                            blockedProcesses.push_back({*executingProcess, globalCounter});
                            delete executingProcess;
                            executingProcess = nullptr;
                        }
                        break;
                    case 'w': // Error
                        if (executingProcess != nullptr) {
                            finishedProcesses.push_back({(int)executingProcess->getId(), "X", "Error"});
                            delete executingProcess;
                            executingProcess = nullptr;
                        }
                        break;
                }
            }
        }

        // --- Actualización y visualización ---
        displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcesses, newProcesses.size(), isPaused);

        if (isPaused) {
            pause_ms(SIMULATION_SPEED_MS);
            continue; // Saltar el resto del ciclo si está en pausa
        }

        // 5. EJECUCIÓN: Si hay un proceso, avanzar su tiempo
        if (executingProcess != nullptr) {
            executingProcess->setTimeElapsed(executingProcess->getTimeElapsed() + 1);
            if (executingProcess->getTimeElapsed() >= executingProcess->getMaxTime()) {
                // El proceso termina normalmente
                float result = executingProcess->getCalculator().calculate();
                stringstream ss;
                ss << fixed << setprecision(2) << result;
                finishedProcesses.push_back({
                    (int)executingProcess->getId(),
                    executingProcess->getCalculator().getOperationString(),
                    ss.str()
                });
                delete executingProcess;
                executingProcess = nullptr;
            }
        }

        globalCounter++;
        pause_ms(SIMULATION_SPEED_MS);
    }

    // Pantalla final
    displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcesses, newProcesses.size(), isPaused);
    cout << "\n\n--- TODOS LOS PROCESOS HAN SIDO PROCESADOS ---\n";
    cout << "Contador Global Final: " << globalCounter << " unidades de tiempo.\n";
    cout << "Presione Enter para terminar...";
    while (_kbhit()) _getch();
    cin.ignore();
}

// --- FUNCIÓN DE VISUALIZACIÓN ACTUALIZADA ---
void displayStatus(const vector<Process>& readyProcesses,
                   const vector<pair<Process, int>>& blockedProcesses,
                   const Process* executingProcess,
                   const vector<FinishedProcess>& finishedProcesses,
                   int newProcessCount, bool isPaused) {
    system("cls");

    cout << "Procesos Nuevos: " << newProcessCount << (isPaused ? " (PAUSADO)" : "") << "\n";
    cout << string(100, '-') << "\n";

    cout << left << setw(35) << "COLA DE LISTOS (Ready)" << "| "
         << setw(30) << "PROCESO EN EJECUCION" << "| "
         << setw(30) << "PROCESOS TERMINADOS" << "\n";
    cout << left << setw(35) << "ID     TME    TT" << "| "
         << setw(30) << "" << "| "
         << setw(30) << "ID     Operacion      Res" << "\n";
    cout << string(100, '-') << "\n";

    size_t executingLines = 5;
    size_t maxRows = max({readyProcesses.size(), finishedProcesses.size(), (executingProcess ? executingLines : 0)});

    for (size_t i = 0; i < maxRows; ++i) {
        // Columna 1: Cola de Listos
        if (i < readyProcesses.size()) {
            cout << left << setw(7) << readyProcesses[i].getId()
                 << setw(7) << readyProcesses[i].getMaxTime()
                 << setw(21) << readyProcesses[i].getTimeElapsed();
        } else {
            cout << left << setw(35) << "";
        }
        cout << "| ";

        // Columna 2: Proceso en Ejecución
        if (executingProcess != nullptr && i < executingLines) {
            string line = "";
            if (i == 0) line = "ID: " + to_string(executingProcess->getId());
            else if (i == 1) line = "Ope: " + executingProcess->getCalculator().getOperationString();
            else if (i == 2) line = "TME: " + to_string(executingProcess->getMaxTime());
            else if (i == 3) line = "TT: " + to_string(executingProcess->getTimeElapsed());
            else if (i == 4) line = "TR: " + to_string(executingProcess->getMaxTime() - executingProcess->getTimeElapsed());
            cout << left << setw(30) << line;
        } else {
            cout << left << setw(30) << "";
        }
        cout << "| ";

        // Columna 3: Procesos Terminados
        if (i < finishedProcesses.size()) {
            stringstream ss;
            ss << left << setw(7) << finishedProcesses[i].id
               << setw(17) << finishedProcesses[i].operationString
               << finishedProcesses[i].resultString;
            cout << left << setw(30) << ss.str();
        } else {
            cout << left << setw(30) << "";
        }
        cout << "\n";
    }
    cout << string(100, '-') << "\n";

    // Nueva sección para Procesos Bloqueados
    cout << "Procesos Bloqueados:" << endl;
    if (blockedProcesses.empty()) {
        cout << " " << endl;
    } else {
        for (const auto& blocked : blockedProcesses) {
            cout << "ID: " << setw(4) << left << blocked.first.getId()
                 << "TT en Bloqueado: " << (globalCounter - blocked.second) << " / " << BLOCKED_TIME << endl;
        }
    }
    cout << string(100, '-') << "\n";
    cout << "Contador Global: " << globalCounter << "\n";
}

// --- FUNCIONES AUXILIARES (Ligeramente adaptadas) ---
bool isIdUnique(const vector<Process>& processes, unsigned int id) {
    if(id == 0) return false;
    for (const Process& p : processes) {
        if (p.getId() == id) return false;
    }
    return true;
}

void generateProcessData(vector<Process>& newProcesses, int numberProcesses) {
    char operations[] = {'+', '-', '*', '/', '%', '^'};
    vector<unsigned int> usedIds;
    cout << "\nGenerando " << numberProcesses << " procesos aleatoriamente..." << endl;
    for (int i = 0; i < numberProcesses; i++) {
        unsigned int maxTime = 6 + rand() % (20 - 6 + 1);
        unsigned int id;
        do {
            id = 1 + rand() % 500; // Rango de ID ampliado para evitar bucles infinitos
            bool isUsed = false;
            for(unsigned int usedId : usedIds) {
                if (id == usedId) {
                    isUsed = true;
                    break;
                }
            }
            if(!isUsed) break;
        } while (true);
        usedIds.push_back(id);

        char operation = operations[rand() % 6];
        float operandA = rand() % 100 + 1;
        float operandB;
        if (operation == '/' || operation == '%') {
            do { operandB = rand() % 100 + 1; } while (operandB == 0);
        } else {
            operandB = rand() % 100 + 1;
        }

        newProcesses[i] = Process(operation, operandA, operandB, maxTime, id);
    }
    cout << "Generacion completada." << endl;
}

void pause_ms(int milliseconds) {
    Sleep(milliseconds);
}
