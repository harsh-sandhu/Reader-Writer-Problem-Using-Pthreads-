#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

//The Shared-Global-Variable
int value=0;

//Number of Readers Accessing the Value
int reader_count=0;

//check if the writer is working on the value
//1 if working, 0 if not working
int writer_working=0;

//Initializing the Mutexes
pthread_mutex_t reader_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t writer_lock = PTHREAD_MUTEX_INITIALIZER;

//Initializing the Conditional Varibles
pthread_cond_t reader_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t writer_cond = PTHREAD_COND_INITIALIZER;


//Reader-Function
void * readerFunction(void * tid){
    long* ThreadID=(long*)tid;

    //lock the reader-mutex
    pthread_mutex_lock(&reader_lock);

    //increase the reader-count
    reader_count+=1;

    //if readers are more accesing the value, lock the writer-mutex
    if(reader_count==1){
        pthread_mutex_lock(&writer_lock);
    }

    //while writer is working, reader to wait
    while(writer_working==1){
        pthread_cond_wait(&reader_cond,&reader_lock);
    }
    
    //unlock the reader-mutex
    pthread_mutex_unlock(&reader_lock);
    
    //Printing the details
    printf("READER thread-ID :%ld\n",*ThreadID);
    printf("VALUE = %d\n",value);
    printf("Reader-Count: %d\n\n",reader_count);
   

    //lock the reader mutex
    pthread_mutex_lock(&reader_lock);

    //decrease the reader count
    reader_count-=1;

    //if there are no readers accessing value, signal the writer and unlock writer mutex
    if(reader_count==0){
	pthread_cond_signal(&writer_cond);
	pthread_mutex_unlock(&writer_lock);
    }	

    //unlock the reader-mutex
    pthread_mutex_unlock(&reader_lock);
}


//Writer-Function
void * writerFunction(void * tid){
    long* ThreadID=(long*)tid;

    //lock the writer mutex
    pthread_mutex_lock(&writer_lock);

    //locking the reader mutex
    pthread_mutex_lock(&reader_lock);

    //writer-working state
    writer_working=1;

    //write-operation
    value+=1;

    //until there are 0 reader, writer is on wait
    while(reader_count>0){
    	pthread_cond_wait(&writer_cond,&writer_lock);
    }

    //printing values
    printf("WRITER thread-ID :%ld\n",*ThreadID);
    printf("VALUE (After Writing Operation) : %d\n",value);
    printf("Reader-Count: %d\n\n",reader_count);

    //writer work done
    writer_working=0;

    //signal the reader, and unlock reader mutex
    pthread_cond_signal(&reader_cond);
    pthread_mutex_unlock(&reader_lock);

    //unlock the writer mutex
    pthread_mutex_unlock(&writer_lock);
}

//Driver-Code
int main(){

    //intializing the number of readers and writers
    int readers=0,writers=0;

    //input for reader and writers
    printf("Enter the number of readers\n");
    scanf("%d",&readers);
    printf("Enter the number of writers\n");
    scanf("%d",&writers);

    //code-fail condtion
    if(writers<=0||readers<=0){
    	printf("Add valid number of readers and writers\n");
    }

    //loop for creation of threads
    for(int i=0;i<(readers>writers?readers:writers);i++){
    	if(i<readers){
	    //Reader Thread Creation
	    pthread_t readerThread;
	    pthread_create(&readerThread,NULL,readerFunction,(void*)&readerThread);
	}
	if(i<writers){
	    //Writer Thread Creation
	    pthread_t writerThread;
	    pthread_create(&writerThread,NULL,writerFunction,(void*)&writerThread);
	}
    }
    pthread_exit(NULL);
}
