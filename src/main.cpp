#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <cctype>
#include <fstream>

#include "Gate.h"
#include "StateVector.h"
#include "Parser.h"

// Usage: qsim <circuit-file>   (asks for the file name if none is given)
int main(int argc, char* argv[]) {
    std::string filename;
    if (argc > 1) {
        filename = argv[1];
    } else {
        std::cout << "Enter circuit file name (e.g. test/version1.txt):" << std::endl;
        std::getline(std::cin, filename);
        filename = trim(filename);
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