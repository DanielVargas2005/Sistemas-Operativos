/*
 Aprendizaje 10 Programa 5 - Algoritmo de Planificación RR (Round-Robin)
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
#define SIMULATION_SPEED_MS 800

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
    void setTimeElapsed(unsigned int t) { timeElapsed = t; }
    unsigned int getMaxTime() const { return maxTime; }
    unsigned int getId() const { return id; }
    unsigned int getTimeElapsed() const { return timeElapsed; }
    const Calculator& getCalculator() const { return calculator; }
    void setArrivalTime(unsigned int t) { arrivalTime = t; }
    void setCompletionTime(unsigned int t) { completionTime = t; }
    void setFirstExecutionTime(unsigned int t) { firstExecutionTime = t; }
    unsigned int getFirstExecutionTime() const { return firstExecutionTime; }
    void setErrorTermination(bool error) { errorTermination = error; }
    void setFirstExecutionDone(bool done) { firstExecutionDone = done; }
    bool hasErrorTerminated() const { return errorTermination; }
    bool hasExecutedFirstTime() const { return firstExecutionDone; }
    unsigned int getArrivalTime() const { return arrivalTime; }
    unsigned int getCompletionTime() const { return completionTime; }
    unsigned int getWaitingTime() const { return waitingTime; }
    unsigned int getResponseTime() const { return responseTime; }
    unsigned int getReturnTime() const { return returnTime; }
    unsigned int getServiceTime() const { return serviceTime; }
    void calculateBcpTimes() {
        serviceTime = timeElapsed;
        if (!errorTermination && timeElapsed >= maxTime) {
            serviceTime = maxTime;
        }
        if (completionTime > 0) {
            returnTime = completionTime - arrivalTime;
            waitingTime = returnTime - serviceTime;
        } else {
             returnTime = 0;
             waitingTime = 0;
        }

        if(firstExecutionDone && firstExecutionTime >= arrivalTime) {
            responseTime = firstExecutionTime - arrivalTime;
        } else {
            responseTime = 0;
        }
    }
};

struct FinishedProcess {
    int id;
    string operationString;
    string resultString;
};

void displayLiveBcp(const vector<Process>& newProcesses,
                    const vector<Process>& readyProcesses,
                    const Process* executingProcess,
                    const vector<pair<Process, int>>& blockedProcesses,
                    const vector<Process>& completedProcesses);
void generateProcessData(vector<Process>& newProcesses, int numberProcesses, vector<unsigned int>& usedIds);
void simulateProcessing(vector<Process>& newProcesses, int numberProcesses, vector<unsigned int>& usedIds, int quantumValue);
void displayStatus(const vector<Process>& readyProcesses,
                   const vector<pair<Process, int>>& blockedProcesses,
                   const Process* executingProcess,
                   const vector<FinishedProcess>& finishedProcesses,
                   int newProcessCount, bool isPaused,
                   int quantumValue, int quantumCounter);
void pause_ms(int milliseconds);
void simulatingBcp(vector<Process>& finishedProcesses);
void displayBcp(const vector<Process>& processes);


int main() {
    srand(time(NULL));
    int numProcesses = 0;
    int quantumValue = 0;
    bool validProcesses = false;
    bool validQuantum = false;

    cout << "--------Algoritmo de Planificacion Round-Robin (RR)--------" << endl << endl;

    do {
        if (!validProcesses) {
            cout << "Cuantos procesos desea generar? ";
            string inputProcesses;
            getline(cin, inputProcesses);
            try {
                if (inputProcesses.empty()) throw runtime_error("La entrada no puede estar vacia.");
                numProcesses = stoi(inputProcesses);
                if (numProcesses <= 0) throw out_of_range("El numero de procesos debe ser mayor a 0.");
                validProcesses = true;
            } catch (const std::invalid_argument& e) {
                cout << "Error: Solo se admiten valores enteros." << endl;
            } catch (const std::out_of_range& e) {
                cout << "Error: " << e.what() << endl;
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << endl;
            }
            if (!validProcesses) {
                pause_ms(3000);
                system("cls");
                cout << "--------Algoritmo de Planificacion Round-Robin (RR)--------" << endl << endl;
                continue;
            }
        }

        if (validProcesses && !validQuantum) {
             cout << "Cual sera el valor para el Quantum? ";
             string inputQuantum;
             getline(cin, inputQuantum);
             try {
                if (inputQuantum.empty()) throw runtime_error("La entrada no puede estar vacia.");
                quantumValue = stoi(inputQuantum);
                if (quantumValue <= 0) throw out_of_range("El Quantum debe ser mayor a 0.");
                validQuantum = true;
            } catch (const std::invalid_argument& e) {
                cout << "Error: Solo se admiten valores enteros." << endl;
            } catch (const std::out_of_range& e) {
                cout << "Error: " << e.what() << endl;
            } catch (const std::exception& e) {
                cout << "Error: " << e.what() << endl;
            }

            if (!validQuantum) {
                 pause_ms(3000);
                 system("cls");
                 cout << "--------Algoritmo de Planificacion Round-Robin (RR)--------" << endl << endl;
                 cout << "Cuantos procesos desea generar? " << numProcesses << endl;
            }
        }

    } while (!validProcesses || !validQuantum);

    system("cls");
    cout << "--------Algoritmo de Planificacion Round-Robin (RR)--------" << endl << endl;
    cout << "Procesos a generar: " << numProcesses << endl;
    cout << "Valor del Quantum: " << quantumValue << endl;

    vector<Process> newProcesses(numProcesses);
    vector<unsigned int> usedIds;
    generateProcessData(newProcesses, numProcesses, usedIds);

    cout << "\nPresione Enter para comenzar la simulacion...";
    cin.clear();
    cin.sync();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    simulateProcessing(newProcesses, numProcesses, usedIds, quantumValue);
    return 0;
}

void simulateProcessing(vector<Process>& newProcesses, int numberProcesses, vector<unsigned int>& usedIds, int quantumValue) {
    vector<Process> readyProcesses;
    vector<pair<Process, int>> blockedProcesses;
    vector<FinishedProcess> finishedProcessesForDisplay;
    vector<Process> completedProcessesForBcp;
    Process* executingProcess = nullptr;

    size_t totalProcessCount = numberProcesses;
    int quantumCounter = 0;

    globalCounter = -1;
    bool isPaused = false;
    bool bcpModeActive = false;
    bool bcpTableDrawn = false;

    while (completedProcessesForBcp.size() < totalProcessCount) {

        if (!isPaused) {
            globalCounter++;
            if (executingProcess != nullptr) {
                executingProcess->setTimeElapsed(executingProcess->getTimeElapsed() + 1);
                quantumCounter++;
            }
        }

        if (_kbhit()) {
            char key = tolower(_getch());
            if (isPaused) {
                if (key == 'c') {
                    isPaused = false;
                    bcpModeActive = false;
                }
            } else {
                switch (key) {
                case 'b':
                    isPaused = true;
                    bcpModeActive = true;
                    bcpTableDrawn = false;
                    break;
                case 'n':
                    {
                        totalProcessCount++;
                        unsigned int newId;
                        do {
                            newId = 1 + rand() % 500;
                            bool isUsed = false;
                            vector<Process> allProcs = newProcesses;
                            allProcs.insert(allProcs.end(), readyProcesses.begin(), readyProcesses.end());
                            if(executingProcess) allProcs.push_back(*executingProcess);
                            for(const auto& bp : blockedProcesses) allProcs.push_back(bp.first);
                            for(const auto& cp : completedProcessesForBcp) allProcs.push_back(cp);
                            for(const auto& p : allProcs) if(p.getId() == newId) isUsed = true;
                            for(unsigned int usedId : usedIds) if(newId == usedId) isUsed = true;

                            if(!isUsed) break;
                        } while(true);
                        usedIds.push_back(newId);
                        unsigned int newMaxTime = 6 + rand() % (15 - 6 + 1);
                        char operations[] = {'+', '-', '*', '/', '%', '^'};
                        char newOperation = operations[rand() % 6];
                        float newOperandA = rand() % 100 + 1;
                        float newOperandB;
                         if (newOperation == '/' || newOperation == '%') {
                            do { newOperandB = rand() % 100 + 1; } while (newOperandB == 0);
                        } else if(newOperation == '^'){
                            newOperandB = rand () % 3 + 1;
                        } else {
                            newOperandB = rand() % 100 + 1;
                        }
                        Process newProcess(newOperation, newOperandA, newOperandB, newMaxTime, newId);
                        newProcesses.push_back(newProcess);
                        while(_kbhit()) _getch();
                    }
                    break;
                case 'p':
                    isPaused = true;
                    break;
                case 'e':
                    if (executingProcess != nullptr) {
                        blockedProcesses.push_back({*executingProcess, globalCounter});
                        delete executingProcess;
                        executingProcess = nullptr;
                        quantumCounter = 0;
                        while(_kbhit()) _getch();
                    }
                    break;
                case 'w':
                    if (executingProcess != nullptr) {
                        executingProcess->setCompletionTime(globalCounter);
                        executingProcess->setErrorTermination(true);
                        completedProcessesForBcp.push_back(*executingProcess);
                        finishedProcessesForDisplay.push_back({(int)executingProcess->getId(), "X", "Error"});
                        delete executingProcess;
                        executingProcess = nullptr;
                        quantumCounter = 0;
                        while(_kbhit()) _getch();
                    }
                    break;
                }
            }
        }

        if (executingProcess != nullptr && executingProcess->getTimeElapsed() >= executingProcess->getMaxTime()) {
            executingProcess->setCompletionTime(globalCounter);
            completedProcessesForBcp.push_back(*executingProcess);
            float result = executingProcess->getCalculator().calculate();
            string resultStr;
             if (isinf(result)) {
                 resultStr = "Error (Div 0)";
                 executingProcess->setErrorTermination(true);
             } else {
                 stringstream ss;
                 ss << fixed << setprecision(2) << result;
                 resultStr = ss.str();
             }
            finishedProcessesForDisplay.push_back({(int)executingProcess->getId(), executingProcess->getCalculator().getOperationString(), resultStr});
            delete executingProcess;
            executingProcess = nullptr;
            quantumCounter = 0;
            while(_kbhit()) _getch();
        }
        else if (executingProcess != nullptr && quantumCounter >= quantumValue) {
            readyProcesses.push_back(*executingProcess);
            delete executingProcess;
            executingProcess = nullptr;
            quantumCounter = 0;
            while(_kbhit()) _getch();
        }

        while ((readyProcesses.size() + blockedProcesses.size() + (executingProcess ? 1 : 0)) < MAX_PROCESSES_IN_MEMORY && !newProcesses.empty()) {
            Process& p = newProcesses.front();
            p.setArrivalTime(globalCounter);
            readyProcesses.push_back(p);
            newProcesses.erase(newProcesses.begin());
        }
        auto it = blockedProcesses.begin();
        while (it != blockedProcesses.end()) {
            if (globalCounter - it->second >= BLOCKED_TIME) {
                readyProcesses.push_back(it->first);
                it = blockedProcesses.erase(it);
            } else {
                ++it;
            }
        }
        if (executingProcess == nullptr && !readyProcesses.empty()) {
            executingProcess = new Process(readyProcesses.front());
            readyProcesses.erase(readyProcesses.begin());
            if (!executingProcess->hasExecutedFirstTime()) {
                executingProcess->setFirstExecutionTime(globalCounter);
                executingProcess->setFirstExecutionDone(true);
            }
            quantumCounter = 0;
        }

        if (bcpModeActive) {
            if (!bcpTableDrawn) {
                displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcessesForDisplay, newProcesses.size(), isPaused, quantumValue, quantumCounter);
                displayLiveBcp(newProcesses, readyProcesses, executingProcess, blockedProcesses, completedProcessesForBcp);
                bcpTableDrawn = true;
            }
        } else {
            displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcessesForDisplay, newProcesses.size(), isPaused, quantumValue, quantumCounter);
        }

        if(isPaused) {
            pause_ms(50);
        } else {
            pause_ms(SIMULATION_SPEED_MS);
        }
    }

    displayStatus(readyProcesses, blockedProcesses, executingProcess, finishedProcessesForDisplay, newProcesses.size(), isPaused, quantumValue, quantumCounter);
    cout << "\n\n--- TODOS LOS PROCESOS HAN SIDO TERMINADOS ---\n";
    cout << "Contador Global Final: " << globalCounter << " unidades de tiempo.\n";
    cout << "Presione Enter para mostrar el BCP...";
    while (_kbhit()) _getch();
    cin.clear();
    cin.sync();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << endl;
    sort(completedProcessesForBcp.begin(), completedProcessesForBcp.end(), [](const Process& a, const Process& b) {
        return a.getId() < b.getId();
    });
    simulatingBcp(completedProcessesForBcp);
}

void displayStatus(const vector<Process>& readyProcesses,
                   const vector<pair<Process, int>>& blockedProcesses,
                   const Process* executingProcess,
                   const vector<FinishedProcess>& finishedProcesses,
                   int newProcessCount, bool isPaused,
                   int quantumValue, int quantumCounter) {
    system("cls");
    cout << "Procesos Nuevos: " << newProcessCount << "\t\tQuantum: " << quantumValue << (isPaused ? " (PAUSADO)" : "") << "\n";
    cout << string(100, '-') << "\n";
    cout << left << setw(35) << "PROCESOS LISTOS        " << "| "
         << setw(30) << "PROCESO EN EJECUCION" << "| "
         << setw(30) << "PROCESOS TERMINADOS" << "\n";
    cout << left << setw(35) << "ID      TME   TT" << "| "
         << setw(30) << "" << "| "
         << setw(30) << "ID      Operacion        Res" << "\n";
    cout << string(100, '-') << "\n";

    size_t executingLines = 6;
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
            else if (i == 4) line = "TR: " + to_string(max(0, (int)executingProcess->getMaxTime() - (int)executingProcess->getTimeElapsed()));
            else if (i == 5) line = "Quantum Actual: " + to_string(quantumCounter);
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
        cout << " (Ninguno)" << endl;
    } else {
        for (const auto& blocked : blockedProcesses) {
            cout << "  ID: " << setw(4) << left << blocked.first.getId()
                 << " TT en Bloqueado: " << max(0, globalCounter - blocked.second) << " / " << BLOCKED_TIME << endl;
        }
    }
    cout << string(100, '-') << "\n";
    cout << "Contador Global: " << globalCounter << "\n";
}

void displayLiveBcp(const vector<Process>& newProcesses,
                    const vector<Process>& readyProcesses,
                    const Process* executingProcess,
                    const vector<pair<Process, int>>& blockedProcesses,
                    const vector<Process>& completedProcesses) {
    cout << "\n\n--- BCP EN EJECUCION (Presione C para continuar) ---" << endl;
    cout << string(155, '-') << endl;
    cout << left
         << setw(5) << "ID"
         << setw(15) << "Estado"
         << setw(20) << "Operacion y Res."
         << setw(12) << "T. Llegada"
         << setw(18) << "T. Finalizacion"
         << setw(12) << "T. Espera"
         << setw(12) << "T. Respuesta"
         << setw(12) << "T. Retorno"
         << setw(12) << "T. Servicio"
         << setw(15) << "T. Restante"
         << setw(8) << "TME" << endl;
    cout << string(155, '-') << endl;

    auto printRow = [](const Process& p, const string& state, const string& result, const string& blockedInfo) {
        cout << left << setw(5) << p.getId();

        string stateStr = state;
        if(state == "Bloqueado") stateStr += " (" + blockedInfo + "s)";
        cout << setw(15) << stateStr;

        string op_res_str;
        float calcResult = p.getCalculator().calculate();
        if (state == "Terminado") {
            if (p.hasErrorTerminated() || isinf(calcResult)) {
                op_res_str = "Error";
            } else {
                stringstream ss_res;
                ss_res << fixed << setprecision(2) << calcResult;
                op_res_str = p.getCalculator().getOperationString() + "=" + ss_res.str();
            }
        } else if (state == "Nuevo") {
            op_res_str = "-";
        } else {
            op_res_str = p.getCalculator().getOperationString();
        }
        cout << setw(20) << op_res_str;

        cout << setw(12) << (p.getArrivalTime() > 0 || state != "Nuevo" ? to_string(p.getArrivalTime()) : "-");
        cout << setw(18) << (p.getCompletionTime() > 0 ? to_string(p.getCompletionTime()) : "-");

        unsigned int currentWait = 0;
        unsigned int calculatedReturn = 0;
        unsigned int calculatedService = p.getTimeElapsed();

        if (state == "Terminado") {
            Process tempP = p;
            tempP.calculateBcpTimes();
            currentWait = tempP.getWaitingTime();
            calculatedReturn = tempP.getReturnTime();
            calculatedService = tempP.getServiceTime();
        } else if (state != "Nuevo") {
             if(globalCounter >= p.getArrivalTime()){
                 currentWait = globalCounter - p.getArrivalTime() - p.getTimeElapsed();
                 if(currentWait < 0) currentWait = 0;
             }
        }

        cout << setw(12) << (state != "Nuevo" ? to_string(currentWait) : "-");

        if (p.hasExecutedFirstTime() && p.getFirstExecutionTime() >= p.getArrivalTime()) {
            cout << setw(12) << (p.getFirstExecutionTime() - p.getArrivalTime());
        } else {
            cout << setw(12) << "-";
        }

        cout << setw(12) << (calculatedReturn > 0 ? to_string(calculatedReturn) : "-");

        cout << setw(12) << (calculatedService > 0 || state == "Terminado" ? to_string(calculatedService) : "0");

        cout << setw(15) << (state != "Terminado" ? to_string(max(0, (int)p.getMaxTime() - (int)p.getTimeElapsed())) : "0");
        cout << setw(8) << p.getMaxTime();

        cout << endl;
    };

    vector<Process> liveNew = newProcesses;
    vector<Process> liveReady = readyProcesses;
    Process liveExecuting;
    bool hasExecuting = false;
    if(executingProcess) {
        liveExecuting = *executingProcess;
        hasExecuting = true;
    }
    vector<pair<Process,int>> liveBlocked = blockedProcesses;
    vector<Process> liveCompleted = completedProcesses;


    for (const auto& p : liveNew) printRow(p, "Nuevo", "-", "");
    for (const auto& p : liveReady) printRow(p, "Listo", "-", "");
    if (hasExecuting) printRow(liveExecuting, "Ejecucion", "-", "");
    for (const auto& pair : liveBlocked) {
        string blockedInfo = to_string(max(0, BLOCKED_TIME - (globalCounter - pair.second)));
        printRow(pair.first, "Bloqueado", "-", blockedInfo);
    }
    for (const auto& p : liveCompleted) {
         printRow(p, "Terminado", "", "");
    }


    cout << string(155, '-') << endl;
}

void simulatingBcp(vector<Process>& finishedProcesses) {
    for (auto& process : finishedProcesses) {
        process.calculateBcpTimes();
    }
    displayBcp(finishedProcesses);
}

void displayBcp(const vector<Process>& processes) {
    system("cls");
    cout << "--- BLOQUE DE CONTROL DE PROCESOS (BCP) FINAL ---" << endl;
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
        float result = p.getCalculator().calculate();
        if (p.hasErrorTerminated() || isinf(result)) {
            cout << setw(18) << (p.hasErrorTerminated() ? "X" : p.getCalculator().getOperationString())
                 << setw(12) << "Error";
        } else {
            stringstream result_ss;
            result_ss << fixed << setprecision(2) << result;
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
    cin.clear();
    cin.sync();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void generateProcessData(vector<Process>& newProcesses, int numberProcesses, vector<unsigned int>& usedIds) {
    char operations[] = {'+', '-', '*', '/', '%', '^'};
    cout << "\nGenerando " << numberProcesses << " procesos aleatoriamente..." << endl;
    for (int i = 0; i < numberProcesses; i++) {
        unsigned int maxTime = 6 + rand() % (15 - 6 + 1);
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
             for(int k=0; k<i; ++k){
                if(newProcesses[k].getId() == id) {
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
        } else if(operation == '^'){
            operandB = rand () % 3 + 1;
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
