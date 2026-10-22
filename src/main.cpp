#include <iostream>
#include <string>
#include <stdexcept>
#include <fstream>

#include "StateVector.h"
#include "Parser.h"

const Gate& single_qubit_gate(const std::string& gate) {
    if (gate == "x") return gate_x;
    if (gate == "y") return gate_y;
    if (gate == "z") return gate_z;
    if (gate == "s") return gate_s;
    if (gate == "h") return gate_h;
    return gate_i;  // "id"
}

void apply_instruction(StateVector& state, const Instruction& instruction, int nr_of_qubits) {
    const std::vector<int>& q = instruction.qubits;
    if (instruction.gate == "cx") {
        state.apply_cnot(q[0], q[1]);
    } else if (instruction.gate == "ccx") {
        state.apply_ccnot(q[0], q[1], q[2]);
    } else {
        state.apply_gate(lift_gate(single_qubit_gate(instruction.gate), q[0], gate_i, nr_of_qubits));
    }
}

// Usage: qsim <circuit-file>   (asks for the file name if none is given)
int main(int argc, char* argv[]) {
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cout << "Enter circuit file name (e.g. test/version1.txt):" << std::endl;
        std::cin >> filename;
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "error: could not open file '" << filename << "'" << std::endl;
        return 1;
    }

    Circuit circuit;
    try {
        circuit = parse_circuit(file);
    } catch (const std::runtime_error& error) {
        std::cerr << filename << ": error: " << error.what() << std::endl;
        return 1;
    }

    StateVector state(circuit.nr_of_qubits);
    for (const Instruction& instruction : circuit.instructions) {
        apply_instruction(state, instruction, circuit.nr_of_qubits);
    }
    state.print_result();
    return 0;
}