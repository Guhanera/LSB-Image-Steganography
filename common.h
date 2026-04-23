#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

void animate(char *s);
void print_help();
OperationType check_operation_type(char *argv[]);

#endif