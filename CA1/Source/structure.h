#ifndef OS_STRUCTURE_H
#define OS_STRUCTURE_H

#define BUFFER 1024
#define ROOM_SIZE 3
#define CATEGORY_SIZE 10
#define CATEGORY_COUNT 4
#define COMPUTER 0
#define ELECTRICAL 1
#define CIVIL 2
#define MECHANIC 3
#define CHOOSE_GROUP 0
#define WAITING_START 1
#define IN_CHAT 2
#define DONE 3
#define TURN_TIMER 60

struct question_and_answer{
    char question[BUFFER];
    char answer[ROOM_SIZE-1][BUFFER];
};
typedef struct question_and_answer q_and_a;

struct client{
    int group_category;
    int stage;
    int fd;
};
typedef struct client user;

struct room_data{
    int port;
    int to_fill;
    int users[ROOM_SIZE];
    q_and_a qa[ROOM_SIZE];
};
typedef struct room_data room;

struct qa_category{
    int index;
    room rooms[CATEGORY_SIZE];
};
typedef struct qa_category category;

#endif
