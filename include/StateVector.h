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

Gate tensor_product(const Gate& gate1, const Gate& gate2);


Gate lift_gate(const Gate& gate, int target, const Gate& identity_gate, int nr_of_qubits);
