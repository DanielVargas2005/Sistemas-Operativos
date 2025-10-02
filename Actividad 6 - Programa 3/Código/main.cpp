/*
 Aprendizaje 6 programa 3 - Algoritmo de planificación FCFS (First Come First Server)
 Versión corregida y actualizada con modelo de 5 estados y tabla BCP.
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
#define SIMULATION_SPEED_MS 500

int globalCounter = 0;

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
    string getOperationString() const {
        stringstream ss;
        ss << fixed << setprecision(0) << operandA << " " << operation << " " << operandB;
        return ss.str();
    }
    float calculate() const {
        switch (operation) {
        case '+':
            return operandA + operandB;
        case '-':
            return operandA - operandB;
        case '*':
            return operandA * operandB;
        case '/':
            return operandA / operandB;
        case '%':
            return fmod(operandA, operandB);
        case '^':
            return pow(operandA, operandB);
        default:
            return 0;
        }
    }
};

class Process {
private:
    unsigned int maxTime;
    unsigned int id;
    unsigned int timeElapsed;
    Calculator calculator;
    bool errorTermination;
    bool firstExecutionDone;
    unsigned int arrivalTime;
    unsigned int completionTime;
    unsigned int waitingTime;
    unsigned int responseTime;
    unsigned int returnTime;
    unsigned int serviceTime;
    unsigned int firstExecutionTime;
public:
    Process() : maxTime(0), id(0), timeElapsed(0), errorTermination(false), firstExecutionDone(false),
        arrivalTime(0), completionTime(0), waitingTime(0), responseTime(0),
        returnTime(0), serviceTime(0), firstExecutionTime(0) {}

    Process(char op, float a, float b, unsigned int t, unsigned int i) :
        maxTime(t), id(i), timeElapsed(0), errorTermination(false), firstExecutionDone(false),
        arrivalTime(0), completionTime(0), waitingTime(0), responseTime(0),
        returnTime(0), serviceTime(0), firstExecutionTime(0) {
        calculator.setOperation(op);
        calculator.setOperandA(a);
        calculator.setOperandB(b);
    }
    void setTimeElapsed(unsigned int t) {
        timeElapsed = t;
    }
    unsigned int getMaxTime() const {
        return maxTime;
    }
    unsigned int getId() const {
        return id;
    }
    unsigned int getTimeElapsed() const {
        return timeElapsed;
    }
    const Calculator& getCalculator() const {
        return calculator;
    }
    void setArrivalTime(unsigned int t) {
        arrivalTime = t;
    }
    void setCompletionTime(unsigned int t) {
        completionTime = t;
    }
    void setFirstExecutionTime(unsigned int t) {
        firstExecutionTime = t;
    }
    void setErrorTermination(bool error) {
        errorTermination = error;
    }
    void setFirstExecutionDone(bool done) {
        firstExecutionDone = done;
    }
    bool hasErrorTerminated() const {
        return errorTermination;
    }
    bool hasExecutedFirstTime() const {
        return firstExecutionDone;
    }
    unsigned int getArrivalTime() const {
        return arrivalTime;
    }
    unsigned int getCompletionTime() const {
        return completionTime;
    }
    unsigned int getWaitingTime() const {
        return waitingTime;
    }
    unsigned int getResponseTime() const {
        return responseTime;
    }
    unsigned int getReturnTime() const {
        return returnTime;
    }
    unsigned int getServiceTime() const {
        return serviceTime;
    }
    void calculateBcpTimes() {
        serviceTime = timeElapsed;
        returnTime = completionTime - arrivalTime;
        waitingTime = returnTime - serviceTime;
        responseTime = firstExecutionTime - arrivalTime;
    }
};

struct FinishedProcess {
    int id;
    string operationString;
    string resultString;
};

void generateProcessData(vector<Process>& newProcesses, int numberProcesses);
bool isIdUnique(const vector<Process>& processes, unsigned int id);
void simulateProcessing(vector<Process>& newProcesses, int numberProcesses);
void displayStatus(const vector<Process>& readyProcesses,
                   const vector<pair<Process, int>>& blockedProcesses,
                   const Process* executingProcess,
                   const vector<FinishedProcess>& finishedProcesses,
                   int newProcessCount, bool isPaused);
void pause_ms(int milliseconds);
void simulatingBcp(vector<Process>& finishedProcesses);
void displayBcp(const vector<Process>& processes);

int main() {
    srand(time(NULL));
    int numProcesses = 0;
    bool valid = false;
    cout << "--------Algoritmo de Planificacion FCFS--------" << endl << endl;
    do {
        cout << "Cuantos procesos desea generar? ";
        string input;
        getline(cin, input);
        system("cls");
        cout << "--------Algoritmo de Planificacion FCFS--------" << endl << endl;
        try {
            if (input.empty()) throw runtime_error("La entrada no puede estar vacia.");
            numProcesses = stoi(input);
            if (numProcesses <= 0) throw out_of_range("El numero debe ser mayor a 0.");
            valid = true;
        } catch (const std::invalid_argument& e) {
            cout << "Error: Solo se admiten valores enteros." << endl;
        } catch (const std::out_of_range& e) {
            cout << "Error: " << e.what() << endl;
        } catch (const std::exception& e) {
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

void simulateProcessing(vector<Process>& newProcesses, int numberProcesses) {
    vector<Process> readyProcesses;
    vector<pair<Process, int>> blockedProcesses;
    vector<FinishedProcess> finishedProcessesForDisplay;
    vector<Process> completedProcessesForBcp;
    Process* executingProcess = nullptr;
    globalCounter = 0;
    bool isPaused = false;
    while (completedProcessesForBcp.size() < static_cast<size_t>(numberProcesses)) {
        while ((readyProcesses.size() + blockedProcesses.size() + (executingProcess ? 1 : 0)) < MAX_PROCESSES_IN_MEMORY && !newProcesses.empty()) {
            Process& p = newProcesses.front();
            p.setArrivalTime(globalCounter);
            readyProcesses.push_back(p);
            newProcesses.erase(newProcesses.begin());
        }
        for (auto it = blockedProcesses.begin(); it != blockedProcesses.end();) {
            if (globalCounter - it->second >= BLOCKED_TIME) {
                readyProcesses.push_back(it->first);
                it = blockedProcesses.erase(it);
            } else {
                ++it;
            }
        }
        if (executingProcess == nullptr && !readyProcesses.empty()) {
            executingProcess = new Process(readyProcesses.front());
            if (!executingProcess->hasExecutedFirstTime()) {
                executingProcess->setFirstExecutionTime(globalCounter);
                executingProcess->setFirstExecutionDone(true);
            }
            readyProcesses.erase(readyProcesses.begin());
        }
        if (_kbhit()) {
            char key = tolower(_getch());
            if (isPaused) {
                if (key == 'c') isPaused = false;
            } else {
                switch (key) {
                case 'p':
                    isPaused = true;
                    break;
                case 'e':
                    if (executingProcess != nullptr) {
                        blockedProcesses.push_back({*executingProcess, globalCounter});
                        delete executingProcess;
                        executingProcess = nullptr;
                        while(_kbhit()){
                            _getch();
                        }
                    }
                    break;
                case 'w':
                    if (executingProcess != nullptr) {
                        executingProcess->setTimeElapsed(executingProcess->getTimeElapsed() + 1);
                        executingProcess->setCompletionTime(globalCounter + 1);
                        executingProcess->setErrorTermination(true);
                        completedProcessesForBcp.push_back(*executingProcess);
                        finishedProcessesForDisplay.push_back({(int)executingProcess->getId(), "X", "Error"});
                        delete executingProcess;
                        executingProcess = nullptr;
                        while(_kbhit()){
                            _getch();
                        }
                    }
                    break;
                default:
                    ;
                }
            }
        }
        displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcessesForDisplay, newProcesses.size(), isPaused);
        if (isPaused) {
            pause_ms(SIMULATION_SPEED_MS);
            continue;
        }
        if (executingProcess != nullptr) {
            executingProcess->setTimeElapsed(executingProcess->getTimeElapsed() + 1);
            if (executingProcess->getTimeElapsed() >= executingProcess->getMaxTime()) {
                executingProcess->setCompletionTime(globalCounter + 1);
                completedProcessesForBcp.push_back(*executingProcess);
                float result = executingProcess->getCalculator().calculate();
                stringstream ss;
                ss << fixed << setprecision(2) << result;
                finishedProcessesForDisplay.push_back({
                    (int)executingProcess->getId(),
                    executingProcess->getCalculator().getOperationString(),
                    ss.str()
                });
                delete executingProcess;
                executingProcess = nullptr;
                while(_kbhit()){
                    _getch();
                }
            }
        }
        globalCounter++;
        pause_ms(SIMULATION_SPEED_MS);
    }
    displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcessesForDisplay, newProcesses.size(), isPaused);
    cout << "\n\n--- TODOS LOS PROCESOS HAN SIDO TERMINADOS ---\n";
    cout << "Contador Global Final: " << globalCounter << " unidades de tiempo.\n";
    cout << "Presione Enter para mostrar el BCP...";
    while (_kbhit()) _getch();
    cin.ignore();
    cout << endl;
    sort(completedProcessesForBcp.begin(), completedProcessesForBcp.end(), [](const Process& a, const Process& b) {
        return a.getId() < b.getId();
    });
    simulatingBcp(completedProcessesForBcp);
}

void simulatingBcp(vector<Process>& finishedProcesses) {
    for (auto& process : finishedProcesses) {
        process.calculateBcpTimes();
    }
    displayBcp(finishedProcesses);
}

void displayBcp(const vector<Process>& processes) {
    //system("cls");
    cout << "--- BLOQUE DE CONTROL DE PROCESOS (BCP) ---" << endl;
    cout << string(120, '-') << endl;
    cout << left
         << setw(5) << "ID"
         << setw(18) << "Operacion"
         << setw(12) << "Resultado"
         << setw(12) << "T. Llegada"
         << setw(18) << "T. Finalizacion"
         << setw(12) << "T. Espera"
         << setw(12) << "T. Respuesta"
         << setw(12) << "T. Retorno"
         << setw(12) << "T. Servicio"
         << setw(7) << "TME"
         << endl;
    cout << string(120, '-') << endl;
    for (const auto& p : processes) {
        cout << left << setw(5) << p.getId();
        if (p.hasErrorTerminated()) {
            cout << setw(18) << "X" << setw(12) << "Error";
        } else {
            stringstream result_ss;
            result_ss << fixed << setprecision(2) << p.getCalculator().calculate();
            cout << setw(18) << p.getCalculator().getOperationString()
                 << setw(12) << result_ss.str();
        }
        cout << setw(12) << p.getArrivalTime()
             << setw(18) << p.getCompletionTime()
             << setw(12) << p.getWaitingTime()
             << setw(12) << p.getResponseTime()
             << setw(12) << p.getReturnTime()
             << setw(12) << p.getServiceTime()
             << setw(7) << p.getMaxTime()
             << endl;
    }
    cout << string(120, '-') << endl << endl;
    cout << "Presione Enter para finalizar...";
    cin.ignore();
}

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
    cout << left << setw(35) << "ID      TME   TT" << "| "
         << setw(30) << "" << "| "
         << setw(30) << "ID      Operacion        Res" << "\n";
    cout << string(100, '-') << "\n";
    size_t executingLines = 5;
    size_t maxRows = max({readyProcesses.size(), finishedProcesses.size(), (executingProcess ? executingLines : 0)});
    for (size_t i = 0; i < maxRows; ++i) {
        if (i < readyProcesses.size()) {
            cout << left << setw(7) << readyProcesses[i].getId()
                 << setw(7) << readyProcesses[i].getMaxTime()
                 << setw(21) << readyProcesses[i].getTimeElapsed();
        } else {
            cout << left << setw(35) << "";
        }
        cout << "| ";
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

bool isIdUnique(const vector<Process>& processes, unsigned int id) {
    if (id == 0) return false;
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
            id = 1 + rand() % 500;
            bool isUsed = false;
            for (unsigned int usedId : usedIds) {
                if (id == usedId) {
                    isUsed = true;
                    break;
                }
            }
            if (!isUsed) break;
        } while (true);
        usedIds.push_back(id);
        char operation = operations[rand() % 6];
        float operandA = rand() % 100 + 1;
        float operandB;
        if (operation == '/' || operation == '%') {
            do {
                operandB = rand() % 100 + 1;
            } while (operandB == 0);
        }
        if(operation == '^'){
            operandB = rand () % 3 + 1;
        }
        else {
            operandB = rand() % 100 + 1;
        }
        newProcesses[i] = Process(operation, operandA, operandB, maxTime, id);
    }
    cout << "Generacion completada." << endl;
}

void pause_ms(int milliseconds) {
    Sleep(milliseconds);
}
