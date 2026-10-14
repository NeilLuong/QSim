#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>

#include "Gate.h"

using Complex = std::complex<double>;

using Gate = std::vector<std::vector<Complex>>;

class StateVector {
private:
    std::vector<Complex> m_state;
public:
    StateVector(int nr_of_qubits) {
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

    void print_result() {
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "| q1q0     | Comp          | Prob        |" << std::endl;
        std::cout << "| 00       | " << m_state[0] << " | " << std::norm(m_state[0]) << " |" << std::endl;
        std::cout << "| 01       | " << m_state[1] << " | " << std::norm(m_state[1]) << " |" << std::endl;
        std::cout << "| 10       | " << m_state[2] << " | " << std::norm(m_state[2]) << " |" << std::endl;
        std::cout << "| 11       | " << m_state[3] << " | " << std::norm(m_state[3]) << " |" << std::endl;
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

Gate on_qubit(const Gate& gate, int target, const Gate& identity_gate) {
    return target == 0 ? tensor_product(identity_gate, gate) : tensor_product(gate, identity_gate);
}

int main() {
    int nr_of_qubits;

    std::cout << "Enter the number of qubits (1, 2 or 3):" << std::endl;
    std::cin >> nr_of_qubits;

    StateVector state(nr_of_qubits);

    std::cout << "Enter gates seperated by spaces. 1 qubit gates have qubit number appended to each gate (X0, H1,...)," << std::endl;
    std::cout << "CNOT is CX<control><target> (CX01 or CX10)." << std::endl;
    std::cout << "Example: H0 CX01" << std::endl;

    std::string line;
    std::getline(std::cin, line);
    std::istringstream tokens(line);
    std::string token;
 
    while (tokens >> token) {
        if (token == "CX01") { state.apply_gate(gate_cx01); continue; }
        if (token == "CX10") { state.apply_gate(gate_cx10); continue; }
 
        if (token.size() != 2 || (token[1] != '0' && token[1] != '1')) {
            std::cout << "Invalid gate: " << token << std::endl;
            continue;
        }
        int target = token[1] - '0';
 
        switch (token[0]) {
            case 'X': state.apply_gate(on_qubit(gate_x, target, gate_i)); break;
            case 'Y': state.apply_gate(on_qubit(gate_y, target, gate_i)); break;
            case 'Z': state.apply_gate(on_qubit(gate_z, target, gate_i)); break;
            case 'I': state.apply_gate(on_qubit(gate_i, target, gate_i)); break;
            case 'S': state.apply_gate(on_qubit(gate_s, target, gate_i)); break;
            case 'H': state.apply_gate(on_qubit(gate_h, target, gate_i)); break;
            default:
                std::cout << "Invalid gate: " << token << std::endl;
        }
    }
 
    state.print_result();
    return 0;
}