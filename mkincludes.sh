#!/bin/bash

echo "enum opcodes {" > op_enum.h
echo "char *opcodestr[] = {" > op_str.h
echo > op_switch.h

for OP in $(cat ops.txt); do
  LOP=$(echo $OP | tr "[A-Z]" "[a-z]")
  echo "  $OP," >> op_enum.h
  echo " \"$OP\"," >> op_str.h
  echo "  case $OP:" >> op_switch.h
  echo "    op_$LOP(exec);" >> op_switch.h
  echo "    break;" >> op_switch.h

done;
echo "  MAX_OP" >> op_enum.h
echo "};" >> op_enum.h
echo "  \"MAX_OP\"" >> op_str.h
echo "};" >> op_str.h


