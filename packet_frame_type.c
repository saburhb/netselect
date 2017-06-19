#include <stdio.h>
#include <stdlib.h>

// Time function, sockets, htons... file stat
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <unistd.h>
#include <strings.h>

#define BUFFERT 188


int main (int argc, char**argv){

	char buf[BUFFERT];
	int n;
	FILE *fd;
	int send_frame_type = 0;
        int current_frame_type = 0;
        int prev_frame_type = 0;
        int new_frame_num = 0;


	if (argc != 2){
                printf("Error usage : %s <filename>\n",argv[0]);
                return EXIT_FAILURE;
        }


	if ((fd = open(argv[1],O_RDONLY))==-1){
		perror("open fail");
                return EXIT_FAILURE;
	}


	bzero(&buf,BUFFERT);

	n=read(fd,buf,BUFFERT);


	while(n){
            if(n==-1){
                perror("read fails");
                return EXIT_FAILURE;
            }


	                /************************ DPI ************************/
            if(0x47 == buf[0])
            {
                if((0x41 == buf[1]) && (0x00 ==buf[2]))
                {
                    if((buf[3] & 0x20) && (buf[4] > 0)) // have adaptation field
                    {
                        if(buf[5] & 0x40) // found key frame
                        {
                            current_frame_type = 1; //I frame
                        }
                        else
                        {
                            current_frame_type = 2;
                        }
                    }
                    else
                    {
                        current_frame_type = 2;
                    }

                    if(buf[1] & 0x40) //Check for payload unit start indicator
                    {
                        new_frame_num++;
                        //printf("%d\n", new_frame_num);
                    }
                }
            }

	    if(current_frame_type == 0)
            {
                send_frame_type = prev_frame_type;
            }
            else
            {
                send_frame_type = current_frame_type;
                prev_frame_type = current_frame_type;
            }
	

	    printf("%d\n", send_frame_type);
	    //printf("%d\n", new_frame_num);
	    bzero(buf,BUFFERT);
	    n=read(fd,buf,BUFFERT);
	
        }//end while 

        close(fd);


	return 0;

}
