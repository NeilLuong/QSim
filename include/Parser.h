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

Circuit parse_circuit(std::istream& input);