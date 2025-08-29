import subprocess
import sys
from PyQt5.QtWidgets import (
    QApplication, QWidget, QVBoxLayout, QHBoxLayout,
    QLabel, QLineEdit, QPushButton, QListWidget, QSpinBox
)
from PyQt5.QtCore import QTimer

PROCESS_NUMBER = 4  # Número de procesos por lote

class BatchGUI(QWidget):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Simulación Procesamiento por Lotes")
        self.processes = []
        self.current_index = 0
        self.current_time = 0
        self.timer = QTimer()
        self.timer.timeout.connect(self.update_execution)
        self.initUI()

    def initUI(self):
        self.layout = QVBoxLayout()
        self.label_num = QLabel("Número total de procesos:")
        self.num_processes_input = QSpinBox()
        self.num_processes_input.setMinimum(1)
        self.start_btn = QPushButton("Aceptar")
        self.start_btn.clicked.connect(self.start_process_input)

        hbox = QHBoxLayout()
        hbox.addWidget(self.label_num)
        hbox.addWidget(self.num_processes_input)
        hbox.addWidget(self.start_btn)
        self.layout.addLayout(hbox)
        self.setLayout(self.layout)

    def start_process_input(self):
        self.num_processes = self.num_processes_input.value()
        self.num_lotes = (self.num_processes + PROCESS_NUMBER - 1) // PROCESS_NUMBER
        self.current_lote = 0
        self.process_inputs = []
        self.layout.removeWidget(self.start_btn)
        self.start_btn.hide()
        self.label_num.setText(f"Ingrese los procesos del lote 1/{self.num_lotes}:")
        self.show_next_lote()

    def show_next_lote(self):
        self.process_inputs = []
        self.input_widgets = []
        start_index = self.current_lote * PROCESS_NUMBER
        end_index = min(start_index + PROCESS_NUMBER, self.num_processes)

        for i in range(start_index, end_index):
            label = QLabel(f"Proceso {i+1}")
            self.layout.addWidget(label)
            self.input_widgets.append(label)

            fields = ["Nombre", "Operacion", "Operando A", "Operando B", "Tiempo max", "ID"]
            line_edits = []
            for field in fields:
                le = QLineEdit()
                le.setPlaceholderText(field)
                self.layout.addWidget(le)
                self.input_widgets.append(le)
                line_edits.append(le)
            self.process_inputs.append(line_edits)

        if hasattr(self, "next_lote_btn"):
            self.layout.removeWidget(self.next_lote_btn)
            self.next_lote_btn.hide()

        self.next_lote_btn = QPushButton("Enviar lote")
        self.next_lote_btn.clicked.connect(self.save_lote)
        self.layout.addWidget(self.next_lote_btn)

    def save_lote(self):
        for line_edits in self.process_inputs:
            try:
                name = line_edits[0].text()
                op = line_edits[1].text()
                a = float(line_edits[2].text())
                b = float(line_edits[3].text())
                t = int(line_edits[4].text())
                id_ = int(line_edits[5].text())
                self.processes.append((name, op, a, b, t, id_))
            except:
                continue  # Ignorar si falta algún campo

        # Limpiar inputs
        for w in self.input_widgets:
            self.layout.removeWidget(w)
            w.hide()
        self.layout.removeWidget(self.next_lote_btn)
        self.next_lote_btn.hide()

        self.current_lote += 1
        if self.current_lote < self.num_lotes:
            self.label_num.setText(f"Ingrese los procesos del lote {self.current_lote+1}/{self.num_lotes}:")
            self.show_next_lote()
        else:
            # Todos los lotes ingresados, mostrar simulación
            self.create_simulation_view()
            self.timer.start(1000)

    def create_simulation_view(self):
        self.pending_list = QListWidget()
        self.running_list = QListWidget()
        self.finished_list = QListWidget()

        for p in self.processes:
            self.pending_list.addItem(f"{p[0]} {p[1]} {p[2]} {p[3]} {p[4]} {p[5]}")

        hbox = QHBoxLayout()
        hbox.addWidget(self.pending_list)
        hbox.addWidget(self.running_list)
        hbox.addWidget(self.finished_list)
        self.layout.addLayout(hbox)

    def update_execution(self):
        if self.current_index < len(self.processes):
            if self.running_list.count() == 0:
                item = self.pending_list.takeItem(0)
                if item:
                    self.running_list.addItem(item.text())
                    self.current_time = self.processes[self.current_index][4]
            else:
                self.current_time -= 1
                if self.current_time <= 0:
                    item = self.running_list.takeItem(0)
                    if item:
                        output = self.run_cpp_process(self.processes[self.current_index])
                        self.finished_list.addItem(output)
                        self.current_index += 1

    def run_cpp_process(self, process):
        proc = subprocess.Popen(["main.exe"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
        proc.stdin.write("1\n")
        name, op, a, b, t, id_ = process
        proc.stdin.write(f"{name}\n{op}\n{a}\n{b}\n{t}\n{id_}\n")
        proc.stdin.flush()
        output = proc.stdout.readline().strip()
        proc.terminate()
        return output

if __name__ == "__main__":
    app = QApplication(sys.argv)
    gui = BatchGUI()
    gui.show()
    sys.exit(app.exec_())
