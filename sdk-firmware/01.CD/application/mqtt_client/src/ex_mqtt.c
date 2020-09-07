#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "../include/MQTTClient.h"
#include <pthread.h>
#include "../include/cJSON.h"
#include <time.h>
#define ADDRESS     "10.82.14.239:1883"
#define QoS 		0
#define CLIENTID	"SMART_CAMERA_VNPT"

#define TOPIC_SEND      "prefix/send_command"
#define TOPIC_CONFIRM "prefix/confirm_command"
#define TOPIC_RESULT "prefix/result_command"

#define VERSION 		"1.0.0"
#define RESOLUTION "720p"

/*bien toan cuc*/
static int flag = 0;
char time_current_read[30];
char time_current_exec[30];
/***************/

MQTTClient client;
cJSON* save_json = NULL;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond1 = PTHREAD_COND_INITIALIZER; 


void publish_command(MQTTClient client, char* topic, char* payload) {
    MQTTClient_message pubmsg = MQTTClient_message_initializer;
    pubmsg.payload = payload;
    pubmsg.payloadlen = strlen(pubmsg.payload);
    pubmsg.qos = QoS;
    pubmsg.retained = 0;
    MQTTClient_deliveryToken token;
    MQTTClient_publishMessage(client, topic, &pubmsg, &token);
    MQTTClient_waitForCompletion(client, token, 1000L);
    printf("Publish: \"%s\"\n", payload);
}

void print_json(cJSON* frame1)
{
    char * str = cJSON_Print(frame1);
    printf("----------\nReceive:\n%s\n---------\n",str);
}

int frame_test_arrvd(void* context, char* topicName, int topicLen, MQTTClient_message* msg)
{
    char* contain;
    cJSON* payload_ptr = NULL;
    payload_ptr = cJSON_Parse(msg->payload);
    print_json(payload_ptr);

    /* create object json confirm_command*/
    cJSON* publish_confirm = NULL;
    publish_confirm = cJSON_CreateObject();
    cJSON* uuid_confirm = NULL;
    cJSON* command_confirm = NULL;
    cJSON* data_confirm = NULL;
    uuid_confirm = cJSON_GetObjectItem(payload_ptr,"uuid");
    command_confirm = cJSON_GetObjectItem(payload_ptr,"command");
    cJSON_AddItemToObjectCS(publish_confirm, "uuid", uuid_confirm);
    cJSON_AddItemToObjectCS(publish_confirm, "command", command_confirm);
    /******************/

    /* result_command*/
    cJSON* uuid_result = NULL;
    cJSON* command_result = NULL;
    /******************/

    /*thoigian*/
    time_t t = time(NULL);
    struct tm *ptm = localtime(&t);
    sprintf(time_current_read, "%02d-%02d-%02dT%02d:%02d:%02d +07:00",ptm->tm_year+1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec);
    /**********/

    sleep(1);
    if(flag == 0){
      flag = 1;
      cJSON_AddItemToObjectCS(publish_confirm,"status",cJSON_CreateString("avaiable"));
      cJSON_AddItemToObjectCS(publish_confirm,"read_at",cJSON_CreateString(time_current_read));
      cJSON_AddItemToObjectCS(publish_confirm, "data", data_confirm = cJSON_CreateObject());
      contain = cJSON_Print(publish_confirm);
      publish_command(client,TOPIC_CONFIRM,contain);


      /*create object json result_command*/
      uuid_result = cJSON_GetObjectItem(payload_ptr,"uuid");
      command_result = cJSON_GetObjectItem(payload_ptr,"command");
      cJSON_AddItemToObjectCS(save_json,"uuid",uuid_result);
      cJSON_AddItemToObjectCS(save_json,"command",command_result);
      /******************/
      pthread_cond_signal(&cond1);
    }else{
      cJSON_AddItemToObjectCS(publish_confirm,"status",cJSON_CreateString("busy"));
      cJSON_AddItemToObjectCS(publish_confirm,"read_at",cJSON_CreateString(time_current_read));
      cJSON_AddItemToObjectCS(publish_confirm, "data", data_confirm = cJSON_CreateObject());
      contain = cJSON_Print(publish_confirm);
      publish_command(client,TOPIC_CONFIRM,contain);
    }
    
    MQTTClient_freeMessage(&msg);
    MQTTClient_free(topicName);
    return 1;
}
/*call API*/
void *background(){

  char* result_command;
  char* true_result;
  cJSON* getItem_result = NULL;
  cJSON* data_result = NULL;

  while(1){
    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond1, &mutex);
    sleep(5);
    /*thoigian*/
    time_t t1 = time(NULL);
    struct tm *ptm1 = localtime(&t1);
    sprintf(time_current_exec, "%02d-%02d-%02dT%02d:%02d:%02d +07:00",ptm1->tm_year+1900, ptm1->tm_mon+1, ptm1->tm_mday, ptm1->tm_hour, ptm1->tm_min, ptm1->tm_sec);
    /**********/

    /*extract command*/
    getItem_result = cJSON_GetObjectItem(save_json,"command");
    true_result = cJSON_Print(getItem_result);
    /*******************/

    if(strcmp(true_result,"\"turn_off_volume\"") == 0){
      cJSON_AddItemToObjectCS(save_json,"exec_at",cJSON_CreateString(time_current_exec));
      cJSON_AddItemToObjectCS(save_json,"result",cJSON_CreateString("true"));
      cJSON_AddItemToObjectCS(save_json,"msg",data_result = cJSON_CreateString("turn_off_volume_success"));
      cJSON_AddItemToObjectCS(save_json,"data",data_result = cJSON_CreateObject());
      cJSON_AddItemToObjectCS(data_result,"volume", cJSON_CreateNumber(0));

    }else if(strcmp(true_result,"\"turn_on_volume\"") == 0){
      cJSON_AddItemToObjectCS(save_json,"exec_at",cJSON_CreateString(time_current_exec));
      cJSON_AddItemToObjectCS(save_json,"result",cJSON_CreateString("true"));
      cJSON_AddItemToObjectCS(save_json,"msg",data_result = cJSON_CreateString("turn_on_volume_success"));
      cJSON_AddItemToObjectCS(save_json,"data",data_result = cJSON_CreateObject());
      cJSON_AddItemToObjectCS(data_result,"volume", cJSON_CreateNumber(1));
    }
    else{
      cJSON_AddItemToObjectCS(save_json,"result",cJSON_CreateString("false"));
    }
    /*publish topic result*/
    result_command = cJSON_Print(save_json);
    publish_command(client, TOPIC_RESULT,result_command);

    /*delete json*/
    cJSON_DeleteItemFromObject(save_json,"uuid");
    cJSON_DeleteItemFromObject(save_json,"command");
    cJSON_DeleteItemFromObject(save_json,"result");
    cJSON_DeleteItemFromObject(save_json,"msg");
    cJSON_DeleteItemFromObject(save_json,"data");
    cJSON_DeleteItemFromObject(save_json,"exec_at");
    //cJSON_DeleteItemFromObject(data_result,"volume");

    /*Flag off*/
    flag = 0;
    pthread_mutex_unlock(&mutex);

  }
}

void MQTTClient_init(){
  int rc;
  MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer; //khoi tao mqtt
  conn_opts.username = "cameraAI";
  conn_opts.password = "VNPT@2020";
    MQTTClient_create(&client, ADDRESS, CLIENTID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    MQTTClient_setCallbacks(client, NULL, NULL, frame_test_arrvd, NULL);
    if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
    {
        printf("Failed to connect, return code %d\n", rc);
        exit(EXIT_FAILURE);
    }else printf("Subscribe broker successful\n");
}


int main(){
  
	MQTTClient_init();
	pthread_t id1;
	pthread_create(&id1, NULL, background, NULL);
  save_json = cJSON_CreateObject();
	while(1){
		MQTTClient_subscribe(client, TOPIC_SEND, QoS);
    
	}
  cJSON_Delete(save_json);
	//pthread_join(id1,NULL);
}
