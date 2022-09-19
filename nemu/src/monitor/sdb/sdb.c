#include <isa.h>
#include <cpu/cpu.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdio.h>
#include "sdb.h"
#include "memory/vaddr.h"

static int is_batch_mode = false;

void init_regex();
void init_wp_pool();
void isa_reg_display(void);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}


static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);
static int cmd_stepi(char *args);
static int cmd_printRegInfo(char *args);
static int cmd_printMenory(char *args);

static struct {
  const char *name;
  const char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  {"si","一条指令一条指令调试 而s是一行一行代码",cmd_stepi},
  {"info","print register info",cmd_printRegInfo},
  {"x", "print memory data",cmd_printMenory},
  /* TODO: Add more commands */

};

#define NR_CMD ARRLEN(cmd_table)

static int cmd_help(char *args) {
  /* extract the first argument */

	printf("sizeof(int* %lu) \n", sizeof(int*));
  printf("sizeof(int %lu) \n", sizeof(int));
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

static int cmd_stepi(char *args){
    char *arg = strtok(NULL, " ");
    if (arg == NULL) {
   	cpu_exec(1);
	return 0;
    }
    

    
    int i = atoi(arg);
    if (0 == i) {
    	printf("No symbol %s in current context\n", arg);
	return 0;
    }
    arg = strtok(NULL, " ");
    if (arg == NULL) {
    	cpu_exec(i);
	return 0;
    }
    printf("too many arguments'%s'\n", arg); 
    return 0;   
}

static int cmd_printRegInfo(char *args){
   char *arg = strtok(NULL, " ");
   if (*arg == 'r') {
     printf(" arguments'%s'\n",arg);
     isa_reg_display();
     return 0;
   }
   printf("unknow arguments'%s'\n",arg);
	
	
   return 0;
}

static int cmd_printMenory(char *args){
  char *arg = strtok(NULL, " ");
  if (arg == NULL) {
     return 0;
  }
  
  int count = atoi(arg);
  if (0 == count) {
      printf("No symbol %s in current context\n",arg);
      return 0;
  }
  
  
  char *men = strtok(NULL, " ");
  long value = strtol(men,NULL,16);

  word_t address =(word_t) value;
  //int* test = (int*) value;
  //printf("test addr = %p\n",test + 1);  
  //int line = count / 4;
  //int ddresidue = count % 4;
  for (int i = 0; i < count; i++) {
	  word_t addr = address  + (i *4 );
     printf("adreess = 0x%x : %x \n", addr , vaddr_read(addr ,4) );
  } 

  return 0;
}

void sdb_set_batch_mode() {
  is_batch_mode = true;
}

void sdb_mainloop() {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef CONFIG_DEVICE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}

void init_sdb() {
  /* Compile the regular expressions. */
  init_regex();

  /* Initialize the watchpoint pool. */
  init_wp_pool();
}
