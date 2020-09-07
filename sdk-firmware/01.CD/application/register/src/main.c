#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "../include/StateMachine.h"
#include <time.h>
#include <pthread.h>
#include "../../media/include/quirc_internal.h"
#include "../../media/include/dbgutil.h"
#include "../../../3rd.party/include/curl.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_scan = PTHREAD_COND_INITIALIZER;
pthread_cond_t cond_curl = PTHREAD_COND_INITIALIZER;
int input;
struct quirc *q;
stateMachine_t stateMachine;
CURL *curl;
CURLcode res;

void nhap()
{
    printf("nhap: ");
    scanf("%d",&input);
    printf("\n");
}

void* StateMachine(void* State)
{                           
    
    StateMachine_Init(&stateMachine);   
    printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState));                  
    while(1)
    {  
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond,&mutex);
        printf("State is now %s.\r\n", StateMachine_GetStateName(stateMachine.currState)); 
        pthread_mutex_unlock(&mutex);
    }
}

void* EvenStateMachine(void* Even)
{
    while(1)
    {
        sleep(1);
        nhap();
        switch(input)
        {    
            case 1:        
                    printf("Even -> Button pushed.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_BUTTON_PUSHED);
                    pthread_cond_signal(&cond); 
            break;
            case 2:         
                    printf("Even -> Scan QR code.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_SCAN_QR);
                    pthread_cond_signal(&cond);
                    pthread_cond_signal(&cond_scan);
                                   
            break;
            case 3: 
                    printf("Even ->wifi successful\n");
                    StateMachine_RunIteration(&stateMachine, EV_WIFI_SUCCESSFUL);
                    pthread_cond_signal(&cond_curl);
                    pthread_cond_signal(&cond);
            break;
            case 4:
                    printf("Even -> Publish register successful\n");
                    StateMachine_RunIteration(&stateMachine, EV_CALL_API);
                    pthread_cond_signal(&cond);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);
                    
                    
            break;
            case 5:

                    printf("Even -> wifi fail\n");
                    StateMachine_RunIteration(&stateMachine, EV_WIFI_FAIL);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);
                    pthread_cond_signal(&cond);
            break;
            case 6:
                    printf("Even -> Button pushed again.\r\n");
                    StateMachine_RunIteration(&stateMachine, EV_BUTTON_PUSHED);
                    StateMachine_RunIteration(&stateMachine, EV_NONE);
                    pthread_cond_signal(&cond);
            break;
        }
        sleep(1);
    }       
}


void thoigian(int count )
{
    unsigned int x_minutes=0;
    unsigned int x_seconds=0;
    unsigned int x_milliseconds=0;
    unsigned int totaltime=0,time_left=0;
    clock_t x_startTime,x_countTime;
    x_startTime=clock();

    
    time_left=count-1;
    while (time_left>0) 
    {           
        x_countTime=clock(); 
        x_milliseconds=x_countTime-x_startTime;
        x_seconds=(x_milliseconds/(CLOCKS_PER_SEC))-(x_minutes*60);
        x_minutes=(x_milliseconds/(CLOCKS_PER_SEC))/60;
        time_left=count--;  
        //printf( "%d s\n",time_left);
        sleep(1);            
    }                        
} 

void* timer(void* hihi)
{
    while(1)
    {             
        if( stateMachine.currState == ST_CAMERA_STARTING)
        {   
            thoigian(8);
            //printf("Even -> Time out QR code\r\n");
            StateMachine_RunIteration(&stateMachine,EV_SCAN_QR_FAIL);
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_signal(&cond);
        }
        if(stateMachine.currState==ST_CAMERA_REGISTERING)
        {
            thoigian(8);
            //printf("Even -> Time out\n");   
            StateMachine_RunIteration(&stateMachine, EV_TIME_OUT) ;
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_signal(&cond);
        }
        if(stateMachine.currState==ST_CAMERA_CONNECTING)
        {
            thoigian(8);
            //printf("Even -> Time out\n");   
            StateMachine_RunIteration(&stateMachine, EV_TIME_OUT_CONNECTING) ;
            StateMachine_RunIteration(&stateMachine, EV_NONE);
            pthread_cond_signal(&cond);
        }
        sleep(1);                   
    }                
}

static void dump_info(struct quirc *q)
{
    int count = quirc_count(q);
    int i;

    printf("%d QR-codes found:\n\n", count);
    for (i = 0; i < count; i++) {
        struct quirc_code code;
        struct quirc_data data;
        quirc_decode_error_t err;

        quirc_extract(q, i, &code);
        err = quirc_decode(&code, &data);

        //printf("\n");

        if (err) {
            printf("  Decoding FAILED: %s\n", quirc_strerror(err));
        } else {
            printf("Decoding successful:\n");
            dump_data(&data);
        }

        printf("\n");
    }
}

struct quirc *q;
void scan_qr(){
    q = quirc_new();
    if (!q) {
        perror("can't create quirc object");
        return;
    }
    int status = -1;
    status = load_jpeg(q, "wifi.jpg");
    if (status < 0) {
        //quirc_destroy(q);
        return;
    }
    quirc_end(q);
}

void *background_qr(){
    while(1){
        pthread_mutex_lock(&mutex);
        pthread_cond_wait(&cond_scan,&mutex);
        scan_qr();
        dump_info(q);
        //quirc_destroy(q);
        pthread_mutex_unlock(&mutex);
    }
}

void curl_api(){
curl = curl_easy_init();
if(curl) {
  curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
  curl_easy_setopt(curl, CURLOPT_URL, "http://itkv5-api.vnpttiengiang.vn//v2/api/camera/registerdevice");
  curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
  struct curl_slist *headers = NULL;
  headers = curl_slist_append(headers, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
  const char *data = "{\n  \"name\": \"VNPT-Camera10\",\n  \"model\": \"VNPT-KV5-CAM10\",\n  \"udid\": \"123-456-789-10\",\n  \"user_key\": \"b1zrct94ig40-1598633528527-10\",\n  \"version\": \"0.01\",\n  \"timestamp\": \"2020-08-08T17:11:21.355590\",\n  \"wifi_address\": \"xxx\",\n  \"mac_address\": \"xxx\",\n  \"local_ip\": \"192.168.0.0\",\n  \"access_token\": \"eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJ1c2VyaWQiOjYwLCJpYXQiOjE1OTkyMDc4MDEsImV4cCI6MTU5OTI5NDIwMX0.V-ViyBqwbFAi5ei7rwe4thyGQ8-sbD75s5QiYf-eb-k\"\n}\n";
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
  res = curl_easy_perform(curl);
  printf("\n");
}
curl_easy_cleanup(curl);
}

void *background_call_api(){
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond_curl,&mutex);
    curl_api();
    pthread_mutex_unlock(&mutex);
}

int main()
{
    pthread_t thread_id1,thread_id2,thread_id3,thread_id4,thread_id5;
    // main
    pthread_create(&thread_id1,NULL,&StateMachine,NULL);
    // test
    pthread_create(&thread_id2,NULL,&EvenStateMachine,NULL);
    //timer
    pthread_create(&thread_id3,NULL,&timer,NULL);
    //scanQR
    pthread_create(&thread_id4,NULL,&background_qr,NULL);
    //curl
    pthread_create(&thread_id5,NULL,&background_call_api,NULL);
    pthread_join(thread_id1,NULL);
    pthread_join(thread_id2,NULL);
    pthread_join(thread_id3,NULL);
    pthread_join(thread_id4,NULL);
    pthread_join(thread_id5,NULL);
    return 0;
}