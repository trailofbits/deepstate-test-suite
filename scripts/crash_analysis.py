#!/usr/bin/env python3.6
"""
crash_analysis.py

    DESCRIPTION:
        Auxiliary script that provides native ints couting support using Manticore in order
        analyze DeepState test runtimes.

    USAGE:

        python3 crash_analysis.py BINARY

"""
import sys
from manticore.native import Manticore

m = Manticore(sys.argv[1], sys.argv[2:])
m.context["trace"] = []
m.context["insts"] = []

def main():

    @m.hook(None)
    def trace(state):
        pc = state.cpu.PC
        inst = state.cpu.instruction

        with m.locked_context() as c:
            c["trace"] += [hex(pc)]
            c["insts"] += ["{} {}".format(inst.mnemonic, inst.op_str)]

            print("{}\t{}".format(c["trace"][-1], c["insts"][-1]))


    def log_func(state, name):
        print("{}: {}()", hex(state.cpu.PC), name)

    for sym in syms:
        m.add_hook(m.resolve(sym), lambda state: log_func(state, sym))

    m.verbosity(2)
    m.run()

    print(len(m.context["trace"]), "instructions recorded")

    print("Last instruction executed:")
    print("%s: %s" % (m.context["trace"][-1], m.context["insts"][-1]))

if __name__ == "__main__":
    main()

