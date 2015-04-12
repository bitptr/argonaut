#ifndef SIGNALS_H

volatile sig_atomic_t exit_flag;

void		chld_handler(int);

#endif /* SIGNALS_H */
