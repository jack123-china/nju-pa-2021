#include <isa.h>
#include "local-include/reg.h"
#include "include/isa-def.h"

extern CPU_state cpu;

const char *regs[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
  int i = 0;
  while(regs[i]){
//     printf("%s    ,%x        ,%d ", regs[i],cpu.gpr[i]._32, cpu.gpr[i]._32);

    printf("%s ", regs[i]);
    i++;
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  return 0;
}
