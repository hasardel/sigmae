
/*
 * Copyright (C) 2015 Florent Pouthier
 * Copyright (C) 2015 Emmanuel Pouthier
 *
 * This file is part of SIGMAE.
 *
 * Aye-Aye is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Aye-Aye is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sigmae/base.h>

pthread_mutex_t mut;
SgVector *tasks;

void *cons(void *nth)
{
  char *val;
  while (1) {
    if (pthread_mutex_trylock(&mut) == 0) {
//      printf("%d\n", tasks->size);
      while (!sg_vector_isempty(tasks)) {
        val = sg_vector_pop(tasks);
        printf("pop <%s>\n", val);
      }
      pthread_mutex_unlock(&mut);
    }
    usleep(500000 + random() * 1.f / RAND_MAX * 500000);
  }
  pthread_exit(NULL);
}

int main()
{
  pthread_t th;
  int i = 0;

  tasks = sg_make_vector();
  if (pthread_mutex_init(&mut, NULL)) return -1;
  if (pthread_create(&th, NULL, cons, NULL)) return -1;

  while (i++ < 100000) {
    printf("<<\n");
    pthread_mutex_lock(&mut);
    printf(">>\n");
    sg_vector_push(tasks, (void*)"Jean");
    sg_vector_push(tasks, (void*)"Anne");
    sg_vector_push(tasks, (void*)"Henry");
    pthread_mutex_unlock(&mut);
    usleep(100 + random() * 1.f / RAND_MAX * 100000);
  }

  pthread_join(th, NULL);

  return 0;
}


