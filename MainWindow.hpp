#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QXmlStreamReader>
#include "Conversation.hpp"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(QWidget *parent = nullptr);
	~MainWindow();

private slots:
	void on_loadXmlBtn_clicked();

	void on_SaveBtn_clicked();

	void on_saveToFolders_clicked();

	void on_massProcessBtn_clicked();

private:
	QString HTML;
	Conversation convo;
	Ui::MainWindow *ui;
	Conversation xmlToConversation(QXmlStreamReader& xmlReader);
	void readXml(QXmlStreamReader& xmlReader, QString title, bool setHtml = true);
	static QString formatName(const QString& name);
};
#endif // MAINWINDOW_HPP
