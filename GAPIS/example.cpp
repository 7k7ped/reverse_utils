#include "inject_sys.h"
int hsub_abc(){
 printf("kernel based hooking..");
}


int main(){
 inject_sys("com.pubg.imobile" , "libanogs.so" , "0xabc", (void *) hsub_abc);

}