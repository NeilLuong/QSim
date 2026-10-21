// ---------------------------------------------------------------------------
// Circuit file parser
//
// Reads a small subset of OpenQASM 3, one statement per line:
//
//   OPENQASM 3.0;              optional, ignored
//   include "stdgates.inc";    optional, ignored
//   qubit[3] q;                required, exactly once, before any gate
//   bit[2] c;                  optional, ignored (no measurement yet)
//   h q[0];                    single-qubit gates: id x y z s h
//   cx q[0], q[1];             control, target
//   ccx q[0], q[1], q[2];      control, control, target
//   barrier q[0];              ignored, but its qubits are still checked
//   // comment                 anything after // is ignored
//
// The whole file is parsed and validated before anything is simulated,
// so a bad file never produces a half-run result.
// ---------------------------------------------------------------------------
#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "StateVector.h"
#include "Gate.h"

struct Instruction {
    std::string gate;         // e.g. "h", "cx", "ccx"
    std::vector<int> qubits;  // qubit indices, in the order written
};

struct Circuit {
    int nr_of_qubits = 0;          // 0 means no qubit register declared yet
    std::string register_name;     // e.g. "q" from "qubit[3] q;"
    std::vector<Instruction> instructions;
};

// Every parse error names the line it came from, so the user can find it quickly.
std::runtime_error parse_error(int line_number, const std::string& message) {
    return std::runtime_error("line " + std::to_string(line_number) + ": " + message);
}

// Returns how many qubits a gate acts on, or 0 if the gate is not supported.
int gate_arity(const std::string& gate) {
    if (gate == "id" || gate == "x" || gate == "y" || gate == "z" || gate == "s" || gate == "h") return 1;
    if (gate == "cx") return 2;
    if (gate == "ccx") return 3;
    return 0;
}

std::string trim(const std::string& text) {
    size_t start = text.find_first_not_of(" \t\r");
    if (start == std::string::npos) return "";
    size_t end = text.find_last_not_of(" \t\r");
    return text.substr(start, end - start + 1);
}

// A name must start with a letter or '_' and contain only letters, digits and '_'.
bool is_identifier(const std::string& text) {
    if (text.empty()) return false;
    if (!std::isalpha(static_cast<unsigned char>(text[0])) && text[0] != '_') return false;
    for (char c : text) {
        if (!std::isalnum(static_cast<unsigned char>(c)) && c != '_') return false;
    }
    return true;
}

// Parses a small non-negative whole number. Rejects signs, decimals and anything too long to be sensible.
bool parse_number(const std::string& text, int& value) {
    if (text.empty() || text.size() > 4) return false;
    for (char c : text) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    value = std::stoi(text);
    return true;
}

// Splits "name[index]" into its parts, e.g. "q[2]" -> ("q", 2), "qubit[3]" -> ("qubit", 3).
// Returns false if the text is not in that form.
bool parse_indexed(const std::string& text, std::string& name, int& index) {
    size_t open = text.find('[');
    if (open == std::string::npos || text.back() != ']') return false;
    name = trim(text.substr(0, open));
    std::string digits = trim(text.substr(open + 1, text.size() - open - 2));
    return is_identifier(name) && parse_number(digits, index);
}

// Parses a comma separated operand list like "q[0], q[1], q[2]" into qubit indices.
std::vector<int> parse_qubit_operands(const std::string& text, const Circuit& circuit, int line_number) {
    if (!text.empty() && text.back() == ',') throw parse_error(line_number, "trailing ',' in qubit list");

    std::vector<int> qubits;
    std::istringstream parts(text);
    std::string part;
    while (std::getline(parts, part, ',')) {
        std::string operand = trim(part);
        std::string name;
        int index;
        if (!parse_indexed(operand, name, index)) {
            throw parse_error(line_number, "expected a qubit like " + circuit.register_name + "[0], got '" + operand + "'");
        }
        if (name != circuit.register_name) {
            throw parse_error(line_number, "unknown register '" + name + "'");
        }
        if (index >= circuit.nr_of_qubits) {
            throw parse_error(line_number, "qubit " + operand + " is out of range, register '" + circuit.register_name
                                           + "' has " + std::to_string(circuit.nr_of_qubits) + " qubits");
        }
        for (int used : qubits) {
            if (used == index) throw parse_error(line_number, "qubit " + operand + " is used more than once");
        }
        qubits.push_back(index);
    }
    return qubits;
}

Circuit parse_circuit(std::istream& input) {
    Circuit circuit;
    std::string line;
    int line_number = 0;

    while (std::getline(input, line)) {
        ++line_number;

        // Drop comments and surrounding whitespace, then skip lines with nothing left.
        std::string statement = trim(line.substr(0, line.find("//")));
        if (statement.empty()) continue;

        if (statement.back() != ';') throw parse_error(line_number, "missing ';' at end of statement");
        statement = trim(statement.substr(0, statement.size() - 1));
        if (statement.find(';') != std::string::npos) {
            throw parse_error(line_number, "only one statement per line is allowed");
        }

        // Split into the first word (keyword or gate name) and the rest (its operands).
        size_t space = statement.find_first_of(" \t");
        std::string keyword = statement.substr(0, space);
        std::string rest = (space == std::string::npos) ? "" : trim(statement.substr(space));

        // File header lines carry nothing we need.
        if (keyword == "OPENQASM" || keyword == "include") continue;

        // Register declarations: "qubit[3] q" or "bit[2] c".
        std::string type;
        int size;
        if (parse_indexed(keyword, type, size) && (type == "qubit" || type == "bit")) {
            if (!is_identifier(rest)) throw parse_error(line_number, "invalid register name '" + rest + "'");
            if (type == "bit") continue;  // classical bits are unused until measurement is supported

            if (circuit.nr_of_qubits != 0) throw parse_error(line_number, "only one qubit register is supported");
            if (size < 1 || size > MAX_QUBITS) {
                throw parse_error(line_number, "qubit count must be between 1 and " + std::to_string(MAX_QUBITS));
            }
            circuit.nr_of_qubits = size;
            circuit.register_name = rest;
            continue;
        }

        // Everything else is a gate or barrier, which needs qubits to act on.
        if (circuit.nr_of_qubits == 0) {
            throw parse_error(line_number, "'" + keyword + "' used before the qubit register is declared");
        }
        std::vector<int> qubits = parse_qubit_operands(rest, circuit, line_number);

        if (keyword == "barrier") continue;  // barriers do not change the state

        int arity = gate_arity(keyword);
        if (arity == 0) throw parse_error(line_number, "unsupported gate '" + keyword + "'");
        if (static_cast<int>(qubits.size()) != arity) {
            throw parse_error(line_number, "'" + keyword + "' expects " + std::to_string(arity) + " qubit(s), got "
                                           + std::to_string(qubits.size()));
        }
        circuit.instructions.push_back({keyword, qubits});
    }

    if (input.bad()) throw std::runtime_error("failed while reading the file");
    if (circuit.nr_of_qubits == 0) throw std::runtime_error("no qubit register declared (e.g. 'qubit[3] q;')");
    return circuit;
}

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