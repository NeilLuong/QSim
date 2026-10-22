#include "StateVector.h"

StateVector::StateVector(int nr_of_qubits) : m_nr_of_qubits(nr_of_qubits)
{
    m_state.resize(1 << nr_of_qubits, Complex(0.0, 0.0));
    m_state[0] = Complex(1.0, 0.0);
}

void StateVector::apply_gate(const Gate &gate)
{
    std::vector<Complex> new_state(m_state.size(), Complex(0.0, 0.0));
    for (int i = 0; i < m_state.size(); ++i)
    {
        for (int j = 0; j < m_state.size(); ++j)
        {
            new_state[i] += gate[i][j] * m_state[j];
        }
    }
    m_state = new_state;
}

void StateVector::apply_cnot(int control, int target)
{
    for (int i = 0; i < m_state.size(); ++i)
    {
        bool control_on = (i >> control) & 1;
        bool target_off = !((i >> target) & 1);
        if (control_on && target_off)
        {
            int j = i ^ (1 << target);
            std::swap(m_state[i], m_state[j]);
        }
    }
}

void StateVector::apply_ccnot(int control1, int control2, int target)
{
    for (int i = 0; i < m_state.size(); ++i)
    {
        bool control_on1 = (i >> control1) & 1;
        bool control_on2 = (i >> control2) & 1;
        bool target_off = !((i >> target) & 1);
        if (control_on1 && control_on2 && target_off)
        {
            int j = i ^ (1 << target);
            std::swap(m_state[i], m_state[j]);
        }
    }
}

void StateVector::print_result()
{
    std::string label;
    for (int q = m_nr_of_qubits - 1; q >= 0; --q)
        label += "q" + std::to_string(q);

    const int comp_width = 20;
    const int prob_width = 8;
    std::string divider = "+" + std::string(label.size() + 2, '-') + "+" + std::string(comp_width + 2, '-') + "+" + std::string(prob_width + 2, '-') + "+";

    std::cout << divider << std::endl;
    std::cout << std::left << "| " << std::setw(label.size()) << label << " | " << std::setw(comp_width) << "Comp"
              << " | " << std::setw(prob_width) << "Prob" << " |" << std::endl;
    std::cout << divider << std::endl;

    for (size_t i = 0; i < m_state.size(); ++i)
    {
        std::string bits;
        for (int q = m_nr_of_qubits - 1; q >= 0; --q)
            bits += ((i >> q) & 1) ? '1' : '0';

        std::ostringstream comp;
        comp << std::fixed << std::setprecision(4) << m_state[i];

        std::cout << "| " << std::setw(label.size()) << bits << " | " << std::setw(comp_width) << comp.str()
                  << " | " << std::setw(prob_width) << std::fixed << std::setprecision(4) << std::norm(m_state[i])
                  << " |" << std::endl;
    }
    std::cout << divider << std::endl;
}

Gate lift_gate(const Gate &gate, int target, const Gate &identity_gate, int nr_of_qubits)
{
    Gate result = {{Complex(1.0, 0.0)}};
    for (int i = nr_of_qubits - 1; i >= 0; --i)
    {
        Gate next_gate = (i == target) ? gate : identity_gate;
        result = tensor_product(result, next_gate);
    }
    return result;
}

Gate tensor_product(const Gate &gate1, const Gate &gate2)
{
    size_t m = gate1.size();
    size_t n = gate2.size();
    Gate result(m * n, std::vector<Complex>(m * n, Complex(0.0, 0.0)));
    for (size_t i = 0; i < m; ++i)
        for (size_t j = 0; j < m; ++j)
            for (size_t k = 0; k < n; ++k)
                for (size_t l = 0; l < n; ++l)
                    result[i * n + k][j * n + l] = gate1[i][j] * gate2[k][l];
    return result;
}