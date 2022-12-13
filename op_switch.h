
  case INVALID:
    op_invalid(exec);
    break;
  case ADD:
    op_add(exec);
    break;
  case CALL:
    op_call(exec);
    break;
  case CONDBEGIN:
    op_condbegin(exec);
    break;
  case CONDELSE:
    op_condelse(exec);
    break;
  case CONDEND:
    op_condend(exec);
    break;
  case CONSTANT:
    op_constant(exec);
    break;
  case DISCARD:
    op_discard(exec);
    break;
  case DIVIDE:
    op_divide(exec);
    break;
  case EQUAL:
    op_equal(exec);
    break;
  case GETGLOBAL:
    op_getglobal(exec);
    break;
  case GETINT:
    op_getint(exec);
    break;
  case GETVAR:
    op_getvar(exec);
    break;
  case GREATER:
    op_greater(exec);
    break;
  case GREATEREQUAL:
    op_greaterequal(exec);
    break;
  case INDEX1:
    op_index1(exec);
    break;
  case INDEXAS:
    op_indexas(exec);
    break;
  case JUMP:
    op_jump(exec);
    break;
  case JUMPREL:
    op_jumprel(exec);
    break;
  case LESS:
    op_less(exec);
    break;
  case LESSEQUAL:
    op_lessequal(exec);
    break;
  case LOOPBEGIN:
    op_loopbegin(exec);
    break;
  case LOOPEND:
    op_loopend(exec);
    break;
  case LOOPEXIT:
    op_loopexit(exec);
    break;
  case LOOPRESTART:
    op_looprestart(exec);
    break;
  case MODULO:
    op_modulo(exec);
    break;
  case MUL:
    op_mul(exec);
    break;
  case MKARRAY:
    op_mkarray(exec);
    break;
  case NOOP:
    op_noop(exec);
    break;
  case NOT:
    op_not(exec);
    break;
  case NOTEQUAL:
    op_notequal(exec);
    break;
  case PRINT:
    op_print(exec);
    break;
  case RET:
    op_ret(exec);
    break;
  case SETGLOBAL:
    op_setglobal(exec);
    break;
  case SETVAR:
    op_setvar(exec);
    break;
  case SUB:
    op_sub(exec);
    break;
