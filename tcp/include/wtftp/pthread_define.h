#ifndef _PTHREAD_DEFINE_H_
#define _PTHREAD_DEFINE_H_


#define      RUNNING            1
#define      NOT_RUNNING        0
#define      RUNNING_QUIT		-1


#define      FLAG_VALID			1
#define      FLAG_NOT_VALID		0


#define      isFlagValid(arg)		(arg->flag == FLAG_VALID)

#endif//_PTHREAD_DEFINE_H_
