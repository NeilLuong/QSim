#pragma once

#include "Gate.h"
#include <iostream>
#include <iomanip>

const int MAX_QUBITS = 10;

class StateVector {
private:
    std::vector<Complex> m_state;
    int m_nr_of_qubits;
public:
    StateVector(int nr_of_qubits);
    void apply_gate(const Gate& gate);
    void apply_cnot(int control, int target);
    void apply_ccnot(int control1, int control2, int target);
    void print_result();
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
}