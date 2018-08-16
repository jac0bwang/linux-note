#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/inotify.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <assert.h>
#include <dirent.h>
#include <inttypes.h>
#include <memory.h>
#include <stdint.h>
#include <stdlib.h>
/*
 * $ start-stop-daemon --start --oknodo --background  --exec /usr/sbin/xxx
 * */
#define CONFIG_FILE "/etc/file.conf"
/* Read all available inotify events from the file descriptor 'fd'.
   wd is the table of watch descriptors for the directories in argv.
   argc is the length of wd and argv.
   argv is the list of watched directories.
   Entry 0 of wd and argv is unused. */

static void handle_events(int fd, int *wd, int argc, char* argv[])
{
	/* Some systems cannot read integer variables if they are not
	   properly aligned. On other systems, incorrect alignment may
	   decrease performance. Hence, the buffer used for reading from
	   the inotify file descriptor should have the same alignment as
	   struct inotify_event. */

	char buf[4096]
		__attribute__ ((aligned(__alignof__(struct inotify_event))));
	const struct inotify_event *event;
	int i;
	ssize_t len;
	char *ptr;

	/* Read some events. */
	len = read(fd, buf, sizeof(buf));
	if (len == -1 && errno != EAGAIN) {
		perror("read");
		exit(EXIT_FAILURE);
	}

	/* If the nonblocking read() found no events to read, then
	   it returns -1 with errno set to EAGAIN. In that case,
	   we exit the loop. */
	if (len <= 0)
		exit(EXIT_FAILURE);

	/* Loop over all events in the buffer */
	for (ptr = buf; ptr < buf + len;
            ptr += sizeof(struct inotify_event) + event->len) {

		event = (const struct inotify_event *) ptr;

		/* Print event type */
		if (event->mask & IN_CLOSE_WRITE)
            system("/usr/sbin/env-save");
        else if (event->mask & IN_IGNORED) {
			printf("IN_IGNORED: ");
            system("/usr/sbin/env-save");
	        wd[0] = inotify_add_watch(fd, CONFIG_FILE, IN_IGNORED | IN_CLOSE_WRITE);
        }
	}
}

int main(int argc, char* argv[])
{
	// init inotify
	int fd = inotify_init();
	if (fd < 0) {
		printf("LOG:inotify_init error\n");
		return -1;
	}

	int wd = inotify_add_watch(fd, CONFIG_FILE, IN_IGNORED | IN_CLOSE_WRITE);
	if (wd < 0) {
		printf("LOG:inotify_add_watch error");
		return -1;
	}

#ifdef SELECT_0
    static fd_set read_fds;
    FD_ZERO(&read_fds);
    FD_SET(fd, &read_fds);

    while (1) {
        int nready = select(fd + 1, &read_fds, NULL, NULL, NULL);
		printf("LOG:select num = %d\n", nready);
        if (nready > 0 && FD_ISSET(fd, &read_fds)) {
			printf("LOG: event happend!\n");
            handle_events(fd, &wd, argc, argv);
        }
		printf("LOG:new loop\n");
    }
#endif

#ifndef EPOLL_0
	int epfd = epoll_create(EPOLL_CLOEXEC);
	if (epfd < 0) {
		printf("LOG:epoll_create error\n");
		return -1;
	}

	struct epoll_event event;
	event.events = EPOLLIN | EPOLLET;
	event.data.fd = fd;

	int re = epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &event);
	if (re != 0) {
		printf("LOG:epoll_ctl error:%s\n", strerror(errno));
		return -1;
	}

	while (1) {
		int num = epoll_wait(epfd, &event, 1, -1);
		printf("LOG:epoll_wait num = %d\n", num);
		if (num > 0 && event.data.fd == fd) {
			printf("LOG: event happend!\n");
			handle_events(fd, &wd, argc, argv);
		}
		printf("LOG:new loop\n");
	}
#endif
	return 0;
}

