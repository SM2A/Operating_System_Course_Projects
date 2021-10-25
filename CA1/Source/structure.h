#ifndef OS_STRUCTURE_H
#define OS_STRUCTURE_H

#define BUFFER 4096
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
#define TURN_TIMER 60

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
};
typedef struct room_data room;

struct qa_category{
    int index;
    room rooms[CATEGORY_SIZE];
};
typedef struct qa_category category;

#endif
