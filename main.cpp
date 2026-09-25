#include <iostream>
#include <complex>
#include <vector>
#include <cmath>
#include <string>

using Complex = std::complex<double>;

using Gate = std::vector<std::vector<Complex>>;

class StateVector {
private:
    std::vector<Complex> m_qubit { Complex(1.0, 0.0), Complex(0.0, 0.0) };
public:
    StateVector() {}

    void apply_gate(Gate gate) {
        Complex new_qubit_0 = gate[0][0] * m_qubit[0] + gate[0][1] * m_qubit[1];
        Complex new_qubit_1 = gate[1][0] * m_qubit[0] + gate[1][1] * m_qubit[1];
        m_qubit[0] = new_qubit_0;
        m_qubit[1] = new_qubit_1;
    }

    void print_result() {
        std::cout << "------------------------------------------" << std::endl;
        std::cout << "| Val      | Comp          | Prob        |" << std::endl;
        std::cout << "| 0        | " << m_qubit[0] << " | " << std::norm(m_qubit[0]) << " |" << std::endl;
        std::cout << "| 1        | " << m_qubit[1] << " | " << std::norm(m_qubit[1]) << " |" << std::endl;
    }
};



int main() {
    StateVector qubit;

    Gate gate_x {
        { Complex(0.0, 0.0), Complex(1.0, 0.0)},
        { Complex(1.0, 0.0), Complex(0.0, 0.0)}
    };

    Gate gate_y {
        { Complex(0.0, 0.0), Complex(0.0, -1.0)},
        { Complex(0.0, 1.0), Complex(0.0, 0.0)}
    };

    Gate gate_z {
        { Complex(1.0, 0.0), Complex(0.0, 0.0)},
        { Complex(0.0, 0.0), Complex(-1.0, 0.0)}
    };

    Gate gate_i {
        { Complex(1.0, 0.0), Complex(0.0, 0.0)},
        { Complex(0.0, 0.0), Complex(1.0, 0.0)}
    };

    Gate gate_s {
        { Complex(1.0, 0.0), Complex(0.0, 0.0)},
        { Complex(0.0, 0.0), Complex(0.0, 1.0)}
    };

    double inv_sqrt2 = 1.0 / std::sqrt(2.0);
    Gate gate_h {
        { Complex(inv_sqrt2, 0.0), Complex(inv_sqrt2, 0.0)},
        { Complex(inv_sqrt2, 0.0), Complex(-inv_sqrt2, 0.0)}
    };

    std::cout << "Enter a sequence of gate(s) to apply to the qubit (X, Y, Z, I, S, H): " << std::endl;
    std::cout << "Example: XYZ, XIH, ..." << std::endl;

    std::string gate_sequence;
    std::cin >> gate_sequence;

    for (auto c : gate_sequence) {
        switch (c) {
            case 'X':
                qubit.apply_gate(gate_x);
                break;
            case 'Y':
                qubit.apply_gate(gate_y);
                break;
            case 'Z':
                qubit.apply_gate(gate_z);
                break;
            case 'I':
                qubit.apply_gate(gate_i);
                break;
            case 'S':
                qubit.apply_gate(gate_s);
                break;
            case 'H':
                qubit.apply_gate(gate_h);
                break;
            default:
                std::cout << "Invalid gate: " << c << std::endl;
        }
    }

    qubit.print_result();
    return 0;
}