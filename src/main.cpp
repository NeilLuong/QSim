#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>
#include <sstream>
#include <iomanip>
#include <limits>
#include <cctype>

#include "Gate.h"

const int MAX_QUBITS = 10;

class StateVector {
private:
    std::vector<Complex> m_state;
    int m_nr_of_qubits;
public:
    StateVector(int nr_of_qubits) : m_nr_of_qubits(nr_of_qubits) {
        m_state.resize(1 << nr_of_qubits, Complex(0.0, 0.0));
        m_state[0] = Complex(1.0, 0.0);
    }

    void apply_gate(const Gate& gate) {
        std::vector<Complex> new_state (m_state.size(), Complex(0.0, 0.0));
        for (int i = 0; i < m_state.size(); ++i) {
            for (int j = 0; j < m_state.size(); ++j) {
                new_state[i] += gate[i][j] * m_state[j];
            }
        }
        m_state = new_state;
    }

    void apply_cnot(int control, int target) {
        for (int i = 0; i < m_state.size(); ++i) {
            bool control_on = ( i >> control) & 1;
            bool target_off = !(( i >> target) & 1);
            if (control_on && target_off) {
                int j = i ^ (1 << target);
                std::swap(m_state[i], m_state[j]);
            }
        }
    }

    void apply_ccnot(int control1, int control2, int target) {
        for (int i = 0; i < m_state.size(); ++i) {
            bool control_on1 = (i >> control1) & 1;
            bool control_on2 = (i >> control2) & 1;
            bool target_off = !((i >> target) & 1);
            if (control_on1 && control_on2 && target_off) {
                int j = i ^ (1 << target);
                std::swap(m_state[i], m_state[j]);
            }
        }
    }

    void print_result() {
        std::string label;
        for (int q = m_nr_of_qubits - 1; q >= 0; --q) label += "q" + std::to_string(q);

        const int comp_width = 20;
        const int prob_width = 8;
        std::string divider = "+" + std::string(label.size() + 2, '-') + "+" + std::string(comp_width + 2, '-')
                            + "+" + std::string(prob_width + 2, '-') + "+";

        std::cout << divider << std::endl;
        std::cout << std::left << "| " << std::setw(label.size()) << label << " | " << std::setw(comp_width) << "Comp"
                  << " | " << std::setw(prob_width) << "Prob" << " |" << std::endl;
        std::cout << divider << std::endl;

        for (size_t i = 0; i < m_state.size(); ++i) {
            std::string bits;
            for (int q = m_nr_of_qubits - 1; q >= 0; --q) bits += ((i >> q) & 1) ? '1' : '0';

            std::ostringstream comp;
            comp << std::fixed << std::setprecision(4) << m_state[i];

            std::cout << "| " << std::setw(label.size()) << bits << " | " << std::setw(comp_width) << comp.str()
                      << " | " << std::setw(prob_width) << std::fixed << std::setprecision(4) << std::norm(m_state[i])
                      << " |" << std::endl;
        }
        std::cout << divider << std::endl;
    }
};

Gate tensor_product(const Gate& gate1, const Gate& gate2) {
        size_t m = gate1.size();
        size_t n = gate2.size();
        Gate result (m*n, std::vector<Complex>(m*n, Complex(0.0, 0.0)));
        for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < m; ++j)
            for (size_t k = 0; k < n; ++k)
                for (size_t l = 0; l < n; ++l)
                    result[i * n + k][j * n + l] = gate1[i][j] * gate2[k][l];
        return result;
    }

Gate lift_gate(const Gate& gate, int target, const Gate& identity_gate, int nr_of_qubits) {
    Gate result = {{Complex(1.0, 0.0)}};
    for (int i = nr_of_qubits - 1; i >= 0; --i) {
        Gate next_gate = (i == target) ? gate : identity_gate;
        result = tensor_product(result, next_gate);
    }
    return result;
    // Gate result;
    // if (target == 1) {
    //     Gate temp = tensor_product(identity_gate, gate);
    //     result = tensor_product(temp, identity_gate);
    // } else if (target == 0) {
    //     Gate temp = tensor_product(identity_gate, identity_gate);
    //     result = tensor_product(temp, gate);
    // } else {
    //     Gate temp = tensor_product(identity_gate, identity_gate);
    //     result = tensor_product(gate, temp);
    // } 
    // return result;
//  return target == 0 ? tensor_product(identity_gate, gate) : tensor_product(gate, identity_gate);
}

// Gates are built as dense 2^n x 2^n matrices and qubit indices are parsed as single digits,
// so keep n small.
bool parse_qubit(char c, int nr_of_qubits, int& qubit) {
    if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    qubit = c - '0';
    return qubit < nr_of_qubits;
}

int main() {
    int nr_of_qubits;

    std::cout << "Enter the number of qubits (1-" << MAX_QUBITS << "):" << std::endl;
    while (!(std::cin >> nr_of_qubits) || nr_of_qubits < 1 || nr_of_qubits > MAX_QUBITS) {
        if (std::cin.eof()) return 1;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Please enter a number between 1 and " << MAX_QUBITS << ":" << std::endl;
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    StateVector state(nr_of_qubits);

    std::cout << "Enter gates seperated by spaces. 1 qubit gates have qubit number appended to each gate (X0, H1,...)," << std::endl;
    std::cout << "CNOT is CX<control><target> (CX01, CX12,...). Qubits are numbered 0 to " << nr_of_qubits - 1 << "." << std::endl;
    std::cout << "Example: H0 CX01" << std::endl;

    std::string line;
    std::getline(std::cin, line);
    std::istringstream tokens(line);
    std::string token;
 
    while (tokens >> token) {
        if (token.rfind("CX", 0) == 0) {
            int control, target;
            if (token.size() != 4 || !parse_qubit(token[2], nr_of_qubits, control)
                || !parse_qubit(token[3], nr_of_qubits, target) || control == target) {
                std::cout << "Invalid gate: " << token << std::endl;
                continue;
            }
            state.apply_cnot(control, target);
            continue;
        } else if (token.rfind("CCX", 0) == 0) {
            int control1, control2, target;
            if (token.size() != 6 || !parse_qubit(token[3], nr_of_qubits, control1)
                                || !parse_qubit(token[4], nr_of_qubits, control2)
                                || !parse_qubit(token[5], nr_of_qubits, target)) {
                std::cout << "Invalid gate: " << token << std::endl;
                continue;
            }
            state.apply_ccnot(control1, control2, target);
            continue;
        }
 
        int target;
        if (token.size() != 2 || !parse_qubit(token[1], nr_of_qubits, target)) {
            std::cout << "Invalid gate: " << token << std::endl;
            continue;
        }
 
        switch (token[0]) {
            case 'X': state.apply_gate(lift_gate(gate_x, target, gate_i, nr_of_qubits)); break;
            case 'Y': state.apply_gate(lift_gate(gate_y, target, gate_i, nr_of_qubits)); break;
            case 'Z': state.apply_gate(lift_gate(gate_z, target, gate_i, nr_of_qubits)); break;
            case 'I': state.apply_gate(lift_gate(gate_i, target, gate_i, nr_of_qubits)); break;
            case 'S': state.apply_gate(lift_gate(gate_s, target, gate_i, nr_of_qubits)); break;
            case 'H': state.apply_gate(lift_gate(gate_h, target, gate_i, nr_of_qubits)); break;
            default:
                std::cout << "Invalid gate: " << token << std::endl;
        }
    }
 
    state.print_result();
    return 0;
}