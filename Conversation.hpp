#ifndef CONVERSATION_HPP
#define CONVERSATION_HPP
#include <QString>
#include <QDate>
#include <QTime>
#include <QList>
#include <QMap>

struct Message {
	QTime time;
	QString sender;
	QString style;
	QString text;
};
typedef QList<Message> SingleConvo;
typedef QMap<QDate,SingleConvo> Conversation;

#endif // CONVERSATION_HPP
