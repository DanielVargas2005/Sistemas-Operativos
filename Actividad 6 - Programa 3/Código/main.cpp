/*
 Aprendizaje 6 programa 3 - Algoritmo de planificación FCFS (First Come First Server)
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
};

struct FinishedProcess {
    int id;
    int batchNumber;
    string operationString;
    string resultString;
};

void generateProcessData(vector<Process>& newsProcess, int numberProcesses);
bool isIdUnique(const vector<Process>& newsProcess, unsigned int id);
void simulateProcessing(vector<Process>& newsProcess, int numberProcesses);
void displayStatus(const vector<Process>& loteEspera,
                   const Process* procesoEjecutando,
                   const vector<FinishedProcess>& finishedProcesses,
                   int lotesNuevos, bool isPaused);
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

    //int lotsNumber = p.lotsCreator(numProcesses);
    //vector<vector<Process>> newsProcess(lotsNumber, vector<Process>(PROCESS_NUMBER))
    vector<Process> newsProcess(numProcesses); 

    // generateProcessData(newsProcess, numProcesses);
    generateProcessData(newsProcess, numProcesses);
    
    cout << "\nPresione Enter para comenzar la simulacion...";
    cin.ignore();
    simulateProcessing(newsProcess, numProcesses);
    return 0;
}

void simulateProcessing(vector<Process>& newsProcess, int numberProcesses) {
    if (newsProcess.empty()) return;

    vector<FinishedProcess> finishedProcesses;
    globalCounter = 0;
    bool isPaused = false;

    // No hay lotes, solo una lista de procesos
    vector<Process> readyProcesses = newsProcess;
    vector<pair<Process, int>> blockedProcesses;

    while (!readyProcesses.empty() || !blockedProcesses.empty()) {
        // 🔄 Revisar bloqueados
        for (auto it = blockedProcesses.begin(); it != blockedProcesses.end();) {
            if (globalCounter - it->second >= 8) {
                readyProcesses.push_back(it->first);
                it = blockedProcesses.erase(it);
            } else {
                ++it;
            }
        }

        if (readyProcesses.empty()) {
            displayStatus(readyProcesses, nullptr, finishedProcesses, 0, isPaused);
            pause_ms(SIMULATION_SPEED_MS);
            globalCounter++;
            continue;
        }

        Process procesoActual = readyProcesses.front();
        readyProcesses.erase(readyProcesses.begin());

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
                            blockedProcesses.push_back({procesoActual, globalCounter});
                            processInterrupted = true;
                            break;
                        case 'w':
                            finishedProcesses.push_back({(int)procesoActual.getId(), 1, "X", "Error"});
                            processAborted = true;
                            break;
                        case 'p':
                            isPaused = true;
                            break;
                    }
                }
            }

            if (processAborted || processInterrupted) break;

            if (isPaused) {
                t--;
                displayStatus(readyProcesses, &procesoActual, finishedProcesses, 0, isPaused);
                pause_ms(SIMULATION_SPEED_MS);
                continue;
            }

            procesoActual.setTimeElapsed(tiempoEjecutadoPrevio + t + 1);
            displayStatus(readyProcesses, &procesoActual, finishedProcesses, 0, isPaused);
            pause_ms(SIMULATION_SPEED_MS);
            globalCounter++;
        }

        if (!processAborted && !processInterrupted) {
            float resultadoNum = procesoActual.getCalculator().calculate();
            stringstream ss;
            ss << fixed << setprecision(2) << resultadoNum;
            finishedProcesses.push_back({
                (int)procesoActual.getId(),
                1,
                procesoActual.getCalculator().getOperationString(),
                ss.str()
            });
        }

        displayStatus(readyProcesses, nullptr, finishedProcesses, 0, isPaused);
        pause_ms(SIMULATION_SPEED_MS * (processInterrupted ? 1 : 3));
    }

    cout << "\n\n--- TODOS LOS PROCESOS HAN SIDO PROCESADOS ---\n";
    cout << "Contador Global Final: " << globalCounter << " unidades de tiempo.\n";
    cout << "Presione Enter para terminar...";
    while (_kbhit()) _getch();
    cin.ignore();
}


void displayStatus(const vector<Process>& loteEspera,
                   const Process* procesoEjecutando,
                   const vector<FinishedProcess>& finishedProcesses,
                   int lotesNuevos, bool isPaused) {
    system("cls");

    cout << "Procesos nuevos: " << lotesNuevos << (isPaused ? " (PAUSADO)" : "") << "\n";
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
    size_t maxRows = max({loteEspera.size(), finishedProcesses.size(), (procesoEjecutando ? executingLines : 0)});

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

        if (i < finishedProcesses.size()) {
            stringstream ss;
            ss << left << setw(7) << finishedProcesses[i].id
               << setw(17) << finishedProcesses[i].operationString
               << finishedProcesses[i].resultString;
            cout << left << setw(30) << ss.str();
            cout << "| ";
            cout << finishedProcesses[i].batchNumber;
        } else {
            cout << left << setw(30) << "" << "|";
        }
        cout << "\n";
    }
    cout << string(110, '-') << "\n";
    cout << "Contador Global: " << globalCounter << "\n";
}

bool isIdUnique(const vector<Process>& newsProcess, unsigned int id) {
    if(id == 0){
        return false;
    } 

    for (const Process& p : newsProcess) {
        if (p.getId() == id) {
            return false;
        }
    }

    return true;
}


void generateProcessData(vector<Process>& newsProcess, int numberProcesses) {
    char operations[] = {'+', '-', '*', '/', '%', '^'};
    int i;
    cout << "\nGenerando " << numberProcesses << " procesos aleatoriamente..." << endl;
    for (i = 0; i < numberProcesses; i++) {
        unsigned int maxTime = 6 + rand() % (20 - 6 + 1);
        unsigned int id;

        do {
            id = 1 + rand() % 200;
        } while (!isIdUnique(newsProcess, id));

        char operation = operations[rand() % 6];
        float operandA = rand() % 50 + 1;
        float operandB;

        if (operation == '/' || operation == '%') {
            do { 
                operandB = rand() % 50 + 1; 
            } while (operandB == 0);
        }
        else if (operation == '^') {
            operandB = rand() % 3 + 1;
        }
        else{
            operandB = rand() % 50 + 1;
        }
        
        newsProcess[i] = Process(operation, operandA, operandB, maxTime, id);
    }
    cout << "Generacion completada." << endl;
}

void pause_ms(int milliseconds) {
    Sleep(milliseconds);
}
