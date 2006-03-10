/*
 * Copyright (c) 2003-2005 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ARCH_ALPHA_ISA_TRAITS_HH__
#define __ARCH_ALPHA_ISA_TRAITS_HH__

namespace LittleEndianGuest {}
using namespace LittleEndianGuest;

#include "arch/alpha/types.hh"
#include "arch/alpha/constants.hh"
#include "base/misc.hh"
#include "config/full_system.hh"
#include "sim/host.hh"
#include "sim/faults.hh"

class ExecContext;
class FastCPU;
class FullCPU;
class Checkpoint;

class StaticInst;
class StaticInstPtr;

#if !FULL_SYSTEM
class SyscallReturn {
        public:
           template <class T>
           SyscallReturn(T v, bool s)
           {
               retval = (uint64_t)v;
               success = s;
           }

           template <class T>
           SyscallReturn(T v)
           {
               success = (v >= 0);
               retval = (uint64_t)v;
           }

           ~SyscallReturn() {}

           SyscallReturn& operator=(const SyscallReturn& s) {
               retval = s.retval;
               success = s.success;
               return *this;
           }

           bool successful() { return success; }
           uint64_t value() { return retval; }


       private:
           uint64_t retval;
           bool success;
};

#endif

namespace AlphaISA
{

    typedef IntReg IntRegFile[NumIntRegs];

    typedef union {
        uint64_t q[NumFloatRegs];	// integer qword view
        double d[NumFloatRegs];		// double-precision floating point view
    } FloatRegFile;

// redirected register map, really only used for the full system case.
extern const int reg_redir[NumIntRegs];

#if FULL_SYSTEM

#include "arch/alpha/isa_fullsys_traits.hh"

#endif
    class MiscRegFile {
      protected:
        uint64_t	fpcr;		// floating point condition codes
        uint64_t	uniq;		// process-unique register
        bool		lock_flag;	// lock flag for LL/SC
        Addr		lock_addr;	// lock address for LL/SC

      public:
        MiscReg readReg(int misc_reg);

        //These functions should be removed once the simplescalar cpu model
        //has been replaced.
        int getInstAsid();
        int getDataAsid();

        MiscReg readRegWithEffect(int misc_reg, Fault &fault, ExecContext *xc);

        Fault setReg(int misc_reg, const MiscReg &val);

        Fault setRegWithEffect(int misc_reg, const MiscReg &val,
                               ExecContext *xc);

        void copyMiscRegs(ExecContext *xc);

#if FULL_SYSTEM
      protected:
        typedef uint64_t InternalProcReg;

        InternalProcReg ipr[NumInternalProcRegs]; // Internal processor regs

      private:
        InternalProcReg readIpr(int idx, Fault &fault, ExecContext *xc);

        Fault setIpr(int idx, InternalProcReg val, ExecContext *xc);

        void copyIprs(ExecContext *xc);
#endif
        friend class RegFile;
    };

    struct RegFile {
        IntRegFile intRegFile;		// (signed) integer register file
        FloatRegFile floatRegFile;	// floating point register file
        MiscRegFile miscRegs;		// control register file
        Addr pc;			// program counter
        Addr npc;			// next-cycle program counter
        Addr nnpc;

#if FULL_SYSTEM
        int intrflag;			// interrupt flag
        inline int instAsid()
        { return miscRegs.getInstAsid(); }
        inline int dataAsid()
        { return miscRegs.getDataAsid(); }
#endif // FULL_SYSTEM

        void serialize(std::ostream &os);
        void unserialize(Checkpoint *cp, const std::string &section);
    };

    static inline ExtMachInst makeExtMI(MachInst inst, const uint64_t &pc);

    StaticInstPtr decodeInst(ExtMachInst);

    static inline bool isCallerSaveIntegerRegister(unsigned int reg) {
        panic("register classification not implemented");
        return (reg >= 1 && reg <= 8 || reg >= 22 && reg <= 25 || reg == 27);
    }

    static inline bool isCalleeSaveIntegerRegister(unsigned int reg) {
        panic("register classification not implemented");
        return (reg >= 9 && reg <= 15);
    }

    static inline bool isCallerSaveFloatRegister(unsigned int reg) {
        panic("register classification not implemented");
        return false;
    }

    static inline bool isCalleeSaveFloatRegister(unsigned int reg) {
        panic("register classification not implemented");
        return false;
    }

    static inline Addr alignAddress(const Addr &addr,
                                         unsigned int nbytes) {
        return (addr & ~(nbytes - 1));
    }

    // Instruction address compression hooks
    static inline Addr realPCToFetchPC(const Addr &addr) {
        return addr;
    }

    static inline Addr fetchPCToRealPC(const Addr &addr) {
        return addr;
    }

    // the size of "fetched" instructions (not necessarily the size
    // of real instructions for PISA)
    static inline size_t fetchInstSize() {
        return sizeof(MachInst);
    }

    static inline MachInst makeRegisterCopy(int dest, int src) {
        panic("makeRegisterCopy not implemented");
        return 0;
    }

    // Machine operations

    void saveMachineReg(AnyReg &savereg, const RegFile &reg_file,
                               int regnum);

    void restoreMachineReg(RegFile &regs, const AnyReg &reg,
                                  int regnum);

    /**
     * Function to insure ISA semantics about 0 registers.
     * @param xc The execution context.
     */
    template <class XC>
    void zeroRegisters(XC *xc);

#if !FULL_SYSTEM
    static inline void setSyscallReturn(SyscallReturn return_value, RegFile *regs)
    {
        // check for error condition.  Alpha syscall convention is to
        // indicate success/failure in reg a3 (r19) and put the
        // return value itself in the standard return value reg (v0).
        if (return_value.successful()) {
            // no error
            regs->intRegFile[SyscallSuccessReg] = 0;
            regs->intRegFile[ReturnValueReg] = return_value.value();
        } else {
            // got an error, return details
            regs->intRegFile[SyscallSuccessReg] = (IntReg) -1;
            regs->intRegFile[ReturnValueReg] = -return_value.value();
        }
    }
#endif
};

static inline AlphaISA::ExtMachInst
AlphaISA::makeExtMI(AlphaISA::MachInst inst, const uint64_t &pc) {
#if FULL_SYSTEM
    AlphaISA::ExtMachInst ext_inst = inst;
    if (pc && 0x1)
        return ext_inst|=(static_cast<AlphaISA::ExtMachInst>(pc & 0x1) << 32);
    else
        return ext_inst;
#else
    return AlphaISA::ExtMachInst(inst);
#endif
}

#endif // __ARCH_ALPHA_ISA_TRAITS_HH__
