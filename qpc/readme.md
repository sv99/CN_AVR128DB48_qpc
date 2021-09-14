qpc
===

It is library port for AVR128.

qs modifications
----------------

Need compile with qs without QS_SPY.

* Remove #error from qs.h and qs_dummy.h
* Add wrap with #ifdef Q_SPY for qs.c, qs_fp.c and qs_rx.c
* Remove qs_64bit.c - not applicable for AVR128 (QS_FUN_PTR_SIZE == 8U)
* qf/qep_hsm.c, qf/qep_msm.c - move #include "qassert.h" after Q_SPY - warning redefine Q_NORETURN
    