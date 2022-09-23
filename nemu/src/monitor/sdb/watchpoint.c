#include "sdb.h"

#define NR_WP 32

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
  
  /* TODO: Add more members if necessary */
  
  uint32_t lastResult ;
  char *expr; 
} WP;

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
word_t expr(char *e, bool *success);

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
    wp_pool[i].lastResult = 0;
    memset(wp_pool[i].expr, 0 , 32);
  }
  
  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
//free_链表中返回一个空闲的监视点结构,
WP* new_wp() {
   if (free_ == NULL){
      assert(0);
   }
   WP* t = free_;
   free_ = free_->next;
   return t;
}

void free_wp(WP *wp){
   wp->next = free_;
   free_ = wp;
   return ;
}

//记录原始表达式的数据
//void recordOrigData(uint32_t last,char *expr, WP* node){
  // node->lastResult = last;
  // node->expr = expr;    
//}


void add_head_wp( uint32_t last,char *expr){
   WP* wp = new_wp();
   
   wp->next = head;
   head = wp->next;
   wp->lastResult = last;
   wp->expr = expr;
   return ;
}

WP* del_head(int no) {
   if (head->NO == no) {
      head = head->next;
      return head;
   }

   WP *last = head;
   WP *ret = NULL ;
   while(last){
      if(no == last->next->NO){
        ret = last->next;
      	last->next = last->next->next; 
	break;
      }
      last = last->next;
   }
   return ret;
}

int checkList() {
   WP* node = head;
   while(node){
      uint32_t now = expr(node->expr, NULL);
      if (now != node->lastResult) {
	 printf("node NO = %d , last data = %d\n",node->NO,node->lastResult );
	 node->lastResult = now;
         return node->NO ;
      }
   }
   return -1;
}


void printWatchPoint() {
  
  WP* node = head;
  while(node){
      printf("old value = %d\n",node->lastResult);
      printf("watch NO = %d \n",node->NO);
  }
}

