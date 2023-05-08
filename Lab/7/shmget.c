#include <sys/shm.h>

#define	ARRAY_SIZE	0x40000
#define	MALLOC_SIZE	0x100000
#define	SHM_SIZE	0x100000
#define	SHM_MODE	0600	/* user read/write */

char	array[ARRAY_SIZE];	/* uninitialized data = bss */

int
main(void) {
	int		shmid;
	char	*ptr, *shmptr;

    int key = 0x1337;
	if ((shmid = shmget(key, SHM_SIZE, 0666)) < 0)
		perror("shmget error");
	if ((shmptr = shmat(shmid, 0, 0)) == (void *)-1)
		perror("shmat error");

    puts(shmptr);

	exit(0);
}
