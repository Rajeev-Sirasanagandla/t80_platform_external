#ifndef _ASM_UPROBES_H
#define _ASM_UPROBES_H
/*
 *  Userspace Probes (UProbes)
 *  uprobes.h
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * Copyright (C) IBM Corporation, 2008
 */
#include <linux/types.h>
#include <linux/ptrace.h>
#include <linux/sched.h>
#include <asm/thread_info.h>

/* Normally defined in Kconfig */
#define CONFIG_URETPROBES 1
#define CONFIG_UPROBES_SSOL 1


typedef u8 uprobe_opcode_t;
#define BREAKPOINT_INSTRUCTION	0xcc
#define BP_INSN_SIZE 1
#define MAX_UINSN_BYTES 16

#ifdef STAPCONF_X86_UNIREGS
#define REGS_IP regs->ip
#define REGS_SP regs->sp
#define REGS_AX regs->ax
#define REGS_CX regs->cx
#else
#ifdef CONFIG_X86_32
#define REGS_IP regs->eip
#define REGS_SP regs->esp
#else
#define REGS_IP regs->rip
#define REGS_SP regs->rsp
#define REGS_AX regs->rax
#define REGS_CX regs->rcx
#endif
#endif /*STAPCONF_X86_UNIREGS */

// SLOT_IP should be 16 for 64-bit apps (include/asm-x86_64/elf.h)
// but 12 for 32-bit apps (arch/x86_64/ia32/ia32_binfmt.c)
#ifdef CONFIG_X86_32
#define SLOT_IP(tsk) 12
#else
#define SLOT_IP(tsk) (test_tsk_thread_flag(tsk, TIF_IA32) ? 12 : 16)
#endif

#define BREAKPOINT_SIGNAL SIGTRAP
#define SSTEP_SIGNAL SIGTRAP

/* Architecture specific switch for where the IP points after a bp hit */
#define ARCH_BP_INST_PTR(inst_ptr)	(inst_ptr - BP_INSN_SIZE)

#define UPFIX_RIP_RAX 0x1	/* (%rip) insn rewritten to use (%rax) */
#define UPFIX_RIP_RCX 0x2	/* (%rip) insn rewritten to use (%rcx) */

#ifdef CONFIG_X86_64
struct uprobe_probept_arch_info {
	unsigned long flags;
	unsigned long rip_target_address;
};

struct uprobe_task_arch_info {
	unsigned long saved_scratch_register;
};
#else
struct uprobe_probept_arch_info {};
struct uprobe_task_arch_info {};
#endif

struct uprobe_probept;
struct uprobe_task;

static int arch_validate_probed_insn(struct uprobe_probept *ppt,
						struct task_struct *tsk);

/* On x86_64, the int3 traps leaves rip pointing past the int3 instruction. */
static inline unsigned long arch_get_probept(struct pt_regs *regs)
{
	return (unsigned long) (REGS_IP - BP_INSN_SIZE);
}

static inline void arch_reset_ip_for_sstep(struct pt_regs *regs)
{
	REGS_IP -= BP_INSN_SIZE;
}

static inline void arch_restore_uret_addr(unsigned long ret_addr,
	struct pt_regs *regs)
{
	REGS_IP = ret_addr;
}

static unsigned long arch_get_cur_sp(struct pt_regs *regs)
{
	return (unsigned long)REGS_SP;
}

static unsigned long arch_hijack_uret_addr(unsigned long trampoline_addr,
		struct pt_regs *regs, struct uprobe_task *utask);

static unsigned long arch_predict_sp_at_ret(struct pt_regs *regs,
		struct task_struct *tsk);

#endif				/* _ASM_UPROBES_H */