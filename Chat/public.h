#ifndef PUBLIC_H
#define PUBLIC_H

typedef enum
{
	MSG_TYPE_ACK,
	MSG_TYPE_LOGIN,
	MSG_TYPE_REGISTER,
	MSG_TYPE_TALK,
	MSG_TYPE_GROUP,
	MSG_TYPE_QUERY,
	MSG_TYPE_QUIT,
	MSG_TYPE_HEART,
	MSG_TYPE_OFFLINE
}MsgType;

#endif
